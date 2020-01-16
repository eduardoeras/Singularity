
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


#include <terralib/common.h>
#include <terralib/dataaccess.h>
#include <terralib/memory.h>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <string>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <set>

// GeoDMA Includes
#include "featuresVertice.hpp"
#include "analysis.hpp"
#include "context.hpp"
#include "project.hpp"
#include "ioFunctions.hpp"
#include "geometryFunctions.hpp"
#include "../featureextraction/spectralFeatures.hpp"
#include "../featureextraction/spatialFeatures.hpp"
#include "../featureextraction/landscapeFeatures.hpp"

#define GDMA_SA_VERTICE_TYPE "FEATURES"

namespace
{
  static gdma::sa::FeaturesVerticeFactory featuresVerticeFactoryInstance;
}

namespace gdma {
namespace sa {

  FeaturesVertice::FeaturesVertice()
  {
    reset();
  }

  FeaturesVertice::FeaturesVertice(const FeaturesVertice& other)
  {
    operator=( other );
  }

  FeaturesVertice::~FeaturesVertice()
  {
  }

  FeaturesVertice& FeaturesVertice::operator=( const FeaturesVertice& other )
  {
    {
      std::lock_guard<std::mutex> lock( getMutex() );
      std::lock_guard<std::mutex> lock2( other.getMutex() );

      reset();

      m_enableSpectralFeatures = other.m_enableSpectralFeatures;
      m_enableSpatialFeatures = other.m_enableSpatialFeatures;
      m_enableLandscapeFeatures = other.m_enableLandscapeFeatures;
      m_inputImageURI = other.m_inputImageURI;
      m_inputFinerVectorsURI = other.m_inputFinerVectorsURI;
      m_finerVectorsClassPropertyName = other.m_finerVectorsClassPropertyName;
    }

    GraphVertice::operator=( other );

    return *this;
  }

  std::string FeaturesVertice::getType() const
  {
    return std::string( GDMA_SA_VERTICE_TYPE );
  }

  void FeaturesVertice::getDescription( std::string& descriptionStr ) const
  {
    descriptionStr = "A vertice capable of generateing features.";
  }

  bool FeaturesVertice::setParameter( const std::string& parameterName,
    const std::string& parameterValue )
  {
    std::lock_guard<std::mutex> lock( getMutex() );

    if( te::common::Convert2UCase( parameterName ) == "ENABLE_SPECTRAL_FEATURES" )
    {
      if( te::common::Convert2UCase( parameterValue ) == "YES" )
      {
        m_enableSpectralFeatures = true;
        return true;
      }
      else if( te::common::Convert2UCase( parameterValue ) == "NO" )
      {
        m_enableSpectralFeatures = false;
        return true;
      }
    }
    else if( te::common::Convert2UCase( parameterName ) == "ENABLE_SPATIAL_FEATURES" )
    {
      if( te::common::Convert2UCase( parameterValue ) == "YES" )
      {
        m_enableSpatialFeatures = true;
        return true;
      }
      else if( te::common::Convert2UCase( parameterValue ) == "NO" )
      {
        m_enableSpatialFeatures = false;
        return true;
      }
    }
    else if( te::common::Convert2UCase( parameterName ) == "ENABLE_LANDSCAPE_FEATURES" )
    {
      if( te::common::Convert2UCase( parameterValue ) == "YES" )
      {
        m_enableLandscapeFeatures = true;
        return true;
      }
      else if( te::common::Convert2UCase( parameterValue ) == "NO" )
      {
        m_enableLandscapeFeatures = false;
        return true;
      }
    }
    else if(
              ( te::common::Convert2UCase( parameterName ) == "FINER_VECTORS_CLASS_PROPERTY_NAME" )
              &&
              ( ! parameterValue.empty() )
           )
    {
      m_finerVectorsClassPropertyName = parameterValue;
      return true;
    }

    return false;
  }

  void FeaturesVertice::getParameters( std::vector< std::pair< std::string, std::string > >& parameters ) const
  {
    parameters.clear();

    std::lock_guard<std::mutex> lock( getMutex() );

    if( m_enableSpectralFeatures )
    {
      parameters.push_back( std::pair< std::string, std::string >(
        "ENABLE_SPECTRAL_FEATURES", "YES" ) );
    }
    else
    {
      parameters.push_back( std::pair< std::string, std::string >(
        "ENABLE_SPECTRAL_FEATURES", "NO" ) );
    }

    if( m_enableSpatialFeatures )
    {
      parameters.push_back( std::pair< std::string, std::string >(
        "ENABLE_SPATIAL_FEATURES", "YES" ) );
    }
    else
    {
      parameters.push_back( std::pair< std::string, std::string >(
        "ENABLE_SPATIAL_FEATURES", "NO" ) );
    }

    if( m_enableLandscapeFeatures )
    {
      parameters.push_back( std::pair< std::string, std::string >(
        "ENABLE_LANDSCAPE_FEATURES", "YES" ) );
    }
    else
    {
      parameters.push_back( std::pair< std::string, std::string >(
        "ENABLE_LANDSCAPE_FEATURES", "NO" ) );
    }

    parameters.push_back( std::pair< std::string, std::string >(
      "FINER_VECTORS_CLASS_PROPERTY_NAME", m_finerVectorsClassPropertyName ) );
  }

  bool FeaturesVertice::setResource( const std::string& contextURIAlias,
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
      else if( te::common::Convert2UCase( verticeURIAlias ) == "FINER_VECTORS" )
      {
        m_inputFinerVectorsURI = resorceURI;
        return true;
      }
    }

    return false;
  }

  void FeaturesVertice::getInParsDescription( std::map< std::string, std::string >& inParsDesc ) const
  {
    inParsDesc.clear();
    inParsDesc[ "ENABLE_SPECTRAL_FEATURES" ] = std::string( "Enable (\"YES\") or disable (\"NO\") the calcule of spectral features." );
    inParsDesc[ "ENABLE_SPATIAL_FEATURES" ] = std::string( "Enable (\"YES\") or disable (\"NO\") the calcule of spatial features." );
    inParsDesc[ "ENABLE_LANDSCAPE_FEATURES" ] = std::string( "Enable (\"YES\") or disable (\"NO\") the calcule of landscape features." );
    inParsDesc[ "FINER_VECTORS_CLASS_PROPERTY_NAME" ] = std::string( "Finer vectors class ID property name (required if generating landscape features)" );
  }

  void FeaturesVertice::getInResDescription( std::map< std::string, std::string >& inResDesc ) const
  {
    inResDesc.clear();
    inResDesc[ "IMAGE" ] = std::string( "Input image URI" );
    inResDesc[ "FINER_VECTORS" ] = std::string( "Finer vectors URI" );
  }

  GraphVertice::OutputType FeaturesVertice::getOutputType() const
  {
    return GraphVertice::VECTOR_URI_OUT_TYPE;
  }

  GraphVertice* FeaturesVertice::clone() const
  {
    FeaturesVertice* vertPtr = new FeaturesVertice( *this );
    return vertPtr;
  }

  bool FeaturesVertice::isConnectable( const GraphVertice& other,
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

  bool FeaturesVertice::isMultiClass() const
  {
    // Checking for a input vertice connection

    std::vector< std::pair< std::string, GraphVertice* > > connections;
    getInputConnections( connections );

    if( connections.empty() )
    {
      return true;
    }
    else
    {
      return connections[ 0 ].second->isMultiClass();
    }
  }

  bool FeaturesVertice::is2ndPassCapable() const
  {
    return true;
  }

  void FeaturesVertice::reset()
  {
    GraphVertice::reset();

    m_enableSpectralFeatures = false;
    m_enableSpatialFeatures = false;
    m_enableLandscapeFeatures = false;
    m_inputImageURI = te::core::URI();
    m_inputFinerVectorsURI = te::core::URI();
    m_finerVectorsClassPropertyName.clear();
  }

  bool FeaturesVertice::executeFirstPass( std::string& errorMessage )
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

      // Checking for a input vertice connection

      std::vector< std::pair< std::string, GraphVertice* > > connections;
      getInputConnections( connections );

      if( connections.size() != 1 )
      {
        errorMessage = "Invalid number of input vertices connections (1 required and allowed)";
        return false;
      }

      // Load the input vertice output result

      boost::shared_ptr< boost::ptr_vector< te::gm::Geometry > > inputVerticeGeometriesPtr;

      {
        boost::ptr_vector< te::gm::Geometry > inputVerticeComplexGeometries;

        if( ! loadResultShapeFileGeoms( connections[ 0 ].second->get1stPassOutputFileName(),
          inputVerticeComplexGeometries ) )
        {
          errorMessage = "Error loading input vertice geometries";
          return false;
        }

        inputVerticeGeometriesPtr.reset( new boost::ptr_vector< te::gm::Geometry >() );
        multi2Single( inputVerticeComplexGeometries, *inputVerticeGeometriesPtr );
      }

      // generate an empty output and returns if no geometries were loaded

      if( inputVerticeGeometriesPtr->empty() )
      {
        if( ! createEmptyResultShapeFile( get1stPassOutputFileName() ) )
        {
          errorMessage = "1St pass output file creaation error";
          return false;
        }

        return true;
      }

      // reference SRID

      const int referenceSRID = inputVerticeGeometriesPtr->at( 0 ).getSRID();
      if( referenceSRID == 0 )
      {
        errorMessage = "Invalid reference SRID";
        return false;
      }

      //  trying to open the input raster, if there is one

      std::unique_ptr< te::rst::Raster > inputRasterPointer;

      if( m_enableSpectralFeatures )
      {
        if( ! m_inputImageURI.isValid() )
        {
          errorMessage = "Invalid raster URI";
          return false;
        }

        if( ! openRaster( m_inputImageURI, inputRasterPointer ) )
        {
          errorMessage = "Raster open error";
          return false;
        }

        if( inputRasterPointer->getSRID() != referenceSRID )
        {
          std::map< std::string, std::string >  rinfo;
          rinfo[ "RTYPE" ] = "EXPANSIBLE";
          rinfo[ "USE_TERRALIB_REPROJECTION" ] = "TRUE";

          std::unique_ptr< te::rst::Raster > reprojRasterPointer(
            inputRasterPointer->transform( referenceSRID, rinfo, te::rst::NearestNeighbor ) );

          inputRasterPointer.reset( reprojRasterPointer.release() );
        }
      }

      // intersection between geometries and the input raster

      if( inputRasterPointer.get() )
      {
        boost::ptr_vector< te::gm::Geometry > rasterBoxGeomVec;
        rasterBoxGeomVec.push_back( te::gm::GetGeomFromEnvelope(
          inputRasterPointer->getGrid()->getExtent(),
          inputRasterPointer->getSRID() ) );

        boost::ptr_vector< te::gm::Geometry > intersectionGeoms;

        geometrySetsIntersection( *inputVerticeGeometriesPtr, rasterBoxGeomVec,
          intersectionGeoms, 0 );

        boost::ptr_vector< te::gm::Geometry > intersectionSingleGeoms;

        multi2Single( intersectionGeoms, intersectionSingleGeoms );

        intersectionGeoms.clear();

        // transfer only polygons greater than one pixel

        inputVerticeGeometriesPtr->clear();

        const double minXSize = inputRasterPointer->getGrid()->getResolutionX();
        const double minYSize = inputRasterPointer->getGrid()->getResolutionY();

        while( ! intersectionSingleGeoms.empty() )
        {
          if(
              ( intersectionSingleGeoms.back().getGeomTypeId() == te::gm::PolygonType )
              &&
              ( intersectionSingleGeoms.back().getMBR()->getWidth() >= minXSize )
              &&
              ( intersectionSingleGeoms.back().getMBR()->getHeight() >= minYSize )
            )
          {
            inputVerticeGeometriesPtr->push_back( intersectionSingleGeoms.pop_back().release() );
          }
          else
          {
            intersectionSingleGeoms.pop_back();
          }
        }
      }

      // generate an empty output and returns if no geometries were intersected

      if( inputVerticeGeometriesPtr->empty() )
      {
        if( ! createEmptyResultShapeFile( get1stPassOutputFileName() ) )
        {
          errorMessage = "1St pass output file creaation error";
          return false;
        }

        return true;
      }

      // creating the geoms maps

      std::map< std::string, te::gm::Geometry* > geomID2GeomPtrMap;
      std::map< std::string, std::size_t > geomID2GeomIdxMap;

      {
        const std::size_t inputVerticeGeometriesSize = inputVerticeGeometriesPtr->size();
        std::string geomID;

        for( std::size_t inputVerticeGeometriesIdx = 0 ; inputVerticeGeometriesIdx <
          inputVerticeGeometriesSize ; ++inputVerticeGeometriesIdx )
        {
          geomID = boost::lexical_cast< std::string >( inputVerticeGeometriesIdx );

          geomID2GeomPtrMap[ geomID ] = &( inputVerticeGeometriesPtr->at( inputVerticeGeometriesIdx ) );

          geomID2GeomIdxMap[ geomID ] = inputVerticeGeometriesIdx;
        }
      }

      // Spectral Features

      std::map<std::string, gdma::fte::FeaturesSummary> spectralFeatures;

      if( m_enableSpectralFeatures )
      {
        // bands

        std::vector<unsigned int> bands;

        for( unsigned int bandIdx = 0 ; bandIdx < inputRasterPointer->getNumberOfBands() ;
          ++bandIdx )
        {
          bands.push_back( bandIdx );
        }

        // Features calcule

        bool result = false;

        try
        {
          result = gdma::fte::SpectralFeatures::getFeatures( inputRasterPointer.get(),
            geomID2GeomPtrMap, bands, spectralFeatures, true );
        }
        catch(...)
        {
          result = false;
        }

        if( ! result )
        {
          errorMessage = "Spectral features calcule error";
          return false;
        }
      }

      // clear resources

      inputRasterPointer.reset();

      // Spatial features

      std::map<std::string, gdma::fte::FeaturesSummary> spatialFeatures;

      if( m_enableSpatialFeatures )
      {
        bool result = false;

        try
        {
          result = gdma::fte::SpatialFeatures::getFeatures( geomID2GeomPtrMap, spatialFeatures, true, -1000 );
        }
        catch(...)
        {
          result = false;
        }

        if( ! result )
        {
          errorMessage = "Spatial features calcule error";
          return false;
        }
      }

      // Landscape features

      std::map<std::string, gdma::fte::FeaturesSummary> landscapeFeatures;

      if( m_enableLandscapeFeatures )
      {
        if( m_finerVectorsClassPropertyName.empty() )
        {
          errorMessage = "Invalid finer vectors class property name";
          return false;
        }

        // Load the finer vectors

        boost::ptr_vector< te::gm::Geometry > complexFinerVectors;
        std::vector< std::string > complexFinerVectorsClasses;

        if( ! loadDataSetData( m_inputFinerVectorsURI, "OGR", "",
          m_finerVectorsClassPropertyName, "", std::vector< std::string >(),
          &complexFinerVectors, &complexFinerVectorsClasses,
          (std::vector< GraphVerticeIDT > *)0,
          (std::vector< std::vector< double > > *)0 ) )
        {
          errorMessage = "Error loading finer geometries";
          return false;
        }

        if( complexFinerVectors.size() == 0 )
        {
          errorMessage = "No finner vectors loaded";
          return false;
        }

        // Reproject finer vectors if needed

        if( complexFinerVectors[ 0 ].getSRID() != referenceSRID )
        {
          transformGeometries( referenceSRID, complexFinerVectors );
        }

        // Convert to single geoms

        std::vector< te::gm::Geometry* > singleFinerVectors;
        std::vector< std::string > singleFinerVectorsClasses;
        std::set< std::string > singleFinerUniqueClasses;

        {
          const std::size_t complexFinerVectorsSize = complexFinerVectors.size();
          std::vector< te::gm::Geometry* > singleGeoms;
          std::size_t singleGeomsSize = 0;
          std::size_t singleGeomsIdx = 0;

          for( std::size_t complexFinerVectorsIdx = 0 ; complexFinerVectorsIdx <
            complexFinerVectorsSize ; ++complexFinerVectorsIdx )
          {
            singleGeoms.clear();
            te::gm::Multi2Single( &( complexFinerVectors[ complexFinerVectorsIdx ] ), singleGeoms );
            singleGeomsSize = singleGeoms.size();

            for( singleGeomsIdx = 0 ; singleGeomsIdx < singleGeomsSize ; ++singleGeomsIdx )
            {
              singleFinerVectors.push_back( singleGeoms[ singleGeomsIdx ] );
              singleFinerVectorsClasses.push_back( complexFinerVectorsClasses[
                complexFinerVectorsIdx ] );
            }

            if( singleFinerUniqueClasses.find( complexFinerVectorsClasses[
                complexFinerVectorsIdx ] ) == singleFinerUniqueClasses.end() )
            {
              singleFinerUniqueClasses.insert( complexFinerVectorsClasses[
                complexFinerVectorsIdx ] );
            }
          }
        }

        // preparing data

        std::vector< te::gm::Geometry* > inputVerticeGeometries;
        {
          const std::size_t inputVerticeGeometriesSize = inputVerticeGeometriesPtr->size();

          inputVerticeGeometries.resize( inputVerticeGeometriesSize );

          for( std::size_t inputVerticeGeometriesIdx = 0 ; inputVerticeGeometriesIdx <
            inputVerticeGeometriesSize ; ++inputVerticeGeometriesIdx )
          {
            inputVerticeGeometries[ inputVerticeGeometriesIdx] =
              &( inputVerticeGeometriesPtr->at( inputVerticeGeometriesIdx ) );
          }
        }

        boost::bimap<std::string, std::size_t> inputVerticeGeometriesBimap;
        {
          std::map< std::string, std::size_t >::const_iterator geomID2GeomIdxMapIt =
            geomID2GeomIdxMap.begin();
          std::map< std::string, std::size_t >::const_iterator geomID2GeomIdxMapItEnd =
            geomID2GeomIdxMap.end();

          while( geomID2GeomIdxMapIt != geomID2GeomIdxMapItEnd )
          {
            inputVerticeGeometriesBimap.insert( boost::bimap<std::string, std::size_t>::value_type(
              geomID2GeomIdxMapIt->first, geomID2GeomIdxMapIt->second ) );

            ++geomID2GeomIdxMapIt;
          }
        }

        boost::bimap<std::string, std::size_t> singleFinerVectorsBimap;
        {
          const std::size_t singleFinerVectorsSize = singleFinerVectors.size();

          for( std::size_t singleFinerVectorsIdx = 0 ; singleFinerVectorsIdx <
            singleFinerVectorsSize ; ++singleFinerVectorsIdx )
          {
            singleFinerVectorsBimap.insert( boost::bimap<std::string, std::size_t>::value_type(
              singleFinerVectorsClasses[ singleFinerVectorsIdx ], singleFinerVectorsIdx ) );
          }
        }

        // features

        bool result = false;

        try
        {
          gdma::fte::LandscapeFeatures landInstance( inputVerticeGeometries,
            inputVerticeGeometriesBimap, singleFinerVectors, singleFinerVectorsBimap,
            singleFinerVectorsClasses, singleFinerUniqueClasses, -1.0 );

          result = landInstance.getFeatures( landscapeFeatures );
        }
        catch(...)
        {
          result = false;
        }

        if( ! result )
        {
          errorMessage = "Landscape features calcule error";
          return false;
        }
      }

      // Preparing features for output

      std::vector< std::string > extraPropertiesNames;

      // info: extraPropertiesValues[ geometry index ][ property index ].
      std::vector< std::vector< double > > extraPropertiesValues;

      {
        std::size_t spectralFeaturesNumber = 0;
        if( ! spectralFeatures.empty() )
        {
          const std::vector<gdma::fte::Attribute>& features = spectralFeatures.begin()->second.getFeatures();
          spectralFeaturesNumber = features.size();

          for( std::size_t featuresIdx = 0 ; featuresIdx < spectralFeaturesNumber ; ++featuresIdx )
          {
            extraPropertiesNames.push_back(  features[ featuresIdx ].getShortName() );
          }
        }

        std::size_t spatialFeaturesNumber = 0;
        if( ! spatialFeatures.empty() )
        {
          const std::vector<gdma::fte::Attribute>& features = spatialFeatures.begin()->second.getFeatures();
          spatialFeaturesNumber = features.size();

          for( std::size_t featuresIdx = 0 ; featuresIdx < spatialFeaturesNumber ; ++featuresIdx )
          {
            extraPropertiesNames.push_back(  features[ featuresIdx ].getShortName() );
          }
        }

        std::size_t landscapeFeaturesNumber = 0;
        if( ! landscapeFeatures.empty() )
        {
          const std::vector<gdma::fte::Attribute>& features = landscapeFeatures.begin()->second.getFeatures();
          landscapeFeaturesNumber = features.size();

          for( std::size_t featuresIdx = 0 ; featuresIdx < landscapeFeaturesNumber ; ++featuresIdx )
          {
            extraPropertiesNames.push_back(  features[ featuresIdx ].getShortName() );
          }
        }

        if( ! extraPropertiesNames.empty() )
        {
          std::vector< double > dummyVec( extraPropertiesNames.size(), 0.0 );
          extraPropertiesValues.resize( inputVerticeGeometriesPtr->size(), dummyVec );

          if( ! spectralFeatures.empty() )
          {
            std::map<std::string, gdma::fte::FeaturesSummary>::const_iterator
              featIt = spectralFeatures.begin();
            const std::map<std::string, gdma::fte::FeaturesSummary>::const_iterator
              featItEnd = spectralFeatures.end();
            std::size_t featureIdx = 0;
            std::size_t inputFeaturesSize = 0;
            std::size_t geomIdx = 0;

            while( featIt != featItEnd )
            {
              assert( geomID2GeomIdxMap.find( featIt->first ) != geomID2GeomIdxMap.end() );
              geomIdx = geomID2GeomIdxMap[ featIt->first ];

              const std::vector<gdma::fte::Attribute>& inputFeatures = featIt->second.getFeatures();
              inputFeaturesSize = inputFeatures.size();

              assert( geomIdx < extraPropertiesValues.size() );
              std::vector< double >& outputVec = extraPropertiesValues[ geomIdx ];

              for( featureIdx = 0 ; featureIdx < inputFeaturesSize ; ++featureIdx )
              {
                outputVec[ featureIdx ] = inputFeatures[ featureIdx ].getAsDouble();
              }

              ++featIt;
            }
          }

          if( ! spatialFeatures.empty() )
          {
            std::map<std::string, gdma::fte::FeaturesSummary>::const_iterator
              featIt = spatialFeatures.begin();
            const std::map<std::string, gdma::fte::FeaturesSummary>::const_iterator
              featItEnd = spatialFeatures.end();
            std::size_t featureIdx = 0;
            std::size_t inputFeaturesSize = 0;
            std::size_t geomIdx = 0;

            while( featIt != featItEnd )
            {
              assert( geomID2GeomIdxMap.find( featIt->first ) != geomID2GeomIdxMap.end() );
              geomIdx = geomID2GeomIdxMap[ featIt->first ];

              const std::vector<gdma::fte::Attribute>& inputFeatures = featIt->second.getFeatures();
              inputFeaturesSize = inputFeatures.size();

              assert( geomIdx < extraPropertiesValues.size() );
              std::vector< double >& outputVec = extraPropertiesValues[ geomIdx ];

              for( featureIdx = 0 ; featureIdx < inputFeaturesSize ; ++featureIdx )
              {
                outputVec[ featureIdx + spectralFeaturesNumber ] = inputFeatures[
                  featureIdx ].getAsDouble();
              }

              ++featIt;
            }
          }

          if( ! landscapeFeatures.empty() )
          {
            std::map<std::string, gdma::fte::FeaturesSummary>::const_iterator
              featIt = landscapeFeatures.begin();
            const std::map<std::string, gdma::fte::FeaturesSummary>::const_iterator
              featItEnd = landscapeFeatures.end();
            std::size_t featureIdx = 0;
            std::size_t inputFeaturesSize = 0;
            std::size_t geomIdx = 0;
            const std::size_t outputVecOffset = spectralFeaturesNumber + spatialFeaturesNumber;

            while( featIt != featItEnd )
            {
              assert( geomID2GeomIdxMap.find( featIt->first ) != geomID2GeomIdxMap.end() );
              geomIdx = geomID2GeomIdxMap[ featIt->first ];

              const std::vector<gdma::fte::Attribute>& inputFeatures = featIt->second.getFeatures();
              inputFeaturesSize = inputFeatures.size();

              assert( geomIdx < extraPropertiesValues.size() );
              std::vector< double >& outputVec = extraPropertiesValues[ geomIdx ];

              for( featureIdx = 0 ; featureIdx < inputFeaturesSize ; ++featureIdx )
              {
                outputVec[ featureIdx + outputVecOffset ] = inputFeatures[ featureIdx ].getAsDouble();
              }

              ++featIt;
            }
          }
        }
      }

      // Clear unused data

      geomID2GeomPtrMap.clear();
      geomID2GeomIdxMap.clear();
      spectralFeatures.clear();
      spatialFeatures.clear();
      landscapeFeatures.clear();

      // save 2 disk

      {
        std::vector< std::string > labels( inputVerticeGeometriesPtr->size(),
          getName() );
        std::vector< GraphVerticeIDT > ids( inputVerticeGeometriesPtr->size(),
          getID() );

        if( ! save2ResultShapeFile( *inputVerticeGeometriesPtr, get1stPassOutputFileName(),
          &labels, &ids, &extraPropertiesNames, &extraPropertiesValues  ) )
        {
          errorMessage = "1St pass output file creaation error";
          return false;
        }
      }
    }

    return true;
  }

  //  -------------------------------------------------------------------------

  FeaturesVerticeFactory::FeaturesVerticeFactory()
  : GraphVerticeFactory( GDMA_SA_VERTICE_TYPE )
  {}

  FeaturesVerticeFactory::~FeaturesVerticeFactory()
  {}

  GraphVertice* FeaturesVerticeFactory::build()
  {
    return new FeaturesVertice();
  }

}  // end namespace sa
}  // end namespace gdma
