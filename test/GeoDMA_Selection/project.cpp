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

#include <map>
#include <string>
#include <vector>

// GeoDMA Includes
#include "project.hpp"

namespace gdma {
namespace sa {

  Project::Project()
  {
    reset();
  }

  Project::Project( const Project& other )
  {
    operator=( other );
  }

  Project& Project::operator=( const Project& other )
  {
    reset();

    std::lock_guard<std::mutex> lock( m_mutex );
    std::lock_guard<std::mutex> lock2( other.m_mutex );

    m_enableCache = other.m_enableCache;
    m_projectName = other.m_projectName;
    m_outputDirectoryName = other.m_outputDirectoryName;
    m_projectDirectoryName = other.m_projectDirectoryName;
    m_analises = other.m_analises;

    return *this;
  }

  Project::~Project()
  {
  }

  bool Project::setOutputDirectoryName( const std::string& outDirName )
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

    std::map< std::string, Analysis >::iterator anaIt =
      m_analises.begin();
    const std::map< std::string, Analysis >::iterator anaItEnd =
      m_analises.end();

    while( anaIt != anaItEnd )
    {
      boost::filesystem::path anaOutDir( m_outputDirectoryName );
      anaOutDir /= anaIt->second.getName();

      if( ! anaIt->second.setOutputDirectoryName( anaOutDir.string() ) )
      {
        return false;
      }

      ++anaIt;
    }

    return true;
  }

  const std::string& Project::getOutputDirectoryName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_outputDirectoryName;
  }

  void Project::reset()
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    m_enableCache = false;
    m_projectName.clear();
    m_outputDirectoryName.clear();
    m_projectDirectoryName.clear();
    m_analises.clear();
  }

  void Project::enableCache( const bool enabled )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    m_enableCache = enabled;

    std::map< std::string, Analysis >::iterator anaIt =
      m_analises.begin();
    const std::map< std::string, Analysis >::iterator anaItEnd =
      m_analises.end();

    while( anaIt != anaItEnd )
    {
      anaIt->second.enableCache( enabled );
      ++anaIt;
    }
  }

  bool Project::isCacheEnabled() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_enableCache;
  }

  bool Project::setProjectDirectoryName( const std::string& dirName )
  {
    boost::filesystem::path completePath;
    try
    {
      completePath = boost::filesystem::system_complete( dirName );
    }
    catch(...)
    {
      return false;
    }

    std::lock_guard<std::mutex> lock( m_mutex );
    m_projectDirectoryName = completePath.string();

    return true;
  }

  const std::string& Project::getProjectDirectoryName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_projectDirectoryName;
  }

  const std::string& Project::getName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_projectName;
  }

  void Project::setName( const std::string& newName )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_projectName = newName;
  }

  bool Project::addAnalysis( const Analysis& analysis )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    const std::string& anaName = analysis.getName();

    Analysis anaCopy;
    anaCopy = analysis;
    anaCopy.enableCache( m_enableCache );
    anaCopy.setProject( *this );

    boost::filesystem::path anaOutPath( m_outputDirectoryName );
    anaOutPath /= anaName;

    if( ! anaCopy.setOutputDirectoryName( anaOutPath.string() ) )
    {
      return false;
    }

    if(
        ( !anaName.empty() )
        &&
        ( m_analises.find( anaName ) == m_analises.end() )
      )
    {
      m_analises[ anaName ] = anaCopy;
      return true;
    }
    else
    {
      return false;
    }
  }

  bool Project::removeAnalysis( const std::string& analysisName )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    std::map< std::string, Analysis >::iterator it = m_analises.find( analysisName );

    if(
        ( !analysisName.empty() )
        &&
        ( it != m_analises.end() )
      )
    {
      m_analises.erase( it );
      return true;
    }
    else
    {
      return false;
    }
  }

  Analysis* Project::getAnalysis( const std::string& analysisName )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    std::map< std::string, Analysis >::iterator it =
      m_analises.find( analysisName );

    if( it == m_analises.end() )
    {
      return 0;
    }
    else
    {
      return &( it->second );
    }
  }

  void Project::getAnalysisNames( std::vector< std::string >& analysisNames ) const
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    analysisNames.clear();

    std::map< std::string, Analysis >::const_iterator it =
      m_analises.begin();
    const std::map< std::string, Analysis >::const_iterator itEnd =
      m_analises.end();

    while( it != itEnd )
    {
      analysisNames.push_back( it->first );

      ++it;
    }
  }

  unsigned int Project::getTotalVerticesCount() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    unsigned int totalVerticesCount = 0;

    std::map< std::string, Analysis >::const_iterator it =
      m_analises.begin();
    const std::map< std::string, Analysis >::const_iterator itEnd =
      m_analises.end();

    while( it != itEnd )
    {
      totalVerticesCount += it->second.getGraph().getVerticesNumber();
      ++it;
    }

    return totalVerticesCount;
  }

}  // end namespace sa
}  // end namespace gdma
