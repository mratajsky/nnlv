/*-
 * Copyright (c) 2016-2017 Michal Ratajsky <michal.ratajsky@gmail.com>
 * All rights reserved.
 *
 * This file is part of Neural Network Learning Visualizer (NNLV).
 *
 * NNLV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NNLV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NNLV.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "supervisednetwork.h"

SupervisedNetwork::SupervisedNetwork(QObject* parent) :
    Network(parent)
{
    init();
}

SupervisedNetwork::SupervisedNetwork(const NetworkInfo::Map& map, QObject* parent) :
    Network(map, parent)
{
    init();
}

void SupervisedNetwork::init()
{
    const auto& store = trainingTableModel()->store();
    connect(&store, &TrainingSampleStore::samplesChanged, this, [this] {
        m_updateNeeded = true;
    });

    connect(this, &Network::neuronWeightChanged, this, [this] {
        m_updateNeeded = true;
    });
    connect(this, &Network::trainingSampleDone, this, [this] {
        m_updateNeeded = true;
    });
}

double SupervisedNetwork::correctPercentage()
{
    if (m_updateNeeded) {
        updateCurrentStatus();
        m_updateNeeded = false;
    }
    return m_correctPercentage;
}

int SupervisedNetwork::correctSamples()
{
    if (m_updateNeeded) {
        updateCurrentStatus();
        m_updateNeeded = false;
    }
    return m_correctSamples;
}

double SupervisedNetwork::error()
{
    if (m_updateNeeded) {
        updateCurrentStatus();
        m_updateNeeded = false;
    }
    return m_error;
}

bool SupervisedNetwork::isStopConditionReached(Network::StopTrainingReason* reason)
{
    int neededSamples = stopSamples();
    if (neededSamples > 0 && correctSamples() >= neededSamples) {
        *reason = StopTrainingReason::SamplesReached;
        return true;
    }
    double neededPercentage = stopPercentage();
    if (neededPercentage > 0 && correctPercentage() >= neededPercentage) {
        *reason = StopTrainingReason::PercentageReached;
        return true;
    }
    double neededError = stopError();
    if (neededError > 0 && error() <= neededError) {
        *reason = StopTrainingReason::ErrorReached;
        return true;
    }
    return false;
}

void SupervisedNetwork::updateCurrentStatus()
{
    //
    // This is the general implementation which calculates the MSE error
    //
    m_error = 0.0;
    const auto& samples = trainingTableModel()->store().samples();
    if (samples.size() > 0) {
        for (const auto& sample : samples) {
            auto outputs = compute(sample.inputs());
            for (int i = 0; i < outputs.size(); i++) {
                double diff = outputs.at(i) - sample.output(i);
                m_error += diff * diff;
            }
        }
        m_error /= samples.size();
    }
}
