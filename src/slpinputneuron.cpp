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
#include "slpinputneuron.h"

#include "colors.h"

SLPInputNeuron::SLPInputNeuron(NetworkLayer *parent) :
    SLPNeuron(parent)
{
}

SLPInputNeuron::SLPInputNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent) :
    SLPNeuron(savedNeuron, parent)
{
}

void SLPInputNeuron::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QColor color;
    if (showValue()) {
        //
        // Input value can any real number and the range should be known
        // by the sample store
        //
        const auto& store = layer()->network()->trainingTableModel()->store();
        color = colorValue(store.minInput(), store.maxInput());
    } else
        color = Colors::ColorNeuronInitial;
    painter->setBrush(color);
    if (m_lastColor != color) {
        m_lastColor = color;
        update();
    }
    painter->drawRect(boundingRect());

    SLPNeuron::paint(painter, option, widget);
}
