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
#include "rbfhiddenneuron.h"

#include <cmath>

#include "colors.h"

RBFHiddenNeuron::RBFHiddenNeuron(NetworkLayer *parent) :
    RBFNeuron(parent)
{
    m_infoMap[NetworkNeuronInfo::Key::Sigma] = m_sigma;
}

RBFHiddenNeuron::RBFHiddenNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent) :
    RBFNeuron(savedNeuron, parent)
{
    setSigma(m_infoMap[NetworkNeuronInfo::Key::Sigma].toDouble());
}

//
// Compute and return the output value of the neuron for the given input
//
double RBFHiddenNeuron::compute(const QVector<double>& input) const
{
    double value = 0.0;
    for (int i = 0; i < input.size(); i++) {
        double diff = input.at(i) - inConnection(i)->weight();
        value += diff * diff;
    }
    return std::exp(-m_beta * value);
}

//
// Compute and set the output value of the neuron for the currently set input values
// in the previous layer
//
void RBFHiddenNeuron::forward()
{
    double value = 0.0;
    const auto& connections = inConnections();
    for (const auto* conn : connections) {
        double diff = conn->neuron1()->value() - conn->weight();
        value += diff * diff;
    }
    value = std::exp(-m_beta * value);
    setValue(value);
}

void RBFHiddenNeuron::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QColor color;
    if (showValue())
        color = colorValue(0, 1);
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

double RBFHiddenNeuron::sigma() const
{
    return m_sigma;
}

void RBFHiddenNeuron::setSigma(double sigma)
{
    if (sigma > 0) {
        m_sigma = sigma;
        m_beta = 1.0 / (sigma * sigma);
    } else {
        m_sigma = 0.0;
        m_beta = 0.0;
    }
    m_infoMap[NetworkNeuronInfo::Key::Sigma] = m_sigma;
}
