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
  \file gdma/lib/featureextraction/landscapeFeatures.cpp

  \brief Functionalities to compute Landscape Ecology features.

  \author Raian Vargas Maretto
  \author Thales Sehn Korting
  \author Emiliano Ferreira Castejon
*/

// TerraLib includes
#include <terralib/common/progress/TaskProgress.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>

// Boost Includes
#include <boost/bimap.hpp>
#include <boost/filesystem.hpp>  // REMOVE
#include <boost/math/constants/constants.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>

// STL Includes
#include <cmath>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

// GeoDMA Includes
#include "geodmalib/common/commandLineProgBar.hpp"
#include "geodmalib/common/observerProgressBar.hpp"
#include "geodmalib/featureextraction/intersectionCache.hpp"
#include "geodmalib/featureextraction/landscapeFeatures.hpp"
#include "geodmalib/featureextraction/multiGeometryAlgorithms.hpp"

// Default constructor
// gdma::fte::LandscapeFeatures::LandscapeFeatures() {}

gdma::fte::LandscapeFeatures::LandscapeFeatures(
    const std::vector<te::gm::Geometry*> &polsCtnerCoarserL,
    const boost::bimap<std::string, std::size_t> &positionsIdsCoarserL,
    const std::vector<te::gm::Geometry*> &polsCtnerFinerL,
    const boost::bimap<std::string, std::size_t> &positionsIdsFinerL,
    const std::vector<std::string> &classesFinerL, const std::set<std::string> &classValuesFinerL,
    const double dummyValueDouble) {
    m_pols_ctner_coarserL = &polsCtnerCoarserL;
    m_positionsIdsCoarserL = &positionsIdsCoarserL;

    m_pols_ctner_finerL = &polsCtnerFinerL;
    m_classes_finerL = &classesFinerL;
    m_class_values_finerL = &classValuesFinerL;
    m_positionsIdsFinerL = &positionsIdsFinerL;

    m_dummyValueDouble = dummyValueDouble;
    m_dummyValueInt = static_cast<int>(dummyValueDouble);

    int sridPols = (*m_pols_ctner_finerL)[0]->getSRID();
    int sridCells = (*m_pols_ctner_coarserL)[0]->getSRID();

    // Verify the SRID
    if (sridCells != sridPols) {
        std::ostringstream output;

        output << "Geometry layers received have different Spatial Reference!" << std::endl;
        output << "\tSRID Target Layer: " << (*m_pols_ctner_coarserL)[0]->getSRID() << std::endl;
        output << "\tSRID Classified Layer: " << (*m_pols_ctner_finerL)[0]->getSRID() << std::endl;

        throw te::common::Exception(TE_TR(output.str()));
    } else {
        te::common::UnitOfMeasurePtr unitCells =
            te::srs::SpatialReferenceSystemManager::getInstance().getUnit(sridCells);

        if (unitCells.get()) {
            if (unitCells->getId() != te::common::UOM_Metre) {
                std::ostringstream output;

                output << "Geometry layer must have a metric spatial reference system, received "
                    << std::endl;
                output << unitCells->getName() << "!" << std::endl;
                output << "It must be reprojected before computing attributes!" << std::endl;

                throw te::common::Exception(TE_TR(output.str()));
            }
        }
    }

    m_class_map_codes.clear();
    int count = 0;
    for (auto classVal : (*m_class_values_finerL)) {
        m_class_map_codes.insert(std::pair<std::string, int>(classVal, count++));
    }

    m_intersectionCache = new IntersectionCache(*m_pols_ctner_coarserL, *m_positionsIdsCoarserL,
        *m_pols_ctner_finerL, *m_positionsIdsFinerL);
}

// Default destructor
gdma::fte::LandscapeFeatures::~LandscapeFeatures() {
    delete m_intersectionCache;
    m_intersectionCache = 0;
}

// Implementation of Template method
bool gdma::fte::LandscapeFeatures::getFeatures(
    std::map<std::string, gdma::fte::FeaturesSummary> &attr_container) {
    boost::bimap<std::string, std::size_t>::const_iterator itCells = m_positionsIdsCoarserL->begin();

    te::common::TaskProgress task(TE_TR("Computing Landscape Features"));
    task.setTotalSteps(m_pols_ctner_coarserL->size());
    task.useTimer(true);

    std::map<std::string, double> templateMap;
    std::map<std::string, int> numberOfPatches;
    while (itCells != m_positionsIdsCoarserL->end()) {
        // Computing classArea
        for (const auto &classVal : (*m_class_values_finerL)) {
            templateMap[classVal] = 0.0;
            numberOfPatches[classVal] = 0;
        }

        std::map<std::string, double> class_areas(templateMap);
        std::map<std::string, double> total_edges(templateMap);
        std::map<std::string, double> parcentLand(templateMap);
        std::map<std::string, double> auxiliaryMap(templateMap);
        std::map<std::string, double> taobi(templateMap);

        // -------------------
        // Class Area Feature

        getClassArea(itCells->right, class_areas);
        attr_container[itCells->left].addFeaturesFromMap(class_areas, "CA", m_class_map_codes);

        // -------------------
        // Percent Land Feature

        getPercentLand(itCells->right, class_areas, parcentLand);
        attr_container[itCells->left].addFeaturesFromMap(parcentLand, "PLAND", m_class_map_codes);

        // -------------------
        // Patch Density Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getPatchDensities(itCells->right, auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "PD", m_class_map_codes);

        // -------------------
        // Number Of Patches Feature

        getNumberOfPatches(itCells->right, numberOfPatches);
        attr_container[itCells->left].addFeaturesFromMap(numberOfPatches, "NP", m_class_map_codes);

        // -------------------
        // Mean Patch Sizes Feature

        // Initializes map that will store the values
        std::map<std::string, double> meanPatchSizes(templateMap);
        getMeanPatchSizes(itCells->right, numberOfPatches, meanPatchSizes);
        attr_container[itCells->left].addFeaturesFromMap(meanPatchSizes, "MPS", m_class_map_codes);

        // -------------------
        // Patch Sizes Standard Deviation Feature

        // Initializes map that will store the values
        std::map<std::string, double> patchSizesSTD(templateMap);
        getPatchSizesStd(itCells->right, meanPatchSizes, numberOfPatches, patchSizesSTD);
        attr_container[itCells->left].addFeaturesFromMap(patchSizesSTD, "PSSD", m_class_map_codes);

        // -------------------
        // Landscape Shape Index Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getLandscapeShapeIndexes(itCells->right, auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "LSI", m_class_map_codes);

        // -------------------
        // Mean Shape Index Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getMeanShapeIndexes(itCells->right, numberOfPatches, auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "MSI", m_class_map_codes);

        // -------------------
        // Area-Weighted Mean Shape Index Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getAWMeanShapeIndexes(itCells->right, class_areas, auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "AWMSI", m_class_map_codes);

        // -------------------
        // Mean Patch Fractal Dimension Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getMeanPatchFractalDimensions(itCells->right, numberOfPatches, auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "MPFD", m_class_map_codes);

        // -------------------
        // Mean Patch Fractal Dimension Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getAreaWeightedMeanPatchFractalDimensions(itCells->right, class_areas, auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "AWMPFD",
            m_class_map_codes);

        // -------------------
        // Total Edges Feature

        getTotalEdges(itCells->right, total_edges);
        attr_container[itCells->left].addFeaturesFromMap(total_edges, "TE", m_class_map_codes);

        // -------------------
        // Edge Density Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getEdgeDensities(itCells->right, total_edges, auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "ED", m_class_map_codes);

        // -------------------
        // Mean Perimeter Area Ratio Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getMeanPerimeterAreaRatios(itCells->right, numberOfPatches, auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "MPAR", m_class_map_codes);

        // -------------------
        // Patch Size Coefficient of Variation Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getPatchSizeCoeffVariation(meanPatchSizes, patchSizesSTD, auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "PSCOV", m_class_map_codes);

        // -------------------
        // Patch Richness Feature
        int patchRichness = getPatchRichness(itCells->right);
        attr_container[itCells->left].addFeature("PATCH_RICHNESS", "PR", patchRichness);

        // -------------------
        // Interspersion and Juxtaposition Index Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getInterspersionAndJuxtapositionIndex(itCells->right, total_edges, patchRichness,
            auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "IJI", m_class_map_codes);

        // -------------------
        // Total Area of the Biggest Object Feature

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getTotalAreaBiggestObject(itCells->right, auxiliaryMap);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "TABO", m_class_map_codes);

        // -------------------
        // Total Area of the Object with Biggest Intersection and the Biggest Intersection Area

        // reset map that will store the values
        auxiliaryMap = *(new std::map<std::string, double>(templateMap));
        getBiggestIntersectionAreas(itCells->right, auxiliaryMap, taobi);
        attr_container[itCells->left].addFeaturesFromMap(auxiliaryMap, "BIA", m_class_map_codes);
        attr_container[itCells->left].addFeaturesFromMap(taobi, "TAOBIA", m_class_map_codes);

        // -------------------
        // Patch Richness Density Feature
        double patchRichnessDensity = getPatchRichnessDensity(itCells->right, patchRichness);
        attr_container[itCells->left].addFeature("PATCH_RICHN_DENSITY", "PRD",
            patchRichnessDensity);

        // -------------------
        // Shannon's Diversity Index Feature
        double shannonDivIndex = getShannonsDiversityIndex(itCells->right, parcentLand);
        attr_container[itCells->left].addFeature("SHANNON_DIV_INDEX", "SHDI", shannonDivIndex);

        // -------------------
        // Simpson's Diversity Index Feature
        double simpsonDivIndex = getSimpsonsDiversityIndex(itCells->right, parcentLand);
        attr_container[itCells->left].addFeature("SIMPSON_DIV_INDEX", "SIDI", simpsonDivIndex);

        // -------------------
        // Shannon's Evenness Index Feature
        double shannonEvenIndex = getShannonsEvennessIndex(shannonDivIndex, patchRichness);
        attr_container[itCells->left].addFeature("SHANNON_EVEN_INDEX", "SHEI", shannonEvenIndex);

        // -------------------
        // Simpson's Evenness Index Feature
        double simpsonEvenIndex = getSimpsonsEvennessIndex(simpsonDivIndex, patchRichness);
        attr_container[itCells->left].addFeature("SIMPSON_EVEN_INDEX", "SIEI", simpsonEvenIndex);

        if (task.isActive() == false)
            throw te::common::Exception(TE_TR("Operation canceled!"));

        task.pulse();
        itCells++;
    }

    return true;
}

void gdma::fte::LandscapeFeatures::getClassArea(std::size_t cellPos,
    std::map<std::string, double> &classAreas) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            double patchArea;
            computePolygonArea(intersectionVector, patchArea);
            classAreas[(*m_classes_finerL)[itIntersect.index()]] += (patchArea / 10000.0);
            itIntersect++;
        }
    } else if (!isValid) {
        for (auto &classArea : classAreas)
            classArea.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getPercentLand(std::size_t cellPos,
    std::map<std::string, double> &class_areas, std::map<std::string, double> &percent_lands) {

    std::vector<te::gm::Geometry*> polygonVector;
    gdma::fte::Multi2SilgleFilterPolygonsRestrict((*m_pols_ctner_coarserL)[cellPos], polygonVector);

    double cellArea = 0.0;
    computePolygonArea(polygonVector, cellArea);
    cellArea /= 10000.0;

    for(auto classArea : class_areas) {
        if (classArea.second != m_dummyValueDouble)
            percent_lands[classArea.first] = (classArea.second / cellArea) * 100;
        else
            percent_lands[classArea.first] = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getPatchDensities(std::size_t cellPos,
    std::map<std::string, double> &patch_densities) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (isValid) {
        if (intersectionGeoms.nnz() > 0) {
            gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
            while (itIntersect != intersectionGeoms.end()) {
                patch_densities[(*m_classes_finerL)[itIntersect.index()]]++;
                itIntersect++;
            }
        }


        std::vector<te::gm::Geometry*> polygonVector;
        gdma::fte::Multi2SilgleFilterPolygonsRestrict(((*m_pols_ctner_coarserL)[cellPos]),
            polygonVector);

        std::map<std::string, double>::iterator it = patch_densities.begin();
        while (it != patch_densities.end()) {
            double landscapeArea = 0.0;
            computePolygonArea(polygonVector, landscapeArea);

            if (landscapeArea <= 0)
                landscapeArea = 0.00000001;

            it->second = (it->second / landscapeArea) * 100000.0 * 100;
            it++;
        }
    } else {
        for (auto &pd : patch_densities)
            pd.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getMeanPatchSizes(std::size_t cellPos,
    std::map<std::string, int> &numberOfPatches, std::map<std::string, double> &mean_patch_sizes) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            double patchArea;
            computePolygonArea(intersectionVector, patchArea);
            patchArea /= 10000.0;

            mean_patch_sizes[(*m_classes_finerL)[itIntersect.index()]] += patchArea;

            itIntersect++;
        }

        std::map<std::string, int>::iterator itCount = numberOfPatches.begin();
        while (itCount != numberOfPatches.end()) {
            if (itCount->second > 0)
                mean_patch_sizes[itCount->first] /= static_cast<double>(itCount->second);
            itCount++;
        }
    } else if (!isValid) {
        for (auto &mps : mean_patch_sizes)
            mps.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getPatchSizesStd(std::size_t cellPos,
    std::map<std::string, double> &mean_patch_sizes, std::map<std::string, int> &numberOfPatches,
    std::map<std::string, double> &patch_sizes_std) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            double patchArea = 0.0;
            computePolygonArea(intersectionVector, patchArea);
            patchArea /= 10000.0;

            patch_sizes_std[(*m_classes_finerL)[itIntersect.index()]] += std::pow(
                patchArea - mean_patch_sizes[(*m_classes_finerL)[itIntersect.index()]],
                2);

            itIntersect++;
        }

        std::map<std::string, int>::iterator itCount = numberOfPatches.begin();
        while (itCount != numberOfPatches.end()) {
            if (itCount->second > 0) {
                patch_sizes_std[itCount->first] /= static_cast<double>(itCount->second);
                patch_sizes_std[itCount->first] = std::sqrt(patch_sizes_std[itCount->first]);
            }
            itCount++;
        }
    } else if (!isValid) {
        for (auto &elem : patch_sizes_std)
            elem.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getLandscapeShapeIndexes(std::size_t cellPos,
    std::map<std::string, double> &landscapeShapeIndexes) {

    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            double patchPerimeter;
            computePolygonPerimeter(intersectionVector, patchPerimeter);
            landscapeShapeIndexes[(*m_classes_finerL)[itIntersect.index()]] += patchPerimeter;

            itIntersect++;
        }

        double landscapeArea;
        std::vector<te::gm::Geometry*> cellGometries;
        gdma::fte::Multi2SilgleFilterPolygonsRestrict((*m_pols_ctner_coarserL)[cellPos],
            cellGometries);
        computePolygonArea(cellGometries, landscapeArea);

        for (auto &elem : landscapeShapeIndexes) {
            elem.second /= (2 * sqrt(boost::math::constants::pi<double>() * landscapeArea));
        }
    } else if (!isValid) {
        for (auto &lsi : landscapeShapeIndexes)
            lsi.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getMeanShapeIndexes(std::size_t cellPos,
    std::map<std::string, int> &numberOfPatches, std::map<std::string, double> &meanShapeIndexes) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            double patchArea, patchPerimeter;
            computePatchAreaAndPerimeter(intersectionVector, patchArea, patchPerimeter);

            meanShapeIndexes[(*m_classes_finerL)[itIntersect.index()]] +=
               (patchPerimeter / (2 * std::sqrt(boost::math::constants::pi<double>() * patchArea)));

            itIntersect++;
        }

        std::map<std::string, int>::iterator itCount = numberOfPatches.begin();
        while (itCount != numberOfPatches.end()) {
            if (itCount->second > 0)
                meanShapeIndexes[itCount->first] /= static_cast<double>(itCount->second);
            itCount++;
        }
    } else if (!isValid) {
        for (auto &msi : meanShapeIndexes)
            msi.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getAWMeanShapeIndexes(std::size_t cellPos,
    std::map<std::string, double> &class_areas, std::map<std::string, double> &awMeanShapeIndexes) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            double patchArea, patchPerimeter;
            computePatchAreaAndPerimeter(intersectionVector, patchArea, patchPerimeter);

            awMeanShapeIndexes[(*m_classes_finerL)[itIntersect.index()]] +=
                (patchPerimeter / (2 * std::sqrt(boost::math::constants::pi<double>() * patchArea)))
                * (patchArea / (class_areas[(*m_classes_finerL)[itIntersect.index()]] * 10000.0));

            itIntersect++;
        }
    } else if (!isValid) {
        for (auto &awmsi : awMeanShapeIndexes)
            awmsi.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getMeanPatchFractalDimensions(std::size_t cellPos,
    std::map<std::string, int> numberOfPatches, std::map<std::string, double> &mpfds) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            double patchArea, patchPerimeter;
            computePatchAreaAndPerimeter(intersectionVector, patchArea, patchPerimeter);

            mpfds[(*m_classes_finerL)[itIntersect.index()]] +=
                ((2 * std::log(patchPerimeter)) / std::log(patchArea));

            itIntersect++;
        }

        for (auto &elem : mpfds) {
            int totalPatches = numberOfPatches[elem.first];
            if (totalPatches > 0)
                elem.second /= static_cast<double>(totalPatches);
        }
    } else if (!isValid) {
        for (auto &elem : mpfds)
            elem.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getAreaWeightedMeanPatchFractalDimensions(std::size_t cellPos,
    std::map<std::string, double> &class_areas, std::map<std::string, double> &awmpfds) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            double patchArea, patchPerimeter;
            computePatchAreaAndPerimeter(intersectionVector, patchArea, patchPerimeter);

            awmpfds[(*m_classes_finerL)[itIntersect.index()]] +=
                ((2 * std::log(patchPerimeter)) / std::log(patchArea)) *
                (patchArea / (class_areas[(*m_classes_finerL)[itIntersect.index()]] * 10000.0));

            itIntersect++;
        }
    } else if (!isValid) {
        for (auto &elem : awmpfds)
            elem.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getEdgeDensities(std::size_t cellPos,
    std::map<std::string, double> &class_perimeters, std::map<std::string, double> &edgeDensities) {
    std::vector<te::gm::Geometry*> polygonVector;
    gdma::fte::Multi2SilgleFilterPolygonsRestrict(((*m_pols_ctner_coarserL)[cellPos]),
        polygonVector);

    double landscapeArea = 0.0;
    computePolygonArea(polygonVector, landscapeArea);

    if (landscapeArea <= 0)
        landscapeArea = 0.00000001;

    std::map<std::string, double>::iterator it = edgeDensities.begin();
    while (it != edgeDensities.end()) {
        if (class_perimeters[it->first] != m_dummyValueDouble)
            it->second = (10000.0 * class_perimeters[it->first] / landscapeArea);
        else
            it->second = m_dummyValueDouble;
        it++;
    }
}

void gdma::fte::LandscapeFeatures::getTotalEdges(std::size_t cellPos,
    std::map<std::string, double> &total_edges) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            if (intersectionVector.size() > 0) {
                for (std::size_t pos = 0; pos < intersectionVector.size(); pos++) {
                    te::gm::Polygon* polIntersect = static_cast<te::gm::Polygon*>(
                        intersectionVector[pos]);

                    total_edges[(*m_classes_finerL)[itIntersect.index()]] +=
                        polIntersect->getPerimeter();
                }
            }
            itIntersect++;
        }
    } else if (!isValid) {
        for (auto &elem : total_edges)
            elem.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getMeanPerimeterAreaRatios(std::size_t cellPos,
    std::map<std::string, int> &numberOfPatches,
    std::map<std::string, double> &meanPerimAreaRatios) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();

        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            double patchArea, patchPerimeter;
            computePatchAreaAndPerimeter(intersectionVector, patchArea, patchPerimeter);

            meanPerimAreaRatios[(*m_classes_finerL)[itIntersect.index()]] +=
                (patchPerimeter / patchArea);

            itIntersect++;
        }

        std::map<std::string, int>::iterator itCount = numberOfPatches.begin();
        while (itCount != numberOfPatches.end()) {
            if (itCount->second > 0)
                meanPerimAreaRatios[itCount->first] /= static_cast<double>(itCount->second);
            itCount++;
        }
    } else if (!isValid) {
        for (auto &elem : meanPerimAreaRatios)
            elem.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getNumberOfPatches(std::size_t cellPos,
    std::map<std::string, int> &numbers_of_patches) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();

        while (itIntersect != intersectionGeoms.end()) {
            numbers_of_patches[(*m_classes_finerL)[itIntersect.index()]]++;
            itIntersect++;
        }
    } else if (!isValid) {
        for (auto &elem : numbers_of_patches)
            elem.second = m_dummyValueInt;
    }
}

void gdma::fte::LandscapeFeatures::getPatchSizeCoeffVariation(
    std::map<std::string, double> &mean_patch_sizes, std::map<std::string, double> &patch_sizes_std,
    std::map<std::string, double> &psCoeffVariation) {
    for (auto &mps : mean_patch_sizes) {
        if (mps.second != m_dummyValueDouble) {
            if (mps.second > 0)
                psCoeffVariation[mps.first] = (patch_sizes_std[mps.first] / mps.second) * 100;
        } else {
            psCoeffVariation[mps.first] = m_dummyValueDouble;
        }
    }
}

void gdma::fte::LandscapeFeatures::getInterspersionAndJuxtapositionIndex(std::size_t cellPos,
    std::map<std::string, double> &total_edges, int patchRichness,
    std::map<std::string, double> &ijIndexes) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();

        int numberOfClasses = patchRichness;

        double totalEdgesAllClasses = 0.0;
        for (auto &tedge : total_edges)
            totalEdgesAllClasses += tedge.second;

        if (numberOfClasses >= 3) {
            itIntersect = intersectionGeoms.begin();

            while (itIntersect != intersectionGeoms.end()) {
                std::vector<te::gm::Geometry*> intersectionVector;
                gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

                for (std::size_t pos = 0; pos < intersectionVector.size(); pos++) {
                    te::gm::Polygon* polIntersection =
                        static_cast<te::gm::Polygon*>(intersectionVector[pos]);
                    ijIndexes[(*m_classes_finerL)[itIntersect.index()]] +=
                        (polIntersection->getPerimeter() / totalEdgesAllClasses) *
                        std::log(polIntersection->getPerimeter() / totalEdgesAllClasses);
                }
                itIntersect++;
            }

            for (auto &ijIndex : ijIndexes) {
                ijIndexes[ijIndex.first] = -ijIndexes[ijIndex.first];
                ijIndexes[ijIndex.first] /= std::log(static_cast<double>(numberOfClasses)-1);
                ijIndexes[ijIndex.first] *= 100;
            }
        }
    } else if (!isValid) {
        for (auto &elem : ijIndexes)
            elem.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getTotalAreaBiggestObject(std::size_t cellPos,
    std::map<std::string, double> &tabo) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*m_pols_ctner_finerL)[itIntersect.index()],
                intersectionVector);

            double polArea;
            computePolygonArea(intersectionVector, polArea);
            polArea /= 10000.0;

            if (polArea > tabo[(*m_classes_finerL)[itIntersect.index()]])
                tabo[(*m_classes_finerL)[itIntersect.index()]] = polArea;

            itIntersect++;
        }
    } else if (!isValid) {
        for (auto &elem : tabo)
            elem.second = m_dummyValueDouble;
    }
}

void gdma::fte::LandscapeFeatures::getBiggestIntersectionAreas(std::size_t cellPos,
    std::map<std::string, double> &bia, std::map<std::string, double> &taobia) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();
        while (itIntersect != intersectionGeoms.end()) {
            std::vector<te::gm::Geometry*> intersectionVector;
            gdma::fte::Multi2SilgleFilterPolygons((*itIntersect).get(), intersectionVector);

            double intersectionArea;
            computePolygonArea(intersectionVector, intersectionArea);
            intersectionArea /= 10000.0;

            std::size_t polIdx = itIntersect.index();
            std::string polId = (*m_classes_finerL)[polIdx];
            if (intersectionArea > bia[polId]) {
                bia[polId] = intersectionArea;

                std::vector<te::gm::Geometry*> polsVector;
                gdma::fte::Multi2SilgleFilterPolygons((*m_pols_ctner_finerL)[polIdx], polsVector);

                double polArea;
                computePolygonArea(polsVector, polArea);
                polArea /= 10000.0;

                taobia[polId] = polArea;
            }

            itIntersect++;
        }
    } else if (!isValid) {
        for (auto &elem : bia) {
            elem.second = m_dummyValueDouble;
            taobia[elem.first] = m_dummyValueDouble;
        }
    }
}

int gdma::fte::LandscapeFeatures::getPatchRichness(std::size_t cellPos) {
    gdma::fte::MatrixLine intersectionGeoms;
    bool isValid = m_intersectionCache->searchForIntersections(cellPos, intersectionGeoms);

    std::set<std::string> patchTypes;

    if (intersectionGeoms.nnz() > 0) {
        gdma::fte::MatrixLine::const_iterator itIntersect = intersectionGeoms.begin();

        while (itIntersect != intersectionGeoms.end()) {
            patchTypes.insert((*m_classes_finerL)[itIntersect.index()]);
            itIntersect++;
        }
    } else if (!isValid) {
        return m_dummyValueInt;
    }

    return patchTypes.size();
}

double gdma::fte::LandscapeFeatures::getPatchRichnessDensity(std::size_t cellPos,
    int &patchRichness) {
    if (patchRichness != m_dummyValueInt) {
        if (patchRichness > 0) {
            std::vector<te::gm::Geometry*> polygonVector;
            gdma::fte::Multi2SilgleFilterPolygonsRestrict((*m_pols_ctner_coarserL)[cellPos],
                polygonVector);

            double landscapeArea;
            computePolygonArea(polygonVector, landscapeArea);

            return (static_cast<double>(patchRichness) / landscapeArea) * 10000.0 * 100.0;
        } else {
            return 0.0;
        }
    } else {
        return m_dummyValueDouble;
    }
}

double gdma::fte::LandscapeFeatures::getShannonsDiversityIndex(std::size_t cellPos,
    std::map<std::string, double> &percent_land) {
    double shannonIndex = 0.0;

    for (auto &prop : percent_land) {
        if (prop.second != m_dummyValueDouble) {
            double proportion = prop.second / 100;
            if (proportion != 0.0)
                shannonIndex += (proportion * std::log(proportion));
        } else {
            shannonIndex = m_dummyValueDouble;
            return shannonIndex;
        }
    }

    return -shannonIndex;
}

double gdma::fte::LandscapeFeatures::getSimpsonsDiversityIndex(std::size_t cellPos,
    std::map<std::string, double> &percent_land) {
    double simpsonIndex = 0.0;

    for (auto &prop : percent_land) {
        if (prop.second != m_dummyValueDouble) {
            double proportion = prop.second / 100;
            simpsonIndex += std::pow(proportion, 2);
        } else {
            simpsonIndex = m_dummyValueDouble;
            return simpsonIndex;
        }
    }

    return 1 - simpsonIndex;
}

double gdma::fte::LandscapeFeatures::getShannonsEvennessIndex(double &shannonDivIndex,
    int &patchRichness) {
    if (patchRichness != m_dummyValueInt) {
        if (patchRichness > 1)
            return shannonDivIndex / std::log(static_cast<double>(patchRichness));
        else
            return 0.0;
    } else {
        return m_dummyValueDouble;
    }
}

double gdma::fte::LandscapeFeatures::getSimpsonsEvennessIndex(double &simpsonDivIndex,
    int &patchRichness) {
    if (patchRichness != m_dummyValueInt) {
        if (patchRichness > 1)
            return (simpsonDivIndex / (1 - (1 / static_cast<double>(patchRichness))));
        else
            return 0.0;
    } else {
        return m_dummyValueDouble;
    }
}

const std::map<std::string, int> &gdma::fte::LandscapeFeatures::getMapClassNames() {
    return m_class_map_codes;
}

double gdma::fte::LandscapeFeatures::getDummyValue() {
    return m_dummyValueDouble;
}

// --------------------------------------------------------------
// Protected Members

void gdma::fte::LandscapeFeatures::computePatchAreaAndPerimeter(
    std::vector<te::gm::Geometry*> &geometryVector, double &area, double &perimeter) {
    area = 0.0;
    perimeter = 0.0;
    for (int posInt = 0; posInt < geometryVector.size(); posInt++) {
        area += static_cast<te::gm::Polygon*>(geometryVector[posInt])->getArea();
        perimeter +=
            static_cast<te::gm::Polygon*>(geometryVector[posInt])->getPerimeter();
    }
}

void gdma::fte::LandscapeFeatures::computePolygonArea(
    std::vector<te::gm::Geometry*> &geometryVector, double &area) {
    area = 0.0;
    for (int posInt = 0; posInt < geometryVector.size(); posInt++) {
        area += static_cast<te::gm::Polygon*>(geometryVector[posInt])->getArea();
    }
}

void gdma::fte::LandscapeFeatures::computePolygonPerimeter(
    std::vector<te::gm::Geometry*> &geometryVector, double &perimeter) {
    perimeter = 0.0;
    for (int posInt = 0; posInt < geometryVector.size(); posInt++) {
        perimeter +=
            static_cast<te::gm::Polygon*>(geometryVector[posInt])->getPerimeter();
    }
}
