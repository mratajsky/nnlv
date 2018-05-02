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
#include "rbfnetwork.h"

#include "graphicsutilities.h"
#include "rbfhiddenlayer.h"
#include "rbfinputlayer.h"
#include "rbfoutputlayer.h"

RBFNetwork::RBFNetwork(const NetworkInfo::Map& map, QObject* parent) :
    SupervisedNetwork(map, parent)
{
    auto weightFunction = createInitialWeightFunction();

    //
    // Create layers
    //
    int inputCount = m_infoMap.value(NetworkInfo::Key::InputNeuronCount).toInt();
    m_inputLayer = new RBFInputLayer(inputCount, this);
    addLayer(m_inputLayer);

    auto hiddenCountList = m_infoMap
                           .value(NetworkInfo::Key::HiddenNeuronCount)
                           .value<QVector<int>>();
    m_hiddenLayer = new RBFHiddenLayer(hiddenCountList.first(), this);
    m_hiddenLayer->setInitialWeightFunction(weightFunction);
    addLayer(m_hiddenLayer);

    m_inputLayer->connectTo(m_hiddenLayer);

    int outputCount = m_infoMap.value(NetworkInfo::Key::OutputNeuronCount).toInt();
    m_outputLayer = new RBFOutputLayer(outputCount, this);
    m_outputLayer->setLearningRate(learningRate());
    addLayer(m_outputLayer);

    m_hiddenLayer->connectTo(m_outputLayer);

    init();
}

RBFNetwork::RBFNetwork(const SavedNetwork& savedNetwork, QObject* parent) :
    SupervisedNetwork(savedNetwork.infoMap(), parent)
{
    auto weightFunction = createInitialWeightFunction();

    auto savedLayers = savedNetwork.savedLayers();
    //
    // Create layers
    //
    m_inputLayer = new RBFInputLayer(savedLayers.at(0), this);
    addLayer(m_inputLayer);

    m_hiddenLayer = new RBFHiddenLayer(savedLayers.at(1), this);
    m_hiddenLayer->setInitialWeightFunction(weightFunction);
    addLayer(m_hiddenLayer);

    m_inputLayer->connectTo(m_hiddenLayer);

    m_outputLayer = new RBFOutputLayer(savedLayers.at(2), this);
    m_outputLayer->setLearningRate(learningRate());
    addLayer(m_outputLayer);

    m_hiddenLayer->connectTo(m_outputLayer);

    init();
}

void RBFNetwork::init()
{
    if (m_infoMap.contains(NetworkInfo::Key::StopSamples))
        m_trainRBFLayer = m_infoMap[NetworkInfo::Key::StopSamples].toBool();

    const auto& store = trainingTableModel()->store();
    connect(&store, &TrainingSampleStore::samplesChanged, this, [this] {
        //
        // Mark the hidden layer as untrained when it no longer matches the
        // training samples
        //
        m_hiddenLayer->untrain();
    });

    connect(this, &Network::infoChanged, this, [this] {
        m_outputLayer->setLearningRate(learningRate());
    });
}

void RBFNetwork::setTrainRBFLayer(bool trainRBFLayer)
{
    if (m_trainRBFLayer != trainRBFLayer) {
        m_trainRBFLayer = trainRBFLayer;
        m_infoMap[NetworkInfo::Key::TrainRBFLayer] = trainRBFLayer;
        emit infoChanged();
    }
}

QVector<double> RBFNetwork::compute(const QVector<double>& input) const
{
    auto vector = input;
    vector = m_hiddenLayer->compute(vector);
    vector = m_outputLayer->compute(vector);
    return vector;
}

void RBFNetwork::computeAndSet(const QVector<double>& input)
{
    m_inputLayer->setValues(input);
    m_hiddenLayer->forward();
    m_outputLayer->forward();
}

void RBFNetwork::prepareTraining()
{
    if (!m_trainRBFLayer)
        return;

    qDebug() << "Training the RBF layer";
    m_hiddenLayer->train(trainingTableModel()->store().samples());
    m_outputLayer->resetRange();
    if (pauseAfterSample())
        pauseTraining();
}

void RBFNetwork::train(const TrainingSample& sample)
{
    m_inputLayer->setValues(sample.inputs());
    m_hiddenLayer->forward();
    m_outputLayer->forward();
    m_outputLayer->updateWeights(sample.outputs());
}

void RBFNetwork::updateScenePosition()
{
    auto height = GraphicsUtilities::sceneViewHeight(scene());
    auto width  = GraphicsUtilities::sceneViewWidth(scene());
    if (height <= 0 || width <= 0)
        return;

    //
    // Find out the maximal number of neurons in a single layer
    //
    int maxNeurons = 0;
    for (auto* l : layers())
        maxNeurons = qMax(maxNeurons, l->neuronCount());
    if (maxNeurons == 0)
        return;
    //
    // Calculate neuron size
    //
    double neuronSize = (height / maxNeurons) * 0.5;
    QRectF neuronRect(-neuronSize / 2, -neuronSize / 2, neuronSize, neuronSize);

    double offset = (width - layerCount() * neuronSize) / (layerCount() + 1);
    for (int i = 0; i < layerCount(); i++) {
        //
        // Move the layer to the correct position
        //
        auto* l = qobject_cast<SLPLayer*>(layer(i));

        l->setX(i * neuronSize + neuronSize / 2 + (i + 1) * offset);
        //
        // Update after moving the layer as this also fixes the connection
        // positions
        //
        l->updateScenePosition(neuronRect);
    }
}
