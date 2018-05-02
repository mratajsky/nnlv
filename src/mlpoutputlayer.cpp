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
#include "mlpoutputlayer.h"

#include "mlpoutputneuron.h"

MLPOutputLayer::MLPOutputLayer(int neurons, Network* parent) :
    MLPLayer(NetworkLayerInfo::Type::Output, parent)
{
    // Default layer name
    setName(tr("Output Layer"));

    for (int i = 0; i < neurons; i++)
        addNeuron(new MLPOutputNeuron(this), true);
}

MLPOutputLayer::MLPOutputLayer(SavedNetworkLayer* layer, Network* parent) :
    MLPLayer(layer->infoMap(), parent)
{
    const auto& savedNeurons = layer->savedNeurons();
    for (auto* savedNeuron : savedNeurons)
        addNeuron(new MLPOutputNeuron(savedNeuron, this));
}

QString MLPOutputLayer::defaultNeuronName(int index) const
{
    return tr("Output #%1").arg(index + 1);
}

MLPOutputNeuron* MLPOutputLayer::mlpOutputNeuron(int index) const
{
    return qobject_cast<MLPOutputNeuron*>(neuron(index));
}

//
// Return the number of neurons in this layer
// This method is overriden here as MLPLayer counts with a bias unit
//
int MLPOutputLayer::neuronCount(bool usableForTrainingOnly) const
{
    Q_UNUSED(usableForTrainingOnly);

    return NetworkLayer::neuronCount();
}

void MLPOutputLayer::updateDelta(const QVector<double>& target)
{
    Q_ASSERT(target.size() == neuronCount());

    for (int i = 0; i < neuronCount(); i++)
        mlpOutputNeuron(i)->updateDelta(target.at(i));
}
