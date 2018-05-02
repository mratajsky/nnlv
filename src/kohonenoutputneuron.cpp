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
#include "kohonenoutputneuron.h"

#include "colors.h"

KohonenOutputNeuron::KohonenOutputNeuron(int x, int y, NetworkLayer* parent) :
    SLPNeuron(parent),
    m_position(x, y)
{
    m_infoMap[NetworkNeuronInfo::Key::PositionX] = QVariant::fromValue(x);
    m_infoMap[NetworkNeuronInfo::Key::PositionY] = QVariant::fromValue(y);
}

KohonenOutputNeuron::KohonenOutputNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent) :
    SLPNeuron(savedNeuron, parent)
{
    m_position.setX(m_infoMap[NetworkNeuronInfo::Key::PositionX].toInt());
    m_position.setY(m_infoMap[NetworkNeuronInfo::Key::PositionY].toInt());
}

const QPoint& KohonenOutputNeuron::position() const
{
    return m_position;
}

//
// Change output neuron learning rate
//
void KohonenOutputNeuron::setLearningRate(double learningRate)
{
    Q_ASSERT(learningRate > 0.0 && learningRate <= 1.0);

    m_learningRate = learningRate;
}

void KohonenOutputNeuron::updateWeights()
{
    const auto& connections = inConnections();
    for (auto* conn : connections) {
        double weight = conn->weight();
        weight += m_learningRate * value() * (conn->neuron1()->value() - weight);
        conn->setWeight(weight);
    }
    update();
}

void KohonenOutputNeuron::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const auto rect = boundingRect();
    if (rect.isEmpty())
        return;

    if (showValue())
        painter->setBrush(Colors::colorKohonenOutputNeuronValue(value()));
    else
        painter->setBrush(Colors::ColorKohonenOutputInitial);
    painter->drawEllipse(rect);

    if (showValue() && qFuzzyCompare(value(), 1.0)) {
        //
        // Draw the number "1" with a bold font into the neuron circle
        //
        auto font = painter->font();
        font.setBold(true);
        font.setPixelSize(qMin(rect.width(), rect.height()) * 0.35);
        painter->setFont(font);
        painter->setPen(Colors::ColorText);
        painter->drawText(rect, QStringLiteral("1"), Qt::AlignHCenter | Qt::AlignVCenter);
    }
}
