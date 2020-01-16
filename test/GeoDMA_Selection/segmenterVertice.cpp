
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


#include <terralib/rp.h>
#include <terralib/common.h>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <string>
#include <map>
#include <memory>
#include <utility>
#include <vector>

// GeoDMA Includes
#include "segmenterVertice.hpp"
#include "analysis.hpp"
#include "context.hpp"
#include "project.hpp"
#include "ioFunctions.hpp"
#include "geometryFunctions.hpp"
#include "rasterFunctions.hpp"

#define GDMA_SA_VERTICE_TYPE "SEGMENTER"

namespace
{
  static gdma::sa::SegmenterVerticeFactory segmenterVerticeFactoryInstance;
}

namespace gdma {
namespace sa {

  SegmenterVertice::SegmenterVertice()
  {
    reset();
  }

  SegmenterVertice::SegmenterVertice(const SegmenterVertice& other)
  {
    operator=( other );
  }

  SegmenterVertice::~SegmenterVertice()
  {
  }

  SegmenterVertice& SegmenterVertice::operator=( const SegmenterVertice& other )
  {
    {
      std::lock_guard<std::mutex> lock( getMutex() );
      std::lock_guard<std::mutex> lock2( other.getMutex() );

      reset();

      m_inputImageURI = other.m_inputImageURI;
      m_segStratName = other.m_segStratName;
      m_segMinSegmentSize = other.m_segMinSegmentSize;
      m_segmentsSimilarityThreshold = other.m_segmentsSimilarityThreshold;
    }

    GraphVertice::operator=( other );

    return *this;
  }

  std::string SegmenterVertice::getType() const
  {
    return std::string( GDMA_SA_VERTICE_TYPE );
  }

  void SegmenterVertice::getDescription( std::string& descriptionStr ) const
  {
    descriptionStr = "A vertice capable of creating segments (homogeneous areas represented by polygons) from an input image.";
  }

  bool SegmenterVertice::setParameter( const std::string& parameterName,
    const std::string& parameterValue )
  {
    std::lock_guard<std::mutex> lock( getMutex() );

    if( te::common::Convert2UCase( parameterName ) == "STRATEGY" )
    {
      if( te::common::Convert2UCase( parameterValue ) == "MEAN" )
      {
        m_segStratName = "RegionGrowingMean";
        return true;
      }
      else if( te::common::Convert2UCase( parameterValue ) == "BAATZ" )
      {
        m_segStratName = "RegionGrowingBaatz";
        return true;
      }
    }
    else if( te::common::Convert2UCase( parameterName ) == "MIN_SEGMENT_SIZE" )
    {
      try
      {
        m_segMinSegmentSize = boost::lexical_cast< unsigned int >( parameterValue );
      }
      catch(...)
      {
        return false;
      }
      return true;
    }
    else if( te::common::Convert2UCase( parameterName ) == "SEGMENTS_SIMILARITY_THRESHOLD" )
    {
      try
      {
        m_segmentsSimilarityThreshold = boost::lexical_cast< double >( parameterValue );
      }
      catch(...)
      {
        return false;
      }
      return true;
    }

    return false;
  }

  void SegmenterVertice::getParameters( std::vector< std::pair< std::string, std::string > >& parameters ) const
  {
    parameters.clear();

    std::lock_guard<std::mutex> lock( getMutex() );

    if( m_segStratName == "RegionGrowingMean" )
    {
      parameters.push_back( std::pair< std::string, std::string >(
        "STRATEGY", "MEAN" ) );
    }
    else if( m_segStratName == "RegionGrowingBaatz" )
    {
      parameters.push_back( std::pair< std::string, std::string >(
        "STRATEGY", "BAATZ" ) );
    }

    parameters.push_back( std::pair< std::string, std::string >(
      "MIN_SEGMENT_SIZE", boost::lexical_cast< std::string >( m_segMinSegmentSize ) ) );

    parameters.push_back( std::pair< std::string, std::string >(
      "SEGMENTS_SIMILARITY_THRESHOLD", boost::lexical_cast< std::string >( m_segmentsSimilarityThreshold ) ) );
  }

  bool SegmenterVertice::setResource( const std::string& contextURIAlias,
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

      if( te::common::Convert2UCase( verticeURIAlias ) == "IMAGE" )
      {
        m_inputImageURI = resorceURI;
        return true;
      }
    }

    return false;
  }

  void SegmenterVertice::getInParsDescription( std::map< std::string, std::string >& inParsDesc ) const
  {
    inParsDesc.clear();
    inParsDesc[ "STRATEGY" ] = std::string( "Segmenter strategy name (MEAN or BAATZ)" );
    inParsDesc[ "MIN_SEGMENT_SIZE" ] = std::string( "A positive minimum segment size (pixels number - default: 100)." );
    inParsDesc[ "SEGMENTS_SIMILARITY_THRESHOLD" ] = std::string( "Segments similarity treshold - Use lower values to merge only those segments that are more similar - Higher values will allow more segments to be merged - valid values range: positive values" );
  }

  void SegmenterVertice::getInResDescription( std::map< std::string, std::string >& inResDesc ) const
  {
    inResDesc.clear();
    inResDesc[ "IMAGE" ] = std::string( "Input image URI" );
  }

  GraphVertice::OutputType SegmenterVertice::getOutputType() const
  {
    return GraphVertice::VECTOR_URI_OUT_TYPE;
  }

  GraphVertice* SegmenterVertice::clone() const
  {
    SegmenterVertice* vertPtr = new SegmenterVertice( *this );
    return vertPtr;
  }

  bool SegmenterVertice::isConnectable( const GraphVertice& other,
    const std::string& alias ) const
  {
    if( other.getOutputType() == VECTOR_URI_OUT_TYPE )
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

  bool SegmenterVertice::isMultiClass() const
  {
    return false;
  }

  bool SegmenterVertice::is2ndPassCapable() const
  {
    return true;
  }

  void SegmenterVertice::reset()
  {
    GraphVertice::reset();

    m_inputImageURI = te::core::URI();
    m_segStratName.clear();
    m_segMinSegmentSize = 0;
    m_segmentsSimilarityThreshold = 0;
  }

  bool SegmenterVertice::executeFirstPass( std::string& errorMessage )
  {
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

      // input connection

      std::vector< std::pair< std::string, GraphVertice* > > inputVertConnections;
      getInputConnections( inputVertConnections );

      if( inputVertConnections.size() > 1 )
      {
        errorMessage = "Invalid number of input vertice connections (just 1 allowed)";
        return false;
      }

      // Load the input vertice output result

      boost::ptr_vector< te::gm::Geometry > inputVerticeGeometries;
      if( ! inputVertConnections.empty() )
      {
        if( ! loadResultShapeFileGeoms( inputVertConnections[ 0 ].second->get1stPassOutputFileName(),
          inputVerticeGeometries ) )
        {
          errorMessage = "Error loading input vertice geometries";
          return false;
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
      }

      //  trying to open the input raster

      std::unique_ptr< te::rst::Raster > inputRasterPointer;

      if( ! openRaster( m_inputImageURI, inputRasterPointer ) )
      {
        errorMessage = "Raster open error";
        return false;
      }

      // Raster clipping / reprojection

      if( !inputVerticeGeometries.empty() )
      {
        te::gm::Envelope inputGeometriesEnv;
        int inputVerticeGeometriesSRID = 0;
        if( ! getGeomSetEnvelope( inputVerticeGeometries, inputGeometriesEnv,
          inputVerticeGeometriesSRID ) )
        {
          errorMessage = "Input geometries envelope calcule error";
          return false;
        }

        std::unique_ptr< te::rst::Raster > auxRasterPointer;
        if( ! clipAndReprojectRaster( *inputRasterPointer, &inputGeometriesEnv,
          inputVerticeGeometriesSRID, auxRasterPointer ) )
        {
          errorMessage = "Input raster clipping/reprojection error";
          return false;
        }

        inputRasterPointer.reset( auxRasterPointer.release() );
      }

      // Creating the algorithm parameters

      te::rp::Segmenter::InputParameters algoInputParams;

      algoInputParams.m_inputRasterPtr = inputRasterPointer.get();

      for( unsigned int bandIdx = 0 ; bandIdx < inputRasterPointer->getNumberOfBands() ;
        ++bandIdx )
      {
        algoInputParams.m_inputRasterBands.push_back( bandIdx );
      }

      algoInputParams.m_enableThreadedProcessing = true;
      algoInputParams.m_maxSegThreads = 0;
      algoInputParams.m_enableBlockProcessing = true;
      algoInputParams.m_maxBlockSize = 0;
      algoInputParams.m_blocksOverlapPercent = 10;
      algoInputParams.m_strategyName = m_segStratName;
      algoInputParams.m_enableProgress = false;
      algoInputParams.m_enableRasterCache = true;

      if( m_segStratName == "RegionGrowingMean" )
      {
        te::rp::SegmenterRegionGrowingMeanStrategy::Parameters strategyParameters;
        strategyParameters.m_minSegmentSize = m_segMinSegmentSize;
        strategyParameters.m_segmentsSimilarityThreshold = m_segmentsSimilarityThreshold;

        algoInputParams.setSegStrategyParams( strategyParameters );
      }
      else if( m_segStratName == "RegionGrowingMean" )
      {
        te::rp::SegmenterRegionGrowingBaatzStrategy::Parameters strategyParameters;
        strategyParameters.m_minSegmentSize = m_segMinSegmentSize;
        strategyParameters.m_segmentsSimilarityThreshold = m_segmentsSimilarityThreshold;
        strategyParameters.m_bandsWeights.clear();
        strategyParameters.m_colorWeight = 0.9;
        strategyParameters.m_compactnessWeight = 0.5;

        algoInputParams.setSegStrategyParams( strategyParameters );
      }
      else
      {
        throw std::runtime_error( "Invalid strategy name" );
      }

      te::rp::Segmenter::OutputParameters algoOutputParams;
      algoOutputParams.m_rType = "EXPANSIBLE";

      /* Executing the algorithm */

      te::rp::Segmenter algorithmInstance;

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

      // Vectorization

      std::unique_ptr< boost::ptr_vector< te::gm::Geometry > >  polygonsPtrs(
        new boost::ptr_vector< te::gm::Geometry > );
      bool vectorizingOK = false;

      try
      {
        std::vector< te::gm::Geometry * > polygonsNakedPtrs;
        algoOutputParams.m_outputRasterPtr->vectorize( polygonsNakedPtrs, 0, 0, 0 );
        convertGeomsVectors( polygonsNakedPtrs, *polygonsPtrs );
        vectorizingOK = true;
      }
      catch(...)
      {
        vectorizingOK = false;
        polygonsPtrs->clear();
      }

      if( !vectorizingOK )
      {
        errorMessage = "Vectorization error";
        return false;
      }

      // Free anused resources

      algoOutputParams.m_outputRasterPtr.reset();
      algorithmInstance.reset();
      inputRasterPointer.reset();

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

  //  -------------------------------------------------------------------------

  SegmenterVerticeFactory::SegmenterVerticeFactory()
  : GraphVerticeFactory( GDMA_SA_VERTICE_TYPE )
  {}

  SegmenterVerticeFactory::~SegmenterVerticeFactory()
  {}

  GraphVertice* SegmenterVerticeFactory::build()
  {
    return new SegmenterVertice();
  }

}  // end namespace sa
}  // end namespace gdma
