#include <map>
#include <string>

// GeoDMA Includes
#include "context.hpp"

namespace gdma {
namespace sa {

  Context::Context()
  {
  }

  Context::Context( const Context& other )
  {
    operator=( other );
  }

  Context::~Context()
  {
  }

  Context& Context::operator=( const Context& other )
  {
    reset();

    std::lock_guard<std::mutex> lock( m_mutex );
    std::lock_guard<std::mutex> lock2( other.m_mutex );

    m_contextName = other.m_contextName;
    m_contextDirectoryName = other.m_contextDirectoryName;
    m_resources = other.m_resources;

    return *this;
  }

  const std::string& Context::getName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_contextName;
  }

  void Context::setName( const std::string& contextName )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_contextName = contextName;
  }

  bool Context::getResource( const std::string& name, te::core::URI& uri ) const
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    if( name.empty() )
      return false;

    std::map< std::string, te::core::URI >::const_iterator rIt =
      m_resources.find( name );

    if( rIt == m_resources.end() )
    {
      return false;
    }
    else
    {
      uri = te::core::URI( rIt->second );

      return true;
    }
  }

  bool Context::addResource( const std::string& name, const te::core::URI& uri )
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    if( name.empty() )
      return false;

    if( m_resources.find( name ) == m_resources.end() )
    {
      m_resources[ name ] = uri;
      return true;
    }
    else
    {
      return false;
    }
  }

  void Context::reset()
  {
    std::lock_guard<std::mutex> lock( m_mutex );

    m_contextName.clear();
    m_resources.clear();
  }

  std::size_t Context::getResourcesCount() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_resources.size();
  }

  void Context::setContextDirectoryName( const std::string& dirName )
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    m_contextDirectoryName = dirName;
  }

  const std::string& Context::getContextDirectoryName() const
  {
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_contextDirectoryName;
  }

}  // end namespace sa
}  // end namespace gdma
