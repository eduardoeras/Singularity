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

/*!
  \file gdma/lib/featureextraction/multiGeometryAlgorithms.cpp

  \brief Util functionalities to deal with geometry collections manipulation.

  \author Raian Vargas Maretto
  \author Thales Sehn Korting
  \author Emiliano Ferreira Castejon
*/

// Terralib includes
#include <terralib/geometry.h>

// STL includes
#include <vector>

// GeoDMA includes
#include "geodmalib/featureextraction/multiGeometryAlgorithms.hpp"

void gdma::fte::Multi2SilgleFilterPolygons(te::gm::Geometry* geometry,
    std::vector<te::gm::Geometry*> &geometryVector) {
    switch (geometry->getGeomTypeId()) {
    case te::gm::MultiPolygonType:
        te::gm::Multi2Single(geometry, geometryVector);
        break;
    case te::gm::PolygonType:
        geometryVector.push_back(geometry);
        break;
    default:
        break;
    }
}

void gdma::fte::Multi2SilgleFilterLineStrings(te::gm::Geometry* geometry,
    std::vector<te::gm::Geometry*> &geometryVector) {
    switch (geometry->getGeomTypeId()) {
    case te::gm::MultiLineStringType:
        te::gm::Multi2Single(geometry, geometryVector);
        break;
    case te::gm::LineStringType:
        geometryVector.push_back(geometry);
        break;
    default:
        break;
    }
}

void gdma::fte::Multi2SilgleFilterPolygonsRestrict(te::gm::Geometry* geometry,
    std::vector<te::gm::Geometry*> &geometryVector) {
    switch (geometry->getGeomTypeId()) {
    case te::gm::MultiPolygonType:
        te::gm::Multi2Single(geometry, geometryVector);
        break;
    case te::gm::PolygonType:
        geometryVector.push_back(geometry);
        break;
    default:
        std::ostringstream errorMsg;
        errorMsg << "Invalid Geometry Type! Expected MultiPolygon or Polygon, got ";
        errorMsg << geometry->getGeometryType() << "." << std::endl;
        throw te::common::Exception(TE_TR(errorMsg.str()));
    }
}

void gdma::fte::Multi2SilgleFilterLineStringRestrict(te::gm::Geometry* geometry,
    std::vector<te::gm::Geometry*> &geometryVector) {
    switch (geometry->getGeomTypeId()) {
    case te::gm::MultiLineStringType:
        te::gm::Multi2Single(geometry, geometryVector);
        break;
    case te::gm::LineStringType:
        geometryVector.push_back(geometry);
        break;
    default:
        std::ostringstream errorMsg;
        errorMsg << "Invalid Geometry Type! Expected MultiLineString or LineString, got ";
        errorMsg << geometry->getGeometryType() << "." << std::endl;
        throw te::common::Exception(TE_TR(errorMsg.str()));
    }
}
