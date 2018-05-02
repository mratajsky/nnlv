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
#include "slpoutputlayer.h"

#include "slpoutputneuron.h"

SLPOutputLayer::SLPOutputLayer(Network* parent) :
    SLPLayer(NetworkLayerInfo::Type::Output, parent)
{
}

SLPOutputLayer::SLPOutputLayer(const NetworkLayerInfo::Map& map, Network* parent) :
    SLPLayer(map, parent)
{
}

SLPOutputLayer::SLPOutputLayer(int neurons, Network *parent) :
    SLPLayer(NetworkLayerInfo::Type::Output, parent)
{
    // Default layer name
    setName(tr("Output Layer"));

    for (int i = 0; i < neurons; i++)
        addNeuron(new SLPOutputNeuron(this), true);
}

SLPOutputLayer::SLPOutputLayer(const SavedNetworkLayer* layer, Network* parent) :
    SLPLayer(layer->infoMap(), parent)
{
    const auto& savedNeurons = layer->savedNeurons();
    for (auto* savedNeuron : savedNeurons)
        addNeuron(new SLPOutputNeuron(savedNeuron, this));
}

//
// Compute and return the value of all output neurons
//
QVector<double> SLPOutputLayer::compute(const QVector<double>& input) const
{
    int size = neuronCount();

    QVector<double> output(size);
    for (int i = 0; i < size; i++) {
        auto* n = qobject_cast<SLPOutputNeuron*>(neuron(i));
        Q_ASSERT(n != nullptr);

        output[i] = n->compute(input);
    }
    return output;
}

//
// Compute and set as current the value of all output neurons
//
void SLPOutputLayer::computeAndSet(const QVector<double>& input)
{
    for (int i = 0; i < neuronCount(); i++) {
        auto* n = qobject_cast<SLPOutputNeuron*>(neuron(i));
        Q_ASSERT(n != nullptr);

        n->setValue(n->compute(input));
    }
}

//
// Change learning rate of the output neurons
//
void SLPOutputLayer::setLearningRate(double learningRate)
{
    for (int i = 0; i < neuronCount(); i++) {
        auto* n = qobject_cast<SLPOutputNeuron*>(neuron(i));
        Q_ASSERT(n != nullptr);

        n->setLearningRate(learningRate);
    }
}

QString SLPOutputLayer::defaultNeuronName(int index) const
{
    return tr("Output #%1").arg(index + 1);
}

void SLPOutputLayer::train(const TrainingSample& sample)
{
    for (int i = 0; i < neuronCount(); i++) {
        auto* n = qobject_cast<SLPOutputNeuron*>(neuron(i));
        Q_ASSERT(n != nullptr);

        n->train(sample.inputs(), static_cast<int>(sample.output(i)));
    }
    updateInConnectionRange();
}
