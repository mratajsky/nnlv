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
#include "slpneuron.h"

#include <QTextLayout>

#include "colors.h"

const QTextOption SLPNeuron::m_textOption = QTextOption(Qt::AlignHCenter | Qt::AlignVCenter);

SLPNeuron::SLPNeuron(NetworkLayer* parent) :
    NetworkNeuron(parent)
{
}

SLPNeuron::SLPNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent) :
    NetworkNeuron(savedNeuron->infoMap(), savedNeuron->inputWeights(), parent)
{
}

QRectF SLPNeuron::boundingRect() const
{
    return m_boundingRect;
}

void SLPNeuron::setBoundingRect(QRectF rect)
{
    if (m_boundingRect != rect) {
        prepareGeometryChange();
        m_boundingRect = rect;
    }
}

void SLPNeuron::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!showValue())
        return;

    //
    // Draw the value inside the neuron
    //
    const auto rect = boundingRect();
    if (rect.isEmpty())
        return;

    // Initial font size
    QFont font = painter->font();
    font.setPixelSize(qMin(rect.width(), rect.height()) * .25);

    QTextLayout txtLayout(QString::number(value(), 'g', 3), font);
    txtLayout.setCacheEnabled(true);
    txtLayout.setFont(font);
    txtLayout.beginLayout();
    auto txtLine = txtLayout.createLine();
    txtLayout.endLayout();

    auto txtWidth = txtLine.naturalTextWidth();
    auto maxWidth = rect.width() - 15; // have some margin on the sides
    if (txtWidth > maxWidth) {
        //
        // Decrease font size to make the number fit inside the neuron
        //
        font.setPixelSize(qMax(7, static_cast<int>(font.pixelSize() * (maxWidth / txtWidth))));
    }
    painter->setFont(font);
    painter->setPen(Colors::ColorText);
    painter->drawText(rect, txtLayout.text(), m_textOption);
}
