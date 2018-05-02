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
#include "rbfhiddenlayer.h"

#include "kmeansclustering.h"
#include "rbfbiasneuron.h"
#include "rbfhiddenneuron.h"

RBFHiddenLayer::RBFHiddenLayer(int neurons, Network *parent) :
    RBFLayer(NetworkLayerInfo::Type::Hidden, parent)
{
    init();
    setName(tr("RBF Layer"));

    addNeuron(new RBFBiasNeuron(this));

    for (int i = 0; i < neurons; i++)
        addNeuron(new RBFHiddenNeuron(this), true);
}

RBFHiddenLayer::RBFHiddenLayer(SavedNetworkLayer* layer, Network* parent) :
    RBFLayer(layer->infoMap(), parent)
{
    init();
    addNeuron(new RBFBiasNeuron(this));

    for (auto* savedNeuron : layer->savedNeurons())
        addNeuron(new RBFHiddenNeuron(savedNeuron, this));
}

void RBFHiddenLayer::init()
{
    m_kmeans = new KMeansClustering(this);
}

bool RBFHiddenLayer::isTrained() const
{
    return m_trained;
}

QString RBFHiddenLayer::defaultNeuronName(int index) const
{
    if (neuron(index)->isBias())
        return tr("Bias");

    return tr("Neuron #%1").arg(index);
}

const KMeansClustering& RBFHiddenLayer::kmeans() const
{
    return *m_kmeans;
}

int RBFHiddenLayer::clusterIndexToNeuronIndex(int clusterIndex) const
{
    // Clusters are indexed from 0 and non-bias neurons from 1
    return clusterIndex + 1;
}

int RBFHiddenLayer::neuronIndexToClusterIndex(int neuronIndex) const
{
    Q_ASSERT(neuronIndex > 0);

    return neuronIndex - 1;
}

void RBFHiddenLayer::train(const QVector<TrainingSample>& samples)
{
    m_kmeans->setTrainingSamples(samples);
    m_kmeans->doClustering(neuronCount(true));

    const auto& clusters = m_kmeans->clusters();
    QMap<int, double> clusterDistance;

    for (int i = 1; i < neuronCount(); i++) {
        auto* rbfNeuron = qobject_cast<RBFHiddenNeuron*>(neuron(i));
        Q_ASSERT(rbfNeuron != nullptr);

        int clusterIndex = neuronIndexToClusterIndex(i);
        if (!clusterDistance.contains(clusterIndex))
            clusterDistance[clusterIndex] = m_kmeans->averageClusterDistance(clusterIndex);

        double closest = m_kmeans->findClosestClusterDistance(clusterIndex);
        rbfNeuron->setSigma(closest);

        for (int j = 0; j < rbfNeuron->inConnectionCount(); j++)
            rbfNeuron->inConnection(j)->setWeight(clusters.at(clusterIndex).at(j));
    }
    updateInConnectionRange();
    m_trained = true;
    emit trained();
}

//
// Set the layer as untrained.
//
// This should be done when the former kmeans run no longer matches the input data and
// indicates that the kmeans should not be trusted anymore.
//
void RBFHiddenLayer::untrain()
{
    m_trained = false;
    emit untrained();
}
