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

#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>

// GeoDMA Includes
#include "localExecuter.hpp"

#define GDMA_SA_EXECUTER_TYPE "LOCAL"

namespace
{
  static gdma::sa::LocalExecuterFactory localExecuterFactoryInstance;
}

namespace gdma {
namespace sa {

  LocalExecuter::LocalExecuter()
  {
    reset();
  }

  LocalExecuter::LocalExecuter(const LocalExecuter&) {}

  LocalExecuter::~LocalExecuter() {}

  bool LocalExecuter::initialize( Project& project, const ExecMode execMode )
  {
    stopExecution();

    std::unique_lock<std::mutex> locker(m_mutex);

    reset();

    switch( execMode )
    {
      case SINGLE :
      {
        m_executer.setExecutionParams( gdma::tp::GraphExecuter::SINGLE, 0, false );
        break;
      }
      case SEQUENTIAL :
      {
        m_executer.setExecutionParams( gdma::tp::GraphExecuter::SEQUENTIAL, 0, false );
        break;
      }
      case CONCURRENT :
      {
        m_executer.setExecutionParams( gdma::tp::GraphExecuter::CONCURRENT, 0, false );
        break;
      }
      default :
        throw std::runtime_error( "Invalid execution mode" );
    }

    m_executer.setCallBack( *this );
    m_execMode = execMode;
    m_projectPtr = &project;

    // Locating all vertices pointers

    std::vector< GraphVertice* > verticesPtrs;

    {
      std::vector< std::string > analysisNames;
      project.getAnalysisNames( analysisNames );

      for( std::size_t analysisNamesIdx = 0 ; analysisNamesIdx < analysisNames.size() ;
        ++analysisNamesIdx )
      {
        const std::string& analysisName = analysisNames[ analysisNamesIdx ];

        Analysis* anaPtr = project.getAnalysis( analysisName );
        if( anaPtr == 0 )
        {
          throw std::runtime_error( "Invalid analysis" );
        }

        Graph& graph = anaPtr->getGraph();

        std::vector< std::string > verticesNames;
        graph.getVerticesNames( verticesNames );

        for( std::size_t verticesNamesIdx = 0 ; verticesNamesIdx < verticesNames.size() ;
          ++verticesNamesIdx )
        {
          const std::string& verticeName = verticesNames[ verticesNamesIdx ];

          GraphVertice* vertPtr = graph.getVertice( verticeName );
          if( vertPtr == 0 )
          {
            throw std::runtime_error( "Invalid vertice" );
          }

          verticesPtrs.push_back( vertPtr );
        }
      }
    }

    // adding 1st pass tasks into the executer

    std::map< VerticeTask*, gdma::tp::TaskIdType > firstPassMap;

    {
      unsigned int currentIterationAddedVerticesCount = 0;
      std::vector< GraphVertice* >::iterator verticesPtrsIt;
      const std::vector< GraphVertice* >::const_iterator verticesPtrsItEnd = verticesPtrs.end();
      gdma::tp::TaskIdType taskId = 0;
      std::vector< VerticeTask* > dependencies;
      bool dependenciesAreOK = false;
      std::size_t dependenciesIdx;
      std::vector< gdma::tp::TaskIdType > dependenciesTaskIds;
      std::map< VerticeTask*, gdma::tp::TaskIdType >::iterator firstPassMapIt;

      do
      {
        currentIterationAddedVerticesCount = 0;

        verticesPtrsIt = verticesPtrs.begin();

        while( verticesPtrsIt != verticesPtrsItEnd )
        {
          GraphVertice* vertPtr = *verticesPtrsIt;

          if( firstPassMap.find( &(vertPtr->get1stPassTask()) ) == firstPassMap.end() )
          {
            // checking if all input connected vertices 1st pass tasks were already added
            // Extracting dependencies task IDs

            dependenciesAreOK = true;
            dependenciesTaskIds.clear();
            vertPtr->get1stPassTaskDeps( dependencies );

            for( dependenciesIdx = 0 ; dependenciesIdx < dependencies.size() ;
              ++dependenciesIdx )
            {
              firstPassMapIt = firstPassMap.find( dependencies[ dependenciesIdx ] );

              if( firstPassMapIt == firstPassMap.end() )
              {
                dependenciesAreOK = false;
                break;
              }
              else
              {
                dependenciesTaskIds.push_back( firstPassMapIt->second );
              }
            }

            // adding task into the executer

            if( dependenciesAreOK )
            {
              if( ! m_executer.addTask( &(vertPtr->get1stPassTask()), dependenciesTaskIds, &taskId ) )
              {
                throw std::runtime_error( "Executer task add error" );
              }

              m_taskID2VerticeMap[ taskId ] = vertPtr;
              firstPassMap[ &(vertPtr->get1stPassTask()) ] = taskId;

              ++currentIterationAddedVerticesCount;
            }
          }

          ++verticesPtrsIt;
        }
      } while( currentIterationAddedVerticesCount );

      if( firstPassMap.size() != verticesPtrs.size() )
      {
        reset();
        return false;
      }
    }

    // adding 2nd pass tasks into the executer

    {
      std::map< VerticeTask*, gdma::tp::TaskIdType > secondPassMap;

      std::vector< GraphVertice* >::const_iterator verticesPtrsIt = verticesPtrs.begin();;
      const std::vector< GraphVertice* >::const_iterator verticesPtrsItEnd = verticesPtrs.end();
      std::vector< VerticeTask* > dependencies;
      std::size_t dependenciesIdx = 0;
      std::vector< gdma::tp::TaskIdType > outDependenciesTaskIds;
      std::map< VerticeTask*, gdma::tp::TaskIdType >::const_iterator
        firstPassMapIt;
      std::map< VerticeTask*, gdma::tp::TaskIdType >::const_iterator
        secondPassMapIt;
      GraphVertice* vertPtr = 0;
      gdma::tp::TaskIdType vert2ndPassTaskId = 0;
      unsigned int currentIterationAddedVerticesCount = 0;
      bool dependenciesAreOK = false;

      do
      {
        currentIterationAddedVerticesCount = 0;

        verticesPtrsIt = verticesPtrs.begin();

        while( verticesPtrsIt != verticesPtrsItEnd )
        {
          vertPtr = *verticesPtrsIt;

          secondPassMapIt =
            secondPassMap.find( &(vertPtr->get2ndPassTask()) );

          if(
              vertPtr->is2ndPassTasksEnabled()
              &&
              ( secondPassMapIt == secondPassMap.end() )
            )
          {
            dependenciesAreOK = true;
            outDependenciesTaskIds.clear();

            dependencies.clear();
            vertPtr->get2stPassTaskDeps( dependencies );

            if( dependencies.empty() )
            {
              //  this is and end-point vertice
              //  or it does not have dependencies

              firstPassMapIt = firstPassMap.find( &(vertPtr->get1stPassTask()) );
              if( firstPassMapIt == firstPassMap.end() )
              {
                throw std::runtime_error( "Vertice 1st Pass task ID not found" );
              }

              outDependenciesTaskIds.push_back( firstPassMapIt->second );
            }
            else
            {
              //  this is an intermidiate vertice
              // checking if all output connected vertices tasks were already added

              for( dependenciesIdx = 0; dependenciesIdx < dependencies.size();
                ++dependenciesIdx )
              {
                secondPassMapIt =
                  secondPassMap.find( dependencies[ dependenciesIdx ] );

                if( secondPassMapIt == secondPassMap.end() )
                {
                  dependenciesAreOK = false;
                  break;
                }
                else
                {
                  outDependenciesTaskIds.push_back( secondPassMapIt->second );
                }
              }
            }

            if( dependenciesAreOK )
            {
              m_executer.addTask( &(vertPtr->get2ndPassTask()), outDependenciesTaskIds, &vert2ndPassTaskId );

              m_taskID2VerticeMap[ vert2ndPassTaskId ] = vertPtr;

              secondPassMap[ &(vertPtr->get2ndPassTask()) ] = vert2ndPassTaskId;

              ++currentIterationAddedVerticesCount;
            }
          }

          ++verticesPtrsIt;
        }
      } while( currentIterationAddedVerticesCount );
    }

    m_isInitialized = true;

    return true;
  }

  bool LocalExecuter::finalize()
  {
    std::unique_lock<std::mutex> lock( m_mutex );

    if( m_executer.getExecuterStatus() == gdma::tp::GraphExecuter::ACTIVE_EXECUTER )
    {
      return false;
    }

    reset();

    return true;
  }

  void LocalExecuter::startExecution()
  {
    m_executer.startExecution();
  }

  void LocalExecuter::stopExecution()
  {
    m_executer.stopExecution();
  }

  void LocalExecuter::waitExecution()
  {
    m_executer.waitExecution();
  }

  Executer::ExecuterStatus LocalExecuter::getExecuterStatus() const
  {
    return ( m_executer.getExecuterStatus() == gdma::tp::GraphExecuter::INACTIVE_EXECUTER )
      ? Executer::INACTIVE_EXECUTER : Executer::ACTIVE_EXECUTER;
  }

  bool LocalExecuter::isInitialized() const
  {
    std::unique_lock<std::mutex> lock( m_mutex );
    return m_isInitialized;
  }

  unsigned int LocalExecuter::getTasksCount() const
  {
    return m_executer.getTasksCount();
  }

  void LocalExecuter::reset()
  {
    Executer::reset();

    m_isInitialized = false;

    m_projectPtr = 0;

    m_execMode = Executer::SINGLE;

    m_executer.stopExecution();
    m_executer.setCallBack( *this );

    m_taskID2VerticeMap.clear();
  }

  void LocalExecuter::taskStarted(const gdma::tp::TaskIdType& taskId)
  {
    std::unique_lock<std::mutex> lock( m_mutex );

    gdma::sa::ExecuterCallBack* callBackPtr = getCallBack();

    if( callBackPtr )
    {
      std::map< gdma::tp::TaskIdType, GraphVertice* >::const_iterator taskID2VerticeMapIt =
        m_taskID2VerticeMap.find( taskId );

      if( taskID2VerticeMapIt == m_taskID2VerticeMap.end() )
      {
        throw std::runtime_error( "Invalid task ID" );
      }

      lock.unlock();

      callBackPtr->verticeTaskStarted(
        taskID2VerticeMapIt->second->getGraph()->getAnalysis()->getName(),
        taskID2VerticeMapIt->second->getGraph()->getName(),
        taskID2VerticeMapIt->second->getName() );
    }
  }

  void LocalExecuter::taskExecuted(const gdma::tp::TaskIdType& taskId,
    const gdma::tp::TaskExecutionResult::BasicResult& basicResut )
  {
    std::unique_lock<std::mutex> lock( m_mutex );

    gdma::sa::ExecuterCallBack* callBackPtr = getCallBack();

    if( callBackPtr )
    {
      std::map< gdma::tp::TaskIdType, GraphVertice* >::const_iterator taskID2VerticeMapIt =
        m_taskID2VerticeMap.find( taskId );

      if( taskID2VerticeMapIt == m_taskID2VerticeMap.end() )
      {
        throw std::runtime_error( "Invalid task ID" );
      }

      lock.unlock();

      callBackPtr->verticeTaskExecuted(
        taskID2VerticeMapIt->second->getGraph()->getAnalysis()->getName(),
        taskID2VerticeMapIt->second->getGraph()->getName(),
        taskID2VerticeMapIt->second->getName(),
        basicResut );
    }
  }

  LocalExecuterFactory::LocalExecuterFactory()
  : ExecuterFactory( GDMA_SA_EXECUTER_TYPE )
  {}

  LocalExecuterFactory::~LocalExecuterFactory()
  {}

  Executer* LocalExecuterFactory::build()
  {
    return new LocalExecuter();
  }

}  // end namespace sa
}  // end namespace gdma

