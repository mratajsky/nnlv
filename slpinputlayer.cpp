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
#include "slpinputlayer.h"

#include "graphicsutilities.h"
#include "slpbiasneuron.h"
#include "slpinputneuron.h"

SLPInputLayer::SLPInputLayer(Network* parent) :
    SLPLayer(NetworkLayerInfo::Type::Input, parent)
{
}

SLPInputLayer::SLPInputLayer(const NetworkLayerInfo::Map& map, Network* parent) :
    SLPLayer(map, parent)
{
}

SLPInputLayer::SLPInputLayer(int neurons, Network *parent) :
    SLPLayer(NetworkLayerInfo::Type::Input, parent)
{
    // Default layer name
    setName(tr("Input Layer"));

    addNeuron(new SLPBiasNeuron(this));
    for (int i = 0; i < neurons; i++)
        addNeuron(new SLPInputNeuron(this), true);
}

SLPInputLayer::SLPInputLayer(const SavedNetworkLayer* layer, Network* parent) :
    SLPLayer(layer->infoMap(), parent)
{
    addNeuron(new SLPBiasNeuron(this));
    const auto& savedNeurons = layer->savedNeurons();
    for (auto* savedNeuron : savedNeurons)
        addNeuron(new SLPInputNeuron(savedNeuron, this));
}

QVector<double> SLPInputLayer::compute(const QVector<double>& input) const
{
    // The input layer doesn't do any computation on the input vector
    return input;
}

QString SLPInputLayer::defaultNeuronName(int index) const
{
    if (neuron(index)->isBias())
        return tr("Bias");

    return tr("Input #%1").arg(index);
}
