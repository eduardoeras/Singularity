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

#include <string>

// GeoDMA Includes
#include "analysis.hpp"

namespace gdma {
namespace sa {

  Analysis::Analysis()
  {
    reset();
  }

  Analysis::Analysis( const Analysis& other )
  {
    operator=( other );
  }

  Analysis::~Analysis()
  {
  }

  const std::string& Analysis::getName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_name;
  }

  Analysis& Analysis::operator=( const Analysis& other )
  {
    reset();

    std::lock_guard<std::mutex> lock( m_mutex );
    std::lock_guard<std::mutex> lock2( other.m_mutex );

    m_enableCache = other.m_enableCache;
    m_projectPtr = other.m_projectPtr;
    m_name = other.m_name;
    m_outputDirectoryName = other.m_outputDirectoryName;

    m_graph = other.m_graph;
    m_graph.setAnalysis( *this );

    m_context = other.m_context;

    return *this;
  }

  void Analysis::setName( const std::string& newName )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_name = newName;
  }

  const Graph& Analysis::getGraph() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_graph;
  }

  Graph& Analysis::getGraph()
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_graph;
  }

  bool Analysis::setGraph( const Graph& newGraph )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    boost::filesystem::path graphOutDir( m_outputDirectoryName );
    graphOutDir /= newGraph.getName();

    m_graph = newGraph;

    if( ! m_graph.setOutputDirectoryName( graphOutDir.string() ) )
    {
      return false;
    }

    m_graph.enableCache( m_enableCache );
    m_graph.setAnalysis( *this );

    return true;
  }

  const Context& Analysis::getContext() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_context;
  }

  void Analysis::setContext( const Context& newContext )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_context = newContext;
  }

  bool Analysis::setOutputDirectoryName( const std::string& outDirName )
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

    boost::filesystem::path graphOutDir( m_outputDirectoryName );
    graphOutDir /= m_graph.getName();

    return m_graph.setOutputDirectoryName( graphOutDir.string() );
  }

  const std::string& Analysis::getOutputDirectoryName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_outputDirectoryName;
  }

  void Analysis::enableCache( const bool enabled )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    m_enableCache = enabled;
    m_graph.enableCache( enabled );
  }

  bool Analysis::isCacheEnabled() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_enableCache;
  }

  void Analysis::reset()
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    m_enableCache = false;
    m_projectPtr = 0;
    m_name.clear();
    m_outputDirectoryName.clear();
    m_graph.reset();
    m_context.reset();
  }

  void Analysis::setProject( const Project& project )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_projectPtr = &project;
  }

  Project const* Analysis::getProject() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_projectPtr;
  }

}  // end namespace sa
}  // end namespace gdma
