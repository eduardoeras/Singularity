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
  \file gdma/lib/common/commandLineProgViewer.cpp

  \brief Command line progress bar viewer, based on TerraLib AbstractProgressViewer.

  \author Raian Vargas Maretto
  \author Thales Sehn Korting
  \author Emiliano Ferreira Castejon
*/

// TerraLib Includes
#include <terralib/core/translator/Translator.h>
#include <terralib/common/progress/AbstractProgressViewer.h>
#include <terralib/common/progress/TaskProgress.h>

// STL includes
#include <chrono>
#include <iostream>
#include <map>
#include <sstream>
#include <cmath>

// GeoDMA includes
#include "geodmalib/common/commandLineProgViewer.hpp"
#include "geodmalib/common/utils.hpp"


gdma::common::CommandLineProgViewer::CommandLineProgViewer() :
    m_barWidth(80) {
    this->m_totalWidth = this->m_barWidth;
    this->m_startTime = std::chrono::high_resolution_clock::now();
    this->m_currentTime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - this->m_startTime);
}

gdma::common::CommandLineProgViewer::CommandLineProgViewer(int barWidth) {
    this->m_barWidth = this->m_totalWidth = barWidth;

    this->m_startTime = std::chrono::high_resolution_clock::now();
    this->m_currentTime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - this->m_startTime);
}


gdma::common::CommandLineProgViewer::~CommandLineProgViewer() {
    std::cout << std::endl;
}

void gdma::common::CommandLineProgViewer::addTask(te::common::TaskProgress* tsk, int taskId) {
    tsk->useTimer(false);
    m_tasks.insert(std::map<int, te::common::TaskProgress*>::value_type(taskId, tsk));

    updateMessage(-1);
    updateValue(-1);
}

void gdma::common::CommandLineProgViewer::removeTask(int taskId) {
    cancelTask(taskId);

    std::map<int, te::common::TaskProgress*>::iterator it = m_tasks.find(taskId);

    if (it != m_tasks.end()) {
        m_tasks.erase(it);
        updateMessage(-1);
    }

    if (m_tasks.empty()) {
        this->m_totalSteps = 0;
        this->m_currentStep = 0;
        this->m_progress = 0;
    }
}

void gdma::common::CommandLineProgViewer::cancelTask(int taskId) {
    std::map<int, te::common::TaskProgress*>::iterator it = m_tasks.find(taskId);

    if (it != m_tasks.end()) {
        // update total and current values
        this->m_totalSteps -= it->second->getTotalSteps();
        this->m_currentStep -= it->second->getCurrentStep();

        // double aux = static_cast<double>(m_currentStep) / static_cast<double>(m_totalSteps);

        // this->m_progress = static_cast<int>(1.0 / this->m_totalSteps);
    }
}

void gdma::common::CommandLineProgViewer::updateValue(int taskId) {
    for (auto &task : m_tasks)
        this->m_currentStep += task.second->getCurrentStep();

    this->m_progress = static_cast<double>(this->m_currentStep) / static_cast<double>(m_totalSteps);

    int previousPosition = this->m_positionBar;
    this->m_positionBar = this->m_barWidth * this->m_progress;

    std::chrono::duration<int> time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - this->m_startTime);

    if ((this->m_positionBar > previousPosition) || time > this->m_currentTime) {
        this->m_currentTime = time;

        setBar();

        std::cout << m_message << " " << this->m_barStr << "\r" << std::flush;
    }
}

void gdma::common::CommandLineProgViewer::setTotalValues(int taskId) {
    m_totalSteps += m_tasks[taskId]->getTotalSteps();
}

void gdma::common::CommandLineProgViewer::updateMessage(int taskId) {
    if (m_tasks.size() == 1)
        this->m_message = m_tasks.begin()->second->getMessage();
    /*else
        m_message = TE_TR("Multi Tasks");*/

    this->m_barWidth = this->m_totalWidth - this->m_message.size();
}

void gdma::common::CommandLineProgViewer::setBar() {
    std::ostringstream barStr;
    barStr << "[";

    for (int i = 0; i < this->m_barWidth; ++i) {
        if (i < this->m_positionBar)
            barStr << "=";
        else if (i == this->m_positionBar)
            barStr << ">";
        else
            barStr << " ";
    }

    barStr << "] " << std::round(this->m_progress * 100.0) << " % | Time: "
        << gdma::common::formatTime(this->m_currentTime.count()) << "\r";

    this->m_barStr = barStr.str();
}
