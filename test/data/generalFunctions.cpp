#include <string>
#include <vector>

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
