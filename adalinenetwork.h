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

#include "adalineinputlayer.h"
#include "adalineoutputlayer.h"
#include "savednetwork.h"
#include "supervisednetwork.h"

class AdalineNetwork : public SupervisedNetwork
{
    Q_OBJECT
public:
    explicit AdalineNetwork(const NetworkInfo::Map& map, QObject* parent = nullptr);
    explicit AdalineNetwork(const SavedNetwork& savedNetwork, QObject* parent = nullptr);

    QVector<double> compute(const QVector<double>& input) const override;
    void computeAndSet(const QVector<double>& input) override;
    void train(const TrainingSample& sample) override;
    void updateScenePosition() override;

private:
    void init();

    AdalineInputLayer* m_inputLayer;
    AdalineOutputLayer* m_outputLayer;
};
