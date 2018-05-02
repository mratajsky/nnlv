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
#include "savednetworkneuron.h"

#include "kohonennetworklimits.h"
#include "mlpnetworklimits.h"
#include "networkinfo.h"
#include "slpnetworklimits.h"

SavedNetworkNeuron::SavedNetworkNeuron(QObject* parent) :
    QObject(parent)
{
}

NetworkNeuronInfo::Map& SavedNetworkNeuron::infoMap()
{
    return m_map;
}

const NetworkNeuronInfo::Map& SavedNetworkNeuron::infoMap() const
{
    return m_map;
}

const QVector<double>& SavedNetworkNeuron::inputWeights() const
{
    return m_inputWeights;
}

void SavedNetworkNeuron::setInputWeights(const QVector<double>& weights)
{
    m_inputWeights = weights;
}
