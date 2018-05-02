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

#include <QColor>
#include <QPainter>

#include "slpneuron.h"

class SLPOutputNeuron : public SLPNeuron
{
    Q_OBJECT
public:
    explicit SLPOutputNeuron(NetworkLayer* parent = nullptr);
    explicit SLPOutputNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent = nullptr);

    virtual double compute(const QVector<double>& input) const;
    virtual void train(const QVector<double>& input, int target);

    double learningRate() const;
    void setLearningRate(double learningRate);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    double m_learningRate = 1.0;
    QColor m_lastColor;
};
