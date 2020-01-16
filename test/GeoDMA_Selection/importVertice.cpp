
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


// #include <terralib/rp.h>
// #include <terralib/classification.h>
#include <terralib/common.h>
#include <terralib/geometry.h>
//
#include <boost/filesystem.hpp>
// #include <boost/lexical_cast.hpp>
//
#include <string>
#include <map>
#include <memory>
#include <utility>
#include <vector>
//
// // GeoDMA Includes
#include "importVertice.hpp"
#include "analysis.hpp"
// #include "context.hpp"
// #include "project.hpp"
#include "graph.hpp"
#include "ioFunctions.hpp"
#include "geometryFunctions.hpp"
#include "rasterFunctions.hpp"

#define GDMA_SA_VERTICE_TYPE "IMPORT"

namespace
{
  static gdma::sa::ImportVerticeFactory importVerticeFactoryInstance;
}

namespace gdma {
namespace sa {

  ImportVertice::ImportVertice()
  {
    reset();
  }

  ImportVertice::ImportVertice(const ImportVertice& other)
  {
    operator=( other );
  }

  ImportVertice::~ImportVertice()
  {
  }

  ImportVertice& ImportVertice::operator=( const ImportVertice& other )
  {
    {
      std::lock_guard<std::mutex> lock( getMutex() );
      std::lock_guard<std::mutex> lock2( other.getMutex() );

      reset();

      m_inputImageURI = other.m_inputImageURI;
      m_inputVectorsURI = other.m_inputVectorsURI;
      m_classLabelsPropertyName = other.m_classLabelsPropertyName;
      m_extraPropertiesNames = other.m_extraPropertiesNames;
    }

    GraphVertice::operator=( other );

    return *this;
  }

  std::string ImportVertice::getType() const
  {
    return std::string( GDMA_SA_VERTICE_TYPE );
  }

  void ImportVertice::getDescription( std::string& descriptionStr ) const
  {
    descriptionStr = "A vertice to import external regions into the graph flow.";
  }

  bool ImportVertice::setParameter( const std::string& parameterName,
    const std::string& parameterValue )
  {
    std::lock_guard<std::mutex> lock( getMutex() );

    if( te::common::Convert2UCase( parameterName ) == "CLASS_LABELS_PROPERTY_NAME" )
    {
      m_classLabelsPropertyName = parameterValue;
      return true;
    }
    else if( te::common::Convert2UCase( parameterName ) == "PROPERTY_NAMES" )
    {
      m_extraPropertiesNames = parameterValue;
      return true;
    }

    return false;
  }

  void ImportVertice::getParameters( std::vector< std::pair< std::string, std::string > >& parameters ) const
  {
    parameters.clear();

    std::lock_guard<std::mutex> lock( getMutex() );

    parameters.push_back( std::pair< std::string, std::string >(
      "CLASS_LABELS_PROPERTY_NAME", m_classLabelsPropertyName ) );
    parameters.push_back( std::pair< std::string, std::string >(
      "PROPERTY_NAMES", m_extraPropertiesNames ) );
  }

  bool ImportVertice::setResource( const std::string& contextURIAlias,
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
      else if( te::common::Convert2UCase( verticeURIAlias ) == "VECTORS" )
      {
        m_inputVectorsURI = resorceURI;
        return true;
      }
    }
    else
    {
      return false;
    }

    return false;
  }

  void ImportVertice::getInParsDescription( std::map< std::string, std::string >& inParsDesc ) const
  {
    inParsDesc.clear();
    inParsDesc[ "CLASS_LABELS_PROPERTY_NAME" ] = "Class labels property (column) name";
    inParsDesc[ "PROPERTY_NAMES" ] = "Semicolon-separated list of extra properties to import (columns names)";
  }

  void ImportVertice::getInResDescription( std::map< std::string, std::string >& inResDesc ) const
  {
    inResDesc.clear();
    inResDesc[ "IMAGE" ] = std::string( "Input image URI" );
    inResDesc[ "VECTORS" ] = std::string( "Input vectors URI" );
  }

  GraphVertice::OutputType ImportVertice::getOutputType() const
  {
    return GraphVertice::VECTOR_URI_OUT_TYPE;
  }

  GraphVertice* ImportVertice::clone() const
  {
    ImportVertice* vertPtr = new ImportVertice( *this );
    return vertPtr;
  }

  bool ImportVertice::isConnectable( const GraphVertice& other,
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

  bool ImportVertice::isMultiClass() const
  {
    return true;
  }

  bool ImportVertice::is2ndPassCapable() const
  {
    return true;
  }

  void ImportVertice::reset()
  {
    GraphVertice::reset();

    m_inputImageURI = te::core::URI();
    m_inputVectorsURI = te::core::URI();
    m_classLabelsPropertyName.clear();
    m_extraPropertiesNames.clear();
  }

  bool ImportVertice::executeFirstPass( std::string& errorMessage )
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

      // Load the input vertice output result

      boost::ptr_vector< te::gm::Geometry > inputVerticeGeometries;

      if( ! inputVertConnections.empty() )
      {
        if( ! loadResultShapeFileGeoms( inputVertConnections[ 0 ].second->get1stPassOutputFileName(),
          inputVerticeGeometries ) )
        {
          errorMessage = "Error loading input vertice result";
          return false;
        }
      }

      // importing data

      boost::ptr_vector< te::gm::Geometry > importedGeometries;
      std::vector< std::string > importedGeometriesLabels;
      std::vector< GraphVerticeIDT > importedGeometriesIDs;
      std::vector< std::string > importedEextraPropertiesNames;
      std::vector< std::vector< double > > importedExtraPropertiesValues;

      if( m_inputImageURI.isValid() )
      { //  Importing raster data
        //  trying to open the input raster

        std::unique_ptr< te::rst::Raster > inputRasterPointer;

        if( !openRaster( m_inputImageURI, inputRasterPointer ) )
        {
          errorMessage = "Raster open error";
          return false;
        }

        // Raster clipping / reprojection

        if(
            ( inputRasterPointer.get() != 0 )
            &&
            ( !inputVerticeGeometries.empty() )
          )
        {
          te::gm::Envelope inputVerticeGeometriesEnv;
          int inputVerticeGeometriesSRID = 0;
          if( ! getGeomSetEnvelope( inputVerticeGeometries, inputVerticeGeometriesEnv,
            inputVerticeGeometriesSRID ) )
          {
            errorMessage = "Input geometries envelope calcule error";
            return false;
          }

          std::unique_ptr< te::rst::Raster > auxRasterPointer;
          if( ! clipAndReprojectRaster( *inputRasterPointer, &inputVerticeGeometriesEnv,
            inputVerticeGeometriesSRID, auxRasterPointer ) )
          {
            errorMessage = "Input raster clipping/reprojection error";
            return false;
          }

          inputRasterPointer.reset( auxRasterPointer.release() );
        }

        // Vectorization

        if( ! vectorizeResultRaster( *inputRasterPointer, importedGeometries,
          &importedGeometriesIDs ) )
        {
          errorMessage = "Vectorization error";
          return false;
        }

        const std::size_t importedGeometriesIDsSize = importedGeometriesIDs.size();

        for( std::size_t importedGeometriesIDsIdx = 0 ; importedGeometriesIDsIdx <
          importedGeometriesIDsSize ; ++importedGeometriesIDsIdx )
        {
          importedGeometriesLabels.push_back( boost::lexical_cast< std::string >(
            importedGeometriesIDs[ importedGeometriesIDsIdx ] ) );
        }
      }
      else if( m_inputVectorsURI.isValid() )
      {  //  Importing vector data
        te::common::Tokenize( m_extraPropertiesNames, importedEextraPropertiesNames, ";" );

        if( ! loadDataSetData( m_inputVectorsURI, "OGR", "", m_classLabelsPropertyName,
          "", importedEextraPropertiesNames, &importedGeometries, &importedGeometriesLabels,
          0, &importedExtraPropertiesValues ) )
        {
          errorMessage = "Vector data loading error";
          return false;
        }

        const std::size_t importedGeometriesLabelsSize = importedGeometriesLabels.size();
        std::map< std::string, GraphVerticeIDT > labels2IdsMap;
        GraphVerticeIDT nextValidID = 1;
        std::map< std::string, GraphVerticeIDT >::const_iterator labels2IdsMapIt;

        for( std::size_t importedGeometriesLabelsIdx = 0 ; importedGeometriesLabelsIdx <
          importedGeometriesLabelsSize ; ++importedGeometriesLabelsIdx )
        {
          labels2IdsMapIt = labels2IdsMap.find( importedGeometriesLabels[
            importedGeometriesLabelsIdx ] );

          if( labels2IdsMapIt == labels2IdsMap.end() )
          {
            labels2IdsMap[ importedGeometriesLabels[ importedGeometriesLabelsIdx ] ]
              = nextValidID;
            importedGeometriesIDs.push_back( nextValidID );
            ++nextValidID;
          }
          else
          {
            importedGeometriesIDs.push_back( labels2IdsMapIt->second );
          }
        }
      }
      else
      {
        errorMessage = "Invalid input image or input vectors URI";
        return false;
      }

      // Intersection with input vertice output

      if( ! inputVerticeGeometries.empty() )
      {
        boost::ptr_vector< te::gm::Geometry > intersection;
        std::vector< std::size_t > importedGeometriesOriginalIndexes;
        geometrySetsIntersection( importedGeometries, inputVerticeGeometries,
          intersection, &importedGeometriesOriginalIndexes );

        // updating

        importedGeometries.clear();

        std::vector< std::string > newLabels;
        std::vector< GraphVerticeIDT > newIDs;
        std::vector< std::vector< double > > newOtherPropertiesValues;
        std::size_t intersectionIdx = intersection.size() - 1;
        std::size_t originalIdx = 0;

        while( !intersection.empty() )
        {
          originalIdx = importedGeometriesOriginalIndexes[ intersectionIdx ];

          importedGeometries.push_back( intersection.pop_back().release() );

          newLabels.push_back( importedGeometriesLabels[ originalIdx ] );

          newIDs.push_back( importedGeometriesIDs[ originalIdx ] );

          if( ! importedExtraPropertiesValues.empty() )
          {
            newOtherPropertiesValues.push_back( importedExtraPropertiesValues[
              originalIdx ] );
          }

          --intersectionIdx;
        }

        importedGeometriesLabels = newLabels;
        importedGeometriesIDs = newIDs;
        importedExtraPropertiesValues = newOtherPropertiesValues;
      }

      // Export to disk

      if( ! save2ResultShapeFile( importedGeometries, get1stPassOutputFileName(),
        &importedGeometriesLabels, &importedGeometriesIDs, &importedEextraPropertiesNames,
        &importedExtraPropertiesValues ) )
      {
        errorMessage = "Output file creation error";
        return false;
      }
    }

    return true;
  }

  //  -------------------------------------------------------------------------

  ImportVerticeFactory::ImportVerticeFactory()
  : GraphVerticeFactory( GDMA_SA_VERTICE_TYPE )
  {}

  ImportVerticeFactory::~ImportVerticeFactory()
  {}

  GraphVertice* ImportVerticeFactory::build()
  {
    return new ImportVertice();
  }

}  // end namespace sa
}  // end namespace gdma
