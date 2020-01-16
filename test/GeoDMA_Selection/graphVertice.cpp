/*  Copyright (C) 2012 National Institute For Space Research (INPE) - Brazil.

This file is part of the GeoDMA - a Toolbox that integrates Data Mining Techniques with object-based
and multi-temporal analysis of satellite remotely sensed imagery.

GeoDMA is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

GeoDMA is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with GeoDMA. See COPYING. If not, write to
GeoDMA Team at <thales@dpi.inpe.br, raian@dpi.inpe.br, castejon@dpi.inpe.br>.
*/

#include <boost/filesystem.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <terralib/rp.h>

#include <string>
#include <utility>
#include <map>
#include <vector>
#include <set>
#include <memory>

// GeoDMA Includes
#include "graphVertice.hpp"
#include "ioFunctions.hpp"
#include "geometryFunctions.hpp"
#include "rasterFunctions.hpp"
#include "../datamanager/dataManager.hpp"

namespace gdma {
namespace sa {

  GraphVertice::GraphVertice()
  : m_generic1stPassTask( *this ), m_generic2ndPassTask( *this )
  {
    reset();
  }

  GraphVertice::GraphVertice(const GraphVertice& other)
  : m_generic1stPassTask( *this ), m_generic2ndPassTask( *this )
  {
    operator=( other );
  }

  GraphVertice::~GraphVertice()
  {
    disconnect();
  }

  GraphVertice& GraphVertice::operator=( const GraphVertice& other )
  {
    disconnect();

    std::lock_guard<std::mutex> lock( m_mutex );
    std::lock_guard<std::mutex> lock2( other.m_mutex );

    reset();

    m_enableCache = other.m_enableCache;
    m_enable2ndPassVerticesTasksExecution = other.m_enable2ndPassVerticesTasksExecution;
    m_graphPtr = other.m_graphPtr;
    m_name = other.m_name;
    m_ID = other.m_ID;
    m_outputDirectoryName = other.m_outputDirectoryName;
    m_inputConnections = other.m_inputConnections;
    m_outputConnections = other.m_outputConnections;
    m_parameters = other.m_parameters;

    return *this;
  }

  void GraphVertice::setName( const std::string& newName )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_name = newName;
  }

  std::string GraphVertice::getName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_name;
  }

  void GraphVertice::setID( const GraphVerticeIDT& newID )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_ID = newID;
  }

  GraphVerticeIDT GraphVertice::getID() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_ID;
  }

  bool GraphVertice::connectInputVertice( GraphVertice& other, const std::string& alias )
  {
    if( isConnectable( other, alias ) )
    {
      std::lock_guard<std::mutex> lock( m_mutex );

      std::map< std::string, GraphVertice* >::iterator conIt = m_inputConnections.find( alias );

      if( conIt == m_inputConnections.end() )
      {
        m_inputConnections[ alias ] = &other;

        std::lock_guard<std::mutex> lock2( other.m_mutex );

        other.m_outputConnections.insert( this );
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

    return true;
  }

  void GraphVertice::getInputConnections( std::vector< std::pair< std::string, GraphVertice* > >& connections ) const
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    connections.clear();

    std::map< std::string, GraphVertice* >::const_iterator conIt = m_inputConnections.begin();
    const std::map< std::string, GraphVertice* >::const_iterator conItEnd = m_inputConnections.end();

    while( conIt != conItEnd )
    {
      connections.push_back( std::pair< std::string, GraphVertice* >(
        conIt->first, conIt->second ) );

      ++conIt;
    }
  }

  void GraphVertice::getOutputConnections( std::vector< GraphVertice* >& connections ) const
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    connections.clear();

    std::set< GraphVertice* >::const_iterator conIt = m_outputConnections.begin();
    const std::set< GraphVertice* >::const_iterator conItEnd = m_outputConnections.end();

    while( conIt != conItEnd )
    {
      connections.push_back( *conIt );

      ++conIt;
    }
  }

  void GraphVertice::get1stPassTaskDeps( std::vector< VerticeTask* >& firstPassTaskDeps ) const
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    firstPassTaskDeps.clear();

    std::map< std::string, GraphVertice* >::const_iterator conIt = m_inputConnections.begin();
    const std::map< std::string, GraphVertice* >::const_iterator conItEnd = m_inputConnections.end();

    while( conIt != conItEnd )
    {
      firstPassTaskDeps.push_back( &conIt->second->get1stPassTask() );

      ++conIt;
    }
  }

  void GraphVertice::get2stPassTaskDeps( std::vector< VerticeTask* >& firstPassTaskDeps ) const
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    firstPassTaskDeps.clear();

    std::set< GraphVertice* >::const_iterator conIt = m_outputConnections.begin();
    const std::set< GraphVertice* >::const_iterator conItEnd = m_outputConnections.end();

    while( conIt != conItEnd )
    {
      firstPassTaskDeps.push_back( &( (*conIt)->get2ndPassTask() ) );

      ++conIt;
    }
  }

  bool GraphVertice::isInputSatisfied() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    std::map< std::string, GraphVertice* >::const_iterator conIt = m_inputConnections.begin();
    const std::map< std::string, GraphVertice* >::const_iterator conItEnd = m_inputConnections.end();

    while( conIt != conItEnd )
    {
      if( conIt->second->get1stPassOutputFileName().empty() )
      {
        return false;
      }

      ++conIt;
    }

    return true;
  }

  void GraphVertice::disconnect()
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    // Disconnecting this vertice from all input vertices

    std::map< std::string, GraphVertice* >::iterator inputConnectionsIt =
      m_inputConnections.begin();
    std::map< std::string, GraphVertice* >::iterator inputConnectionsItEnd =
      m_inputConnections.end();

    while( inputConnectionsIt != inputConnectionsItEnd )
    {
      std::lock_guard<std::mutex> lock2( inputConnectionsIt->second->m_mutex );

      inputConnectionsIt->second->m_outputConnections.erase( this );

      ++inputConnectionsIt;
    }

    // Disconnecting this vertice from all output vertices

    std::set< GraphVertice* >::iterator outputConnectionsIt =
      m_outputConnections.begin();
    const std::set< GraphVertice* >::iterator outputConnectionsItEnd =
      m_outputConnections.end();
    std::map< std::string, GraphVertice* >::iterator inputConnectionsItAux;

    while( outputConnectionsIt != outputConnectionsItEnd )
    {
      std::lock_guard<std::mutex> lock2( (*outputConnectionsIt)->m_mutex );

      inputConnectionsIt = (*outputConnectionsIt)->m_inputConnections.begin();
      inputConnectionsItEnd = (*outputConnectionsIt)->m_inputConnections.end();

      while( inputConnectionsIt != inputConnectionsItEnd )
      {
        if( inputConnectionsIt->second == this )
        {
          inputConnectionsItAux = inputConnectionsIt;
          ++inputConnectionsIt;
          (*outputConnectionsIt)->m_inputConnections.erase( inputConnectionsItAux );
        }
        else
        {
          ++inputConnectionsIt;
        }
      }

      ++outputConnectionsIt;
    }

    m_inputConnections.clear();
    m_outputConnections.clear();
  }

  const std::string GraphVertice::get1stPassOutputFileName() const
  {
    boost::filesystem::path outFilePath( getOutputDirectoryName() );

    if( getOutputType() == RASTER_URI_OUT_TYPE )
    {
      outFilePath /= "firstPassOutput.tif";
    }
    else if( getOutputType() == VECTOR_URI_OUT_TYPE )
    {
      outFilePath /= "firstPassOutput.shp";
    }
    else
    {
      throw std::runtime_error( "Invalid output type" );
    }

    return outFilePath.string();
  }

  const std::string GraphVertice::get2ndPassOutputFileName() const
  {
    boost::filesystem::path outFilePath( getOutputDirectoryName() );

    if( getOutputType() == RASTER_URI_OUT_TYPE )
    {
      outFilePath /= "secondPassOutput.tif";
    }
    else if( getOutputType() == VECTOR_URI_OUT_TYPE )
    {
      outFilePath /= "secondPassOutput.shp";
    }
    else
    {
      throw std::runtime_error( "Invalid output type" );
    }

    return outFilePath.string();
  }

  bool GraphVertice::setOutputDirectoryName( const std::string& outDirName )
  {
    boost::filesystem::path completePath;
    try
    {
      completePath = boost::filesystem::system_complete( outDirName );
    }
    catch(...)
    {
      return false;
    }

    std::lock_guard<std::mutex> lock( m_mutex );
    m_outputDirectoryName = completePath.string();

    return true;
  }

  const std::string GraphVertice::getOutputDirectoryName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_outputDirectoryName;
  }

  void GraphVertice::enableCache( const bool enabled )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_enableCache = enabled;
  }

  bool GraphVertice::isCacheEnabled() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_enableCache;
  }

  void GraphVertice::setGraph( const Graph& graph )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_graphPtr = &graph;
  }

  Graph const* GraphVertice::getGraph() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_graphPtr;
  }

  void GraphVertice::reset()
  {
    m_enableCache = false;
    m_enable2ndPassVerticesTasksExecution = false;
    m_graphPtr = 0;
    m_ID = GDMA_SA_DEFAULT_INVALID_CLASS_ID;
    m_name.clear();
    m_outputDirectoryName.clear();
    m_inputConnections.clear();
    m_outputConnections.clear();
    m_parameters.clear();
  }

  VerticeTask& GraphVertice::get1stPassTask()
  {
    return m_generic1stPassTask;
  }

  VerticeTask& GraphVertice::get2ndPassTask()
  {
    return m_generic2ndPassTask;
  }

  std::mutex& GraphVertice::getMutex() const
  {
    return m_mutex;
  }

  bool GraphVertice::enable2ndPassTasks( const bool enable )
  {
    if( is2ndPassCapable() )
    {
      std::lock_guard<std::mutex> lock( m_mutex );
      m_enable2ndPassVerticesTasksExecution = enable;
      return true;
    }
    else
    {
      return (!enable);
    }
  }

  bool GraphVertice::is2ndPassTasksEnabled() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_enable2ndPassVerticesTasksExecution;
  }

  bool GraphVertice::executeSecondPass( std::string& errorMessage )
  {
    errorMessage.clear();

    if(
        m_enable2ndPassVerticesTasksExecution
        &&
        (
          ( ! m_enableCache )
          ||
          ( ! boost::filesystem::exists( get2ndPassOutputFileName() ) )
        )
      )
    {
      // Clear old output files

      {
        const boost::filesystem::path outFilePath( get2ndPassOutputFileName() );
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

      // find output vertices info

      std::vector< GraphVertice* > outConnections;
      getOutputConnections( outConnections );

      const std::size_t outConnectionsSize = outConnections.size();
      std::vector< std::string >
        outConnections2ndPassOutputFileNames;
      std::vector< GraphVerticeIDT >
        outConnections2ndPassVertIDs;
      std::vector< std::string >
        outConnections2ndPassVertNames;

      {
        for( std::size_t outConnectionsIdx = 0 ; outConnectionsIdx <
          outConnections.size() ; ++outConnectionsIdx )
        {
          if( outConnections[ outConnectionsIdx ]->get2ndPassTask().getVerticeTaskExecResult().getBasicResult()
            != gdma::tp::TaskExecutionResult::SUCCESS )
          {
            errorMessage = "Invalid 2st Pass result: vertice " +
              outConnections[ outConnectionsIdx ]->getName();
            return false;
          }

          outConnections2ndPassOutputFileNames.push_back(
            outConnections[ outConnectionsIdx ]->get2ndPassOutputFileName() );
          outConnections2ndPassVertIDs.push_back(
            outConnections[ outConnectionsIdx ]->getID() );
          outConnections2ndPassVertNames.push_back(
            outConnections[ outConnectionsIdx ]->getName() );
        }
      }

      if( outConnections2ndPassOutputFileNames.empty() )
      {  //  this is an end vertice with no children
        if( getOutputType() == GraphVertice::RASTER_URI_OUT_TYPE )
        {
          if( ! diskRasterFileCopy( get1stPassOutputFileName(),
            get2ndPassOutputFileName() ) )
          {
            errorMessage = "2nd Pass raster copy error";
            return false;
          }
        }
        else if( getOutputType() == GraphVertice::VECTOR_URI_OUT_TYPE )
        {
          if( ! gdma::dm::diskVectorFileCopy( get1stPassOutputFileName(),
            get2ndPassOutputFileName() ) )
          {
            errorMessage = "2nd Pass vector copy error";
            return false;
          }
        }
        else
        {
          throw std::runtime_error( "Invalid output type" );
        }
      }
      else
      {  //  this is an internal vertice
        // loading 2nd passa vectors from other output vertices

        std::vector< boost::shared_ptr< boost::ptr_vector< te::gm::Geometry > > > outConnections2ndPassOutputGeometries;

        {
          for( std::size_t outConnectionsIdx = 0 ; outConnectionsIdx < outConnectionsSize ;
            ++outConnectionsIdx )
          {
            // load geometries

            boost::shared_ptr< boost::ptr_vector< te::gm::Geometry > > loadedGeometriesPtr(
              new boost::ptr_vector< te::gm::Geometry >() );

            if( outConnections[ outConnectionsIdx ]->getOutputType() ==
              VECTOR_URI_OUT_TYPE )
            {
              if( ! loadResultShapeFile( outConnections[ outConnectionsIdx ]->get2ndPassOutputFileName(),
                0, loadedGeometriesPtr.get(), 0, 0, 0 ) )
              {
                errorMessage = "2nd Pass file load error";
                return false;
              }
            }
            else if( outConnections[ outConnectionsIdx ]->getOutputType() ==
              VECTOR_URI_OUT_TYPE )
            {
              if( ! vectorizeResultRaster( outConnections[ outConnectionsIdx ]->get2ndPassOutputFileName(),
                *loadedGeometriesPtr, 0 ) )
              {
                errorMessage = "2nd Pass file vectorize error";
                return false;
              }
            }
            else
            {
              throw std::runtime_error( "Invalid output type" );
            }

            outConnections2ndPassOutputGeometries.push_back( loadedGeometriesPtr );
          }
        }

        // Spatial resolve

        std::vector< boost::shared_ptr< boost::ptr_vector< te::gm::Geometry > > > outNonIntersectedVectors;
        boost::shared_ptr< boost::ptr_vector< te::gm::Geometry > >outIntersectedVectors(
          new boost::ptr_vector< te::gm::Geometry >() );

        if( ! geometriesSetsSpatialResolve( outConnections2ndPassOutputGeometries, outNonIntersectedVectors,
          *outIntersectedVectors ) )
        {
          errorMessage = "2nd Pass vector spatial resolve error";
          return false;
        }

        // Clean unused data

        outConnections2ndPassOutputGeometries.clear();

        // Generating this vertice 2nd pass output

        if( getOutputType() == GraphVertice::RASTER_URI_OUT_TYPE )
        {
          // Guessint 1st pass output raster number of rows/cols

          unsigned int outXRes = 0;
          unsigned int outYRes = 0;
          int outSRID = 0;

          {
            std::unique_ptr< te::rst::Raster > rasterPtr;
            if( ! openDiskRaster( get1stPassOutputFileName(),
              rasterPtr ) )
            {
              errorMessage = "1nd Pass raster open error";
              return false;
            }

            outXRes = rasterPtr->getGrid()->getResolutionX();
            outYRes = rasterPtr->getGrid()->getResolutionY();
            outSRID = rasterPtr->getSRID();
          }

          // Grouping geometries

          std::vector< boost::shared_ptr< boost::ptr_vector< te::gm::Geometry > > > groupedGeometries;
          groupedGeometries = outNonIntersectedVectors;
          groupedGeometries.push_back( outIntersectedVectors );

          std::vector< GraphVerticeIDT > geomGroupIDs;
          geomGroupIDs = outConnections2ndPassVertIDs;
          geomGroupIDs.push_back( GDMA_SA_DEFAULT_UNKNOWN_CLASS_ID );

          // Creating a raster

          std::unique_ptr< te::rst::Raster > outRasterizedResultPtr;

          if( ! rasterizeVectors( groupedGeometries, geomGroupIDs, outXRes,
            outYRes, outSRID, outRasterizedResultPtr ) )
          {
            errorMessage = "2nd Pass rasterization error";
            return false;
          }

          // Create the output raster disk file

          if( ! te::rp::Copy2DiskRaster( *outRasterizedResultPtr,
            get2ndPassOutputFileName() ) )
          {
            errorMessage = "2nd Pass raster creation error";
            return false;
          }
        }
        else if( getOutputType() == GraphVertice::VECTOR_URI_OUT_TYPE )
        {
          // Preparing output

          boost::ptr_vector< te::gm::Geometry > outGeometries;
          std::vector< std::string > outGeomLabels;
          std::vector< GraphVerticeIDT > outgeomsIDs;

          {
            const std::size_t outNonIntersectedVectorsSize = outNonIntersectedVectors.size();

            for( std::size_t outNonIntersectedVectorsIdx = 0 ; outNonIntersectedVectorsIdx <
              outNonIntersectedVectorsSize ; ++outNonIntersectedVectorsIdx )
            {
              boost::ptr_vector< te::gm::Geometry >& gVec =
                *outNonIntersectedVectors[ outNonIntersectedVectorsIdx ];

              while( !gVec.empty() )
              {
                outGeometries.push_back( gVec.pop_back().release() );
                outGeomLabels.push_back( outConnections2ndPassVertNames[
                  outNonIntersectedVectorsIdx ] );
                outgeomsIDs.push_back( outConnections2ndPassVertIDs[
                  outNonIntersectedVectorsIdx ] );
              }
            }

            while( ! outIntersectedVectors->empty() )
            {
              outGeometries.push_back( outIntersectedVectors->pop_back().release() );
              outGeomLabels.push_back( GDMA_SA_DEFAULT_UNKNOWN_CLASS_NAME );
              outgeomsIDs.push_back( GDMA_SA_DEFAULT_UNKNOWN_CLASS_ID );
            }
          }

          // Exporting to shape file

          if( ! save2ResultShapeFile( outGeometries, get2ndPassOutputFileName(),
            &outGeomLabels, &outgeomsIDs, 0, 0 ) )
          {
            errorMessage = "2nd Pass vector file creation error";
            return false;
          }
        }
        else
        {
          throw std::runtime_error( "Invalid output type" );
        }
      }
    }

    return true;
  }

  //  -------------------------------------------------------------------------

  GraphVertice::Generic1stPassVerticeTask::Generic1stPassVerticeTask( GraphVertice& vertice )
  : m_vertice( vertice )
  {
  }

  GraphVertice::Generic1stPassVerticeTask::~Generic1stPassVerticeTask()
  {
  }

  void GraphVertice::Generic1stPassVerticeTask::execute()
  {
    bool returnValue = false;
    std::string errorMessageStr;

    try
    {
      returnValue = m_vertice.executeFirstPass( errorMessageStr );
    }
    catch(...)
    {
      returnValue = false;
    }

    std::lock_guard<std::mutex> lock( m_vertice.getMutex() );

    std::unique_ptr< ExecutionResult > execResultPtr(
      static_cast< ExecutionResult* >( getVerticeTaskExecResult().clone() ) );
    execResultPtr->setErrorMessage( errorMessageStr );

    if( returnValue )
    {
      execResultPtr->setBasicResult( ExecutionResult::SUCCESS );
    }
    else
    {
      execResultPtr->setBasicResult( ExecutionResult::FAILED );
    }

    setExecResult( *execResultPtr );
  }

  //  -------------------------------------------------------------------------

  GraphVertice::Generic2ndPassVerticeTask::Generic2ndPassVerticeTask( GraphVertice& vertice )
  : m_vertice( vertice )
  {
  }

  GraphVertice::Generic2ndPassVerticeTask::~Generic2ndPassVerticeTask()
  {
  }

  void GraphVertice::Generic2ndPassVerticeTask::execute()
  {
    bool returnValue = false;
    std::string errorMessageStr;

    try
    {
      returnValue = m_vertice.executeSecondPass( errorMessageStr );
    }
    catch(...)
    {
      returnValue = false;
    }

    std::lock_guard<std::mutex> lock( m_vertice.getMutex() );

    std::unique_ptr< ExecutionResult > execResultPtr(
      static_cast< ExecutionResult* >( getVerticeTaskExecResult().clone() ) );
    execResultPtr->setErrorMessage( errorMessageStr );

    if( returnValue )
    {
      execResultPtr->setBasicResult( ExecutionResult::SUCCESS );
    }
    else
    {
      execResultPtr->setBasicResult( ExecutionResult::FAILED );
    }

    setExecResult( *execResultPtr );
  }

}  // end namespace sa
}  // end namespace gdma
