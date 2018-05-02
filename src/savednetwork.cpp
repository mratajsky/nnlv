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
#include "savednetwork.h"

#include "adalinenetworklimits.h"
#include "kohonennetworklimits.h"
#include "mlpnetworklimits.h"
#include "rbfnetworklimits.h"
#include "slpnetworklimits.h"

SavedNetwork::SavedNetwork(QObject* parent) :
    QObject(parent)
{
}

TrainingSampleStore& SavedNetwork::trainingSampleStore()
{
    return m_store;
}

const TrainingSampleStore& SavedNetwork::trainingSampleStore() const
{
    return m_store;
}

NetworkInfo::Map& SavedNetwork::infoMap()
{
    return m_infoMap;
}

const NetworkInfo::Map& SavedNetwork::infoMap() const
{
    return m_infoMap;
}

QVector<SavedNetworkLayer*>& SavedNetwork::savedLayers()
{
    return m_layers;
}

const QVector<SavedNetworkLayer*>& SavedNetwork::savedLayers() const
{
    return m_layers;
}

//
// Verify the NetworkInfo values in this network
//
bool SavedNetwork::verify()
{
    const auto& map = infoMap();

    //
    // Verify initial weights
    //
    if (map.contains(NetworkInfo::Key::InitialWeightsRandomMin)
            && map.contains(NetworkInfo::Key::InitialWeightsRandomMax)) {
        double min = map[NetworkInfo::Key::InitialWeightsRandomMin].toDouble();
        double max = map[NetworkInfo::Key::InitialWeightsRandomMax].toDouble();
        if (max < min) {
            m_verifyError = "Invalid initial weight range";
            return false;
        }
    }

    //
    // Verify number of layers
    //
    if (m_layers.isEmpty()) {
        m_verifyError = "There are no layers in the network";
        return false;
    }
    switch (infoType()) {
        case NetworkInfo::Type::SLP:
            if (m_layers.size() != 2) {
                m_verifyError = "SLP network is required to have one input layer and one output layer";
                return false;
            }
            break;
        case NetworkInfo::Type::Adaline:
            if (m_layers.size() != 2) {
                m_verifyError = "Adaline network is required to have one input layer and one output layer";
                return false;
            }
            break;
        case NetworkInfo::Type::MLP:
            if (m_layers.size() < 2 + MLPNetworkLimits::minHiddenLayers) {
                m_verifyError = "MLP network has too few layers";
                return false;
            }
            if (m_layers.size() > 2 + MLPNetworkLimits::maxHiddenLayers) {
                m_verifyError = "MLP network has too many layers";
                return false;
            }
            break;
        case NetworkInfo::Type::Kohonen:
            if (m_layers.size() != 2) {
                m_verifyError = "Kohonen network is required to have one input layer and one output layer";
                return false;
            }
            break;
        case NetworkInfo::Type::RBF:
            if (m_layers.size() != 3) {
                m_verifyError = "RBF network is required to have one input layer, one hidden layer and one output layer";
                return false;
            }
            break;
        default:
            break;
    }

    //
    // Verify correctness of layers
    //
    if (m_layers.first()->infoType() != NetworkLayerInfo::Type::Input) {
        m_verifyError = "The first layer must be an input layer";
        return false;
    }
    if (m_layers.last()->infoType() != NetworkLayerInfo::Type::Output) {
        m_verifyError = "The last layer must be an output layer";
        return false;
    }
    for (int i = 1; i < m_layers.size() - 1; i++) {
        switch (m_layers.at(i)->infoType()) {
            case NetworkLayerInfo::Type::Input:
                m_verifyError = "The input layer may not be preceded by another layer";
                return false;
            case NetworkLayerInfo::Type::Output:
                if (i != m_layers.size() - 1) {
                    m_verifyError = "The output layer may only be the last layer";
                    return false;
                }
                break;
            case NetworkLayerInfo::Type::Hidden:
                break;
            default:
                Q_UNREACHABLE();
                return false;
        }
    }
    return true;
}

QString SavedNetwork::verifyError() const
{
    return m_verifyError;
}
