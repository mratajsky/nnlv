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
#include "mlplayer.h"

#include "mlpbiasneuron.h"
#include "mlpneuron.h"

MLPLayer::MLPLayer(NetworkLayerInfo::Type type, Network* parent) :
    SLPLayer(type, parent)
{
}

MLPLayer::MLPLayer(const NetworkLayerInfo::Map& map, Network* parent) :
    SLPLayer(map, parent)
{
}

QString MLPLayer::defaultNeuronName(int index) const
{
    if (neuron(index)->isBias())
        return tr("Bias");

    return tr("Neuron #%1").arg(index);
}

QVector<double> MLPLayer::compute(const QVector<double>& input) const
{
    QVector<double> output;

    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = mlpNeuron(i);
        // Skip the bias
        if (neuron != nullptr)
            output.append(neuron->compute(input));
    }
    return output;
}

void MLPLayer::computeAndSet(const QVector<double>& input)
{
    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = mlpNeuron(i);
        // Skip the bias
        if (neuron != nullptr)
            neuron->setValue(neuron->compute(input));
    }
}

//
// Update neurons in the current layer based on previous layer values and
// the connection weights
//
void MLPLayer::forward()
{
    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = mlpNeuron(i);
        // Skip the bias
        if (neuron != nullptr)
            neuron->forward();
    }
}

MLPNeuron* MLPLayer::mlpNeuron(int index) const
{
    return qobject_cast<MLPNeuron*>(neuron(index));
}

void MLPLayer::setActivationFunction(MLPActivation::Function function)
{
    Q_ASSERT(function != MLPActivation::Function::Unknown);

    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = mlpNeuron(i);
        if (neuron != nullptr)
            neuron->setActivationFunction(function);
    }
}

void MLPLayer::setLearningRate(double learningRate)
{
    Q_ASSERT(learningRate > 0.0 && learningRate <= 1.0);

    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = mlpNeuron(i);
        if (neuron != nullptr)
            neuron->setLearningRate(learningRate);
    }
}

void MLPLayer::updateDelta()
{
    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = mlpNeuron(i);
        // Skip the bias
        if (neuron != nullptr)
            neuron->updateDelta();
    }
}

void MLPLayer::updateWeights()
{
    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = mlpNeuron(i);
        // Skip the bias
        if (neuron != nullptr)
            neuron->updateWeights();
    }
    updateInConnectionRange();
}
