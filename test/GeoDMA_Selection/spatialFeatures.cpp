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
  \file gdma/lib/featureextraction/spatialFeatures.cpp

  \brief Functionalities to compute segmentation-based spatial features for regions based on 
  geometries.

  \author Raian Vargas Maretto
  \author Thales Sehn Korting
  \author Emiliano Ferreira Castejon
*/

// TerraLib includes
#include <terralib/common/MatrixUtils.h>
#include <terralib/dataaccess.h>
#include <terralib/geometry.h>
#include <terralib/memory.h>
#include <terralib/memory/CachedRaster.h>
#include <terralib/raster.h>
#include <terralib/common.h>

// CGAL Includes
#include <CGAL/convex_hull_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Min_ellipse_2.h>
#include <CGAL/Min_ellipse_2_traits_2.h>
#include <CGAL/min_quadrilateral_2.h>
#include <CGAL/Polygon_2.h>

// Boost
#include <boost/filesystem/convenience.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/algorithm/string.hpp>  // REMOVE IT

// STL includes
#include <math.h>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

// GeoDMA includes
#include "geodmalib/featureextraction/spatialFeatures.hpp"
#include "geodmalib/featureextraction/featuresSummary.hpp"
#include "geodmalib/common/commandLineProgBar.hpp"

// Default constructor
gdma::fte::SpatialFeatures::SpatialFeatures() {}

// Default destructor
gdma::fte::SpatialFeatures::~SpatialFeatures() { }

/*!
  \brief Concrete method to compute the features. Implements Features::getAttributes
  \exception te::srs::Exception if the Geometry is not a Polygon or Multipolygon.
*/
bool gdma::fte::SpatialFeatures::getFeatures(std::map<std::string, te::gm::Geometry*> gm_container,
    std::map<std::string, gdma::fte::FeaturesSummary> &attr_container,
    const bool useMultiThreads,
    const double dummyValueDouble)
{
  te::common::TaskProgress task(TE_TR("Computing Spatial Features"));
  task.setTotalSteps(gm_container.size());
  task.useTimer(true);

  std::mutex mutex;
  std::map<std::string, te::gm::Geometry*>::const_iterator geomsIt =
    gm_container.begin();
  std::map<std::string, te::gm::Geometry*>::const_iterator geomsItEnd =
    gm_container.end();
  bool returnValue = true;
  unsigned int procsNumber = te::common::GetPhysProcNumber();

  if( useMultiThreads )
  {
    unsigned int threadIdx = 0;
    std::vector< std::shared_ptr< std::thread > > threads;
    std::shared_ptr< std::thread > thread;

    for( threadIdx = 0 ; threadIdx < procsNumber ; ++threadIdx )
    {
      thread.reset( new std::thread( getFeaturesThreadEntry,
        &geomsIt, &geomsItEnd, &attr_container,
        &dummyValueDouble, &returnValue, &mutex,
        &task ) );
      threads.push_back( thread );
    }

    for( threadIdx = 0 ; threadIdx < procsNumber ; ++threadIdx )
    {
      threads[ threadIdx ]->join();
    }
  }
  else
  {
    getFeaturesThreadEntry( &geomsIt, &geomsItEnd, &attr_container,
      &dummyValueDouble, &returnValue, &mutex,
      &task );
  }

  return returnValue;
}

// Polygon Area
double gdma::fte::SpatialFeatures::computeArea(te::gm::Polygon* polygon) {
    return polygon->getArea();
}

// Polygon bounding box area
double gdma::fte::SpatialFeatures::computePBoxArea(te::gm::Envelope const * const pbox) {
    return pbox->getArea();
}

// Polygon Perimeter
double gdma::fte::SpatialFeatures::computePerimeter(te::gm::Polygon* polygon) {
    return polygon->getPerimeter();
}

// Polygon Fractal Dimension
double gdma::fte::SpatialFeatures::computeFractalDim(double area, double perimeter) {
    if (area == 1.0)
        area = 1.00000001;

    return 2 * ((std::log(perimeter) / 4) / std::log(area));
}

// Polygon Bounding Box Heigth
double gdma::fte::SpatialFeatures::computePBoxLength(te::gm::Envelope const * const boundingBox) {
    return boundingBox->getHeight();
}

// Polygon Bounding Box Width
double gdma::fte::SpatialFeatures::computePBoxWidth(te::gm::Envelope const * const boundingBox) {
    return boundingBox->getWidth();
}

// Ratio between the perimeter and the area of the polygon
double gdma::fte::SpatialFeatures::computePerimAreaRatio(double area, double perimeter) {
    if (area == 0.0 || perimeter == 0.0)
        return 0.0;

    return perimeter / area;
}

// Compacity of the polygon
double gdma::fte::SpatialFeatures::computeCompacity(double area, double perimAreaRatio) {
    if (area == 0.0 || perimAreaRatio == 0.0)
        return 0.0;

    return perimAreaRatio / std::sqrt(area);
}

typedef CGAL::Cartesian<double> CgCartModel;
typedef CGAL::Point_2<CgCartModel> CgPoint;
typedef CGAL::Min_ellipse_2_traits_2<CgCartModel> CgEllipseTraits;
typedef CGAL::Min_ellipse_2<CgEllipseTraits> CgMinEllipse;
typedef CGAL::Polygon_2<CgCartModel> CgPolygon;

// Computes the coefficients of the minimum ellipse that circunscribes the given polygon.
bool gdma::fte::SpatialFeatures::fitEllipse(te::gm::Polygon const * const polygon, std::vector<double> &coefs,
    double &offsetX, double &offsetY) {
    coefs.resize(6);

    double maxX = std::numeric_limits<double>::min();
    double maxY = std::numeric_limits<double>::min();
    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();

    te::gm::LinearRing const* ringPol = (te::gm::LinearRing const*)(*polygon)[0];

    for (int posPt = 0; posPt < ringPol->getNPoints(); posPt++) {
        double xCurCoord = ringPol->getX(posPt);
        double yCurCoord = ringPol->getY(posPt);

        // compute minimum and maximum X coordinates
        if (xCurCoord > maxX)
            maxX = xCurCoord;
        else if (xCurCoord < minX)
            minX = xCurCoord;

        // compute minimum and maximum Y coordinates
        if (yCurCoord > maxY)
            maxY = yCurCoord;
        else if (yCurCoord < minY)
            minY = yCurCoord;
    }

    offsetX = minX - 1;
    offsetY = minY - 1;

    CgPolygon cg_pol;
    for (int posPt = 0; posPt < (ringPol->getNPoints() - 1); posPt++) {
        cg_pol.push_back(CgPoint(ringPol->getX(posPt) - offsetX, ringPol->getY(posPt) - offsetY));
    }

    double aCoef, bCoef, cCoef, dCoef, eCoef, fCoef;

    try {
        CGAL::Min_ellipse_2<CgEllipseTraits> cg_ellipse(cg_pol.vertices_begin(),
            cg_pol.vertices_end(), true);

        CgMinEllipse::Ellipse ellipseRep = cg_ellipse.ellipse();
        ellipseRep.double_coefficients(aCoef, cCoef, bCoef, dCoef, eCoef, fCoef);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    coefs[0] = aCoef;
    coefs[1] = bCoef;
    coefs[2] = cCoef;
    coefs[3] = dCoef;
    coefs[4] = eCoef;
    coefs[5] = fCoef;

    return true;
}

// Computes the charachteristics of the minimum ellipse that circunscribes the polygon.
bool gdma::fte::SpatialFeatures::solveEllipse(std::vector<double> coefs, double &minor_radius,
    double &major_radius, double &centerX, double &centerY, double &angle) {
    // Computes the ellipse angle (orientation)
    angle = std::atan2(coefs[1], (coefs[0] - coefs[2])) / 2;

    // Computes the scaled major/minor radius
    double cosTheta, sinTheta;
    cosTheta = std::cos(angle);
    sinTheta = std::sin(angle);

    double ap, cp;
    ap = coefs[0] * cosTheta * cosTheta + coefs[1] * cosTheta * sinTheta +
        coefs[2] * sinTheta * sinTheta;
    cp = coefs[0] * sinTheta * sinTheta - coefs[1] * cosTheta * sinTheta +
        coefs[2] * cosTheta * cosTheta;

    // Compute Translations
    // T = [[a(1) a(2)/2]' [a(2)/2 a(3)]'];
    boost::numeric::ublas::matrix<double> transM(2, 2, 0.0);
    transM(0, 0) = coefs[0];
    transM(1, 0) = coefs[1] / 2;
    transM(0, 1) = coefs[1] / 2;
    transM(1, 1) = coefs[2];

    // trans2M = 2 * T
    boost::numeric::ublas::matrix<double> trans2M(2, 2);
    trans2M = 2 * transM;

    // invTrans2M = inv(2 * T)  --> T Inverse of 2 * T
    boost::numeric::ublas::matrix<double> invTrans2M(2, 2);
    te::common::GetInverseMatrix(trans2M, invTrans2M);

    // negInvTrans2M = -inv(2 * T) --> Negative of the inverse of 2 * T
    boost::numeric::ublas::matrix<double> negInvTrans2M(2, 2);
    negInvTrans2M = -invTrans2M;

    // Matrix with coefficients d and e of the Ellipse equation
    // (ax^2 + bxy + cy^2 + dx + ey + f = 0)
    // coefsTransM = [e f]'
    boost::numeric::ublas::matrix<double> coefsTransM(2, 1);
    coefsTransM(0, 0) = coefs[3];
    coefsTransM(1, 0) = coefs[4];

    // prodTransM = t = -inv(2*T)*[a(4) a(5)]';
    boost::numeric::ublas::matrix<double> prodTransM(2, 1);
    boost::numeric::ublas::axpy_prod(negInvTrans2M, coefsTransM, prodTransM);

    // Coordinates of the center of the ellipse
    centerX = prodTransM(0, 0);
    centerY = prodTransM(1, 0);

    // transpProdTransM = t'
    boost::numeric::ublas::matrix<double> transpProdTransM(
        boost::numeric::ublas::trans(prodTransM));

    // auxValM = t' * T
    boost::numeric::ublas::matrix<double> auxValM(1, 2);
    boost::numeric::ublas::axpy_prod(transpProdTransM, transM, auxValM);

    // Computes the scale factor
    // valM = t' * T * t
    boost::numeric::ublas::matrix<double> valM(1, 1);
    boost::numeric::ublas::axpy_prod(auxValM, prodTransM, valM);

    // Scale factor
    double scale = 1 / (valM(0, 0) - coefs[5]);

    // Computes major and minor radius
    major_radius = 1 / std::sqrt(scale * ap);
    minor_radius = 1 / std::sqrt(scale * cp);

    return true;
}

// Generates a geometry (polygon) for the ellipse that circunscribes the polygon.
te::gm::Polygon* gdma::fte::SpatialFeatures::createEllipsePolygon(double minor_radius,
    double major_radius, double centerX, double centerY, double angle, int srid, double offsetX,
    double offsetY) {
    int N = 100;

    double dx = (2 * boost::math::constants::pi<double>()) / N;

    te::gm::Polygon* polEllipse = new te::gm::Polygon(0, te::gm::PolygonType, srid);
    te::gm::LinearRing* pring = new te::gm::LinearRing(N+1, te::gm::LineStringType, srid);

    // Building Rotation Matrix
    boost::numeric::ublas::matrix<double> RM(2, 2, 0.0);
    RM(0, 0) = std::cos(angle);
    RM(1, 0) = std::sin(angle);
    RM(0, 1) = -std::sin(angle);
    RM(1, 1) = std::cos(angle);

    for (int i = 0; i < N; i++) {
        double ang = i * dx;

        double m_x = major_radius * std::cos(ang);
        double m_y = minor_radius * std::sin(ang);

        boost::numeric::ublas::matrix<double> m_vec(2, 1);
        m_vec(0, 0) = m_x;
        m_vec(1, 0) = m_y;

        boost::numeric::ublas::matrix<double> m_d1(RM.size1(), m_vec.size2());
        boost::numeric::ublas::axpy_prod(RM, m_vec, m_d1);

        double px = m_d1(0, 0) + centerX + offsetX;
        double py = m_d1(1, 0) + centerY + offsetY;

        pring->setPoint(i, px, py);
    }
    pring->setPoint(N, pring->getPointN(0)->getX(), pring->getPointN(0)->getY());

    polEllipse->add(pring);

    return polEllipse;
}

// Computes Elliptic Fit
double gdma::fte::SpatialFeatures::computeEllipticFit(double ellipseArea, double polygonArea) {
    if (polygonArea == 0.0 || ellipseArea == 0.0) {
        return 0.0;
    }

    return (ellipseArea > polygonArea) ? (polygonArea / ellipseArea) : (ellipseArea / polygonArea);
}

// Computes Gyration Radius feature
bool gdma::fte::SpatialFeatures::computeGyrationRadius(te::gm::Polygon* polygon, double &gyrRadius,
    double &maxDistance) {
    te::gm::LinearRing const* ringPol = (te::gm::LinearRing const*)(*polygon)[0];
    double sumDist = 0.0;
    maxDistance = std::numeric_limits<double>::min();

    for (int posPt = 0; posPt < ringPol->getNPoints(); posPt++) {
        std::unique_ptr<te::gm::Point> centroid(polygon->getCentroid());
        double xCentroid = centroid->getX();
        double yCentroid = centroid->getY();

        double xPt = ringPol->getX(posPt);
        double yPt = ringPol->getY(posPt);

        double distance = std::sqrt(std::pow((xPt - xCentroid), 2) +
            std::pow((yPt - yCentroid), 2));

        if (distance >= maxDistance)
            maxDistance = distance;

        sumDist += distance;
    }

    gyrRadius = sumDist / ringPol->getNPoints();

    return true;
}

// Computes Polygon Circle
double gdma::fte::SpatialFeatures::computePolygonCircle(double radius, double polArea) {
    double circleArea = boost::math::constants::pi<double>() * (radius * radius);

    return 1 - (polArea / circleArea);
}

// Computes Polygon Shape Index
double gdma::fte::SpatialFeatures::computePolShapeIndex(double polPerimeter, double polArea) {
    if (polArea <= 0.0)
        return 0.0;

    return polPerimeter / (4 * std::sqrt(polArea));
}

// Computes Polygon Density
double gdma::fte::SpatialFeatures::computePolygonDensity(double polRadius, double polArea) {
    if (polRadius <= 0.0)
        return 0.0;

    return polArea / polRadius;
}

// Computes the minimum rectangle outside the object.
te::gm::Polygon* gdma::fte::SpatialFeatures::fitRectangle(te::gm::Polygon* polygon) {
    int srid = polygon->getSRID();

    // Generate a CGAL polygon to compute the convex hull and then the minimum enclosing rectangle
    CgPolygon cg_pol;
    te::gm::LinearRing const* ringPol = (te::gm::LinearRing const*)(*polygon)[0];
    for (int posPt = 0; posPt < ringPol->getNPoints(); posPt++) {
        cg_pol.push_back(CgPoint(ringPol->getX(posPt), ringPol->getY(posPt)));
    }

    // Compute the convex hull. It is necessary because a precondition of the minimum rectangle
    // algorithm is that the polygon must be convex.
    CgPolygon cg_convHull;
    CGAL::convex_hull_2(cg_pol.vertices_begin(), cg_pol.vertices_end(),
        std::back_inserter(cg_convHull));

    // If the polygon is clockwise orientede, reverse it. It is necessary because a precondition of
    // the minimum rectangle algorithm is that the polygon must be counterclockwise oriented
    if (!cg_convHull.is_counterclockwise_oriented())
        cg_convHull.reverse_orientation();

    // Generate the minimum enclosing rectangle
    CgPolygon cg_minRectangle;
    CGAL::min_rectangle_2(cg_convHull.vertices_begin(), cg_convHull.vertices_end(),
        std::back_inserter(cg_minRectangle));

    // Generate the TerraLib polygon with the minimum rectangle.
    te::gm::Polygon* polRectangle = new te::gm::Polygon(0, te::gm::PolygonType, srid);
    te::gm::LinearRing* pring = new te::gm::LinearRing(cg_minRectangle.size() + 1,
        te::gm::LineStringType, srid);

    for (int posPt = 0; posPt < cg_minRectangle.size(); posPt++) {
        pring->setPoint(posPt, cg_minRectangle.vertex(posPt).x(),
            cg_minRectangle.vertex(posPt).y());
    }

    pring->setPoint(cg_minRectangle.size(), cg_minRectangle.vertex(0).x(),
        cg_minRectangle.vertex(0).y());

    polRectangle->add(pring);

    return polRectangle;
}

// Computes the Rectangular fit feature
double gdma::fte::SpatialFeatures::computeRectangularFit(double polArea, double rectArea) {
    if (polArea == 0.0 || rectArea == 0.0)
        return 0.0;

    return polArea / rectArea;
}

void gdma::fte::SpatialFeatures::getFeaturesThreadEntry(
    std::map<std::string, te::gm::Geometry*>::const_iterator * const geomsItPtr,
    std::map<std::string, te::gm::Geometry*>::const_iterator const * const geomsItEndPtr,
    std::map<std::string, gdma::fte::FeaturesSummary>* attrContainerPtr,
    double const * const dummyValueDoublePtr,
    bool * const returnValuePtr,
    std::mutex * const mutexPtr,
    te::common::TaskProgress * const taskProgressPtr )
{
  std::unique_lock< std::mutex > locker( *mutexPtr, std::defer_lock );
  te::gm::Geometry* inputGeomPtr = 0;
  std::string inputGeomString;
  std::vector<te::gm::Geometry*> auxPolVec;
  te::gm::Polygon* polygon = 0;
  double polArea = 0;
  double polPerimeter = 0;
  double fractalDim = 0;
  double perimAreaRatio = 0;
  double polCompacity = 0;
  te::gm::Envelope const * boundingBox = 0;
  double pboxArea = 0;
  double pboxPerimeter = 0;
  double pBoxLength = 0;
  double pBoxWidth = 0;
  std::vector<double> coefs;
  double offsetX = 0;
  double offsetY = 0;
  double minor_radius = 0;
  double major_radius = 0;
  double centerX = 0;
  double centerY = 0;
  double angle = 0;
  double ellipseArea = 0;
  double ellipticFit = 0;
  std::unique_ptr<te::gm::Polygon> min_rectangle;
  double gyrationRadius = 0;
  double polRadius = 0;
  double polCircle = 0;
  double polShapeIdx = 0;
  double polDensity = 0;
  double rectArea = 0;
  double rectangularFit = 0;

  while( true )
  {
    locker.lock();

    if( (*returnValuePtr) == false )
    {
      return;
    }

    if( (*geomsItPtr) == (*geomsItEndPtr) )
    {
      return;
    }

    if (taskProgressPtr->isActive() == false)
    {
      (*returnValuePtr) = false;
      return;
    }

    inputGeomString = (*geomsItPtr)->first;
    inputGeomPtr = (*geomsItPtr)->second;
    geomsItPtr->operator++();

    locker.unlock();

    auxPolVec.clear();

    switch (inputGeomPtr->getGeomTypeId())
    {
      case te::gm::PolygonType:
          auxPolVec.push_back(inputGeomPtr);
          break;
      case te::gm::MultiPolygonType:
          te::gm::Multi2Single(inputGeomPtr, auxPolVec);
          break;
      default:
      {
          locker.lock();

          (*returnValuePtr) = false;

          std::ostringstream output;
          output << "Invalid Geometry Type! Expected Polygon or MyltiPolygon. ";
          output << "Got " << inputGeomPtr->getGeometryType() << ".";
          output << std::endl;

          throw te::common::Exception(TE_TR(output.str()));
          break;
      }
    }

    polygon = static_cast<te::gm::Polygon*>(auxPolVec[0]);

    // Polygon Area
    polArea = computeArea(polygon);

    // Polygon Perimeter
    polPerimeter = computePerimeter(polygon);

    // Polygon Fractal Dimension
    fractalDim = computeFractalDim(polArea, polPerimeter);

    // Ratio between the perimeter and the area of the polygon
    perimAreaRatio = computePerimAreaRatio(polArea, polPerimeter);

    // Compacity of the polygon
    polCompacity = computeCompacity(polArea, perimAreaRatio);

    boundingBox = polygon->getMBR();

    // Polygon bounding box area
    pboxArea = computePBoxArea(boundingBox);

    // Polygon Bounding Box Perimeter
    pboxPerimeter = computePerimeter((te::gm::Polygon*)polygon->getEnvelope());

    // Polygon Bounding Box Heigth
    pBoxLength = computePBoxLength(boundingBox);

    // Polygon Bounding Box Width
    pBoxWidth = computePBoxWidth(boundingBox);

    // Computes the coefficients of the ellipse equation

    coefs.clear();
    gdma::fte::SpatialFeatures::fitEllipse(polygon, coefs, offsetX, offsetY);

    // Computes the ellipse charachteristics.
    // Feature POL_ANGLE

    gdma::fte::SpatialFeatures::solveEllipse(coefs, minor_radius, major_radius,
        centerX, centerY, angle);

    // Computes Elliptic Fit
    ellipseArea = boost::math::constants::pi<double>() * major_radius * minor_radius;
    ellipticFit = gdma::fte::SpatialFeatures::computeEllipticFit(ellipseArea, polArea);

    // Computes the Gyration Radius
    gdma::fte::SpatialFeatures::computeGyrationRadius(polygon, gyrationRadius, polRadius);

    // Computes Polygon Circle
    polCircle = gdma::fte::SpatialFeatures::computePolygonCircle(polRadius, polArea);

    // Computes Polygon Shape Index
    polShapeIdx = gdma::fte::SpatialFeatures::computePolShapeIndex(polPerimeter,
        polArea);

    // Computes Polygon Density
    polDensity = gdma::fte::SpatialFeatures::computePolygonDensity(polRadius, polArea);

    min_rectangle.reset(fitRectangle(polygon));
    rectArea = min_rectangle->getArea();
    rectangularFit = gdma::fte::SpatialFeatures::computeRectangularFit(polArea,
      rectArea);

    locker.lock();

    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_AREA", "P_AREA", polArea);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_PERIMETER", "P_PERIM", polPerimeter);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_FRACTALDIM", "P_FRACDIM", fractalDim);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_PERIM_AREA_RATIO", "P_PERARAT", perimAreaRatio);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_COMPACITY", "P_COMPAC", polCompacity);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_BBOX_AREA", "PBOX_AREA", pboxArea);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_BBOX_PERIM", "PBOX_PERIM", pboxPerimeter);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_BBOX_LENGTH", "PBOX_LEN", pBoxLength);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_BBOX_WIDTH", "PBOX_WIDTH", pBoxLength);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_ANGLE", "POL_ANGLE", angle);
    if (!std::isinf(ellipticFit) && !std::isnan(ellipticFit))
      attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_ELLIPTIC_FIT", "PELLIP_FIT", ellipticFit);
    else
      attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_ELLIPTIC_FIT", "PELLIP_FIT",
        (*dummyValueDoublePtr) );
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_GYRATION_RATIUS", "PGYRATIUS", gyrationRadius);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_RADIUS", "POLRADIUS", polRadius);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_CIRCLE", "PCIRCLE", polCircle);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_SHAPE_INDEX", "PSAHPEIDX", polShapeIdx);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_DENSITY", "PDENSITY", polDensity);
    attrContainerPtr->operator[]( inputGeomString ).addFeature("POL_RECTANGULAR_FIT", "PRECTFIT", rectangularFit);

    taskProgressPtr->pulse();

    locker.unlock();
  }
}
