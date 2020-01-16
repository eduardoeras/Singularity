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

// #include <terralib/dataaccess.h>
// #include <terralib/memory.h>
// #include <terralib/rp.h>
// #include <terralib/datatype.h>
// #include <terralib/sam/rtree.h>
// #include <terralib/common.h>
//
// #include <boost/filesystem.hpp>
//
#include <string>
// #include <memory>
#include <vector>
// #include <map>
// #include <fstream>
// #include <limits>
// #include <algorithm>
// #include <set>

// GeoDMA Includes
#include "generalFunctions.hpp"
#include "defines.hpp"
#include "../common/utils.hpp"

namespace gdma {
namespace sa {

  void findTokens( const std::string& str, std::vector< std::string >& tokens )
  {
    tokens.clear();

    const std::int64_t strSize = (std::int64_t)str.size();
    std::int64_t tokenStart = 0;
    std::int64_t tokenEnd = -1;
    std::int64_t currentPos = 0;

    while( currentPos < strSize )
    {
      if( str[ currentPos ] == '\\' )
      {
        if( currentPos == ( strSize - 1 ) )
        {
          ++currentPos;
        }
        else
        {
          if( tokenEnd >= tokenStart )
          {
            tokenEnd += 2;
          }
          else
          {
            tokenStart = currentPos;
            tokenEnd = currentPos + 1;
          }

          currentPos += 2;
        }
      }
      else if( str[ currentPos ] == ' ' )
      {
        // Flush the last token if it exists
        if( tokenEnd >= tokenStart )
        {
          tokens.push_back( str.substr( tokenStart, tokenEnd - tokenStart + 1 ) );
          tokenStart = 0;
          tokenEnd = -1;
        }

        ++currentPos;
      }
      else if( str[ currentPos ] == '#' )
      {
        currentPos = strSize;
      }
      else if( str[ currentPos ] == '\"' )
      {
        // Flush the last token if it exists

        if( tokenEnd >= tokenStart )
        {
          tokens.push_back( str.substr( tokenStart, tokenEnd - tokenStart + 1 ) );
          tokenStart = 0;
          tokenEnd = -1;
        }

        if( currentPos < ( strSize - 1 ) )
        {
          tokenStart = tokenEnd = currentPos + 1;

          while( true )
          {
            tokenEnd = str.find_first_of( "\"", tokenEnd );

            if( tokenEnd >= strSize )
            {
              break;
            }
            else if( str[ tokenEnd - 1 ] != '\\' )
            {
              break;
            }
            else
            {
              tokenEnd = tokenEnd + 1;
            }
          }

          if( tokenEnd >= strSize )
          {
            currentPos = strSize;
          }
          else if( tokenEnd > tokenStart )
          {
            tokens.push_back( str.substr( tokenStart, tokenEnd - tokenStart ) );

            currentPos = tokenEnd + 1;
            tokenStart = 0;
            tokenEnd = -1;
          }
          else
          {
            currentPos = tokenEnd + 1;
          }
        }
        else
        {
          ++currentPos;
        }
      }
      else
      {
        if( tokenEnd >= tokenStart )
        {
          ++tokenEnd;
        }
        else
        {
          tokenStart = tokenEnd = currentPos;
        }

        ++currentPos;
      }
    }

    // Flush the last token if it exists
    if( tokenEnd >= tokenStart )
    {
      tokens.push_back( str.substr( tokenStart, tokenEnd - tokenStart + 1 ) );
    }
  }

}  // end namespace sa
}  // end namespace gdma
