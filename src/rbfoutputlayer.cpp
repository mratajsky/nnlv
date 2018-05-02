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
#include "rbfoutputlayer.h"

#include "rbfoutputneuron.h"

RBFOutputLayer::RBFOutputLayer(int neurons, Network *parent) :
    RBFLayer(NetworkLayerInfo::Type::Output, parent)
{
    // Default layer name
    setName(tr("Output Layer"));

    for (int i = 0; i < neurons; i++)
        addNeuron(new RBFOutputNeuron(this), true);

    m_minValue = qInf();
    m_maxValue = -qInf();
}

RBFOutputLayer::RBFOutputLayer(SavedNetworkLayer* layer, Network* parent) :
    RBFLayer(layer->infoMap(), parent)
{
    const auto& savedNeurons = layer->savedNeurons();
    for (auto* savedNeuron : savedNeurons)
        addNeuron(new RBFOutputNeuron(savedNeuron, this));

    m_minValue = qInf();
    m_maxValue = -qInf();
}

QString RBFOutputLayer::defaultNeuronName(int index) const
{
    return tr("Output #%1").arg(index + 1);
}

//
// Forward values from the previous layer
//
void RBFOutputLayer::forward()
{
    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = rbfOutputNeuron(i);

        neuron->forward();
        m_minValue = qMin(m_minValue, neuron->value());
        m_maxValue = qMax(m_maxValue, neuron->value());
    }

    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = rbfOutputNeuron(i);

        neuron->setValueRange(m_minValue, m_maxValue);
    }
}

bool RBFOutputLayer::inConnectionWeightsSettable() const
{
    return false;
}

RBFOutputNeuron* RBFOutputLayer::rbfOutputNeuron(int index) const
{
    return qobject_cast<RBFOutputNeuron*>(neuron(index));
}

void RBFOutputLayer::resetRange()
{
    m_minValue = qInf();
    m_maxValue = -qInf();

    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = rbfOutputNeuron(i);

        neuron->setValueRange(m_minValue, m_maxValue);
    }
}

void RBFOutputLayer::setLearningRate(double learningRate)
{
    Q_ASSERT(learningRate > 0.0 && learningRate <= 1.0);

    const auto neuronItems = neurons();
    for (auto* neuron : neuronItems) {
        auto* rbfNeuron = qobject_cast<RBFOutputNeuron*>(neuron);
        rbfNeuron->setLearningRate(learningRate);
    }
}

void RBFOutputLayer::updateWeights(const QVector<double>& target)
{
    Q_ASSERT(target.size() == neuronCount());

    for (int i = 0; i < neuronCount(); i++) {
        auto* rbfNeuron = qobject_cast<RBFOutputNeuron*>(neuron(i));

        rbfNeuron->updateWeights(target.at(i));
    }
    updateInConnectionRange();
}
