
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

#include <boost/filesystem.hpp>

#include <string>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <limits>

// GeoDMA Includes
#include "selectVertice.hpp"
#include "ioFunctions.hpp"
#include "geometryFunctions.hpp"
#include "../common/utils.hpp"

#define GDMA_SA_VERTICE_TYPE "SELECT"

namespace
{
  static gdma::sa::SelectVerticeFactory SelectVerticeFactoryInstance;
}

namespace gdma {
namespace sa {

  SelectVertice::SelectVertice()
  {
    reset();
  }

  SelectVertice::SelectVertice(const SelectVertice& other)
  {
    operator=( other );
  }

  SelectVertice::~SelectVertice()
  {
  }

  SelectVertice& SelectVertice::operator=( const SelectVertice& other )
  {
    {
      std::lock_guard<std::mutex> lock( getMutex() );
      std::lock_guard<std::mutex> lock2( other.getMutex() );

      reset();

      m_selectWhereClause = other.m_selectWhereClause;
    }

    GraphVertice::operator=( other );

    return *this;
  }

  std::string SelectVertice::getType() const
  {
    return std::string( GDMA_SA_VERTICE_TYPE );
  }

  void SelectVertice::getDescription( std::string& descriptionStr ) const
  {
    descriptionStr = "A vertice capable of executing a SQL query over the input vertices results";
  }

  bool SelectVertice::setParameter( const std::string& parameterName,
    const std::string& parameterValue )
  {
    std::lock_guard<std::mutex> lock( getMutex() );

    if( te::common::Convert2UCase( parameterName ) == "SELECT_WHERE_CLAUSE" )
    {
      m_selectWhereClause = parameterValue;
      return true;
    }

    return false;
  }

  void SelectVertice::getParameters( std::vector< std::pair< std::string, std::string > >& parameters ) const
  {
    parameters.clear();
  }

  bool SelectVertice::setResource( const std::string& contextURIAlias,
    const std::string& verticeURIAlias )
  {
    return false;
  }

  void SelectVertice::getInParsDescription( std::map< std::string, std::string >& inParsDesc ) const
  {
    inParsDesc[ "SELECT_WHERE_CLAUSE" ] = "Select WHERE clause."
      " Allowed string substitutions:"
      " _CLASSNAME_PROP_NAME_:The default class name property name used when with datasets"
      " , _CLASSID_PROP_NAME_:The default class ID property name used when with datasets"
      " , _UNKNOWN_CLASS_NAME_:The default class name for unknown classes"
      " , _INVALID_CLASS_NAME_:The default class name for invalid classes"
      " , _INVALID_CLASS_ID_:The default class ID (numeric value) for invalid classes"
      " , _UNKNOWN_CLASS_ID_:The default class ID (numeric value) for unknown classes"
      " , _VERTICE_NAME_:Vertice name"
      " , _VERTICE_TYPE_:Vertice type"
      " , _VERTICE_ID_:Vertice ID";
  }

  void SelectVertice::getInResDescription( std::map< std::string, std::string >& inResDesc ) const
  {
    inResDesc.clear();
  }

  GraphVertice::OutputType SelectVertice::getOutputType() const
  {
    return GraphVertice::VECTOR_URI_OUT_TYPE;
  }

  GraphVertice* SelectVertice::clone() const
  {
    SelectVertice* vertPtr = new SelectVertice( *this );
    return vertPtr;
  }

  bool SelectVertice::isConnectable( const GraphVertice& other,
    const std::string& alias ) const
  {
    if( other.getOutputType() == VECTOR_URI_OUT_TYPE )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  bool SelectVertice::isMultiClass() const
  {
    return false;
  }

  bool SelectVertice::is2ndPassCapable() const
  {
    return true;
  }

  bool SelectVertice::executeFirstPass( std::string& errorMessage )
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

      // Checking for a input vertice connection

      std::vector< std::pair< std::string, GraphVertice* > > connections;
      getInputConnections( connections );

      if( connections.empty() )
      {
        errorMessage = "No Input vertice found";
        return false;
      }

      // iterating over each input vertice 1st pass result
      // loading geometries

      std::unique_ptr< boost::ptr_vector< te::gm::Geometry > > unitedLoadedGeometriesPtr;

      for( std::size_t connectionsIdx = 0 ; connectionsIdx < connections.size() ;
        ++connectionsIdx )
      {
        GraphVertice const * const inputVerticePtr = connections[ connectionsIdx ].second;

        std::unique_ptr< te::da::DataSource > ogrDataSourcePtr( te::da::DataSourceFactory::make(
          "OGR", "file://" + inputVerticePtr->get1stPassOutputFileName() ) );
        if( ogrDataSourcePtr.get() == 0 )
        {
          errorMessage = "Input vertice 1st Pass result open error";
          return false;
        }

        ogrDataSourcePtr->open();

        if( ! ogrDataSourcePtr->isOpened() )
        {
          errorMessage = "Input vertice 1st Pass result open error";
          return false;
        }

        std::vector<std::string> dataSetNames = ogrDataSourcePtr->getDataSetNames();
        te::da::DataSourceTransactorPtr transactorPtr =
          ogrDataSourcePtr->getTransactor();
        std::size_t geomPropIdx = std::numeric_limits< std::size_t >::max();
        std::string geomPropName;
        std::string sqlQueryStr;

        for( unsigned int dataSetNamesIdx = 0 ; dataSetNamesIdx < dataSetNames.size() ;
          ++dataSetNamesIdx )
        {
          te::da::DataSetPtr datasetPtr = transactorPtr->getDataSet(
            dataSetNames[ dataSetNamesIdx ] );

          // Find properties indexes

          geomPropIdx = std::numeric_limits< std::size_t >::max();
          geomPropName.clear();

          {
            const std::size_t nProperties = datasetPtr->getNumProperties();

            for( std::size_t propIdx = 0 ; propIdx < nProperties ; ++propIdx )
            {
              if( datasetPtr->getPropertyDataType( propIdx ) == te::dt::GEOMETRY_TYPE )
              {
                geomPropIdx = propIdx;
                geomPropName = datasetPtr->getPropertyName( propIdx );
                break;
              }
            }

            if( geomPropIdx == std::numeric_limits< std::size_t >::max() )
            {
              errorMessage = "Geometry not found inside input vertice 1st Pass result";
              return false;
            }
          }

          // Query

          sqlQueryStr = "SELECT " + geomPropName + " FROM "
            + dataSetNames[ dataSetNamesIdx ] +
            ( m_selectWhereClause.empty() ? " ;" : ( " WHERE " + m_selectWhereClause ) );

          gdma::common::subStrReplace( "_CLASSNAME_PROP_NAME_", GDMA_SA_DEFAULT_CLASSNAME_PROP_NAME, sqlQueryStr );
          gdma::common::subStrReplace( "_CLASSID_PROP_NAME_", GDMA_SA_DEFAULT_CLASSID_PROP_NAME, sqlQueryStr );
          gdma::common::subStrReplace( "_UNKNOWN_CLASS_NAME_", GDMA_SA_DEFAULT_UNKNOWN_CLASS_NAME, sqlQueryStr );
          gdma::common::subStrReplace( "_INVALID_CLASS_NAME_", GDMA_SA_DEFAULT_INVALID_CLASS_NAME, sqlQueryStr );
          gdma::common::subStrReplace( "_INVALID_CLASS_ID_", boost::lexical_cast< std::string >(
            GDMA_SA_DEFAULT_INVALID_CLASS_ID ), sqlQueryStr );
          gdma::common::subStrReplace( "_UNKNOWN_CLASS_ID_", boost::lexical_cast< std::string >(
            GDMA_SA_DEFAULT_UNKNOWN_CLASS_ID ), sqlQueryStr );
          gdma::common::subStrReplace( "_VERTICE_NAME_", getName(), sqlQueryStr );
          gdma::common::subStrReplace( "_VERTICE_TYPE_", getType(), sqlQueryStr );
          gdma::common::subStrReplace( "_VERTICE_ID_", boost::lexical_cast< std::string >(
            getID() ), sqlQueryStr );

          datasetPtr.reset();
          try
          {
            datasetPtr.reset( transactorPtr->query( sqlQueryStr ).release() );
          }
          catch( const te::da::Exception& exc )
          {
            errorMessage = "Input vertice 1st Pass result query error: " + std::string( exc.what() );
            return false;
          }
          catch(...)
          {
            errorMessage = "Input vertice 1st Pass result query error";
            return false;
          }

          if( datasetPtr.get() == 0 )
          {
            errorMessage = "Input vertice 1st Pass result query error";
            return false;
          }

          if( ! datasetPtr->moveBeforeFirst() )
          {
            errorMessage = "Input vertice 1st Pass result query error";
            return false;
          }

          // Load geometries

          std::unique_ptr< boost::ptr_vector< te::gm::Geometry > > loadedGeomsPtr(
            new boost::ptr_vector< te::gm::Geometry >() );

          while( datasetPtr->moveNext() )
          {
            loadedGeomsPtr->push_back( datasetPtr->getGeometry(
              geomPropIdx ).release() );
          }

          // Union with the geometries already loaded

          if( ! loadedGeomsPtr->empty() )
          {
            if( unitedLoadedGeometriesPtr.get() )
            {
              std::unique_ptr< boost::ptr_vector< te::gm::Geometry > > unionResultPtr(
                new boost::ptr_vector< te::gm::Geometry >() );

              geometrySetsUnion( *loadedGeomsPtr, *unitedLoadedGeometriesPtr,
                *unionResultPtr );

              unitedLoadedGeometriesPtr.reset( unionResultPtr.release() );
            }
            else
            {
              unitedLoadedGeometriesPtr.reset( loadedGeomsPtr.release() );
            }
          }
        }
      }

      // Export to the output file

      std::vector< std::string > geomLabels( unitedLoadedGeometriesPtr->size(), getName() );
      std::vector< GraphVerticeIDT > geomsIDs( unitedLoadedGeometriesPtr->size(),
        getID() );

      if( ! save2ResultShapeFile( *unitedLoadedGeometriesPtr, get1stPassOutputFileName(),
        &geomLabels, &geomsIDs, 0, 0 ) )
      {
        errorMessage = "Output file creation error";
        return false;
      }
    }

    return true;
  }

  void SelectVertice::reset()
  {
    GraphVertice::reset();
  }

  //  -------------------------------------------------------------------------

  SelectVerticeFactory::SelectVerticeFactory()
  : GraphVerticeFactory( GDMA_SA_VERTICE_TYPE )
  {}

  SelectVerticeFactory::~SelectVerticeFactory()
  {}

  GraphVertice* SelectVerticeFactory::build()
  {
    return new SelectVertice();
  }

}  // end namespace sa
}  // end namespace gdma
