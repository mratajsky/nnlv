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

#include "rbflayer.h"
#include "rbfoutputneuron.h"
#include "savednetworklayer.h"

class RBFOutputLayer : public RBFLayer
{
    Q_OBJECT
public:
    explicit RBFOutputLayer(int neurons, Network *parent = nullptr);
    explicit RBFOutputLayer(SavedNetworkLayer* layer, Network* parent = nullptr);

    void forward() override;
    bool inConnectionWeightsSettable() const override;
    RBFOutputNeuron* rbfOutputNeuron(int index) const;
    void resetRange();
    void setLearningRate(double learningRate);
    void updateWeights(const QVector<double>& target);

protected:
    QString defaultNeuronName(int index) const override;

private:
    double m_minValue;
    double m_maxValue;
};
