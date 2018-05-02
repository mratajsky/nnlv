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
#include "slpbiasneuron.h"

#include <QTextOption>

#include "colors.h"

SLPBiasNeuron::SLPBiasNeuron(NetworkLayer* parent) :
    SLPNeuron(parent)
{
    setIsBias(true);
    //
    // Default name
    //
    setName(tr("Bias"));
}

void SLPBiasNeuron::addOutConnection(NetworkConnection* conn)
{
    //
    // Override the default connection line look
    //
    conn->setPenStyle(Qt::DashLine);

    SLPNeuron::addOutConnection(conn);
}

void SLPBiasNeuron::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const auto rect = boundingRect();
    if (rect.isEmpty())
        return;

    painter->setBrush(Colors::ColorBias);
    painter->drawEllipse(rect);

    auto font = painter->font();
    font.setPixelSize(qMin(rect.width(), rect.height()) / 4);
    painter->setFont(font);
    painter->setPen(Colors::ColorText);
    QTextOption textOption(Qt::AlignHCenter | Qt::AlignVCenter);
    painter->drawText(rect, QStringLiteral("1"), textOption);
}
