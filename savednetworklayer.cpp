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
#include "savednetworklayer.h"

#include <cmath>
#include <QPoint>

#include "adalinenetworklimits.h"
#include "kohonennetworklimits.h"
#include "mlpnetworklimits.h"
#include "networkinfo.h"
#include "rbfnetworklimits.h"
#include "slpnetworklimits.h"

SavedNetworkLayer::SavedNetworkLayer(QObject* parent)
    : QObject(parent)
{
}

NetworkLayerInfo::Map& SavedNetworkLayer::infoMap()
{
    return const_cast<NetworkLayerInfo::Map&>(static_cast<const SavedNetworkLayer*>(this)->infoMap());
}

const NetworkLayerInfo::Map& SavedNetworkLayer::infoMap() const
{
    return m_map;
}

QVector<SavedNetworkNeuron*>& SavedNetworkLayer::savedNeurons()
{
    return m_neurons;
}

const QVector<SavedNetworkNeuron*>& SavedNetworkLayer::savedNeurons() const
{
    return m_neurons;
}

//
// Verify the NetworkLayerInfo values in this layer
//
bool SavedNetworkLayer::verify()
{
    auto type = infoType();
    if (type == NetworkLayerInfo::Type::Unknown) {
        auto name = infoMap()[NetworkLayerInfo::Key::Name].toString();
        if (!name.isEmpty())
            m_verifyError = QString("The type of layer '%1' is unknown or not recognized").arg(name);
        else
            m_verifyError = "All layers are required to include a valid type";
        return false;
    }

    //
    // Verify number of neurons in the layer.
    //
    // It is required that this layer is owned by a SavedNetwork, which is used
    // to determine the network type.
    //
    NetworkInfo* networkInfo = dynamic_cast<NetworkInfo*>(parent());
    Q_ASSERT(networkInfo != nullptr);
    switch (networkInfo->infoType()) {
        case NetworkInfo::Type::SLP:
            if (type != NetworkLayerInfo::Type::Input &&
                    type != NetworkLayerInfo::Type::Output) {
                m_verifyError = "SLP network may only contain input and output layers";
                return false;
            }
            if (!SLPNetworkLimits::verifyNeuronCount(m_neurons.size(), type)) {
                m_verifyError = QString("SLP %1 layer neuron count must be an integer in the range %2 - %3")
                                .arg(infoTypeAsString())
                                .arg(SLPNetworkLimits::minNeurons(type))
                                .arg(SLPNetworkLimits::maxNeurons(type));
                return false;
            }
            break;
        case NetworkInfo::Type::Adaline:
            if (type != NetworkLayerInfo::Type::Input &&
                    type != NetworkLayerInfo::Type::Output) {
                m_verifyError = "Adaline network may only contain input and output layers";
                return false;
            }
            if (!AdalineNetworkLimits::verifyNeuronCount(m_neurons.size(), type)) {
                m_verifyError = QString("Adaline %1 layer neuron count must be an integer in the range %2 - %3")
                                .arg(infoTypeAsString())
                                .arg(AdalineNetworkLimits::minNeurons(type))
                                .arg(AdalineNetworkLimits::maxNeurons(type));
                return false;
            }
            break;
        case NetworkInfo::Type::MLP:
            if (type != NetworkLayerInfo::Type::Input &&
                    type != NetworkLayerInfo::Type::Hidden &&
                    type != NetworkLayerInfo::Type::Output) {
                m_verifyError = "MLP network may only contain input, hidden and output layers";
                return false;
            }
            if (!MLPNetworkLimits::verifyNeuronCount(m_neurons.size(), type)) {
                m_verifyError = QString("MLP %1 layer neuron count must be an integer in the range %2 - %3")
                                .arg(infoTypeAsString())
                                .arg(MLPNetworkLimits::minNeurons(type))
                                .arg(MLPNetworkLimits::maxNeurons(type));
                return false;
            }
            break;
        case NetworkInfo::Type::Kohonen:
            if (type != NetworkLayerInfo::Type::Input &&
                    type != NetworkLayerInfo::Type::Output) {
                m_verifyError = "Kohonen network may only contain input and output layers";
                return false;
            }
            if (!KohonenNetworkLimits::verifyNeuronCount(m_neurons.size(), type)) {
                m_verifyError = QString("Kohonen %1 layer neuron count must be an integer in the range %2 - %3")
                                .arg(infoTypeAsString())
                                .arg(KohonenNetworkLimits::minNeurons(type))
                                .arg(KohonenNetworkLimits::maxNeurons(type));
                return false;
            }

            if (type == NetworkLayerInfo::Type::Output) {
                QVector<QPoint> positions;
                int neuronSideCount = std::sqrt(m_neurons.size());
                //
                // Verify that every neuron has valid X and Y positions without duplicates
                //
                for (int i = 0; i < m_neurons.size(); i++) {
                    const auto& map = m_neurons.at(i)->infoMap();
                    if (!map.contains(NetworkNeuronInfo::Key::PositionX)
                            || !map.contains(NetworkNeuronInfo::Key::PositionY)) {
                        m_verifyError = "All output neurons must include X and Y positions";
                        return false;
                    }
                    QPoint p(map[NetworkNeuronInfo::Key::PositionX].toInt(),
                            map[NetworkNeuronInfo::Key::PositionY].toInt());
                    if (p.x() < 0 || p.x() > neuronSideCount) {
                        m_verifyError = QString("Output neuron X-position %1 is invalid (permitted range is 0 - %2)")
                                        .arg(p.x())
                                        .arg(neuronSideCount);
                        return false;
                    }
                    if (p.y() < 0 || p.y() > neuronSideCount) {
                        m_verifyError = QString("Output neuron Y-position %1 is invalid (permitted range is 0 - %2)")
                                        .arg(p.y())
                                        .arg(neuronSideCount);
                        return false;
                    }
                    if (positions.contains(p)) {
                        m_verifyError = QString("There are multiple output neurons with position (%1, %2)")
                                        .arg(p.x())
                                        .arg(p.y());
                        return false;
                    }
                    positions.append(p);
                }
            }
            break;
        case NetworkInfo::Type::RBF:
            if (type != NetworkLayerInfo::Type::Input &&
                    type != NetworkLayerInfo::Type::Hidden &&
                    type != NetworkLayerInfo::Type::Output) {
                m_verifyError = "RBF network may only contain input, hidden and output layers";
                return false;
            }
            if (!RBFNetworkLimits::verifyNeuronCount(m_neurons.size(), type)) {
                m_verifyError = QString("RBF %1 layer neuron count must be an integer in the range %2 - %3")
                                .arg(infoTypeAsString())
                                .arg(RBFNetworkLimits::minNeurons(type))
                                .arg(RBFNetworkLimits::maxNeurons(type));
                return false;
            }
            break;
        default:
            Q_UNREACHABLE();
            return false;
    }
    return true;
}

QString SavedNetworkLayer::verifyError() const
{
    return m_verifyError;
}
