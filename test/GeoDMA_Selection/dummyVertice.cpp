
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

#include <boost/filesystem.hpp>
// #include <boost/lexical_cast.hpp>

#include <string>
#include <map>
#include <memory>
#include <utility>
#include <vector>

// GeoDMA Includes
#include "dummyVertice.hpp"
#include "ioFunctions.hpp"

#define GDMA_SA_VERTICE_TYPE "DUMMY"

namespace
{
  static gdma::sa::DummyVerticeFactory DummyVerticeFactoryInstance;
}

namespace gdma {
namespace sa {

  DummyVertice::DummyVertice()
  {
    reset();
  }

  DummyVertice::DummyVertice(const DummyVertice& other)
  {
    operator=( other );
  }

  DummyVertice::~DummyVertice()
  {
  }

  DummyVertice& DummyVertice::operator=( const DummyVertice& other )
  {
    {
      std::lock_guard<std::mutex> lock( getMutex() );
      std::lock_guard<std::mutex> lock2( other.getMutex() );

      reset();
    }

    GraphVertice::operator=( other );

    return *this;
  }

  std::string DummyVertice::getType() const
  {
    return std::string( GDMA_SA_VERTICE_TYPE );
  }

  void DummyVertice::getDescription( std::string& descriptionStr ) const
  {
    descriptionStr = "A dummy vertice that only is cabable of propagate results from the input connected vertex";
  }

  bool DummyVertice::setParameter( const std::string& parameterName,
    const std::string& parameterValue )
  {
    return false;
  }

  void DummyVertice::getParameters( std::vector< std::pair< std::string, std::string > >& parameters ) const
  {
    parameters.clear();
  }

  bool DummyVertice::setResource( const std::string& contextURIAlias,
    const std::string& verticeURIAlias )
  {
    return false;
  }

  void DummyVertice::getInParsDescription( std::map< std::string, std::string >& inParsDesc ) const
  {
    inParsDesc.clear();
  }

  void DummyVertice::getInResDescription( std::map< std::string, std::string >& inResDesc ) const
  {
    inResDesc.clear();
  }

  GraphVertice::OutputType DummyVertice::getOutputType() const
  {
    return GraphVertice::VECTOR_URI_OUT_TYPE;
  }

  GraphVertice* DummyVertice::clone() const
  {
    DummyVertice* vertPtr = new DummyVertice( *this );
    return vertPtr;
  }

  bool DummyVertice::isConnectable( const GraphVertice& other,
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

  bool DummyVertice::isMultiClass() const
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

  bool DummyVertice::is2ndPassCapable() const
  {
    return true;
  }

  bool DummyVertice::executeFirstPass( std::string& errorMessage )
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

      // Load the input vertice output result

      boost::ptr_vector< te::gm::Geometry > inputVerticeGeometries;

      if( ! connections.empty() )
      {
        if( ! loadResultShapeFileGeoms( connections[ 0 ].second->get1stPassOutputFileName(),
          inputVerticeGeometries ) )
        {
          errorMessage = "Error loading input vertice geometries";
          return false;
        }
      }

      // Export to the output file

      std::vector< std::string > geomLabels( inputVerticeGeometries.size(), getName() );
      std::vector< GraphVerticeIDT > geomsIDs( inputVerticeGeometries.size(),
        getID() );

      if( ! save2ResultShapeFile( inputVerticeGeometries, get1stPassOutputFileName(),
        &geomLabels, &geomsIDs, 0, 0 ) )
      {
        errorMessage = "Output file creation error";
        return false;
      }
    }

    return true;
  }

  void DummyVertice::reset()
  {
    GraphVertice::reset();
  }

  //  -------------------------------------------------------------------------

  DummyVerticeFactory::DummyVerticeFactory()
  : GraphVerticeFactory( GDMA_SA_VERTICE_TYPE )
  {}

  DummyVerticeFactory::~DummyVerticeFactory()
  {}

  GraphVertice* DummyVerticeFactory::build()
  {
    return new DummyVertice();
  }

}  // end namespace sa
}  // end namespace gdma
