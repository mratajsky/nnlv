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
#include "mlpneuron.h"

#include "colors.h"
#include "mlpactivation.h"

MLPNeuron::MLPNeuron(NetworkLayer* parent) :
    SLPNeuron(parent)
{
}

MLPNeuron::MLPNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent) :
    SLPNeuron(savedNeuron, parent)
{
}

double MLPNeuron::compute(const QVector<double>& input)
{
    // Bias unit
    double value = inConnection(0)->weight();

    // Connections from the real neurons
    for (int i = 1; i < inConnectionCount(); i++)
        value += input[i - 1] * inConnection(i)->weight();

    return MLPActivation::function(m_activationFunction, value);
}

void MLPNeuron::forward()
{
    double value = 0.0;
    for (const auto* conn : inConnections())
        value += conn->neuron1()->value() * conn->weight();

    setValue(MLPActivation::function(m_activationFunction, value));
}

double MLPNeuron::delta() const
{
    return m_delta;
}

void MLPNeuron::setActivationFunction(MLPActivation::Function function)
{
    m_activationFunction = function;
}

void MLPNeuron::setLearningRate(double learningRate)
{
    m_learningRate = learningRate;
}

void MLPNeuron::updateDelta()
{
    m_delta = 0.0;
    const auto& connections = outConnections();
    for (const auto* conn : connections) {
        const auto* outNeuron = qobject_cast<MLPNeuron*>(conn->neuron2());
        m_delta += conn->weight() * outNeuron->delta();
    }
    m_delta *= MLPActivationDerivative::function(m_activationFunction, value());
}

//
// Update the weights of the neuron's input connections
//
void MLPNeuron::updateWeights()
{
    const auto& connections = inConnections();
    for (auto* conn : connections) {
        double weight = conn->weight() - m_learningRate * m_delta * conn->neuron1()->value();

        conn->setWeight(weight);
    }
}

void MLPNeuron::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QColor color;
    if (showValue()) {
        switch (m_activationFunction) {
            case MLPActivation::Function::Sigmoid:
                color = colorValue(0, 1);
                break;
            case MLPActivation::Function::Tanh:
                color = colorValue(-1, 1);
                break;
            default:
                Q_UNREACHABLE();
                break;
        }
    } else
        color = Colors::ColorNeuronInitial;

    painter->setBrush(color);
    if (m_lastColor != color) {
        m_lastColor = color;
        update();
    }
    painter->drawEllipse(boundingRect());

    SLPNeuron::paint(painter, option, widget);
}
