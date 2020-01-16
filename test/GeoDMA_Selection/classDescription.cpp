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
  \file gdma/lib/classification/classTypology.cpp

  \brief This file contains the ClassTypology class definition.

  \author Raian Vargas Maretto
  \author Thales Sehn Korting
  \author Emiliano Ferreira Castejon
*/

// TerraLib Includes
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/dataaccess/utils/Utils.h>

// STL Includes
#include <map>
#include <set>
#include <string>

// GeoDMA Includes
#include "geodmalib/classification/classDescription.hpp"

gdma::cl::ClassDescription::ClassDescription() :
    m_sampleSet(0) {
}

gdma::cl::ClassDescription::ClassDescription(std::string classLabel,
    const te::da::DataSetType* schema, std::string classColor) {
    m_label = classLabel;
    m_color = classColor;

    m_sampleSet = 0;
    te::da::GetEmptyOIDSet(schema, m_sampleSet);
}

gdma::cl::ClassDescription::ClassDescription(const gdma::cl::ClassDescription& other) {
    this->m_label = other.m_label;
    this->m_color = other.m_color;
    this->m_metadata = other.m_metadata;
    this->m_sampleSet = other.m_sampleSet->clone();
}

gdma::cl::ClassDescription::~ClassDescription() {
    delete m_sampleSet;
    m_sampleSet = 0;
}

gdma::cl::ClassDescription& gdma::cl::ClassDescription::operator=(
    const gdma::cl::ClassDescription& rhs) {
    m_label = rhs.m_label;
    m_color = rhs.m_color;
    m_metadata = rhs.m_metadata;
    m_sampleSet = rhs.m_sampleSet->clone();
    return *this;
}

void gdma::cl::ClassDescription::setLabel(std::string newLabel) {
    m_label = newLabel;
}

std::string gdma::cl::ClassDescription::getLabel() {
    return m_label;
}

void gdma::cl::ClassDescription::setColor(std::string newColor) {
    m_color = newColor;
}

std::string gdma::cl::ClassDescription::getColor() {
    return m_color;
}

int gdma::cl::ClassDescription::getMetadataSize() {
    return m_metadata.size();
}

void gdma::cl::ClassDescription::addMetadata(std::string name, std::string description) {
    m_metadata[name] = description;
}

bool gdma::cl::ClassDescription::removeMetadata(std::string name) {
    std::map<std::string, std::string>::iterator it = m_metadata.find(name);

    if (it != m_metadata.end()) {
        m_metadata.erase(it);
        return true;
    } else {
        return false;
    }
}

std::string gdma::cl::ClassDescription::getMetadataValue(std::string name) {
    std::map<std::string, std::string>::iterator it = m_metadata.find(name);

    if (it != m_metadata.end())
        return it->second;
    else
        return "";
}

const std::map<std::string, std::string>& gdma::cl::ClassDescription::getAllMetadata() {
    return m_metadata;
}

void gdma::cl::ClassDescription::addSample(te::da::ObjectId* oid) {
    m_sampleSet->add(oid->clone());
    m_sampleSetString.insert(oid->getValueAsString());
}

void gdma::cl::ClassDescription::removeSample(te::da::ObjectId* oid) {
    m_sampleSetString.erase(oid->getValueAsString());
    m_sampleSet->remove(oid);
}

te::da::ObjectId* gdma::cl::ClassDescription::getSample(std::string id) {
    for (auto &oid : *m_sampleSet) {
        if (oid->getValueAsString() == id)
            return oid;
    }
}

void gdma::cl::ClassDescription::removeSample(std::string id) {
    te::da::ObjectId* oidToRemove;
    for (auto &oid : *m_sampleSet) {
        if (oid->getValueAsString() == id) {
            oidToRemove = oid;
            break;
        }
    }

    m_sampleSetString.erase(oidToRemove->getValueAsString());
    m_sampleSet->remove(oidToRemove);
    delete oidToRemove;
    oidToRemove = 0;
}

bool gdma::cl::ClassDescription::containsId(te::da::ObjectId* oid) {
    return m_sampleSet->contains(oid);
}

bool gdma::cl::ClassDescription::containsId(std::string id) {
    return m_sampleSetString.find(id) != m_sampleSetString.end();
}

int gdma::cl::ClassDescription::getNumberOfSamples() {
    return m_sampleSet->size();
}

const te::da::ObjectIdSet* gdma::cl::ClassDescription::getSampleSet() {
    return m_sampleSet;
}

const std::set<std::string>& gdma::cl::ClassDescription::getSampleSetAsString() {
    return m_sampleSetString;
}
