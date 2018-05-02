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
#include "mlpinputlayer.h"

#include "mlpbiasneuron.h"
#include "mlpinputneuron.h"

MLPInputLayer::MLPInputLayer(int neurons, Network* parent) :
    MLPLayer(NetworkLayerInfo::Type::Input, parent)
{
    // Default layer name
    setName(tr("Input Layer"));

    addNeuron(new MLPBiasNeuron(this));
    for (auto i = 0; i < neurons; i++)
        addNeuron(new MLPInputNeuron(this), true);
}

MLPInputLayer::MLPInputLayer(SavedNetworkLayer* layer, Network* parent) :
    MLPLayer(layer->infoMap(), parent)
{
    addNeuron(new MLPBiasNeuron(this));
    const auto& savedNeurons = layer->savedNeurons();
    for (auto* savedNeuron : savedNeurons)
        addNeuron(new MLPInputNeuron(savedNeuron, this));
}

QString MLPInputLayer::defaultNeuronName(int index) const
{
    if (neuron(index)->isBias())
        return tr("Bias");

    return tr("Input #%1").arg(index);
}
