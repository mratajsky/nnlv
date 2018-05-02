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

#include "mlpactivation.h"
#include "slpneuron.h"

class MLPNeuron : public SLPNeuron
{
    Q_OBJECT
public:
    explicit MLPNeuron(NetworkLayer* parent);
    explicit MLPNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent);

    double compute(const QVector<double>& input);
    double delta() const;
    void forward();
    void setActivationFunction(MLPActivation::Function function);
    void setLearningRate(double learningRate);
    void updateDelta();
    void updateWeights();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    double m_delta;
    QColor m_lastColor;
    double m_learningRate = 1.0;
    MLPActivation::Function m_activationFunction = MLPActivation::Function::Sigmoid;
};
