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
  \file gdma/lib/featureextraction/featuresSummary.cpp

  \brief A structure to hold the set of attributes.

  \author Raian Vargas Maretto
  \author Thales Sehn Korting
  \author Emiliano Ferreira Castejon
*/

// TerraLib Includes
#include <terralib/statistics/core/NumericStatisticalSummary.h>

// STL Includes
#include <map>
#include <string>
#include <vector>

// GeoDMA includes
#include "geodmalib/featureextraction/featuresSummary.hpp"

// Default Constructor
gdma::fte::FeaturesSummary::FeaturesSummary() :
    m_features() {
}

// Copy Constructor
gdma::fte::FeaturesSummary::FeaturesSummary(const FeaturesSummary &rhs) :
    m_features() {
    m_features = rhs.m_features;
}

// Default Destructor
gdma::fte::FeaturesSummary::~FeaturesSummary() {
    clear();
}

// Add String value as feature
void gdma::fte::FeaturesSummary::addFeature(std::string name, std::string shortName,
    std::string value) {
    gdma::fte::Attribute attrib(name, shortName, value);
    m_features.push_back(attrib);
}

// Add int 16 bit value as feature
void gdma::fte::FeaturesSummary::addFeature(std::string name, std::string shortName,
    int16_t value) {
    gdma::fte::Attribute attrib(name, shortName, value);
    m_features.push_back(attrib);
}

// Add int 32 bit value as feature
void gdma::fte::FeaturesSummary::addFeature(std::string name, std::string shortName,
    int32_t value) {
    gdma::fte::Attribute attrib(name, shortName, value);
    m_features.push_back(attrib);
}

// Add int 64 bit value as feature
void gdma::fte::FeaturesSummary::addFeature(std::string name, std::string shortName,
    int64_t value) {
    gdma::fte::Attribute attrib(name, shortName, value);
    m_features.push_back(attrib);
}

// Add float value as feature
void gdma::fte::FeaturesSummary::addFeature(std::string name, std::string shortName,
    float value) {
    gdma::fte::Attribute attrib(name, shortName, value);
    m_features.push_back(attrib);
}

// Add float value as feature
void gdma::fte::FeaturesSummary::addFeature(std::string name, std::string shortName,
    double value) {
    gdma::fte::Attribute attrib(name, shortName, value);
    m_features.push_back(attrib);
}

// Add Statistical summary
void gdma::fte::FeaturesSummary::addStatistics(te::stat::NumericStatisticalSummary statSummary,
    int band, double noDataValue) {
    // Amplitude
    std::string amplName = "AMPLITUDE_BAND_" + std::to_string(band);
    std::string amplShortName = "AMPL_B" + std::to_string(band);
    this->addFeature(amplName, amplShortName, statSummary.m_amplitude);

    // Count
    std::string countName = "COUNT_BAND_" + std::to_string(band);
    std::string countShortName = "COUNT_B" + std::to_string(band);
    this->addFeature(countName, countShortName, statSummary.m_count);

    // Kurtosis
    std::string kurtName = "KURTOSIS_BAND_" + std::to_string(band);
    std::string kurtShortName = "KURT_B" + std::to_string(band);
    this->addFeature(kurtName, kurtShortName, statSummary.m_kurtosis);

    // Maximum Value
    std::string maxName = "MAXIMUM_VAL_BAND_" + std::to_string(band);
    std::string maxShortName = "MAX_VAL_B" + std::to_string(band);
    this->addFeature(maxName, maxShortName, statSummary.m_maxVal);

    // Mean
    std::string meanName = "MEAN_BAND_" + std::to_string(band);
    std::string meanShortName = "MEAN_B" + std::to_string(band);
    this->addFeature(meanName, meanShortName, statSummary.m_mean);

    // Median
    std::string medName = "MEDIAN_BAND_" + std::to_string(band);
    std::string medShortName = "MEDIAN_B" + std::to_string(band);
    this->addFeature(medName, medShortName, statSummary.m_median);

    // Minimum Value
    std::string minName = "MINIMUM_VAL_BAND_" + std::to_string(band);
    std::string minShortName = "MIN_VAL_B" + std::to_string(band);
    this->addFeature(minName, minShortName, statSummary.m_minVal);

    // Mode
    std::string modName = "MODE_BAND_" + std::to_string(band);
    std::string modShortName = "MODE_B" + std::to_string(band);
    this->addFeature(modName, modShortName, statSummary.m_mode[0]);

    // Number of Modes
    std::string NmodName = "NUM_MODES_BAND_" + std::to_string(band);
    std::string NmodShortName = "N_MOD_B" + std::to_string(band);
    if (statSummary.m_mode[0] != noDataValue)
        this->addFeature(NmodName, NmodShortName, static_cast<int>(statSummary.m_mode.size()));
    else
        this->addFeature(NmodName, NmodShortName, static_cast<int>(noDataValue));
    // Skewness
    std::string skwName = "SKEWNESS_BAND_" + std::to_string(band);
    std::string skwShortName = "SKEW_B" + std::to_string(band);
    this->addFeature(skwName, skwShortName, statSummary.m_skewness);

    // Standard Deviation
    std::string stdDevName = "STD_DEVIATION_BAND_" + std::to_string(band);
    std::string stdDevShortName = "STDDEV_B" + std::to_string(band);
    this->addFeature(stdDevName, stdDevShortName, statSummary.m_stdDeviation);

    // Sum
    std::string sumName = "SUM_BAND_" + std::to_string(band);
    std::string sumShortName = "SUM_B" + std::to_string(band);
    this->addFeature(sumName, sumShortName, statSummary.m_sum);

    // Valid Count
    std::string valCName = "VALID_COUNT_BAND_" + std::to_string(band);
    std::string valCShortName = "VLDCNT_B" + std::to_string(band);
    this->addFeature(valCName, valCShortName, statSummary.m_validCount);

    // Coefficient variation
    std::string varCoefName = "VAR_COEF_BAND_" + std::to_string(band);
    std::string varCoefShortName = "VARCOEF_B" + std::to_string(band);
    this->addFeature(varCoefName, varCoefShortName, statSummary.m_varCoeff);

    // Variance
    std::string varName = "VARIANCE_BAND_" + std::to_string(band);
    std::string varShortName = "VAR_B" + std::to_string(band);
    this->addFeature(varName, varShortName, statSummary.m_variance);
}

void gdma::fte::FeaturesSummary::addTextures(te::rp::Texture textures, int band,
    std::string direction) {
    // Contrast
    std::string name = "CONTRAST_" + direction + "_BAND_" + std::to_string(band);
    std::string shortName = "CONT" + direction + "_B" + std::to_string(band);
    this->addFeature(name, shortName, textures.m_contrast);

    // Dissimilarity
    name = "DISSIMILARITY_" + direction + "_BAND_" + std::to_string(band);
    shortName = "DIS" + direction + "_B" + std::to_string(band);
    this->addFeature(name, shortName, textures.m_dissimilarity);

    // Energy
    name = "ENERGY_" + direction + "_BAND_" + std::to_string(band);
    shortName = "ENERG" + direction + "_B" + std::to_string(band);
    this->addFeature(name, shortName, textures.m_energy);

    // Entropy
    name = "ENTROPY_" + direction + "_BAND_" + std::to_string(band);
    shortName = "ENTR" + direction + "_B" + std::to_string(band);
    this->addFeature(name, shortName, textures.m_entropy);

    // Homogeneity
    name = "HOMOGENEITY_" + direction + "_BAND_" + std::to_string(band);
    shortName = "HOMOG" + direction + "_B" + std::to_string(band);
    this->addFeature(name, shortName, textures.m_entropy);
}

void gdma::fte::FeaturesSummary::addFeaturesFromMap(std::map<std::string, double> &map_features,
    std::string featureName, std::map<std::string, int> &mapClassNames) {
    std::map<std::string, double>::iterator it = map_features.begin();

    while (it != map_features.end()) {
        std::stringstream name;
        std::stringstream shortName;
        name << featureName << "_" << it->first;
        shortName << featureName << "_" << mapClassNames[it->first];
        addFeature(name.str(), shortName.str(), it->second);
        it++;
    }
}

void gdma::fte::FeaturesSummary::addFeaturesFromMap(std::map<std::string, int> &map_features,
    std::string featureName, std::map<std::string, int> &mapClassNames) {
    std::map<std::string, int>::iterator it = map_features.begin();

    while (it != map_features.end()) {
        std::stringstream name;
        std::stringstream shortName;
        name << featureName << "_" << it->first;
        shortName << featureName << "_" << mapClassNames[it->first];
        addFeature(name.str(), shortName.str(), it->second);
        it++;
    }
}

// Assignment operator
gdma::fte::FeaturesSummary& gdma::fte::FeaturesSummary::operator=(
    const gdma::fte::FeaturesSummary &rhs) {
    clear();

    this->m_features = rhs.m_features;

    return *this;
}

// Clear the structure
void gdma::fte::FeaturesSummary::clear() {
    m_features.clear();
}

const std::vector<gdma::fte::Attribute>& gdma::fte::FeaturesSummary::getFeatures() const {
    return m_features;
}

// Print all features of the summary
std::string gdma::fte::FeaturesSummary::toString() {
    std::ostringstream output;
    output << "gdma::fte::FeaturesSummary::toString()" << std::endl;

    output << std::endl;
    output << "  FEATURES" << std::endl;

    for (int pos = 0; pos < this->m_features.size(); pos++) {
        output << "     " << this->m_features[pos].getName() << "(" <<
            this->m_features[pos].getShortName() << "): ";

        switch (this->m_features[pos].getValueType()) {
        case te::dt::STRING_TYPE:
            output << this->m_features[pos].getString() << " - STRING" << std::endl;
            break;
        case te::dt::INT16_TYPE:
            output << this->m_features[pos].getInt16() << " - INT16" << std::endl;
            break;
        case te::dt::INT32_TYPE:
            output << this->m_features[pos].getInt32() << " - INT32" << std::endl;
            break;
        case te::dt::INT64_TYPE:
            output << this->m_features[pos].getInt64() << " - INT64" << std::endl;
            break;
        case te::dt::FLOAT_TYPE:
            output << this->m_features[pos].getFloat() << " - FLOAT" << std::endl;
            break;
        case te::dt::DOUBLE_TYPE:
            output << this->m_features[pos].getDouble() << " - DOUBLE" << std::endl;
            break;
        default:
            break;
        }
    }

    return output.str();
}
