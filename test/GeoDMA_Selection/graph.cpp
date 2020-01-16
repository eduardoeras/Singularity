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

// GeoDMA Includes
#include "graph.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <memory>
#include <utility>
#include <map>
#include <vector>
#include <limits>

namespace gdma {
namespace sa {

  Graph::Graph()
  {
    reset();
  }

  Graph::Graph( const Graph& other )
  {
    operator=( other );
  }

  Graph::~Graph()
  {
  }

  Graph& Graph::operator=( const Graph& other )
  {
    reset();

    std::lock_guard<std::mutex> lock( m_mutex );
    std::lock_guard<std::mutex> lock2( other.m_mutex );

    m_enableCache = other.m_enableCache;
    m_nextValidAddedVerticeIDCounter = other.m_nextValidAddedVerticeIDCounter;
    m_analysisPtr = other.m_analysisPtr;
    m_graphName = other.m_graphName;
    m_outputDirectoryName = other.m_outputDirectoryName;

    // Cloning each vertice

    std::unique_ptr< GraphVertice > newVerticePtr;

    VertContT::const_iterator
      otherVerticesIt = other.m_verticesPtrs.begin();
    const VertContT::const_iterator
      otherVerticesItEnd = other.m_verticesPtrs.end();

    while( otherVerticesIt != otherVerticesItEnd )
    {
      newVerticePtr.reset( otherVerticesIt->second->clone() );
      newVerticePtr->disconnect();
      newVerticePtr->setGraph( *this );

      GraphVerticeIDT vertID = newVerticePtr->getID();

      m_verticesPtrs[ vertID ].reset( newVerticePtr.release() );

      ++otherVerticesIt;
    }

    // Recreating connections

    otherVerticesIt = other.m_verticesPtrs.begin();
    std::vector< std::pair< std::string, GraphVertice* > > connections;

    while( otherVerticesIt != otherVerticesItEnd )
    {
      const GraphVerticeIDT& currentVerticeID = otherVerticesIt->first;

      connections.clear();
      otherVerticesIt->second->getInputConnections( connections );

      for( std::size_t connectionsIdx = 0 ; connectionsIdx < connections.size() ;
        ++connectionsIdx )
      {
        const GraphVerticeIDT& otherVerticeID = connections[ connectionsIdx ].second->getID();
        const std::string& connectionAlias = connections[ connectionsIdx ].first;

        m_verticesPtrs[ currentVerticeID ]->connectInputVertice( *m_verticesPtrs[ otherVerticeID ],
          connectionAlias );
      }

      ++otherVerticesIt;
    }

    return *this;
  }

  void Graph::reset()
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    m_enableCache = false;
    m_nextValidAddedVerticeIDCounter = 1;
    m_analysisPtr = 0;
    m_graphName.clear();
    m_outputDirectoryName.clear();
    m_verticesPtrs.clear();
  }

  const std::string& Graph::getName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_graphName;
  }

  void Graph::setName( const std::string& newName )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_graphName = newName;
  }

  bool Graph::addVertice( GraphVertice* verticePtr,
    const std::vector< std::pair< GraphVerticeIDT, std::string > >& inputConnections )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    if( verticePtr == 0 )
    {
      return false;
    }

    std::unique_ptr< GraphVertice > verticeInternalPtr( verticePtr );

    if( verticeInternalPtr->getName().empty() )
    {
      return false;
    }

    if( m_nextValidAddedVerticeIDCounter == std::numeric_limits< GraphVerticeIDT >::max() )
    {
      return false;
    }

    // checking  for vertices with the same ID or same name

    VertContT::const_iterator vIt = m_verticesPtrs.find( verticeInternalPtr->getID() );

    if( vIt != m_verticesPtrs.end() )
    {
      return false;
    }

    vIt = m_verticesPtrs.begin();
    const VertContT::const_iterator vItEnd = m_verticesPtrs.end();

    while( vIt != vItEnd )
    {
      if( vIt->second->getName() == verticeInternalPtr->getName() )
      {
        return false;
      }

      ++vIt;
    }

    // Input Connectios to other vertices

    verticeInternalPtr->disconnect();

    VertContT::const_iterator verticesPtrsIt;

    const std::size_t inputConnectionsSize = inputConnections.size();

    for( std::size_t inputConnectionsIdx = 0 ; inputConnectionsIdx <
      inputConnectionsSize ; ++inputConnectionsIdx )
    {
      verticesPtrsIt = m_verticesPtrs.find( inputConnections[ inputConnectionsIdx ].first );

      if( verticesPtrsIt == m_verticesPtrs.end() )
      {
        return false;
      }
      else
      {
        if( ! verticeInternalPtr->connectInputVertice( *( verticesPtrsIt->second ),
          inputConnections[ inputConnectionsIdx ].second ) )
        {
          return false;
        }
      }
    }

    // Adding

    boost::filesystem::path vertOutDir( m_outputDirectoryName );
    vertOutDir /= boost::lexical_cast< std::string >( verticeInternalPtr->getID() )
      + "_" + verticeInternalPtr->getName();

    if( ! verticeInternalPtr->setOutputDirectoryName( vertOutDir.string() ) )
    {
      return false;
    }

    verticeInternalPtr->setID( m_nextValidAddedVerticeIDCounter );
    verticeInternalPtr->enableCache( m_enableCache );
    verticeInternalPtr->setGraph( *this );

    m_verticesPtrs[ m_nextValidAddedVerticeIDCounter ].reset( verticeInternalPtr.release() );

    // Updating the internal added vertices counter

    ++m_nextValidAddedVerticeIDCounter;

    return true;
  }

  GraphVertice* Graph::getVertice( const GraphVerticeIDT& verticeID )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    VertContT::const_iterator it =  m_verticesPtrs.find( verticeID );

    if( it != m_verticesPtrs.end() )
    {
      return it->second.get();
    }
    else
    {
      return 0;
    }
  }

  GraphVertice* Graph::getVertice( const std::string& verticeName )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    VertContT::const_iterator it = m_verticesPtrs.begin();
    const VertContT::const_iterator itEnd =  m_verticesPtrs.end();

    while( it != itEnd )
    {
      if( it->second->getName() == verticeName )
      {
        return it->second.get();
      }

      ++it;
    }

    return 0;
  }

  void Graph::getVerticesNames( std::vector< std::string >& verticesNames ) const
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    verticesNames.clear();

    VertContT::const_iterator it = m_verticesPtrs.begin();
    const VertContT::const_iterator itEnd =  m_verticesPtrs.end();

    while( it != itEnd )
    {
      verticesNames.push_back( it->second->getName() );
      ++it;
    }
  }

  void Graph::getVerticesIDs( std::vector< GraphVerticeIDT >& verticesIDs ) const
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    verticesIDs.clear();

    VertContT::const_iterator it = m_verticesPtrs.begin();
    const VertContT::const_iterator itEnd =  m_verticesPtrs.end();

    while( it != itEnd )
    {
      verticesIDs.push_back( it->first );
      ++it;
    }
  }

  std::size_t Graph::getVerticesNumber() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_verticesPtrs.size();
  }

  bool Graph::setOutputDirectoryName( const std::string& outDirName )
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

    // updating vertices directories

    VertContT::iterator vertIt = m_verticesPtrs.begin();
    const VertContT::iterator vertItEnd = m_verticesPtrs.end();

    while( vertIt != vertItEnd )
    {
      boost::filesystem::path vertOutDir( m_outputDirectoryName );
      vertOutDir /= boost::lexical_cast< std::string >( vertIt->second->getID() )
        + "_" + vertIt->second->getName();

      if( ! vertIt->second->setOutputDirectoryName( vertOutDir.string() ) )
      {
        return false;
      }

      ++vertIt;
    }

    return true;
  }

  const std::string& Graph::getOutputDirectoryName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_outputDirectoryName;
  }

  void Graph::enableCache( const bool enabled )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    m_enableCache = enabled;

    VertContT::iterator vertIt = m_verticesPtrs.begin();
    const VertContT::iterator vertItEnd = m_verticesPtrs.end();

    while( vertIt != vertItEnd )
    {
      vertIt->second->enableCache( enabled );

      ++vertIt;
    }
  }

  bool Graph::isCacheEnabled() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_enableCache;
  }

  void Graph::setAnalysis( const Analysis& analysis )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_analysisPtr = &analysis;
  }

  Analysis const* Graph::getAnalysis() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_analysisPtr;
  }

}  // end namespace sa
}  // end namespace gdma
