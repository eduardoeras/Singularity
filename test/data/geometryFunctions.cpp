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
#include <memory>
#include <vector>
#include <map>
// #include <fstream>
#include <limits>
#include <algorithm>
#include <set>

// GeoDMA Includes
#include "geometryFunctions.hpp"
#include "defines.hpp"
// #include "../common/utils.hpp"

namespace gdma {
namespace sa {

  bool rasterizeVectors(
    const std::vector< boost::shared_ptr< boost::ptr_vector< te::gm::Geometry > > >& inputVectors,
    const std::vector< GraphVerticeIDT >& inputVectorsIDs,
    const double outXRes,
    const double outYRes,
    const int outSRID,
    std::unique_ptr< te::rst::Raster >& outRasterizedResultPtr )
  {
    outRasterizedResultPtr.reset();

    const std::size_t inputVectorsSize = inputVectors.size();

    if( inputVectorsSize == 0 )
    {
      return false;
    }

    // Guessing the output raster geometry

    double llX = std::numeric_limits< double >::max();
    double llY = std::numeric_limits< double >::max();
    double urX = -1.0 * std::numeric_limits< double >::max();
    double urY = -1.0 * std::numeric_limits< double >::max();

    {
      std::size_t geomsVecIdx = 0;
      std::size_t geomsVecSize = 0;

      std::unique_ptr< te::gm::Geometry > transformedGeomPtr;
      te::gm::Geometry const* currentGeomPtr = 0;
      te::gm::Envelope const * envPtr = 0;

      for( std::size_t inputVectorsIdx = 0 ; inputVectorsIdx < inputVectorsSize ; ++inputVectorsIdx )
      {
        const boost::ptr_vector< te::gm::Geometry >& geomsVec = *( inputVectors[ inputVectorsIdx ] );
        geomsVecSize = geomsVec.size();

        for( geomsVecIdx = 0 ; geomsVecIdx < geomsVecSize ; ++geomsVecIdx )
        {
          currentGeomPtr = &( geomsVec[ inputVectorsIdx ] );

          //  checking if all geometries have the same srid
          if( currentGeomPtr->getSRID() != outSRID )
          {
            transformedGeomPtr.reset( static_cast< te::gm::Geometry* >( currentGeomPtr->clone() ) );
            transformedGeomPtr->transform( outSRID );
            currentGeomPtr = transformedGeomPtr.get();
          }

          envPtr = currentGeomPtr->getMBR();

          if( llX > envPtr->m_llx ) llX = envPtr->m_llx;
          if( llY > envPtr->m_lly ) llY = envPtr->m_lly;

          if( urX < envPtr->m_urx ) urX = envPtr->m_urx;
          if( urY < envPtr->m_ury ) urY = envPtr->m_ury;
        }
      }
    }

    // Creating the output raster

    {
      std::vector< te::rst::BandProperty* > bandsProperties;
      bandsProperties.push_back( new te::rst::BandProperty( 0, GDMA_SA_DEF_TL_RASTER_DATA_TYPE ) );
      bandsProperties[ 0 ]->m_colorInterp = te::rst::GrayIdxCInt;
      bandsProperties[ 0 ]->m_noDataValue = GDMA_SA_DEFAULT_INVALID_CLASS_ID;

      try
      {
        outRasterizedResultPtr.reset(
          te::rst::RasterFactory::make( "EXPANSIBLE", new te::rst::Grid( outXRes,
          outYRes, new te::gm::Envelope( llX, llY, urX, urY ),
          outSRID ), bandsProperties,
          std::map< std::string, std::string >(), 0, 0 ) );
      }
      catch(...)
      {
        outRasterizedResultPtr.reset();
      }

      if( outRasterizedResultPtr.get() == 0 )
      {
        return false;
      }

      // Fill with GDMA_SA_DEFAULT_INVALID_CLASS_ID values

      const unsigned int nCols = outRasterizedResultPtr->getNumberOfColumns();
      const unsigned int nRows = outRasterizedResultPtr->getNumberOfRows();
      unsigned int col = 0;
      te::rst::Band& outBand = *outRasterizedResultPtr->getBand( 0 );
      const double invalidPixelValue = static_cast< double >( GDMA_SA_DEFAULT_INVALID_CLASS_ID );

      for( unsigned int row = 0 ; row < nRows ; ++row )
      {
        for( col = 0 ; col < nCols ; ++col )
        {
          outBand.setValue( col, row, invalidPixelValue );
        }
      }
    }

    // rasterizing

    {
      std::size_t geomsVecIdx = 0;
      std::size_t geomsVecSize = 0;
      te::gm::Geometry* currentMultiGeomPtr = 0;
      std::vector< te::gm::Geometry * > singleGeomsPtrs;
      std::size_t singleGeomsPtrsIdx = 0;
      std::size_t singleGeomsPtrsSize = 0;
      te::rst::Band& outBand = *outRasterizedResultPtr->getBand( 0 );
      unsigned int polygonIndex = 0;
      const double unkPixelValue = static_cast< double >( GDMA_SA_DEFAULT_UNKNOWN_CLASS_ID );
      const double invalidPixelValue = static_cast< double >( GDMA_SA_DEFAULT_INVALID_CLASS_ID );
      double outputPixelValue = 0;
      std::unique_ptr< te::gm::Geometry > transformedMultiGeomPtr;

      for( std::size_t inputVectorsIdx = 0 ; inputVectorsIdx < inputVectorsSize ; ++inputVectorsIdx )
      {
        const boost::ptr_vector< te::gm::Geometry >& geomsVec = *( inputVectors[ inputVectorsIdx ] );
        geomsVecSize = geomsVec.size();

        for( geomsVecIdx = 0 ; geomsVecIdx < geomsVecSize ; ++geomsVecIdx )
        {
          currentMultiGeomPtr = ( te::gm::Geometry* )( &( geomsVec[ inputVectorsIdx ] ) );

          //  checking if all geometries have the same srid
          if( currentMultiGeomPtr->getSRID() != outSRID )
          {
            transformedMultiGeomPtr.reset( static_cast< te::gm::Geometry* >( currentMultiGeomPtr->clone() ) );
            transformedMultiGeomPtr->transform( outSRID );
            currentMultiGeomPtr = transformedMultiGeomPtr.get();
          }

          singleGeomsPtrs.clear();
          te::gm::Multi2Single( currentMultiGeomPtr, singleGeomsPtrs );
          singleGeomsPtrsSize = singleGeomsPtrs.size();

          for( singleGeomsPtrsIdx = 0 ; singleGeomsPtrsIdx < singleGeomsPtrsSize ;
            ++singleGeomsPtrsIdx )
          {
            if( singleGeomsPtrs[ singleGeomsPtrsIdx ]->getGeomTypeId() == te::gm::PolygonType )
            {
              te::rst::PolygonIterator<double> it = te::rst::PolygonIterator<double>::begin(
                outRasterizedResultPtr.get(),
                ( (te::gm::Polygon*)singleGeomsPtrs[ singleGeomsPtrsIdx ] ) );
              te::rst::PolygonIterator<double> itEnd = te::rst::PolygonIterator<double>::end(
                outRasterizedResultPtr.get(),
                ( (te::gm::Polygon*)singleGeomsPtrs[ singleGeomsPtrsIdx ] ) );

              while( it != itEnd )
              {
                outBand.getValue( it.getColumn(), it.getRow(), outputPixelValue );

                if( outputPixelValue == invalidPixelValue )
                {
                  outBand.setValue( it.getColumn(), it.getRow(), static_cast< double >( ++polygonIndex ) );
                }
                else
                {
                  outBand.setValue( it.getColumn(), it.getRow(), unkPixelValue );
                }

                ++it;
              }
            }
          }
        }
      }
    }

    return true;
  }

  bool geometriesSetsSpatialResolve(
    const std::vector< boost::shared_ptr< boost::ptr_vector< te::gm::Geometry > > >& geomGroups,
    std::vector< boost::shared_ptr< boost::ptr_vector< te::gm::Geometry > > >& nonIntersectedGroups,
    boost::ptr_vector< te::gm::Geometry >& intersectedGeoms )
  {
    nonIntersectedGroups.clear();
    intersectedGeoms.clear();

    // globals

    const std::size_t geomGroupsSize = geomGroups.size();

    if( geomGroupsSize == 0 )
    {
      return true;
    }
    else if( geomGroupsSize == 1 )
    {  // no spatial resolution need to be done
      boost::shared_ptr< boost::ptr_vector< te::gm::Geometry > > newGroupPtr(
        new boost::ptr_vector< te::gm::Geometry >() );

      nonIntersectedGroups.push_back( newGroupPtr );

      boost::ptr_vector< te::gm::Geometry >& inputGroup = *geomGroups[ 0 ];
      boost::ptr_vector< te::gm::Geometry >& newGroup = *newGroupPtr;
      const std::size_t inGroupSize = inputGroup.size();

      for( std::size_t inGroupIdx = 0 ; inGroupIdx < inGroupSize ;
        ++inGroupIdx )
      {
        newGroup.push_back( (te::gm::Geometry*)inputGroup[ inGroupIdx ].clone() );
      }
    }
    else
    {
      // defining the global SRID

      if( geomGroups[ 0 ].get() == 0 ) return false;
      int referenceSRID = geomGroups[ 0 ]->at( 0 ).getSRID();


      // preparing output

      nonIntersectedGroups.resize( geomGroupsSize );

      {
        for( std::size_t geomGroupsIdx = 0 ; geomGroupsIdx < geomGroupsSize ;
          ++geomGroupsIdx )
        {
          nonIntersectedGroups[ geomGroupsIdx ].reset( new boost::ptr_vector< te::gm::Geometry >() );
        }
      }

      // Creating the indexing trees
      // This tree SRID = referenceSRID
      std::vector< te::sam::rtree::Index< te::gm::Geometry const* > > groupsRTrees( geomGroupsSize );

      {
        te::gm::Envelope treeMbr;
        std::size_t geomsVecIdx = 0;
        std::size_t geomsVecSize = 0;

        for( std::size_t geomGroupsIdx = 0 ; geomGroupsIdx < geomGroupsSize ;
          ++geomGroupsIdx )
        {
          te::sam::rtree::Index< te::gm::Geometry const* >& tree = groupsRTrees[ geomGroupsIdx ];

          const boost::ptr_vector< te::gm::Geometry >& geomsVec = *geomGroups[ geomGroupsIdx ];
          geomsVecSize = geomsVec.size();

          for( geomsVecIdx = 0 ; geomsVecIdx < geomsVecSize ; ++geomsVecIdx )
          {
            treeMbr = *geomsVec[ geomsVecIdx ].getMBR();
            if( geomsVec[ geomsVecIdx ].getSRID() != referenceSRID )
            {
              treeMbr.transform( geomsVec[ geomsVecIdx ].getSRID(), referenceSRID );
            }

            tree.insert( treeMbr, &geomsVec[ geomsVecIdx ] );
          }
        }
      }

      // Spatial resolve

      std::set< te::gm::Geometry* > internalIntersectedGeoms;

      {
        std::size_t geomGroupsIdx2 = 0;
        std::vector< te::gm::Geometry const* > treeResult;
        std::size_t gGroup1Size = 0;
        std::size_t gGroup1Idx = 0;
        std::size_t treeResultSize = 0;
        std::size_t treeResultIdx = 0;
        std::unique_ptr< te::gm::Geometry > group1GeomDiff;
        std::unique_ptr< te::gm::Geometry > auxDiff;
        te::gm::Envelope treeSearchMbr;
        bool group1GeomHasBeenChanged = false;

        for( std::size_t geomGroupsIdx1 = 0 ; geomGroupsIdx1 < geomGroupsSize ;
          ++geomGroupsIdx1 )
        {
          const boost::ptr_vector< te::gm::Geometry >& gGroup1 =
            *( geomGroups[ geomGroupsIdx1 ] );
          gGroup1Size = gGroup1.size();

          for( gGroup1Idx = 0 ; gGroup1Idx < gGroup1Size ; ++gGroup1Idx )
          {
            group1GeomDiff.reset( (te::gm::Geometry*)gGroup1[ gGroup1Idx ].clone() );
            if( group1GeomDiff->getSRID() != referenceSRID )
            {
              group1GeomDiff->transform( referenceSRID );
            }

            group1GeomHasBeenChanged = false;

            for( geomGroupsIdx2 = 0 ; geomGroupsIdx2 < geomGroupsSize ; ++geomGroupsIdx2 )
            {
              if( geomGroupsIdx1 != geomGroupsIdx2 )
              {
                treeSearchMbr = *group1GeomDiff->getMBR();
                if( group1GeomDiff->getSRID() != referenceSRID )
                {
                  treeSearchMbr.transform( group1GeomDiff->getSRID(), referenceSRID );
                }

                treeResult.clear();
                groupsRTrees[ geomGroupsIdx2 ].search( treeSearchMbr, treeResult );

                if( ! treeResult.empty() )
                {
                  treeResultSize = treeResult.size();

                  for( treeResultIdx = 0 ; treeResultIdx < treeResultSize ; ++treeResultIdx )
                  {
                    if( ! areDisjoint( *group1GeomDiff, *treeResult[ treeResultIdx ] ) )
                    {
                      geometryDifference( *group1GeomDiff, *treeResult[ treeResultIdx ],
                        auxDiff );

                      group1GeomDiff.reset( auxDiff.release() );

                      group1GeomHasBeenChanged = true;
                    }
                  }
                }
              }
            }

            if( group1GeomHasBeenChanged )
            {
              geometryDifference( gGroup1[ gGroup1Idx ], *group1GeomDiff,
                auxDiff );

              internalIntersectedGeoms.insert( auxDiff.release() );

              nonIntersectedGroups[ geomGroupsIdx1 ]->push_back( group1GeomDiff.release() );
            }
            else
            {
              nonIntersectedGroups[ geomGroupsIdx1 ]->push_back( group1GeomDiff.release() );
            }
          }
        }
      }

      // Free resources

      groupsRTrees.clear();

      // Creating the intersected geoms indexing trees
      // This tree SRID = referenceSRID

      te::sam::rtree::Index< te::gm::Geometry* > intersectedGroupsRTree;

      {
        te::gm::Envelope treeMbr;

        std::set< te::gm::Geometry* >::iterator it = internalIntersectedGeoms.begin();
        std::set< te::gm::Geometry* >::iterator itEnd = internalIntersectedGeoms.end();

        while( it != itEnd )
        {
          treeMbr = *( (*it)->getMBR() );
          if( (*it)->getSRID() != referenceSRID )
          {
            treeMbr.transform( (*it)->getSRID(), referenceSRID );
          }

          intersectedGroupsRTree.insert( treeMbr, *it );

          ++it;
        }
      }

      // union of intersected geoms

      {
        te::gm::Envelope treeSearchMbr;
        std::vector< te::gm::Geometry* > treeResult;
        std::size_t treeResultSize = 0;
        std::size_t treeResultIdx = 0;
        std::unique_ptr< te::gm::Geometry > unionPtr;
        std::unique_ptr< te::gm::Geometry > auxUnionPtr;
        te::gm::Geometry* otherGeomNPtr = 0;
        bool geomWasMerged = false;

        while( ! internalIntersectedGeoms.empty() )
        {
          unionPtr.reset( *( internalIntersectedGeoms.begin() ) );

          // remove from the set

          internalIntersectedGeoms.erase( internalIntersectedGeoms.begin() );

          treeSearchMbr = *unionPtr->getMBR();
          if( unionPtr->getSRID() != referenceSRID )
          {
            treeSearchMbr.transform( unionPtr->getSRID(), referenceSRID );
          }

          // remove from tree

          intersectedGroupsRTree.remove( treeSearchMbr, unionPtr.get() );

          // search for intersections

          treeResult.clear();
          intersectedGroupsRTree.search( treeSearchMbr, treeResult );

          treeResultSize = treeResult.size();

          geomWasMerged = false;

          for( treeResultIdx = 0 ; treeResultIdx < treeResultSize ; ++treeResultIdx )
          {
            otherGeomNPtr = treeResult[ treeResultIdx ];

            if( ! areDisjoint( *unionPtr, *otherGeomNPtr ) )
            {
              treeSearchMbr = *otherGeomNPtr->getMBR();
              if( otherGeomNPtr->getSRID() != referenceSRID )
              {
                treeSearchMbr.transform( otherGeomNPtr->getSRID(), referenceSRID );
              }

              internalIntersectedGeoms.erase( otherGeomNPtr );

              intersectedGroupsRTree.remove( treeSearchMbr, otherGeomNPtr );

              geometryUnion( *unionPtr, *otherGeomNPtr, auxUnionPtr );

              unionPtr.reset( auxUnionPtr.release() );

              geomWasMerged = true;
            }
          }

          if( geomWasMerged )
          {
            // insert into tree

            treeSearchMbr = *( (unionPtr->getMBR() ) );
            if( unionPtr->getSRID() != referenceSRID )
            {
              treeSearchMbr.transform( unionPtr->getSRID(), referenceSRID );
            }

            intersectedGroupsRTree.insert( treeSearchMbr, unionPtr.get() );

            // insert the merge into the set

            internalIntersectedGeoms.insert( unionPtr.release() );
          }
          else
          {
            // insert into the output vector
            intersectedGeoms.push_back( unionPtr.release() );
          }
        }
      }
    }

    return true;
  }

  void convertGeomsVectors( const std::vector< te::gm::Geometry* >& inputGeomsVec,
    boost::ptr_vector< te::gm::Geometry >& outputGeomsVec )
  {
    outputGeomsVec.clear();

    const std::size_t inputGeomsVecSize = inputGeomsVec.size();

    for( std::size_t inputGeomsVecIdx = 0 ; inputGeomsVecIdx < inputGeomsVecSize ;
      ++inputGeomsVecIdx )
    {
      outputGeomsVec.push_back( inputGeomsVec[ inputGeomsVecIdx ] );
    }
  }

  void convertGeomsVectors(
    boost::ptr_vector< te::gm::Geometry >& inputGeomsVec,
    std::vector< te::gm::Geometry* >& outputGeomsVec,
    bool release )
  {
    outputGeomsVec.clear();

    const std::size_t inputGeomsVecSize = inputGeomsVec.size();

    if( release )
    {
      std::auto_ptr< te::gm::Geometry > nullPtr;

      for( std::size_t inputGeomsVecIdx = 0 ; inputGeomsVecIdx < inputGeomsVecSize ;
        ++inputGeomsVecIdx )
      {
        outputGeomsVec.push_back( inputGeomsVec.replace( inputGeomsVecIdx,
          nullPtr ).release() );
      }

      inputGeomsVec.clear();
    }
    else
    {
      for( std::size_t inputGeomsVecIdx = 0 ; inputGeomsVecIdx < inputGeomsVecSize ;
        ++inputGeomsVecIdx )
      {
        outputGeomsVec.push_back( &( inputGeomsVec[ inputGeomsVecIdx ] ) );
      }
    }
  }

  void geometryIntersection(
     const te::gm::Geometry& geom1,
     const te::gm::Geometry& geom2,
     std::unique_ptr< te::gm::Geometry >& result )
  {
    result.reset();

    std::vector< te::gm::Geometry* > geoms1;
    te::gm::Multi2Single( (te::gm::Geometry*)&geom1, geoms1 );

    std::unique_ptr< te::gm::Geometry > reprojGeom2;
    std::vector< te::gm::Geometry* > geoms2;
    if( geom2.getSRID() == geom1.getSRID() )
    {
      te::gm::Multi2Single( (te::gm::Geometry*)&geom2, geoms2 );
    }
    else
    {
      reprojGeom2.reset( (te::gm::Geometry*)geom2.clone() );
      reprojGeom2->transform( geom1.getSRID() );
      te::gm::Multi2Single( reprojGeom2.get(), geoms2 );
    }

    const std::size_t geoms1Size = geoms1.size();
    const std::size_t geoms2Size = geoms2.size();

    if( ( geoms1Size == 1 ) && ( geoms2Size == 1 ) )
    {
      if( ! geom1.disjoint( geoms2[ 0 ] ) )
      {
        result.reset( geom1.intersection( geoms2[ 0 ] ) );
      }
    }
    else
    {
      std::size_t geoms2Idx = 0;
      std::unique_ptr< te::gm::Geometry > intersectionPtr;
      std::unique_ptr< te::gm::Geometry > unionPtr;

      for( std::size_t geoms1Idx = 0 ; geoms1Idx < geoms1Size ; ++geoms1Idx )
      {
        for( geoms2Idx = 0 ; geoms2Idx < geoms2Size ; ++geoms2Idx )
        {
          if( ! geoms1[ geoms1Idx ]->disjoint( geoms2[ geoms2Idx ] ) )
          {
            intersectionPtr.reset( geoms1[ geoms1Idx ]->intersection( geoms2[ geoms2Idx ] ) );

            if( intersectionPtr.get() )
            {
              if( result.get() )
              {
                geometryUnion( *result, *intersectionPtr, unionPtr  );

                if( unionPtr.get() )
                {
                  result.reset( unionPtr.release() );
                }
              }
              else
              {
                result.reset( intersectionPtr.release() );
              }
            }
          }
        }
      }
    }
  }

  void geometryDifference( const te::gm::Geometry& geom1,
     const te::gm::Geometry& geom2,
     std::unique_ptr< te::gm::Geometry >& result )
  {
    result.reset();

    std::vector< te::gm::Geometry* > geoms1;
    te::gm::Multi2Single( (te::gm::Geometry*)&geom1, geoms1 );

    std::unique_ptr< te::gm::Geometry > reprojGeom2;
    std::vector< te::gm::Geometry* > geoms2;
    if( geom2.getSRID() == geom1.getSRID() )
    {
      te::gm::Multi2Single( (te::gm::Geometry*)&geom2, geoms2 );
    }
    else
    {
      reprojGeom2.reset( (te::gm::Geometry*)geom2.clone() );
      reprojGeom2->transform( geom1.getSRID() );
      te::gm::Multi2Single( reprojGeom2.get(), geoms2 );
    }

    const std::size_t geoms1Size = geoms1.size();
    const std::size_t geoms2Size = geoms2.size();

    if( ( geoms1Size == 1 ) && ( geoms2Size == 1 ) )
    {
      result.reset( geom1.difference( geoms2[ 0 ] ) );
    }
    else
    {
      std::size_t geoms2Idx = 0;
      std::unique_ptr< te::gm::Geometry > diffPtr;
      std::unique_ptr< te::gm::Geometry > auxDiffPtr;
      std::unique_ptr< te::gm::Geometry > auxUnionPtr;

      for( std::size_t geoms1Idx = 0 ; geoms1Idx < geoms1Size ; ++geoms1Idx )
      {
        diffPtr.reset( (te::gm::Geometry*)geoms1[ geoms1Idx ]->clone() );

        for( geoms2Idx = 0 ; geoms2Idx < geoms2Size ; ++geoms2Idx )
        {
          geometryDifference( *diffPtr, *geoms2[ geoms2Idx ], auxDiffPtr );
          diffPtr.reset( auxDiffPtr.release() );
        }

        if( diffPtr.get() )
        {
          if( result.get() )
          {
            geometryUnion( *result, *diffPtr, auxUnionPtr );

            if( auxUnionPtr.get() )
            {
              result.reset( auxUnionPtr.release() );
            }
          }
          else
          {
            result.reset( diffPtr.release() );
          }
        }
      }
    }
  }

  void geometryUnion( const te::gm::Geometry& geom1,
      const te::gm::Geometry& geom2,
      std::unique_ptr< te::gm::Geometry >& result )
  {
    result.reset();

    std::vector< te::gm::Geometry* > singleGeoms1;
    te::gm::Multi2Single( (te::gm::Geometry*)&geom1, singleGeoms1 );

    std::vector< te::gm::Geometry* > singleGeoms2;
    std::unique_ptr< te::gm::Geometry > reprojGeom2;
    if( geom2.getSRID() == geom1.getSRID() )
    {
      te::gm::Multi2Single( (te::gm::Geometry*)&geom2, singleGeoms2 );
    }
    else
    {
      reprojGeom2.reset( (te::gm::Geometry*)geom2.clone() );
      reprojGeom2->transform( geom1.getSRID() );

      te::gm::Multi2Single( reprojGeom2.get(), singleGeoms2 );
    }

    const std::size_t geoms1Size = singleGeoms1.size();
    const std::size_t geoms2Size = singleGeoms2.size();

    if( ( geoms1Size == 1 ) && ( geoms2Size == 1 ) )
    {
      result.reset( geom1.Union( singleGeoms2[ 0 ] ) );
    }
    else
    {
      for( std::size_t geoms2Idx = 0 ; geoms2Idx < geoms2Size ; ++geoms2Idx )
      {
        singleGeoms1.push_back( singleGeoms2[ geoms2Idx ] );
      }

      result = te::gm::GetGeometryUnion( singleGeoms1 );
    }
  }

  bool areDisjoint( const te::gm::Geometry& geom1, const te::gm::Geometry& geom2 )
  {
    std::vector< te::gm::Geometry* > geoms1;
    te::gm::Multi2Single( (te::gm::Geometry*)&geom1, geoms1 );

    std::unique_ptr< te::gm::Geometry > reprojGeom2;
    std::vector< te::gm::Geometry* > geoms2;
    if( geom2.getSRID() == geom1.getSRID() )
    {
      te::gm::Multi2Single( (te::gm::Geometry*)&geom2, geoms2 );
    }
    else
    {
      reprojGeom2.reset( (te::gm::Geometry*)geom2.clone() );
      reprojGeom2->transform( geom1.getSRID() );
      te::gm::Multi2Single( reprojGeom2.get(), geoms2 );
    }

    const std::size_t geoms1Size = geoms1.size();
    const std::size_t geoms2Size = geoms2.size();

    if( ( geoms1Size == 1 ) && ( geoms2Size == 1 ) )
    {
      return geom1.disjoint( geoms2[ 0 ] );
    }
    else
    {
      std::size_t geoms2Idx = 0;

      for( std::size_t geoms1Idx = 0 ; geoms1Idx < geoms1Size ; ++geoms1Idx )
      {
        for( geoms2Idx = 0 ; geoms2Idx < geoms2Size ; ++geoms2Idx )
        {
          if( ! geoms1[ geoms1Idx ]->disjoint( geoms2[ geoms2Idx ] ) )
          {
            return false;
          }
        }
      }
    }

    return true;
  }

  void geometrySetsIntersection(
     const boost::ptr_vector< te::gm::Geometry >& inputGeomsVec1,
     const boost::ptr_vector< te::gm::Geometry >& inputGeomsVec2,
     boost::ptr_vector< te::gm::Geometry >& outputGeomsVec,
     std::vector< std::size_t > * const outputGeomsOriginIdxPtr )
  {
    outputGeomsVec.clear();

    if( inputGeomsVec1.empty() || inputGeomsVec2.empty() ) return;

    if( outputGeomsOriginIdxPtr ) outputGeomsOriginIdxPtr->clear();

    const int srid1 = inputGeomsVec1[ 0 ].getSRID();
    const int srid2 = inputGeomsVec2[ 0 ].getSRID();

    // Creating the indexing trees

    te::sam::rtree::Index< te::gm::Geometry const* >  rTree2;

    {
      te::gm::Envelope auxEnv2;
      const std::size_t inputGeomsVec2Size = inputGeomsVec2.size();

      for( std::size_t inputGeomsVec2Idx = 0 ; inputGeomsVec2Idx < inputGeomsVec2Size ; ++inputGeomsVec2Idx )
      {
        auxEnv2 = *inputGeomsVec2[ inputGeomsVec2Idx ].getMBR();
        auxEnv2.transform( srid2, srid1 );

        rTree2.insert( auxEnv2, &inputGeomsVec2[ inputGeomsVec2Idx ] );
      }
    }

    // processing

    const std::size_t inputGeomsVec1Size = inputGeomsVec1.size();
    std::vector< te::gm::Geometry const* > treeResult;
    std::size_t treeResultIdx = 0;
    std::size_t treeResultSize = 0;
    std::unique_ptr< te::gm::Geometry > reprojGeom2;
    std::vector< te::gm::Geometry* > singleGeoms1Vec;
    std::vector< te::gm::Geometry* > singleGeoms2Vec;
    std::size_t singleGeoms1VecSize = 0;
    std::size_t singleGeoms1VecIdx = 0;
    std::size_t singleGeoms2VecSize = 0;
    std::size_t singleGeoms2VecIdx = 0;
    std::unique_ptr< te::gm::Geometry > intersection;

    for( std::size_t inputGeomsVec1Idx = 0 ; inputGeomsVec1Idx < inputGeomsVec1Size ;
      ++inputGeomsVec1Idx )
    {
      singleGeoms1Vec.clear();
      te::gm::Multi2Single( (te::gm::Geometry*)&( inputGeomsVec1[ inputGeomsVec1Idx ] ), singleGeoms1Vec );
      singleGeoms1VecSize = singleGeoms1Vec.size();

      for( singleGeoms1VecIdx = 0 ; singleGeoms1VecIdx < singleGeoms1VecSize ;
        ++singleGeoms1VecIdx )
      {
        treeResult.clear();
        rTree2.search( *( singleGeoms1Vec[ singleGeoms1VecIdx ]->getMBR() ), treeResult );
        treeResultSize = treeResult.size();

        for( treeResultIdx = 0 ; treeResultIdx < treeResultSize ; ++treeResultIdx )
        {
          singleGeoms2Vec.clear();

          if( srid1 == srid2 )
          {
            te::gm::Multi2Single( treeResult[ treeResultIdx ], singleGeoms2Vec );
          }
          else
          {
            reprojGeom2.reset( (te::gm::Geometry*)treeResult[ treeResultIdx ]->clone() );
            reprojGeom2->transform( srid1 );
            te::gm::Multi2Single( reprojGeom2.get(), singleGeoms2Vec );
          }

          singleGeoms2VecSize = singleGeoms2Vec.size();

          for( singleGeoms2VecIdx = 0 ; singleGeoms2VecIdx < singleGeoms2VecSize ;
            ++singleGeoms2VecIdx )
          {
            if( ! singleGeoms1Vec[ singleGeoms1VecIdx ]->disjoint(
              singleGeoms2Vec[ singleGeoms2VecIdx ] ) )
            {
              intersection.reset( singleGeoms1Vec[ singleGeoms1VecIdx ]->intersection(
                singleGeoms2Vec[ singleGeoms2VecIdx ] ) );

              if( intersection.get() )
              {
                outputGeomsVec.push_back( intersection.release() );
                if( outputGeomsOriginIdxPtr ) outputGeomsOriginIdxPtr->push_back( inputGeomsVec1Idx );
              }
            }
          }
        }
      }
    }
  }

  void geometrySetsUnion(
     const boost::ptr_vector< te::gm::Geometry >& inputGeomsVec1,
     const boost::ptr_vector< te::gm::Geometry >& inputGeomsVec2,
     boost::ptr_vector< te::gm::Geometry >& outputGeomsVec )
  {
    outputGeomsVec.clear();

    const std::size_t inputGeomsVec1Size = inputGeomsVec1.size();
    const std::size_t inputGeomsVec2Size = inputGeomsVec2.size();

    if( inputGeomsVec1Size == 0 )
    {
      if( inputGeomsVec2Size == 0 ) return;

      for( std::size_t inputGeomsVec2Idx = 0 ; inputGeomsVec2Idx < inputGeomsVec2Size ;
        ++inputGeomsVec2Idx )
      {
        outputGeomsVec.push_back( (te::gm::Geometry*)inputGeomsVec2[ inputGeomsVec2Idx ].clone() );
      }
    }
    else if( inputGeomsVec2Size == 0 )
    {
      for( std::size_t inputGeomsVec1Idx = 0 ; inputGeomsVec1Idx < inputGeomsVec1Size ;
        ++inputGeomsVec1Idx )
      {
        outputGeomsVec.push_back( (te::gm::Geometry*)inputGeomsVec1[ inputGeomsVec1Idx ].clone() );
      }
    }

    // internal vector of geometries set 2
    // Creating the indexing trees

    boost::ptr_vector< te::gm::Geometry > internalInputGeomsVec2;
    te::sam::rtree::Index< std::size_t >  rTree2;

    {
      const int srid1 = inputGeomsVec1[ 0 ].getSRID();
      const int srid2 = inputGeomsVec2[ 0 ].getSRID();
      std::unique_ptr< te::gm::Geometry > auxGeomPtr;

      for( std::size_t inputGeomsVec2Idx = 0 ; inputGeomsVec2Idx < inputGeomsVec2Size ; ++inputGeomsVec2Idx )
      {
        auxGeomPtr.reset( (te::gm::Geometry*)inputGeomsVec2[ inputGeomsVec2Idx ].clone() );
        if( srid1 != srid2 ) auxGeomPtr->transform( srid1 );

        rTree2.insert( *auxGeomPtr->getMBR(), inputGeomsVec2Idx );

        internalInputGeomsVec2.push_back( auxGeomPtr.release() );
      }
    }

    // union between set 1 and 2

    {
      std::vector< std::size_t  > treeResult;
      std::size_t treeResultIdx = 0;
      std::size_t treeResultSize = 0;
      std::vector< te::gm::Geometry* > singleGeoms1Vec;
      std::size_t singleGeoms1VecSize = 0;
      std::size_t singleGeoms1VecIdx = 0;
      std::size_t inputGeomsVec2Idx = 0;
      std::unique_ptr< te::gm::Geometry > singleGeom1Ptr;
      bool isDisjoint = false;
      std::unique_ptr< te::gm::Geometry > unionGeomPtr;

      for( std::size_t inputGeomsVec1Idx = 0 ; inputGeomsVec1Idx < inputGeomsVec1Size ;
        ++inputGeomsVec1Idx )
      {
        singleGeoms1Vec.clear();
        te::gm::Multi2Single( (te::gm::Geometry*)&( inputGeomsVec1[ inputGeomsVec1Idx ] ), singleGeoms1Vec );
        singleGeoms1VecSize = singleGeoms1Vec.size();

        for( singleGeoms1VecIdx = 0 ; singleGeoms1VecIdx < singleGeoms1VecSize ;
          ++singleGeoms1VecIdx )
        {
          singleGeom1Ptr.reset( (te::gm::Geometry*)singleGeoms1Vec[ singleGeoms1VecIdx ]->clone() );

          treeResult.clear();
          rTree2.search( *( singleGeom1Ptr->getMBR() ), treeResult );
          treeResultSize = treeResult.size();

          if( treeResultSize == 0 )
          {
            outputGeomsVec.push_back( singleGeom1Ptr.release() );
          }
          else
          {  // there is intersectioin with other geometries box from the second set
            isDisjoint = true;

            for( treeResultIdx = 0 ; treeResultIdx < treeResultSize ; ++treeResultIdx )
            {
              inputGeomsVec2Idx = treeResult[ treeResultIdx ];

              if( ! areDisjoint( *singleGeom1Ptr, internalInputGeomsVec2[ inputGeomsVec2Idx ] ) )
              {
                geometryUnion( *singleGeom1Ptr, internalInputGeomsVec2[
                  inputGeomsVec2Idx ], unionGeomPtr );

                rTree2.remove( *( internalInputGeomsVec2[ inputGeomsVec2Idx ].getMBR() ), inputGeomsVec2Idx );
                rTree2.insert( *( unionGeomPtr->getMBR() ), inputGeomsVec2Idx );

                internalInputGeomsVec2.replace( inputGeomsVec2Idx, unionGeomPtr.release() );

                isDisjoint = false;

                break;
              }
            }

            // no intersection found

            if( isDisjoint )
            {
              outputGeomsVec.push_back( singleGeom1Ptr.release() );
            }
          }
        }
      }
    }

    // union within set 2

    {
      std::vector< std::size_t  > treeResult;
      std::size_t treeResultIdx = 0;
      std::size_t treeResultSize = 0;
      std::size_t internalInputGeomsVec2Idx = 0;
      bool isDisjoint = false;
      std::unique_ptr< te::gm::Geometry > unionGeomPtr;

      while( ! internalInputGeomsVec2.empty() )
      {
        treeResult.clear();
        rTree2.search( *( internalInputGeomsVec2.back().getMBR() ), treeResult );
        treeResultSize = treeResult.size();

        isDisjoint = true;

        for( treeResultIdx = 0 ; treeResultIdx < treeResultSize ; ++treeResultIdx )
        {
          internalInputGeomsVec2Idx = treeResult[ treeResultIdx ];

          if(
              ( internalInputGeomsVec2Idx != ( internalInputGeomsVec2.size() - 1 ) )
              &&
              ( ! areDisjoint( internalInputGeomsVec2.back(),
                internalInputGeomsVec2[ internalInputGeomsVec2Idx ] )
              )
            )
          {
            geometryUnion( internalInputGeomsVec2.back(), internalInputGeomsVec2[
              internalInputGeomsVec2Idx ], unionGeomPtr );

            rTree2.remove( *( internalInputGeomsVec2.back().getMBR() ),
              internalInputGeomsVec2.size() - 1 );

            internalInputGeomsVec2.pop_back();

            rTree2.remove( *( internalInputGeomsVec2[ internalInputGeomsVec2Idx ].getMBR() ),
              internalInputGeomsVec2Idx );
            rTree2.insert( *( unionGeomPtr->getMBR() ), internalInputGeomsVec2Idx );

            internalInputGeomsVec2.replace( internalInputGeomsVec2Idx, unionGeomPtr.release() );

            isDisjoint = false;

            break;
          }
        }

        // no intersection found

        if( isDisjoint )
        {
          rTree2.remove( *( internalInputGeomsVec2.back().getMBR() ),
            internalInputGeomsVec2.size() - 1 );
          outputGeomsVec.push_back( internalInputGeomsVec2.pop_back().release() );
        }
      }
    }
  }

  void multi2Single(
    const boost::ptr_vector< te::gm::Geometry >& inputGeomsVec,
    std::vector< te::gm::Geometry* >& outputGeomsVec )
  {
    outputGeomsVec.clear();

    const std::size_t inputGeomsVecSize = inputGeomsVec.size();
    std::vector< te::gm::Geometry* > singleGeomsVec;
    std::size_t singleGeomsVecSize = 0;
    std::size_t singleGeomsVecIdx = 0;

    for( std::size_t inputGeomsVecIdx = 0 ; inputGeomsVecIdx < inputGeomsVecSize ;
      ++inputGeomsVecIdx )
    {
      singleGeomsVec.clear();
      te::gm::Multi2Single( (te::gm::Geometry*)&( inputGeomsVec[ inputGeomsVecIdx ] ), singleGeomsVec );

      singleGeomsVecSize = singleGeomsVec.size();

      for( singleGeomsVecIdx = 0 ; singleGeomsVecIdx < singleGeomsVecSize ;
        ++singleGeomsVecIdx )
      {
        outputGeomsVec.push_back( singleGeomsVec[ singleGeomsVecIdx ] );
      }
    }
  }

  void multi2Single(
    const boost::ptr_vector< te::gm::Geometry >& inputGeomsVec,
    boost::ptr_vector< te::gm::Geometry >& outputGeomsVec )
  {
    outputGeomsVec.clear();

    const std::size_t inputGeomsVecSize = inputGeomsVec.size();
    std::vector< te::gm::Geometry* > singleGeomsVec;
    std::size_t singleGeomsVecSize = 0;
    std::size_t singleGeomsVecIdx = 0;

    for( std::size_t inputGeomsVecIdx = 0 ; inputGeomsVecIdx < inputGeomsVecSize ;
      ++inputGeomsVecIdx )
    {
      singleGeomsVec.clear();
      te::gm::Multi2Single( (te::gm::Geometry*)&( inputGeomsVec[ inputGeomsVecIdx ] ), singleGeomsVec );

      singleGeomsVecSize = singleGeomsVec.size();

      for( singleGeomsVecIdx = 0 ; singleGeomsVecIdx < singleGeomsVecSize ;
        ++singleGeomsVecIdx )
      {
        outputGeomsVec.push_back( (te::gm::Geometry*)singleGeomsVec[ singleGeomsVecIdx ]->clone() );
      }
    }
  }

  void transformGeometries( const int targetSRID,
    boost::ptr_vector< te::gm::Geometry >& geomsVec )
  {
    const std::size_t geomsVecSize = geomsVec.size();

    for( std::size_t geomsVecIdx = 0 ; geomsVecIdx < geomsVecSize ;
      ++geomsVecIdx )
    {
      geomsVec[ geomsVecIdx ].transform( targetSRID );
    }
  }

  bool getGeomSetEnvelope(
    const boost::ptr_vector< te::gm::Geometry >& inputGeomsVec,
    te::gm::Envelope& envelope, int& envelopeSRID )
  {
    if( inputGeomsVec.empty() )
    {
      return false;
    }
    else
    {
      envelopeSRID = inputGeomsVec[ 0 ].getSRID();

      const std::size_t inputGeomsVecSize = inputGeomsVec.size();
      envelope.m_llx = envelope.m_lly = std::numeric_limits< double >::max();
      envelope.m_urx = envelope.m_ury = -1.0 * std::numeric_limits< double >::max();
      te::gm::Envelope const * gEnvPtr = 0;

      for( std::size_t inputGeomsVecIdx = 0 ; inputGeomsVecIdx < inputGeomsVecSize ;
        ++inputGeomsVecIdx )
      {
        if( inputGeomsVec[ inputGeomsVecIdx ].getSRID() != envelopeSRID )
        {
          return false;
        }

        gEnvPtr = inputGeomsVec[ inputGeomsVecIdx ].getMBR();

        envelope.m_llx = std::min( envelope.m_llx, gEnvPtr->m_llx );
        envelope.m_llx = std::min( envelope.m_llx, gEnvPtr->m_urx );

        envelope.m_lly = std::min( envelope.m_lly, gEnvPtr->m_lly );
        envelope.m_lly = std::min( envelope.m_lly, gEnvPtr->m_ury );

        envelope.m_urx = std::max( envelope.m_urx, gEnvPtr->m_llx );
        envelope.m_urx = std::max( envelope.m_urx, gEnvPtr->m_urx );

        envelope.m_ury = std::max( envelope.m_ury, gEnvPtr->m_lly );
        envelope.m_ury = std::max( envelope.m_ury, gEnvPtr->m_ury );
      }

      return true;
    }
  }

}  // end namespace sa
}  // end namespace gdma
