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

#include "kmeansclustering.h"
#include "rbflayer.h"
#include "savednetworklayer.h"

class RBFHiddenLayer : public RBFLayer
{
    Q_OBJECT
public:
    RBFHiddenLayer(int neurons, Network *parent);
    RBFHiddenLayer(SavedNetworkLayer* layer, Network* parent);

    bool isTrained() const;
    void train(const QVector<TrainingSample>& samples);
    void untrain();
    const KMeansClustering& kmeans() const;

    int clusterIndexToNeuronIndex(int clusterIndex) const;
    int neuronIndexToClusterIndex(int neuronIndex) const;

signals:
    void trained();
    void untrained();

protected:
    QString defaultNeuronName(int index) const override;

private:
    void init();

    bool m_trained = false;
    KMeansClustering* m_kmeans;
};
