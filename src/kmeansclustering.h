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

#include <random>
#include <QObject>

#include "trainingsample.h"

class KMeansClustering : public QObject
{
    Q_OBJECT
public:
    using KCluster = QVector<double>;
    using KClusterVector = QVector<KCluster>;

    explicit KMeansClustering(QObject* parent = nullptr);
    explicit KMeansClustering(const QVector<TrainingSample>& samples, QObject* parent = nullptr);

    void setTrainingSamples(const QVector<TrainingSample>& samples);
    void doClustering(int clusterCount);

    double averageClusterDistance(int clusterIndex) const;
    double findClosestClusterDistance(int clusterIndex) const;
    const KClusterVector& clusters() const;
    int sampleClusterIndex(int sampleIndex) const;

private:
    KCluster computeAverageSum(int index);
    int findClosestCluster(const TrainingSample& sample);
    void recalculateClusters();

    std::mt19937 m_generator;
    QVector<TrainingSample> m_samples;
    QVector<KCluster> m_clusters;
    QMap<int, int> m_sampleClusterMap;
};
