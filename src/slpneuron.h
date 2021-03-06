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
#pragma once

#include "common.h"

#include <QPainter>
#include <QTextOption>
#include <QTimer>

#include "networkneuron.h"
#include "savednetworkneuron.h"

class SLPNeuron : public NetworkNeuron
{
    Q_OBJECT
public:
    explicit SLPNeuron(NetworkLayer* parent = nullptr);
    explicit SLPNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent = nullptr);

    QRectF boundingRect() const override;
    void setBoundingRect(QRectF rect);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    static const QTextOption m_textOption;

    QRectF m_boundingRect;
};
