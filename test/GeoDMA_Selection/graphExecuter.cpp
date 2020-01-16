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

#include <vector>
#include <list>
#include <limits>

// GeoDMA Includes
#include "graphExecuter.hpp"
#include "taskExecutionResult.hpp"

namespace gdma {
namespace tp {

  GraphExecuter::TasksContainerElementType::TasksContainerElementType()
  :
    m_taskPtr(0),
    m_takeTasksOwnership( false ),
    m_status(INACTIVE),
    m_executerTaskId(0)
  {
  }

  GraphExecuter::TasksContainerElementType::TasksContainerElementType(const TasksContainerElementType& other)
  :
    m_taskPtr(0),
    m_takeTasksOwnership( false ),
    m_status(INACTIVE),
    m_executerTaskId(0)
  {
    operator=( other );
  }

  GraphExecuter::TasksContainerElementType::~TasksContainerElementType()
  {
    if( ( m_taskPtr != 0 ) && m_takeTasksOwnership )
    {
      delete m_taskPtr;
    }
  }

  GraphExecuter::TasksContainerElementType& GraphExecuter::TasksContainerElementType::operator=(
    const TasksContainerElementType& other)
  {
    if( ( m_taskPtr != 0 ) && m_takeTasksOwnership )
    {
      delete m_taskPtr;
    }

    m_taskPtr = other.m_taskPtr;
    other.m_taskPtr = 0;

    m_takeTasksOwnership = other.m_takeTasksOwnership;
    m_status = other.m_status;
    m_executerTaskId = other.m_executerTaskId;
    m_dependencies = other.m_dependencies;

    return *this;
  }

  // ---------------------------------------------------------------------------------------------------

  GraphExecuter::GraphExecuter()
  {
    reset();
  }

  GraphExecuter::GraphExecuter(const GraphExecuter&)
  {
    reset();
  }

  GraphExecuter::~GraphExecuter()
  {
    if( m_executer.getExecuterStatus() != Executer::INACTIVE_EXECUTER )
    {
      throw std::runtime_error( "Invalid executer status at destruction time" );
    }
  }

  bool GraphExecuter::setExecutionParams( const ExecMode newExecMode,
    const unsigned int threadsNumber, const bool takeTasksOwnership )
  {
    stopExecution();

    std::unique_lock<std::mutex> locker(m_mutex);

    reset();

    m_takeTasksOwnership = takeTasksOwnership;

    bool returnValue = false;

    switch( newExecMode )
    {
      case SINGLE :
      {
        m_executionMode = SINGLE;
        returnValue = m_executer.setExecutionParams( Executer::SINGLE, threadsNumber,
          false );
        break;
      }
      case SEQUENTIAL :
      {
        m_executionMode = SEQUENTIAL;
        returnValue = m_executer.setExecutionParams( Executer::SEQUENTIAL, threadsNumber,
          false );
        break;
      }
      case CONCURRENT :
      {
        m_executionMode = CONCURRENT;
        returnValue = m_executer.setExecutionParams( Executer::CONCURRENT, threadsNumber,
          false );
        break;
      }
      default :
        throw std::runtime_error( "Invalid execution mode" );
    }

    m_executer.setCallBack( *this );

    return returnValue;
  }

  GraphExecuter::ExecMode GraphExecuter::getExecutionMode() const
  {
    std::unique_lock<std::mutex> locker(m_mutex);
    return m_executionMode;
  }

  GraphExecuter::ExecuterStatus GraphExecuter::getExecuterStatus() const
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    switch( m_executer.getExecuterStatus() )
    {
      case Executer::INACTIVE_EXECUTER :
      {
        return INACTIVE_EXECUTER;
        break;
      }
      case Executer::ACTIVE_EXECUTER :
      {
        return ACTIVE_EXECUTER;
        break;
      }
      default :
      {
        throw std::runtime_error( "Invalid executer status" );
        return INACTIVE_EXECUTER;
      }
    }
  }

  void GraphExecuter::startExecution()
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    // Add all inactive tasks with all dependencies solved into the executer
    addITasks2Exec();

    locker.unlock();

    m_executer.startExecution();
  }

  void GraphExecuter::stopExecution()
  {
    m_executer.stopExecution();
  }

  void GraphExecuter::waitExecution()
  {
    m_executer.waitExecution();
  }

  bool GraphExecuter::addTask(Task* taskPtr, const std::vector< TaskIdType >& dependencies,
    TaskIdType* taskIdTypePtr)
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    const std::size_t dependenciesSize = dependencies.size();
    TasksContainerType::iterator tasksContainerIt;
    TasksContainerType::iterator tasksContainerItEnd = m_tasksContainer.end();

    // Checking  dependencies

    for( std::size_t dependenciesIdx = 0 ; dependenciesIdx < dependenciesSize ;
      ++dependenciesIdx )
    {
      tasksContainerIt = m_tasksContainer.find( dependencies[ dependenciesIdx ] );

      if( tasksContainerIt == tasksContainerItEnd )
      {
        return false;
      }
    }

    TaskIdType taskId = 0;
    if( ! getUniqueTaskId( taskId ) )
    {
      return false;
    }

    if( taskIdTypePtr )
    {
      *taskIdTypePtr = taskId;
    }

    {
      TasksContainerElementType auxTasksContainerElement;
      auxTasksContainerElement.m_dependencies.insert( auxTasksContainerElement.m_dependencies.end(),
        dependencies.begin(), dependencies.end() );
      auxTasksContainerElement.m_status = INACTIVE;
      auxTasksContainerElement.m_taskPtr = taskPtr;
      auxTasksContainerElement.m_takeTasksOwnership = m_takeTasksOwnership;

      m_tasksContainer[ taskId ] = auxTasksContainerElement;
    }

    addITasks2Exec();

    return true;
  }

  bool GraphExecuter::removeTask(TaskIdType taskId, Task** taskPtr)
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    // Can we remove this task ?

    TasksContainerType::iterator targetTaskContainerIt = m_tasksContainer.find(taskId);
    const TasksContainerType::iterator tasksContainerItEnd = m_tasksContainer.end();
    if( targetTaskContainerIt == tasksContainerItEnd )
    {
      return false;
    }

    if(
        ( targetTaskContainerIt->second.m_status != INACTIVE )
        &&
        ( targetTaskContainerIt->second.m_status != FINISHED )
      )
    {
      return false;
    }

    // Checking dependencies

    std::vector< TasksContainerType::iterator > dependenciesItsVector;
    std::vector< std::list< TaskIdType >::iterator > dependenciesListItsVector;

    std::list< TaskIdType >::iterator dependenciesListIt;
    std::list< TaskIdType >::iterator dependenciesListItEnd;

    TasksContainerType::iterator tasksContainerIt = m_tasksContainer.begin();

    while( tasksContainerIt != tasksContainerItEnd )
    {
      dependenciesListIt = tasksContainerIt->second.m_dependencies.begin();
      dependenciesListItEnd = tasksContainerIt->second.m_dependencies.end();

      while( dependenciesListIt != dependenciesListItEnd )
      {
        if( *dependenciesListIt == taskId )
        {
          if( tasksContainerIt->second.m_status != FINISHED )
          {
            return false;
          }

          dependenciesItsVector.push_back( tasksContainerIt );
          dependenciesListItsVector.push_back( dependenciesListIt );

          break;
        }

        ++dependenciesListIt;
      }

      ++tasksContainerIt;
    }

    // Updating  dependencies

    std::size_t dependenciesItsVectorSize = dependenciesItsVector.size();

    for( std::size_t dependenciesItsVectorIdx = 0 ; dependenciesItsVectorIdx <
      dependenciesItsVectorSize ; ++dependenciesItsVectorIdx )
    {
      dependenciesItsVector[ dependenciesItsVectorIdx ]->second.m_dependencies.erase(
        dependenciesListItsVector[ dependenciesItsVectorIdx ] );
    }

    // Erasing

    if( m_takeTasksOwnership )
    {
      if( taskPtr )
      {
        *taskPtr = targetTaskContainerIt->second.m_taskPtr;
        targetTaskContainerIt->second.m_taskPtr = 0;
      }
    }
    else
    {
      if( taskPtr )
      {
        *taskPtr = targetTaskContainerIt->second.m_taskPtr;
      }
    }

    m_tasksContainer.erase( targetTaskContainerIt );

    // Free unused Ids

    if( m_tasksContainer.empty() )
    {
      m_nextValidFreeTaskId = 0;
      m_freeTasksIds.clear();
    }

    return true;
  }

  void GraphExecuter::setCallBack(ExecuterCallBack& newExecCallBack)
  {
    std::unique_lock<std::mutex> locker(m_mutex);
    m_execCallBackPtr = &newExecCallBack;
  }

  bool GraphExecuter::getTaskStatus(TaskIdType taskId, TaskStatus& taskStatus) const
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    TasksContainerType::const_iterator tasksContainerIt = m_tasksContainer.find(taskId);

    if( tasksContainerIt == m_tasksContainer.end() )
    {
      return false;
    }

    taskStatus = tasksContainerIt->second.m_status;

    return true;
  }

  bool GraphExecuter::isTaskPresent(const TaskIdType taskId) const
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    TasksContainerType::const_iterator tasksContainerIt = m_tasksContainer.find(taskId);

    if( tasksContainerIt == m_tasksContainer.end() )
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  unsigned int GraphExecuter::getTasksCount() const
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    return (unsigned int)m_tasksContainer.size();
  }

  bool GraphExecuter::getUniqueTaskId( TaskIdType& newId )
  {
    if( m_freeTasksIds.empty() )
    {
      if( m_nextValidFreeTaskId == std::numeric_limits< TaskIdType >::max() )
      {
        return false;
      }
      else
      {
        newId = m_nextValidFreeTaskId;
        ++m_nextValidFreeTaskId;
        return true;
      }
    }
    else
    {
      TaskIdType returnValue = m_freeTasksIds.back();
      m_freeTasksIds.pop_back();
      return returnValue;
    }

    return false;
  }

  void GraphExecuter::reset()
  {
    m_executionMode = SEQUENTIAL;
    m_takeTasksOwnership = false;
    m_execCallBackPtr = 0;
    m_nextValidFreeTaskId = 0;

    m_executer.stopExecution();
    m_executer.setCallBack( *this );

    m_tasksContainer.clear();
    m_freeTasksIds.clear();
  }

  void GraphExecuter::taskStarted(const TaskIdType& internalExecuterTaskId)
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    TasksContainerType::iterator tasksContainerIt = m_tasksContainer.begin();
    const TasksContainerType::iterator tasksContainerItEnd = m_tasksContainer.end();

    while( tasksContainerIt != tasksContainerItEnd )
    {
      if( tasksContainerIt->second.m_executerTaskId == internalExecuterTaskId )
      {
        break;
      }

      ++tasksContainerIt;
    }

    if( tasksContainerIt == tasksContainerItEnd )
    {
      throw std::runtime_error( "Invalid execution state" );
    }

    tasksContainerIt->second.m_status = EXECUTING;

    TaskIdType taskID = tasksContainerIt->first;

    locker.unlock();

    if( m_execCallBackPtr )
    {
      m_execCallBackPtr->taskStarted( taskID );
    }
  }

  void GraphExecuter::taskExecuted(const TaskIdType& internalExecuterTaskId,
    const TaskExecutionResult::BasicResult& basicResut )
  {
    // Removing task reference from the internal executer

    if( !m_executer.removeTask( internalExecuterTaskId, 0 ) )
    {
      throw std::runtime_error( "Invalid execution state" );
    }

    // Updating task status

    std::unique_lock<std::mutex> locker(m_mutex);

    TasksContainerType::iterator tasksContainerIt = m_tasksContainer.begin();
    const TasksContainerType::iterator tasksContainerItEnd = m_tasksContainer.end();

    while( tasksContainerIt != tasksContainerItEnd )
    {
      if( tasksContainerIt->second.m_executerTaskId == internalExecuterTaskId )
      {
        tasksContainerIt->second.m_executerTaskId =
          std::numeric_limits< TaskIdType >::max();
        tasksContainerIt->second.m_status = FINISHED;

        break;
      }

      ++tasksContainerIt;
    }

    if( tasksContainerIt == tasksContainerItEnd )
    {
      throw std::runtime_error( "Invalid execution state" );
    }

    const TaskIdType taskId = tasksContainerIt->first;

    // Add all inactive tasks with all dependencies solved into the executer
    addITasks2Exec();

    locker.unlock();

    // Execute a callback

    if( m_execCallBackPtr )
    {
      m_execCallBackPtr->taskExecuted(taskId, basicResut);
    }
  }

  void GraphExecuter::addITasks2Exec()
  {
    // Add inactive tasks with all dependencies solved into the executer

    TasksContainerType::iterator tasksContainerIt = m_tasksContainer.begin();
    const TasksContainerType::iterator tasksContainerItEnd = m_tasksContainer.end();

    std::list< TaskIdType >::iterator dependenciesListIt;
    std::list< TaskIdType >::iterator dependenciesListItEnd;

    while( tasksContainerIt != tasksContainerItEnd )
    {
      if( tasksContainerIt->second.m_status == INACTIVE )
      {
        dependenciesListIt = tasksContainerIt->second.m_dependencies.begin();
        dependenciesListItEnd = tasksContainerIt->second.m_dependencies.end();

        while( dependenciesListIt != dependenciesListItEnd )
        {
          if( m_tasksContainer.find( *dependenciesListIt )->second.m_status != FINISHED )
          {
            break;
          }

          ++dependenciesListIt;
        }

        if( dependenciesListIt == dependenciesListItEnd )
        {
          m_executer.addTask( tasksContainerIt->second.m_taskPtr, &(tasksContainerIt->second.m_executerTaskId) );
          tasksContainerIt->second.m_status = QUEUED;
        }
      }

      ++tasksContainerIt;
    }
  }

}  // end namespace tp
}  // end namespace gdma
