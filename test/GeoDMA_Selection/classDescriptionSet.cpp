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
  \file gdma/lib/classification/sampleSet.cpp

  \brief This file contains the SampleSet class implementation.

  \author Raian Vargas Maretto
  \author Thales Sehn Korting
  \author Emiliano Ferreira Castejon
*/

// TerraLib Includes
#include <terralib/common/STLUtils.h>
#include <terralib/common/progress/TaskProgress.h>
#include <terralib/classification/ROI.h>
#include <terralib/dataaccess.h>

// STL Includes
#include <map>
#include <string>
#include <utility>
#include <vector>

// GeoDMA Includes
#include "geodmalib/classification/classDescription.hpp"
#include "geodmalib/classification/classDescriptionSet.hpp"
#include "geodmalib/common/messageMediator.hpp"
#include "geodmalib/common/utils.hpp"
#include "geodmalib/datamanager/dataManager.hpp"
#include "geodmalib/featureextraction/featuresSummary.hpp"

gdma::cl::ClassDescriptionSet::ClassDescriptionSet() {
}

gdma::cl::ClassDescriptionSet::ClassDescriptionSet(const ClassDescriptionSet& other) {
    this->m_classSet = other.m_classSet;
}

gdma::cl::ClassDescriptionSet& gdma::cl::ClassDescriptionSet::operator=(
    const gdma::cl::ClassDescriptionSet& rhs) {
    this->m_classSet = rhs.m_classSet;
    return *this;
}

gdma::cl::ClassDescriptionSet::~ClassDescriptionSet() {
}

bool gdma::cl::ClassDescriptionSet::addClass(gdma::cl::ClassDescription& clazz) {
    std::map<std::string, gdma::cl::ClassDescription>::iterator it =
        m_classSet.find(clazz.getLabel());
    if (it == m_classSet.end()) {
        m_classSet[clazz.getLabel()] = clazz;
        return true;
    } else {
        return false;
    }
}

bool gdma::cl::ClassDescriptionSet::removeClass(std::string label) {
    std::map<std::string, gdma::cl::ClassDescription>::iterator it = m_classSet.find(label);
    if (it != m_classSet.end()) {
        m_classSet.erase(it);
        return true;
    } else {
        return false;
    }
}

gdma::cl::ClassDescription* gdma::cl::ClassDescriptionSet::getClass(std::string label) {
    std::map<std::string, gdma::cl::ClassDescription>::iterator it = m_classSet.find(label);
    if (it != m_classSet.end())
        return &m_classSet[label];
    else
        return 0;
}

bool gdma::cl::ClassDescriptionSet::setClassLabel(ClassDescription& clazz,
    std::string newLabel) {
    std::string oldLabel = clazz.getLabel();

    std::map<std::string, gdma::cl::ClassDescription>::iterator it =
        m_classSet.find(oldLabel);

    if (it == m_classSet.end())
        return false;

    clazz.setLabel(newLabel);
    m_classSet[newLabel] = clazz;

    m_classSet.erase(it);
    return true;
}

int gdma::cl::ClassDescriptionSet::getNumberOfClasses() {
    return m_classSet.size();
}

int gdma::cl::ClassDescriptionSet::getNumberOfSamples(std::string classLabel) {
    gdma::cl::ClassDescription* clazz = this->getClass(classLabel);

    if (clazz == 0) {
        std::stringstream msg;
        msg << "Class \"" << classLabel << "\" does not exist!";
        gdma::common::MessageMediator::getInstance().sendErrorMsg(msg.str());
        return 0;
    }
    return clazz->getNumberOfSamples();
}

std::map<std::string, gdma::cl::ClassDescription>::iterator gdma::cl::ClassDescriptionSet::begin() {
    return m_classSet.begin();
}

std::map<std::string, gdma::cl::ClassDescription>::iterator gdma::cl::ClassDescriptionSet::end() {
    return m_classSet.end();
}

void gdma::cl::ClassDescriptionSet::clear() {
    m_classSet.clear();
}

int gdma::cl::ClassDescriptionSet::getTotalNumberOfSamples() {
    int total = 0;

    for (auto &clazz : m_classSet)
        total += clazz.second.getNumberOfSamples();

    return total;
}

bool gdma::cl::ClassDescriptionSet::saveSamplesInDataSource(std::string dataSourceId,
    std::string columnName) {
    std::map<std::string, gdma::fte::FeaturesSummary> attr_ctner;

    for (auto &clazz : m_classSet) {
        const te::da::ObjectIdSet* sampleSet = clazz.second.getSampleSet();
        if (sampleSet && sampleSet->size() > 0) {
            for (auto &oid : *sampleSet) {
                gdma::fte::FeaturesSummary featureSum;
                featureSum.addFeature(columnName, columnName, clazz.first);
                attr_ctner[oid->getValueAsString()] = featureSum;
            }
        }
    }

    return gdma::dm::updateDataSource(dataSourceId, attr_ctner);
}

bool gdma::cl::ClassDescriptionSet::loadSamplesFromDataSource(std::string dataSourceId,
    std::string columnName) {
    if (m_classSet.size() > 0)
        m_classSet.clear();

    te::da::DataSourcePtr dataSource = te::da::GetDataSource(dataSourceId);
    te::da::DataSourceTransactorPtr transactor = dataSource->getTransactor();
    std::vector<std::string> dsNames = transactor->getDataSetNames();

    for (auto &dsName : dsNames) {
        te::da::DataSetTypePtr dsType = dataSource->getDataSetType(dsName);
        te::da::DataSetPtr dataSet = transactor->getDataSet(dsName);

        te::common::TaskProgress task(TE_TR("Loading Data"));
        task.setTotalSteps(dataSet->size());
        task.useTimer(true);

        // Get object id properties from the dataset.
        std::vector<std::string> pnames;
        te::da::GetOIDPropertyNames(dsType.get(), pnames);

        dataSet->moveBeforeFirst();
        std::size_t samplesPos = te::da::GetPropertyPos(dataSet.get(), columnName);

        if (samplesPos == std::string::npos) {
            std::stringstream msg;
            msg << "Column \"" << columnName << "\" does not exist in the data set!";
            gdma::common::MessageMediator::getInstance().sendErrorMsg(msg.str());
            return false;
        }

        while (dataSet->moveNext()) {
            std::string classLabel = dataSet->getString(samplesPos);

            if (classLabel.compare("") != 0) {
                gdma::cl::ClassDescription* clazz = this->getClass(classLabel);

                if (clazz == 0) {
                    std::string color = gdma::common::generateRandomHexColor();
                    clazz = new gdma::cl::ClassDescription(classLabel, dsType.get(), color);
                    m_classSet.insert(std::pair<std::string, gdma::cl::ClassDescription>(classLabel,
                        *clazz));
                }

                te::da::ObjectId oid = *te::da::GenerateOID(dataSet.get(), pnames);
                clazz = this->getClass(classLabel);
                clazz->addSample(oid.clone());
            }

            if (task.isActive() == false)
                throw te::common::Exception(TE_TR("Operation canceled!"));
            task.pulse();
        }
    }
    return true;
}
