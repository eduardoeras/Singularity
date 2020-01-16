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

// Terralib
#include <terralib/common.h>

#include <limits>
#include <memory>
#include <iostream>

// GeoDMA Includes
#include "executer.hpp"
#include "defines.hpp"
#include "taskExecutionResult.hpp"

namespace gdma {
namespace tp {

  // ---------------------------------------------------------------------------

  Executer::TasksContainerKeyType::TasksContainerKeyType()
  : m_takeTasksOwnership( false ), m_id(0), m_status(Executer::INACTIVE),
    m_taskPtr(0)
  {
  }

  Executer::TasksContainerKeyType::TasksContainerKeyType(const TasksContainerKeyType& other)
  :
    m_takeTasksOwnership( false ),
    m_id(0),
    m_status(Executer::INACTIVE),
    m_taskPtr(0)
  {
    operator=( other );
  }

  Executer::TasksContainerKeyType::~TasksContainerKeyType()
  {
    if( ( m_taskPtr != 0 ) && m_takeTasksOwnership )
    {
      delete m_taskPtr;
    }
  }

  Executer::TasksContainerKeyType& Executer::TasksContainerKeyType::operator=(
    const TasksContainerKeyType& other)
  {
    if( ( m_taskPtr != 0 ) && m_takeTasksOwnership )
    {
      delete m_taskPtr;
    }

    m_taskPtr = other.m_taskPtr;
    other.m_taskPtr = 0;

    m_takeTasksOwnership = other.m_takeTasksOwnership;
    m_id = other.m_id;
    m_status = other.m_status;

    return *this;
  }

  // ---------------------------------------------------------------------------

  Executer::ThreadsVector::ThreadsVector()
  {
  }

  Executer::ThreadsVector::~ThreadsVector()
  {
    clear();
  }

  Executer::ThreadsVector::ThreadsVector(const Executer::ThreadsVector&)
  {
  }

  Executer::ThreadsVector& Executer::ThreadsVector::operator=( const Executer::ThreadsVector& )
  {
    return *this;
  }

  void Executer::ThreadsVector::add( std::thread* threadPtr )
  {
    m_threads.push_back( std::shared_ptr< std::thread >( threadPtr ) );
  }

  bool Executer::ThreadsVector::empty()
  {
    return m_threads.empty();
  }

  std::size_t Executer::ThreadsVector::size()
  {
    return m_threads.size();
  }

  void Executer::ThreadsVector::clear()
  {
    for( std::size_t threadsIdx = 0 ; threadsIdx < m_threads.size() ;
      ++threadsIdx )
    {
      m_threads[ threadsIdx ]->join();
    }

    m_threads.clear();
  }

  // ---------------------------------------------------------------------------

  Executer::ExecuterStatusHandler::ExecuterStatusHandler(
    unsigned int* activeThreadsNumberCounterPtr,
    ExecuterStatus* executerStatusPtr,  std::mutex* mutexPtr )
  {
    std::unique_lock<std::mutex> locker(*mutexPtr);

    m_activeThreadsNumberCounterPtr = activeThreadsNumberCounterPtr;
    m_executerStatusPtr = executerStatusPtr;
    m_mutexPtr = mutexPtr;

    ++(*m_activeThreadsNumberCounterPtr);
    (*m_executerStatusPtr) = ACTIVE_EXECUTER;
  }

  Executer::ExecuterStatusHandler::~ExecuterStatusHandler()
  {
    std::unique_lock<std::mutex> locker(*m_mutexPtr);

    --(*m_activeThreadsNumberCounterPtr);

    if( (*m_activeThreadsNumberCounterPtr) == 0 )
    {
      (*m_executerStatusPtr) = INACTIVE_EXECUTER;
    }
  }

  // ---------------------------------------------------------------------------

  Executer::Executer()
  {
    reset();
  }

  Executer::Executer(const Executer&)
  {
    reset();
  }

  Executer::~Executer()
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    if( m_executerStatus != INACTIVE_EXECUTER )
    {
      throw std::runtime_error( "Invalid executer status at destruction time" );
    }
  }

  bool Executer::setExecutionParams( const ExecMode newExecMode,
    const unsigned int threadsNumber, const bool takeTasksOwnership )
  {
    stopExecution();

    std::unique_lock<std::mutex> locker(m_mutex);

    reset();

    m_takeTasksOwnership = takeTasksOwnership;

    switch( newExecMode )
    {
      case SINGLE :
      {
        m_executionMode = SINGLE;
        m_maxThreadsNumber = 0;
        m_threadsExecutionCommand = THREAD_PROCESS_NEXT_AND_EXIT;

        break;
      }
      case SEQUENTIAL :
      {
        m_executionMode = SEQUENTIAL;
        m_maxThreadsNumber = 0;
        m_threadsExecutionCommand = THREAD_PROCESS_ALL_AND_EXIT;

        break;
      }
      case CONCURRENT :
      {
        m_executionMode = CONCURRENT;
        m_maxThreadsNumber = threadsNumber ? threadsNumber : te::common::GetPhysProcNumber();
        m_threadsExecutionCommand = THREAD_PROCESS_ALL_AND_SLEEP;

        break;
      }
      default :
        throw std::runtime_error( "Invalid execution mode" );
    }

    return true;
  }

  void Executer::startExecution()
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    if( m_executerStatus == INACTIVE_EXECUTER )
    {
      if( m_executionMode == SINGLE )
      {
        m_executerStatus = ACTIVE_EXECUTER;

        locker.unlock();  // to avoid lock recursion

        threadFunction( &m_threadParameters );

        locker.lock();
      }
      else if( m_executionMode == SEQUENTIAL )
      {
        m_executerStatus = ACTIVE_EXECUTER;

        locker.unlock();  // to avoid lock recursion

        threadFunction( &m_threadParameters );

        locker.lock();
      }
      else if( m_executionMode == CONCURRENT )
      {
        if( !m_threadsVector.empty() )
        {
          throw std::runtime_error( "Invalid execution status" );
        }

        m_executerStatus = ACTIVE_EXECUTER;

        for( unsigned int threadIdx = 0 ; threadIdx < m_maxThreadsNumber ; ++threadIdx )
        {
          m_threadsVector.add( new std::thread( threadFunction, &m_threadParameters ) );
        }

        locker.unlock();

        m_awakeAllThreadsExecutionCondVar.notify_all();
      }
      else
      {
        throw std::runtime_error( "Invalid execution mode" );
      }
    }
  }

  void Executer::stopExecution()
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    if( m_executerStatus == ACTIVE_EXECUTER )
    {
      const ThreadsExecCommand oldThreadsExecutionCommand = m_threadsExecutionCommand;

      m_threadsExecutionCommand = THREAD_EXIT_EXECUTION;

      locker.unlock();

      m_awakeAllThreadsExecutionCondVar.notify_all();

      m_threadsVector.clear();

      locker.lock();

      m_threadsExecutionCommand = oldThreadsExecutionCommand;
    }
  }

  void Executer::waitExecution()
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    if( m_executerStatus == ACTIVE_EXECUTER )
    {
      const ThreadsExecCommand oldThreadsExecutionCommand = m_threadsExecutionCommand;

      m_threadsExecutionCommand = THREAD_PROCESS_ALL_AND_EXIT;

      locker.unlock();

      m_awakeAllThreadsExecutionCondVar.notify_all();

      m_threadsVector.clear();

      m_threadsExecutionCommand = oldThreadsExecutionCommand;
    }
  }

  Executer::ExecMode Executer::getExecutionMode() const
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    return m_executionMode;
  }

  Executer::ExecuterStatus Executer::getExecuterStatus() const
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    return m_executerStatus;
  }

  bool Executer::addTask( Task* taskPtr, TaskIdType* taskIdTypePtr )
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    TasksContainerKeyType key;
    if( !getUniqueTaskId( key.m_id ) )
    {
      return false;
    }

    key.m_takeTasksOwnership = m_takeTasksOwnership;
    key.m_status = INACTIVE;
    key.m_taskPtr = taskPtr;

    if( taskIdTypePtr )
    {
      *taskIdTypePtr = key.m_id;
    }

    m_activeTasks.push_back( key );

    if( m_executerStatus == ACTIVE_EXECUTER )
    {
      locker.unlock();

      m_awakeAllThreadsExecutionCondVar.notify_all();
    }

    return true;
  }

  bool Executer::removeTask(TaskIdType taskId, Task** taskPtr)
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    TasksContainerType::iterator tasksIt = m_finishedTasks.begin();
    TasksContainerType::iterator tasksItEnd = m_finishedTasks.end();
    bool taskFound = false;

    while(tasksIt != tasksItEnd)
    {
      if(tasksIt->m_id == taskId)
      {
        if( m_takeTasksOwnership )
        {
          if( taskPtr )
          {
            *taskPtr = tasksIt->m_taskPtr;
            tasksIt->m_taskPtr = 0;
          }
        }
        else
        {
          if( taskPtr )
          {
            *taskPtr = tasksIt->m_taskPtr;
          }
        }

        m_finishedTasks.erase( tasksIt );

        taskFound = true;

        break;
      }

      ++tasksIt;
    }

    if( !taskFound )
    {
      tasksIt = m_activeTasks.begin();
      tasksItEnd = m_activeTasks.end();

      while(tasksIt != tasksItEnd)
      {
        if(
            (tasksIt->m_id == taskId)
            &&
            (tasksIt->m_status == INACTIVE)
          )
        {
          if( m_takeTasksOwnership )
          {
            if( taskPtr )
            {
              *taskPtr = tasksIt->m_taskPtr;
              tasksIt->m_taskPtr = 0;
            }
          }
          else
          {
            if( taskPtr )
            {
              *taskPtr = tasksIt->m_taskPtr;
            }
          }

          m_activeTasks.erase( tasksIt );

          taskFound = true;

          break;
        }

        ++tasksIt;
      }
    }

    if( taskFound )
    {
      if( m_activeTasks.empty() && m_finishedTasks.empty() )
      {
        m_nextValidFreeTaskId = 0;
        m_freeTasksIds.clear();
      }

      return true;
    }
    else
    {
      return false;
    }
  }

  void Executer::setCallBack(ExecuterCallBack& newExecCallBack)
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    m_threadParameters.m_execCallBackPtr = &newExecCallBack;
  }

  void Executer::reset()
  {
    m_executerStatus = INACTIVE_EXECUTER;
    m_executionMode = SEQUENTIAL;
    m_threadsExecutionCommand = THREAD_EXIT_EXECUTION;
    m_takeTasksOwnership = false;
    m_maxThreadsNumber = 0;
    m_nextValidFreeTaskId = 0;
    m_activeTasks.clear();
    m_finishedTasks.clear();
    m_threadsVector.clear();
    m_freeTasksIds.clear();
    m_activeThreadsNumberCounter = 0;

    m_threadParameters.m_threadsExecutionCommandPtr = &m_threadsExecutionCommand;
    m_threadParameters.m_execCallBackPtr = 0;
    m_threadParameters.m_activeTasksPtr = &m_activeTasks;
    m_threadParameters.m_finishedTasksPtr = &m_finishedTasks;
    m_threadParameters.m_mutexPtr = &m_mutex;
    m_threadParameters.m_awakeAllThreadsExecutionMutexPtr = &m_awakeAllThreadsExecutionMutex;
    m_threadParameters.m_awakeAllThreadsExecutionCondVarPtr = &m_awakeAllThreadsExecutionCondVar;
    m_threadParameters.m_activeThreadsNumberCounterPtr = &m_activeThreadsNumberCounter;
    m_threadParameters.m_executerStatusPtr = &m_executerStatus;
  }

  bool Executer::getUniqueTaskId( TaskIdType& newId )
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
  }

  void Executer::threadFunction(ThreadParameters* threadParametersPtr)
  {
//    std::cout << std::endl << "thread created - " << std::this_thread::get_id() << std::endl;

    std::unique_lock<std::mutex> locker(*threadParametersPtr->m_mutexPtr, std::defer_lock );
    std::unique_lock<std::mutex> awakeAllThreadsExecutionlocker(
      *threadParametersPtr->m_awakeAllThreadsExecutionMutexPtr, std::defer_lock );
    ExecuterStatusHandler executerStatusHandlerInstance(
      threadParametersPtr->m_activeThreadsNumberCounterPtr,
      threadParametersPtr->m_executerStatusPtr,
      threadParametersPtr->m_mutexPtr );
    TasksContainerType::iterator activeTasksIt;
    TasksContainerType::iterator activeTasksItEnd;
    TaskIdType taskId = 0;

    while(true)
    {
      // Checking what this thread must do

      locker.lock();

      if( *( threadParametersPtr->m_threadsExecutionCommandPtr ) == THREAD_EXIT_EXECUTION )
      {
//        std::cout << std::endl << "thread exit (THREAD_EXIT_EXECUTION) - " << std::this_thread::get_id() << std::endl;

        locker.unlock();

        return;
      }
      else
      {
        locker.unlock();

        // process data loop

        while( true )
        {
          // Looking for a task to execute

          locker.lock();

//          std::cout << std::endl << "Looking for a new task to execute - after lock - " << std::this_thread::get_id() << std::endl;

          activeTasksIt = threadParametersPtr->m_activeTasksPtr->begin();
          activeTasksItEnd = threadParametersPtr->m_activeTasksPtr->end();

          while(activeTasksIt != activeTasksItEnd)
          {
            if(activeTasksIt->m_status == INACTIVE)
            {
              break;
            }

            ++activeTasksIt;
          }

          if( activeTasksIt == activeTasksItEnd )
          { // No more data to process
            if(
                *( threadParametersPtr->m_threadsExecutionCommandPtr )
                ==
                THREAD_PROCESS_ALL_AND_SLEEP
              )
            {
//              std::cout << std::endl << "thread will sleep (THREAD_PROCESS_ALL_AND_SLEEP) - " << std::this_thread::get_id() << std::endl;

              locker.unlock();

              awakeAllThreadsExecutionlocker.lock();

              threadParametersPtr->m_awakeAllThreadsExecutionCondVarPtr->wait(
                awakeAllThreadsExecutionlocker );

//              std::cout << std::endl << "thread woke up (THREAD_PROCESS_ALL_AND_SLEEP) - " << std::this_thread::get_id() << std::endl;

              awakeAllThreadsExecutionlocker.unlock();
            }
            else
            {  //  THREAD_EXIT_EXECUTION || THREAD_PROCESS_NEXT_AND_EXIT || THREAD_PROCESS_ALL_AND_EXIT
//              std::cout << std::endl << "thread exit (THREAD_PROCESS_ALL_AND_EXIT) - " << std::this_thread::get_id() << std::endl;

              locker.unlock();

              return;
            }
          }
          else
          {
//            std::cout << std::endl << "thread will execute a task - " << std::this_thread::get_id() << std::endl;

            activeTasksIt->m_status = EXECUTING;
            taskId = activeTasksIt->m_id;

            locker.unlock();

            // Execute a callback

            if(threadParametersPtr->m_execCallBackPtr)
            {
              threadParametersPtr->m_execCallBackPtr->taskStarted(taskId);
            }

            // Task execution

            try
            {
              activeTasksIt->m_taskPtr->execute();
            }
            catch(...)
            {
            }

            locker.lock();

            const TaskExecutionResult::BasicResult basicResult =
              activeTasksIt->m_taskPtr->getExecutionResult().getBasicResult();
            activeTasksIt->m_status = FINISHED;

            threadParametersPtr->m_finishedTasksPtr->push_back(*activeTasksIt);
            threadParametersPtr->m_activeTasksPtr->erase(activeTasksIt);

            locker.unlock();

            // Execute a callback

            if(threadParametersPtr->m_execCallBackPtr)
            {
              threadParametersPtr->m_execCallBackPtr->taskExecuted(taskId,
                basicResult );
            }

            // THREAD_PROCESS_NEXT_AND_EXIT ?

            locker.lock();

            if(
                (
                  *( threadParametersPtr->m_threadsExecutionCommandPtr )
                  ==
                  THREAD_PROCESS_NEXT_AND_EXIT
                )
                ||
                (
                  *( threadParametersPtr->m_threadsExecutionCommandPtr )
                  ==
                  THREAD_EXIT_EXECUTION
                )
              )
            {
//              std::cout << std::endl << "thread exit (THREAD_PROCESS_NEXT_AND_EXIT) - " << std::this_thread::get_id() << std::endl;

              locker.unlock();

              return;
            }
            else
            {
              locker.unlock();
            }
          }
        }
      }
    }
  }

  bool Executer::isTaskPresent(const TaskIdType taskId) const
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    TasksContainerType::iterator tasksIt = m_finishedTasks.begin();
    TasksContainerType::iterator tasksItEnd = m_finishedTasks.end();

    while(tasksIt != tasksItEnd)
    {
      if(tasksIt->m_id == taskId)
      {
        return true;
      }

      ++tasksIt;
    }

    tasksIt = m_activeTasks.begin();
    tasksItEnd = m_activeTasks.end();

    while(tasksIt != tasksItEnd)
    {
      if(tasksIt->m_id == taskId)
      {
        return true;
      }

      ++tasksIt;
    }

    return false;
  }

  bool Executer::getTaskStatus(TaskIdType taskId, TaskStatus& taskStatus) const
  {
    std::unique_lock<std::mutex> locker(m_mutex);

    TasksContainerType::iterator tasksIt = m_finishedTasks.begin();
    TasksContainerType::iterator tasksItEnd = m_finishedTasks.end();

    while(tasksIt != tasksItEnd)
    {
      if(tasksIt->m_id == taskId)
      {
        taskStatus = tasksIt->m_status;
        return true;
      }

      ++tasksIt;
    }

    tasksIt = m_activeTasks.begin();
    tasksItEnd = m_activeTasks.end();

    while(tasksIt != tasksItEnd)
    {
      if(tasksIt->m_id == taskId)
      {
        taskStatus = tasksIt->m_status;
        return true;
      }

      ++tasksIt;
    }

    return false;
  }

}  // end namespace tp
}  // end namespace gdma
