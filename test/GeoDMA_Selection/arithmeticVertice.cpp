
//   Copyright (C) 2012 National Institute For Space Research (INPE) - Brazil.
//
//   This file is part of the GeoDMA - a Toolbox that integrates Data Mining Techniques with object-based
//   and multi-temporal analysis of satellite remotely sensed imagery.
//
//   GeoDMA is free software: you can redistribute it and/or modify
//   it under the terms of the GNU Lesser General Public License as published by
//   the Free Software Foundation, either version 3 of the License,
//   or (at your option) any later version.
//
//   GeoDMA is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//   GNU Lesser General Public License for more details.
//
//   You should have received a copy of the GNU Lesser General Public License
//   along with GeoDMA. See COPYING. If not, write to
//   GeoDMA Team at <thales@dpi.inpe.br, raian@dpi.inpe.br, castejon@dpi.inpe.br>.

#include <terralib/geometry.h>
#include <terralib/common.h>
#include <terralib/rp.h>

#include <boost/filesystem.hpp>
// #include <boost/lexical_cast.hpp>

#include <string>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <algorithm>

// GeoDMA Includes
#include "arithmeticVertice.hpp"
#include "ioFunctions.hpp"
#include "rasterFunctions.hpp"
#include "geometryFunctions.hpp"
#include "graph.hpp"
#include "analysis.hpp"
#include "context.hpp"
#include "../common/utils.hpp"

#define GDMA_SA_VERTICE_TYPE "ARITHMETIC"

namespace
{
  static gdma::sa::ArithmeticVerticeFactory ArithmeticVerticeFactoryInstance;
}

namespace gdma {
namespace sa {

  ArithmeticVertice::ArithmeticVertice()
  {
    reset();
  }

  ArithmeticVertice::ArithmeticVertice(const ArithmeticVertice& other)
  {
    operator=( other );
  }

  ArithmeticVertice::~ArithmeticVertice()
  {
  }

  ArithmeticVertice& ArithmeticVertice::operator=( const ArithmeticVertice& other )
  {
    {
      std::lock_guard<std::mutex> lock( getMutex() );
      std::lock_guard<std::mutex> lock2( other.getMutex() );

      reset();

      m_arithString = other.m_arithString;
      m_thresholdRangesString = other.m_thresholdRangesString;
      m_resources = other.m_resources;
    }

    GraphVertice::operator=( other );

    return *this;
  }

  std::string ArithmeticVertice::getType() const
  {
    return std::string( GDMA_SA_VERTICE_TYPE );
  }

  void ArithmeticVertice::getDescription( std::string& descriptionStr ) const
  {
    descriptionStr = "A vertice capable of performing arithmetic operations over raster data";
  }

  bool ArithmeticVertice::setParameter( const std::string& parameterName,
    const std::string& parameterValue )
  {
    std::lock_guard<std::mutex> lock( getMutex() );

    if( te::common::Convert2UCase( parameterName ) == "ARITHMETIC_STRING" )
    {
      m_arithString = parameterValue;
      return true;
    }
    else if( te::common::Convert2UCase( parameterName ) == "THRESHOLD_RANGES" )
    {
      m_thresholdRangesString = parameterValue;
      return true;
    }

    return false;
  }

  void ArithmeticVertice::getParameters( std::vector< std::pair< std::string, std::string > >& parameters ) const
  {
    parameters.clear();

    std::lock_guard<std::mutex> lock( getMutex() );

    parameters.push_back( std::pair< std::string, std::string >(
      "ARITHMETIC_STRING", m_arithString ) );
    parameters.push_back( std::pair< std::string, std::string >(
      "THRESHOLD_RANGES", m_thresholdRangesString ) );
  }

  bool ArithmeticVertice::setResource( const std::string& contextURIAlias,
    const std::string& verticeURIAlias )
  {
    if(
        ( getGraph() == 0 )
        ||
        ( getGraph()->getAnalysis() == 0 )
      )
    {
      throw std::runtime_error( "Using an uninitialized context URI Vertice" );
    }

    te::core::URI resorceURI;

    if(
        getGraph()->getAnalysis()->getContext().getResource( contextURIAlias, resorceURI )
        &&
        resorceURI.isValid()
      )
    {
      std::lock_guard<std::mutex> lock( getMutex() );

      m_resources[ verticeURIAlias ] = resorceURI;

      return true;
    }
    else
    {
      return false;
    }

    return false;
  }

  void ArithmeticVertice::getInParsDescription( std::map< std::string, std::string >& inParsDesc ) const
  {
    inParsDesc.clear();
    inParsDesc[ "ARITHMETIC_STRING" ] = std::string( "Arithmetic string to be executed. "
      " It must be formed by a combination of the following elements, separated by spaces:"
      " Operators: +, -, *, /. Real Numbers (negative numbers must follow the form \"-1.0\")."
      " Raster bands: Raster_alias_name:band_number(starting at zero)."
    );
    inParsDesc[ "THRESHOLD_RANGES" ] = std::string( "A semicolon-separated list of threshold-ranges pointing what output pixels must be considered (10>20;50>60;70>80)." );
  }

  void ArithmeticVertice::getInResDescription( std::map< std::string, std::string >& inResDesc ) const
  {
    inResDesc.clear();
  }

  GraphVertice::OutputType ArithmeticVertice::getOutputType() const
  {
    return GraphVertice::VECTOR_URI_OUT_TYPE;
  }

  GraphVertice* ArithmeticVertice::clone() const
  {
    ArithmeticVertice* vertPtr = new ArithmeticVertice( *this );
    return vertPtr;
  }

  bool ArithmeticVertice::isConnectable( const GraphVertice& other,
    const std::string& alias ) const
  {
    if(
        ( other.getOutputType() == VECTOR_URI_OUT_TYPE )
        ||
        ( other.getOutputType() == RASTER_URI_OUT_TYPE )
      )
    {
      std::vector< std::pair< std::string, GraphVertice* > > connections;
      getInputConnections( connections );

      if( connections.empty() )
      {
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }

  bool ArithmeticVertice::isMultiClass() const
  {
    return false;
  }

  bool ArithmeticVertice::is2ndPassCapable() const
  {
    return true;
  }

  bool ArithmeticVertice::executeFirstPass( std::string& errorMessage )
  {
    errorMessage.clear();

    if(
        ( ! isCacheEnabled() )
        ||
        ( ! boost::filesystem::exists( get1stPassOutputFileName() ) )
      )
    {
      // Create the output directory

      if( ! createDirectory( getOutputDirectoryName() ) )
      {
        errorMessage = "Output directory creation error";
        return false;
      }

      // Clear old output files

      {
        const boost::filesystem::path outFilePath( get1stPassOutputFileName() );
        const boost::filesystem::path outFilePathWithoutExtension =
          outFilePath.parent_path() / outFilePath.stem();
        boost::filesystem::directory_iterator dirIt( outFilePath.parent_path() );
        const boost::filesystem::directory_iterator dirItE;

        while( dirIt != dirItE )
        {
          if( boost::filesystem::is_regular_file( *dirIt ) )
          {
            const boost::filesystem::path otherFilePathWithoutExtension =
              dirIt->path().parent_path() / dirIt->path().stem();

            if( outFilePathWithoutExtension == otherFilePathWithoutExtension )
            {
              boost::filesystem::remove( dirIt->path() );
            }
          }

          ++dirIt;
        }
      }

      // Checking the arithmetic string

      {
        if( m_arithString.empty() )
        {
          errorMessage = "Empty arithmetic string";
          return false;
        }

        std::size_t findPos = m_arithString.find_first_not_of(
          " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789:-.+-*/()" );

        if( findPos < m_arithString.size() )
        {
          errorMessage = "Invalid character found inside the arithmetic string - " +
            m_arithString[ findPos ];
          return false;
        }
      }

      // pre-processing the thresholds-string

      std::vector< std::pair< double, double > > thresholdRanges;

      {
        if( m_thresholdRangesString.empty() )
        {
          errorMessage = "Empty threshold ranges string";
          return false;
        }

        std::vector< std::string > tokens;
        te::common::Tokenize( m_thresholdRangesString, tokens, ";" );

        for( std::size_t tokensIdx = 0 ; tokensIdx < tokens.size() ; ++tokensIdx )
        {
          const std::string& token = tokens[ tokensIdx ];

          std::size_t rangeSeparatorPos = token.find( ">" );
          if( rangeSeparatorPos >= token.size() )
          {
            errorMessage = "Invalid threshold-ranges";
            return false;
          }

          std::string startStr = token.substr( 0, rangeSeparatorPos );
          std::string endStr = token.substr( rangeSeparatorPos + 1, token.size() - rangeSeparatorPos + 1);
          std::pair< double, double > range;

          try
          {
            range.first = boost::lexical_cast< double >( startStr );
            range.second = boost::lexical_cast< double >( endStr );
          }
          catch(...)
          {
            errorMessage = "Invalid threshold-ranges";
            return false;
          }

          if( range.second < range.first )
          {
            errorMessage = "Invalid threshold-ranges";
            return false;
          }

          thresholdRanges.push_back( range );
        }
      }

      // Checking for a input vertice connection

      std::vector< std::pair< std::string, GraphVertice* > > connections;
      getInputConnections( connections );

      // Load the input vertice output result

      boost::ptr_vector< te::gm::Geometry > inputVerticeGeometries;
      te::gm::Envelope inputVerticeGeometriesEnvelope;
      int inputVerticeGeometriesSRID = 0;

      if( ! connections.empty() )
      {
        if( connections[ 0 ].second->getOutputType() ==
          VECTOR_URI_OUT_TYPE )
        {
          if( ! loadResultShapeFile( connections[ 0 ].second->get1stPassOutputFileName(),
            0, &inputVerticeGeometries, 0, 0, 0 ) )
          {
            errorMessage = "Input vertice 2nd Pass file load error";
            return false;
          }
        }
        else if( connections[ 0 ].second->getOutputType() ==
          VECTOR_URI_OUT_TYPE )
        {
          if( ! vectorizeResultRaster( connections[ 0 ].second->get1stPassOutputFileName(),
            inputVerticeGeometries, 0 ) )
          {
            errorMessage = "Input vertice 2nd Pass file load error";;
            return false;
          }
        }
        else
        {
          throw std::runtime_error( "Invalid output type" );
        }

        if( inputVerticeGeometries.empty() )
        {
           // generate an empty output and returns if no geometries were loaded
          if( ! createEmptyResultShapeFile( get1stPassOutputFileName() ) )
          {
            errorMessage = "1St pass output file creaation error";
            return false;
          }

          return true;
        }

        getGeomSetEnvelope( inputVerticeGeometries, inputVerticeGeometriesEnvelope,
          inputVerticeGeometriesSRID );
      }

      // Input rasters

      std::vector< std::shared_ptr< te::rst::Raster > > inputRastersPtrs;
      std::vector< std::string > inputRastersAliases;

      {
        std::map< std::string, te::core::URI >::const_iterator resourcesIt =
          m_resources.begin();
        const std::map< std::string, te::core::URI >::const_iterator resourcesItEnd =
          m_resources.end();
        std::unique_ptr< te::rst::Raster > rasterPtr;

        while( resourcesIt != resourcesItEnd )
        {
          if( ! openRaster( resourcesIt->second, rasterPtr ) )
          {
            errorMessage = "Coult not open input raster [" + resourcesIt->first + "]";
            return false;
          }

          // clipping reprojection

          if( ! connections.empty() )
          {
            std::unique_ptr< te::rst::Raster > reprojRasterPointer;

            if( ! clipAndReprojectRaster( *rasterPtr, &inputVerticeGeometriesEnvelope,
              inputVerticeGeometriesSRID, reprojRasterPointer ) )
            {
              errorMessage = "Input raster clipping/reprojection error";
              return false;
            }

            rasterPtr.reset( reprojRasterPointer.release() );
          }

          inputRastersPtrs.push_back( std::shared_ptr< te::rst::Raster >(
            rasterPtr.release() ) );
          inputRastersAliases.push_back( resourcesIt->first );

          ++resourcesIt;
        }
      }

      // Parsing the arithmetic string

      std::string arithString;

      {
        const std::size_t inputRastersAliasesSize = inputRastersAliases.size();

        std::vector< std::string > tokens;
        te::common::Tokenize( m_arithString, tokens, " " );
        bool rasterAlliasFound = false;
        std::size_t inputRastersAliasesIdx = 0;

        for( std::size_t tokensIdx = 0 ; tokensIdx < tokens.size() ; ++tokensIdx )
        {
          std::string& token = tokens[ tokensIdx ];

          if( token.find( ":" ) < token.size() )
          {
            rasterAlliasFound = false;

            for( inputRastersAliasesIdx = 0 ; inputRastersAliasesIdx <
              inputRastersAliasesSize ; ++inputRastersAliasesIdx )
            {
              if( token.find( inputRastersAliases[ inputRastersAliasesIdx ] ) <
                token.size() )
              {
                gdma::common::subStrReplace( inputRastersAliases[ inputRastersAliasesIdx ],
                  boost::lexical_cast< std::string >( inputRastersAliasesIdx ),
                  token );

                rasterAlliasFound = true;

                break;
              }
            }

            if( ! rasterAlliasFound )
            {
              errorMessage = "Invalid raster resource URI alias found inside the arithmetic string: " + token;
              return false;
            }

            token = "R" + token;
          }

          if( tokensIdx ) arithString.append( " " );

          arithString.append( token );
        }
      }

      // Executing the arithmetic

      std::unique_ptr< te::rst::Raster > arithRasterPtr;

      {
        te::rp::ArithmeticOperations::InputParameters algoInputParams;

        for( std::size_t inputRastersPtrsIdx = 0 ; inputRastersPtrsIdx <
          inputRastersPtrs.size() ; ++inputRastersPtrsIdx )
        {
          algoInputParams.m_inputRasters.push_back( inputRastersPtrs[ inputRastersPtrsIdx ].get() );
        }

        algoInputParams.m_arithmeticString = arithString;
        algoInputParams.m_normalize = false;
        algoInputParams.m_enableProgress = false;
        algoInputParams.m_interpMethod = te::rst::NearestNeighbor;

        /* Create output raster info */

        std::map<std::string, std::string> orinfo;
        orinfo[ "RTYPE" ] = "EXPANSIBLE";

        /* Defining output parameters */

        te::rp::ArithmeticOperations::OutputParameters algoOutputParams;
        algoOutputParams.m_rInfo = orinfo;
        algoOutputParams.m_rType = "EXPANSIBLE";

        te::rp::ArithmeticOperations algorithmInstance;

        if( ! algorithmInstance.initialize( algoInputParams ) )
        {
          errorMessage = "Algorithm initialization error - " + te::rp::Module::getLastLogStr();
          return false;
        }

        if( ! algorithmInstance.execute( algoOutputParams ) )
        {
          errorMessage = "Algorithm execution error - "  + te::rp::Module::getLastLogStr();
          return false;
        }

        arithRasterPtr.reset( algoOutputParams.m_outputRasterPtr.release() );
      }

//      te::rp::Copy2DiskRaster( *arithRasterPtr, getOutputDirectoryName() + "/aritmetic_result.tif" );

      // Free unused resources

      inputRastersPtrs.clear();
      inputRastersAliases.clear();

      // Thresholding

      {
        const std::size_t thresholdRangesSize = thresholdRanges.size();
        const unsigned int nCols = arithRasterPtr->getNumberOfColumns();
        const unsigned int nRows = arithRasterPtr->getNumberOfRows();
        te::rst::Band& outBand = *arithRasterPtr->getBand( 0 );
        const unsigned int nblocksX = outBand.getProperty()->m_nblocksx;
        const unsigned int nblocksY = arithRasterPtr->getBand( 0 )->getProperty()->m_nblocksy;
        const unsigned int blkH = outBand.getProperty()->m_blkh;
        const unsigned int blkW = outBand.getProperty()->m_blkw;

        unsigned int blkStartRow = 0;
        unsigned int blkStartCol = 0;
        unsigned int blkRowBound = 0;
        unsigned int blkColBound = 0;
        unsigned int col = 0;
        unsigned int row = 0;
        double value = 0;
        std::size_t thresholdRangesIdx = 0;

        for( unsigned int blkY = 0 ; blkY < nblocksY ; ++blkY )
        {
          blkStartRow = blkY * blkH;
          blkRowBound = std::min( nRows, blkStartRow + blkH );

          for( unsigned int blkX = 0 ; blkX < nblocksX ; ++blkX )
          {
            blkStartCol = blkX * blkW;
            blkColBound = std::min( nCols, blkStartCol + blkW );

            for( row = blkStartRow ; row < blkRowBound ; ++row )
            {
              for( col = blkStartCol ; col < blkColBound ; ++col )
              {
                outBand.getValue( col, row, value );

                for( thresholdRangesIdx = 0 ; thresholdRangesIdx < thresholdRangesSize ;
                  ++thresholdRangesIdx )
                {
                  if(
                      ( value >= thresholdRanges[ thresholdRangesIdx ].first )
                      &&
                      ( value <= thresholdRanges[ thresholdRangesIdx ].second )
                    )
                  {
                    outBand.setValue( col, row, 1.0 );
                  }
                  else
                  {
                    outBand.setValue( col, row, 0.0 );
                  }
                }
              }
            }
          }
        }
      }

//      te::rp::Copy2DiskRaster( *arithRasterPtr, getOutputDirectoryName() + "/threshold_result.tif" );

      // Filtering

      {
        te::rp::Filter::InputParameters algoInputParams;
        algoInputParams.m_enableProgress = false;
        algoInputParams.m_filterType = te::rp::Filter::InputParameters::ModeFilterT;
        algoInputParams.m_inRasterBands.push_back( 0 );
        algoInputParams.m_inRasterPtr = arithRasterPtr.get();
        algoInputParams.m_iterationsNumber = 1;
        algoInputParams.m_windowH = 3;
        algoInputParams.m_windowW = 3;

        /* Create output raster info */

        std::map<std::string, std::string> orinfo;
        orinfo[ "RTYPE" ] = "EXPANSIBLE";

        /* Defining output parameters */

        te::rp::Filter::OutputParameters algoOutputParams;
        algoOutputParams.m_rInfo = orinfo;
        algoOutputParams.m_rType = "EXPANSIBLE";

        te::rp::Filter algorithmInstance;

        if( ! algorithmInstance.initialize( algoInputParams ) )
        {
          errorMessage = "Algorithm initialization error - " + te::rp::Module::getLastLogStr();
          return false;
        }

        if( ! algorithmInstance.execute( algoOutputParams ) )
        {
          errorMessage = "Algorithm execution error - "  + te::rp::Module::getLastLogStr();
          return false;
        }

        arithRasterPtr.reset( algoOutputParams.m_outputRasterPtr.release() );
      }

//      te::rp::Copy2DiskRaster( *arithRasterPtr, getOutputDirectoryName() + "/filter_result.tif" );

      // Vectorizing

      std::unique_ptr< boost::ptr_vector< te::gm::Geometry > >  polygonsPtrs(
        new boost::ptr_vector< te::gm::Geometry > );

      {
        bool vectorizingOK = false;
        std::vector< te::gm::Geometry * > polygonsNakedPtrs;
        std::vector< double > polygonsValues;

        try
        {
          arithRasterPtr->vectorize( polygonsNakedPtrs, 0, 0, &polygonsValues );
          vectorizingOK = true;
        }
        catch(...)
        {
          vectorizingOK = false;
        }

        if( !vectorizingOK )
        {
          errorMessage = "Vectorization error";
          return false;
        }

        const std::size_t polygonsNakedPtrsSize = polygonsNakedPtrs.size();

        for( std::size_t polygonsNakedPtrsIdx = 0 ; polygonsNakedPtrsIdx <
          polygonsNakedPtrsSize ; ++polygonsNakedPtrsIdx )
        {
          if( polygonsValues[ polygonsNakedPtrsIdx ] == 1.0 )
          {
            polygonsPtrs->push_back( polygonsNakedPtrs[ polygonsNakedPtrsIdx ] );
          }
          else
          {
            delete polygonsNakedPtrs[ polygonsNakedPtrsIdx ];
          }
        }
      }

      // Free anused resources

      arithRasterPtr.reset();

      // Intersection and export

      {
        if( !inputVerticeGeometries.empty() )
        {
          std::unique_ptr< boost::ptr_vector< te::gm::Geometry > > intersectionGeomsVec(
            new boost::ptr_vector< te::gm::Geometry >() );

          geometrySetsIntersection( *polygonsPtrs, inputVerticeGeometries,
            *intersectionGeomsVec, 0 );

          polygonsPtrs.reset( intersectionGeomsVec.release() );
        }

        std::vector< std::string > geomLabels( polygonsPtrs->size(), getName() );
        std::vector< GraphVerticeIDT > geomsIDs( polygonsPtrs->size(), getID() );

        if( ! save2ResultShapeFile( *polygonsPtrs, get1stPassOutputFileName(),
          &geomLabels, &geomsIDs, 0, 0 ) )
        {
          errorMessage = "Output file creation error";
          return false;
        }
      }
    }

    return true;
  }

  void ArithmeticVertice::reset()
  {
    GraphVertice::reset();

    m_arithString.clear();
    m_thresholdRangesString.clear();
    m_resources.clear();
  }

  //  -------------------------------------------------------------------------

  ArithmeticVerticeFactory::ArithmeticVerticeFactory()
  : GraphVerticeFactory( GDMA_SA_VERTICE_TYPE )
  {}

  ArithmeticVerticeFactory::~ArithmeticVerticeFactory()
  {}

  GraphVertice* ArithmeticVerticeFactory::build()
  {
    return new ArithmeticVertice();
  }

}  // end namespace sa
}  // end namespace gdma
