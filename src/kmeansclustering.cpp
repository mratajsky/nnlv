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
#include "kmeansclustering.h"

#include <cmath>
#include <utility>
#include <QApplication>

#include "vectorutilities.h"

KMeansClustering::KMeansClustering(QObject* parent) :
    QObject(parent),
    m_generator(std::random_device{}())
{
}

KMeansClustering::KMeansClustering(const QVector<TrainingSample>& samples, QObject* parent) :
    QObject(parent),
    m_generator(std::random_device{}()),
    m_samples(samples)
{
}

void KMeansClustering::setTrainingSamples(const QVector<TrainingSample>& samples)
{
    m_samples = samples;
}

void KMeansClustering::doClustering(int clusterCount)
{
    // Initialize here allowing to call this function repeatedly
    m_clusters.clear();
    m_sampleClusterMap.clear();
    if (m_samples.isEmpty())
        return;

    std::uniform_int_distribution<int> dist(0, m_samples.size() - 1);
    QSet<int> used;
    //
    // Randomly choose cluster centers according to the sample space
    //
    for (int i = 0; i < clusterCount; i++) {
        // Pick a random initial sample, make sure to select different ones
        // if there is enough of them available
        int index;
        do {
            index = dist(m_generator);
        } while (used.contains(index) && used.size() < m_samples.size());
        used << index;
        m_clusters.append(m_samples.at(index).inputs());
    }
    // Find the closest cluster for each sample
    for (int i = 0; i < m_samples.size(); i++)
        m_sampleClusterMap[i] = findClosestCluster(m_samples.at(i));

    //
    // Repeatedly:
    //  move the cluster center to the position where the average distance
    //    to all the cluster's samples is the smallest
    //  change every sample's cluster to the cluster which is closest to it
    //  repeat until no changes happen
    //
    int iteration = 1;
    while (true) {
        recalculateClusters();
        int changed = 0;
        for (int i = 0; i < m_samples.size(); i++) {
            int clusterIndex = findClosestCluster(m_samples.at(i));
            if (clusterIndex != m_sampleClusterMap[i]) {
                changed++;
                m_sampleClusterMap[i] = clusterIndex;
            }
        }
        qDebug() << "K-Means iteration" << iteration << "changed assignments:" << changed;
        if (changed == 0)
            break;
        iteration++;
        QApplication::processEvents();
    }
    qDebug() << "K-Means finished in" << iteration << "iterations";
}

double KMeansClustering::averageClusterDistance(int clusterIndex) const
{
    const KCluster& cluster = m_clusters.at(clusterIndex);
    int count = 0;
    double totalDistance = 0.0;
    for (int i = 0; i < m_samples.size(); i++) {
        if (m_sampleClusterMap[i] != clusterIndex) {
            // Sample belongs to a different cluster
            continue;
        }
        totalDistance += VectorUtilities::distance(cluster, m_samples.at(i).inputs());
        count++;
    }
    return (count > 0) ? totalDistance / count : 0;
}

const KMeansClustering::KClusterVector& KMeansClustering::clusters() const
{
    return m_clusters;
}

int KMeansClustering::sampleClusterIndex(int sampleIndex) const
{
    if (m_sampleClusterMap.contains(sampleIndex))
        return m_sampleClusterMap[sampleIndex];

    return -1;
}

int KMeansClustering::findClosestCluster(const TrainingSample& sample)
{
    double minDistance = qInf();
    int minIndex = -1;
    for (int i = 0; i < m_clusters.size(); i++) {
        double distance = VectorUtilities::distance(m_clusters.at(i), sample.inputs());
        if (distance < minDistance) {
            minDistance = distance;
            minIndex = i;
        }
    }
    return minIndex;
}

double KMeansClustering::findClosestClusterDistance(int clusterIndex) const
{
    double minDistance = qInf();
    const auto& cluster = m_clusters.at(clusterIndex);
    for (int i = 0; i < m_clusters.size(); i++) {
        if (i == clusterIndex)
            continue;
        double distance = VectorUtilities::distance(m_clusters.at(i), cluster);
        if (distance < minDistance)
            minDistance = distance;
    }
    return minDistance;
}

KMeansClustering::KCluster KMeansClustering::computeAverageSum(int index)
{
    int count = 0;
    KCluster sum(m_clusters.at(0).size(), 0.0);
    for (int i = 0; i < m_samples.size(); i++) {
        if (m_sampleClusterMap[i] != index) {
            // Sample belongs to a different cluster
            continue;
        }
        VectorUtilities::addEach(sum, m_samples.at(i).inputs());
        count++;
    }
    if (count > 0)
        VectorUtilities::divideEach(sum, count);
    return sum;
}

void KMeansClustering::recalculateClusters()
{
    for (int i = 0; i < m_clusters.size(); i++)
        m_clusters[i] = computeAverageSum(i);
}
