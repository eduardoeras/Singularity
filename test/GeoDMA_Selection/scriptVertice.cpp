
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

#include <boost/filesystem.hpp>

#include <string>
#include <map>
#include <utility>
#include <vector>
#include <cstdio>

#include "scriptVertice.hpp"
#include "analysis.hpp"

#include "graph.hpp"
#include "ioFunctions.hpp"
#include "../common/utils.hpp"

#define GDMA_SA_VERTICE_TYPE "SCRIPT"

namespace
{
  static gdma::sa::ScriptVerticeFactory scriptVerticeFactoryInstance;
}

namespace gdma {
namespace sa {

  ScriptVertice::ScriptVertice()
  {
    reset();
  }

  ScriptVertice::ScriptVertice(const ScriptVertice& other)
  {
    operator=( other );
  }

  ScriptVertice::~ScriptVertice()
  {
  }

  ScriptVertice& ScriptVertice::operator=( const ScriptVertice& other )
  {
    {
      std::lock_guard<std::mutex> lock( getMutex() );
      std::lock_guard<std::mutex> lock2( other.getMutex() );

      reset();

      m_outputType = other.m_outputType;
      m_inputType = other.m_inputType;
      m_maxInputConnections = other.m_maxInputConnections;
      m_command = other.m_command;
      m_resources = other.m_resources;
    }

    GraphVertice::operator=( other );

    return *this;
  }

  std::string ScriptVertice::getType() const
  {
    return std::string( GDMA_SA_VERTICE_TYPE );
  }

  void ScriptVertice::getDescription( std::string& descriptionStr ) const
  {
    descriptionStr = "A vertice that allows the execution of an external script or command.";
  }

  bool ScriptVertice::setParameter( const std::string& parameterName,
    const std::string& parameterValue )
  {
    std::lock_guard<std::mutex> lock( getMutex() );

    if( te::common::Convert2UCase( parameterName ) == "OUTPUT_TYPE" )
    {
      if( te::common::Convert2UCase( parameterValue ) == "VECTOR" )
      {
        m_outputType = VECTOR_URI_OUT_TYPE;
        return true;
      }
      else if( te::common::Convert2UCase( parameterValue ) == "RASTER" )
      {
        m_outputType = RASTER_URI_OUT_TYPE;
        return true;
      }
    }
    else if( te::common::Convert2UCase( parameterName ) == "INPUT_TYPE" )
    {
      if( te::common::Convert2UCase( parameterValue ) == "VECTOR" )
      {
        m_inputType = VECTOR_URI_OUT_TYPE;
        return true;
      }
      else if( te::common::Convert2UCase( parameterValue ) == "RASTER" )
      {
        m_inputType = RASTER_URI_OUT_TYPE;
        return true;
      }
    }
    else if( te::common::Convert2UCase( parameterName ) == "MAX_INPUT_CONNECTIONS" )
    {
      unsigned int maxCons = 0;

      try
      {
        maxCons = boost::lexical_cast< unsigned int >( parameterValue );
      }
      catch(...)
      {
        return false;
      }

      m_maxInputConnections = maxCons;
      return true;
    }
    else if( te::common::Convert2UCase( parameterName ) == "COMMAND" )
    {
      if( ! parameterValue.empty() )
      {
        m_command = parameterValue;
        return true;
      }
    }

    return false;
  }

  void ScriptVertice::getParameters( std::vector< std::pair< std::string, std::string > >& parameters ) const
  {
    parameters.clear();

    std::lock_guard<std::mutex> lock( getMutex() );

    parameters.push_back( std::pair< std::string, std::string >(
      "OUTPUT_TYPE", boost::lexical_cast< std::string >( m_outputType ) ) );
    parameters.push_back( std::pair< std::string, std::string >(
      "INPUT_TYPE", boost::lexical_cast< std::string >( m_inputType ) ) );
    parameters.push_back( std::pair< std::string, std::string >(
      "MAX_INPUT_CONNECTIONS", boost::lexical_cast< std::string >(
      m_maxInputConnections ) ) );
    parameters.push_back( std::pair< std::string, std::string >(
      "COMMAND", m_command ) );
  }

  bool ScriptVertice::setResource( const std::string& contextURIAlias,
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

      m_resources[ std::pair< std::string, std::string >( contextURIAlias,
        verticeURIAlias ) ] = resorceURI;

      return true;
    }
    else
    {
      return false;
    }

    return false;
  }

  void ScriptVertice::getInParsDescription( std::map< std::string, std::string >& inParsDesc ) const
  {
    inParsDesc.clear();

    inParsDesc[ "OUTPUT_TYPE" ] = "Vertice output type (VECTOR/RASTER).";
    inParsDesc[ "INPUT_TYPE" ] = "Vertice accpeted input type (VECTOR/RASTER).";
    inParsDesc[ "MAX_INPUT_CONNECTIONS" ] = "Maximum number of input connections.";
    inParsDesc[ "COMMAND" ] = "External command to run."
      " Allowed string substitutions:"
      " _OUT_DIRECTORY_:Output Directory"
      " , _1ST_PASS_OUTPUT_FILE_: First pass output file name"
      " , _2ND_PASS_OUTPUT_FILE_: Second pass output file name"
      " , _CLASSNAME_PROP_NAME_:The default class name property name used when with datasets"
      " , _CLASSID_PROP_NAME_:The default class ID property name used when with datasets"
      " , _UNKNOWN_CLASS_NAME_:The default class name for unknown classes"
      " , _INVALID_CLASS_NAME_:The default class name for invalid classes"
      " , _INVALID_CLASS_ID_:The default class ID (numeric value) for invalid classes"
      " , _UNKNOWN_CLASS_ID_:The default class ID (numeric value) for unknown classes"
      " , _VERTICE_NAME_:Vertice name"
      " , _VERTICE_TYPE_:Vertice type"
      " , _VERTICE_ID_:Vertice ID"
      " , _1ST_PASS_INPUT_FILES_:Semicolon separated input vertices first pass output file names"
      " , _2ST_PASS_INPUT_FILES_:Semicolon separated output vertices second pass output file names"
      " , _VERTICE_RESOURCES_:Semicolon separated tuples of vertice resources [context alias;vertice alias;URI string]";
  }

  void ScriptVertice::getInResDescription( std::map< std::string, std::string >& inResDesc ) const
  {
    inResDesc.clear();
  }

  GraphVertice::OutputType ScriptVertice::getOutputType() const
  {
    std::lock_guard<std::mutex> lock( getMutex() );
    return m_outputType;
  }

  GraphVertice* ScriptVertice::clone() const
  {
    ScriptVertice* vertPtr = new ScriptVertice( *this );
    return vertPtr;
  }

  bool ScriptVertice::isConnectable( const GraphVertice& other,
    const std::string& alias ) const
  {
    OutputType otherOutType = other.getOutputType();

    std::vector< std::pair< std::string, GraphVertice* > > connections;
    getInputConnections( connections );

    std::lock_guard<std::mutex> lock( getMutex() );

    if(
        ( otherOutType == m_inputType )
        &&
        ( connections.size() < m_maxInputConnections )
      )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  bool ScriptVertice::isMultiClass() const
  {
    return true;
  }

  bool ScriptVertice::is2ndPassCapable() const
  {
    return true;
  }

  void ScriptVertice::reset()
  {
    GraphVertice::reset();

    m_outputType = INVALID_OUT_TYPE;
    m_inputType = INVALID_OUT_TYPE;
    m_maxInputConnections = 0;
    m_command.clear();
    m_resources.clear();
  }

  bool ScriptVertice::executeFirstPass( std::string& errorMessage )
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

      // input connections

      std::vector< std::pair< std::string, GraphVertice* > > inputVertConnections;
      getInputConnections( inputVertConnections );

      if( inputVertConnections.size() > m_maxInputConnections )
      {
        errorMessage = "Invalid number of input vertice connections";
        return false;
      }

      // output connections

      std::vector< GraphVertice* > outputVertConnections;
      getOutputConnections( outputVertConnections );

      // checking command

      if( m_command.empty() )
      {
        errorMessage = "Empty invalid command";
        return false;
      }

      // command replacements

      std::string internalCommand = m_command;

      {
        gdma::common::subStrReplace( "_OUT_DIRECTORY_", getOutputDirectoryName(), internalCommand );

        gdma::common::subStrReplace( "_1ST_PASS_OUTPUT_FILE_", get1stPassOutputFileName(), internalCommand );

        gdma::common::subStrReplace( "_2ND_PASS_OUTPUT_FILE_", get2ndPassOutputFileName(), internalCommand );

        gdma::common::subStrReplace( "_CLASSNAME_PROP_NAME_", GDMA_SA_DEFAULT_CLASSNAME_PROP_NAME, internalCommand );

        gdma::common::subStrReplace( "_CLASSID_PROP_NAME_", GDMA_SA_DEFAULT_CLASSID_PROP_NAME, internalCommand );

        gdma::common::subStrReplace( "_UNKNOWN_CLASS_NAME_", GDMA_SA_DEFAULT_UNKNOWN_CLASS_NAME, internalCommand );

        gdma::common::subStrReplace( "_INVALID_CLASS_NAME_", GDMA_SA_DEFAULT_INVALID_CLASS_NAME, internalCommand );

        gdma::common::subStrReplace( "_INVALID_CLASS_ID_", boost::lexical_cast< std::string >(
          GDMA_SA_DEFAULT_INVALID_CLASS_ID ), internalCommand );

        gdma::common::subStrReplace( "_UNKNOWN_CLASS_ID_", boost::lexical_cast< std::string >(
          GDMA_SA_DEFAULT_UNKNOWN_CLASS_ID ), internalCommand );

        gdma::common::subStrReplace( "_VERTICE_NAME_", getName(), internalCommand );

        gdma::common::subStrReplace( "_VERTICE_TYPE_", getType(), internalCommand );

        gdma::common::subStrReplace( "_VERTICE_ID_", boost::lexical_cast< std::string >(
          getID() ), internalCommand );

        {
          std::string newStr;

          for( std::size_t inputVertConnectionsIdx = 0 ; inputVertConnectionsIdx <
            inputVertConnections.size() ; ++inputVertConnectionsIdx )
          {
            if( inputVertConnectionsIdx ) newStr.append( ";" );

            newStr.append( inputVertConnections[
              inputVertConnectionsIdx ].second->get1stPassOutputFileName() );
          }

          gdma::common::subStrReplace( "_1ST_PASS_INPUT_FILES_", newStr, internalCommand );
        }

        {
          std::string newStr;

          for( std::size_t outputVertConnectionsIdx = 0 ; outputVertConnectionsIdx <
            outputVertConnections.size() ; ++outputVertConnectionsIdx )
          {
            if( outputVertConnectionsIdx ) newStr.append( ";" );

            newStr.append( outputVertConnections[outputVertConnectionsIdx ]->get1stPassOutputFileName() );
          }

          gdma::common::subStrReplace( "_2ST_PASS_INPUT_FILES_", newStr, internalCommand );
        }

        {
          std::string newStr;

          std::map< std::pair< std::string, std::string >, te::core::URI >::const_iterator resourcesIt =
            m_resources.begin();
          const std::map< std::pair< std::string, std::string >, te::core::URI >::const_iterator resourcesItEnd =
            m_resources.begin();

          while( resourcesIt != resourcesItEnd )
          {
            if( resourcesIt != m_resources.begin() ) newStr.append( ";" );

            newStr.append( resourcesIt->first.first + ";" + resourcesIt->first.second
              + ";" + resourcesIt->second.uri() );

            ++resourcesIt;
          }

          gdma::common::subStrReplace( "_VERTICE_RESOURCES_", newStr, internalCommand );
        }

        // backslash char replacements

        gdma::common::subStrReplace( "\\\"", "\"", internalCommand );
      }

      // executing the external script

      FILE* pipePtr = 0;
      try
      {
#ifdef WIN32
        pipePtr = _popen( internalCommand.c_str(), "r" );
#else
        pipePtr = popen(internalCommand.c_str(), "r");
#endif
      }
      catch(...)
      {
      }

      if( pipePtr )
      {
        std::string internalErrorMessage = "External command execution error [";

        unsigned char auxChar = 0;
        while( fscanf( pipePtr, "%c", &auxChar ) == 1 )
        {
          internalErrorMessage.append( 1, auxChar );
        }

        internalErrorMessage.append( "]" );

#ifdef WIN32
        int returnValue = _pclose( pipePtr );
#else
        int returnValue = pclose(pipePtr);
#endif

        if( returnValue != EXIT_SUCCESS )
        {
          errorMessage = internalErrorMessage;
          return false;
        }
      }
      else
      {
        errorMessage = "External command execution error";
        return false;
      }
    }

    return true;
  }

  //  -------------------------------------------------------------------------

  ScriptVerticeFactory::ScriptVerticeFactory()
  : GraphVerticeFactory( GDMA_SA_VERTICE_TYPE )
  {}

  ScriptVerticeFactory::~ScriptVerticeFactory()
  {}

  GraphVertice* ScriptVerticeFactory::build()
  {
    return new ScriptVertice();
  }

}  // end namespace sa
}  // end namespace gdma
