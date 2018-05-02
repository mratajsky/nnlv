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
#include "slpoutputneuron.h"

#include "colors.h"

SLPOutputNeuron::SLPOutputNeuron(NetworkLayer *parent) :
    SLPNeuron(parent)
{
}

SLPOutputNeuron::SLPOutputNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent) :
    SLPNeuron(savedNeuron, parent)
{
}

double SLPOutputNeuron::compute(const QVector<double>& input) const
{
    // Bias unit
    double value = inConnection(0)->weight();
    //
    // Connections from the real input neurons
    //
    for (int i = 1; i < inConnectionCount(); i++)
        value += input[i - 1] * inConnection(i)->weight();

    return (value >= 0) ? 1 : 0;
}

double SLPOutputNeuron::learningRate() const
{
    return m_learningRate;
}

//
// Change output neuron learning rate
//
void SLPOutputNeuron::setLearningRate(double learningRate)
{
    Q_ASSERT(learningRate > 0.0 && learningRate <= 1.0);

    m_learningRate = learningRate;
}

void SLPOutputNeuron::train(const QVector<double>& input, int target)
{
    int value = compute(input);
    int diff = value - target;
    if (diff != 0) {
        for (int i = 0; i < inConnectionCount(); i++) {
            auto* conn = inConnection(i);

            double weight = m_learningRate * diff;
            if (i > 0)
                weight *= input[i - 1];

            conn->setWeight(conn->weight() - weight);
        }
    }
    setValue(value);
}

void SLPOutputNeuron::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QColor color;
    if (showValue()) {
        //
        // Output value is either 0 or 1
        //
        color = colorValue(0, 1);
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
