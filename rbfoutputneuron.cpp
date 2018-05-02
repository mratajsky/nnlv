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
#include "rbfoutputneuron.h"

#include "colors.h"

RBFOutputNeuron::RBFOutputNeuron(NetworkLayer *parent) :
    RBFNeuron(parent)
{
    m_minValue = qInf();
    m_maxValue = -qInf();
}

RBFOutputNeuron::RBFOutputNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent) :
    RBFNeuron(savedNeuron, parent)
{
    m_minValue = qInf();
    m_maxValue = -qInf();
}

double RBFOutputNeuron::compute(const QVector<double>& input) const
{
    double value = inConnection(0)->weight();
    for (int i = 1; i < input.size(); i++)
        value += input.at(i - 1) * inConnection(i)->weight();

    return value;
}

void RBFOutputNeuron::forward()
{
    double value = 0.0;
    for (const auto* conn : inConnections())
        value += conn->neuron1()->value() * conn->weight();

    setValue(value);
}

void RBFOutputNeuron::resetValue()
{
    m_minValue = qInf();
    m_maxValue = -qInf();

    NetworkNeuron::resetValue();
}

void RBFOutputNeuron::setLearningRate(double learningRate)
{
    m_learningRate = learningRate;
}

void RBFOutputNeuron::setValueRange(double min, double max)
{
    m_minValue = min;
    m_maxValue = max;
}

void RBFOutputNeuron::updateWeights(double target)
{
    for (int i = 0; i < inConnectionCount(); i++) {
        auto* conn = inConnection(i);

        double weight = conn->weight() + m_learningRate * (target - value()) * conn->neuron1()->value();

        conn->setWeight(weight);
    }
}

void RBFOutputNeuron::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QColor color;
    if (showValue())
        color = colorValue(m_minValue, m_maxValue);
    else
        color = Colors::ColorNeuronInitial;
    painter->setBrush(color);
    if (m_lastColor != color) {
        m_lastColor = color;
        update();
    }
    painter->drawEllipse(boundingRect());

    SLPNeuron::paint(painter, option, widget);
}
