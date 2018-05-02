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
#include "kohonennetwork.h"

#include "graphicsutilities.h"
#include "kohoneninputlayer.h"
#include "kohonenoutputlayer.h"
#include "kohonennetworkdefaults.h"

KohonenNetwork::KohonenNetwork(const NetworkInfo::Map& map, QObject* parent) :
    Network(map, parent)
{
    auto weightFunction = createInitialWeightFunction();

    //
    // Create layers
    //
    int inputCount = m_infoMap.value(NetworkInfo::Key::InputNeuronCount).toInt();
    m_inputLayer = new KohonenInputLayer(inputCount, this);
    m_inputLayer->setInitialWeightFunction(weightFunction);
    addLayer(m_inputLayer);

    int outputCount = m_infoMap.value(NetworkInfo::Key::OutputNeuronCount).toInt();
    m_outputLayer = new KohonenOutputLayer(outputCount, this);
    addLayer(m_outputLayer);

    m_inputLayer->connectTo(m_outputLayer);
}

KohonenNetwork::KohonenNetwork(const SavedNetwork& savedNetwork, QObject* parent) :
    Network(savedNetwork.infoMap(), parent)
{
    auto weightFunction = createInitialWeightFunction();

    //
    // Create layers
    //
    auto savedLayers = savedNetwork.savedLayers();

    m_inputLayer = new KohonenInputLayer(savedLayers.at(0), this);
    m_inputLayer->setInitialWeightFunction(weightFunction);
    addLayer(m_inputLayer);

    m_outputLayer = new KohonenOutputLayer(savedLayers.last(), this);
    addLayer(m_outputLayer);

    m_inputLayer->connectTo(m_outputLayer);
}

void KohonenNetwork::computeAndSet(const QVector<double>& input)
{
    m_inputLayer->setValues(input);
    m_outputLayer->computeAndSet();
}

NetworkInfo::Map KohonenNetwork::createDefaultInfoMap() const
{
    auto map = Network::createDefaultInfoMap();

    map[NetworkInfo::Key::MaxEpochs] = KohonenNetworkDefaults::maxTrainingEpochs;

    return map;
}

void KohonenNetwork::prepareTraining()
{
    m_localMaxEpochs = maxTrainingEpochs();
    m_localLearningRate = learningRate();

    m_outputLayer->setLearningRate(m_localLearningRate);
}

void KohonenNetwork::train(const TrainingSample& sample)
{
    m_inputLayer->setValues(sample.inputs());
    m_outputLayer->train(trainingEpochs(), m_localMaxEpochs);
}

void KohonenNetwork::updateScenePosition()
{
    auto height = GraphicsUtilities::sceneViewHeight(scene());
    auto width  = GraphicsUtilities::sceneViewWidth(scene());
    if (height <= 0 || width <= 0)
        return;

    m_inputLayer->updateScenePosition();
    auto inputRect = m_inputLayer->boundingRect();
    auto inputHeight = inputRect.height();
    auto inputWidth = inputRect.width();

    m_outputLayer->updateScenePosition(inputWidth * 2);
    auto outputRect = m_outputLayer->boundingRect();
    auto outputHeight = outputRect.height();
    auto outputWidth = outputRect.width();

    m_outputLayer->setX((width - outputWidth + inputWidth * 2) / 2);
    m_outputLayer->setY((height - outputHeight) / 2);

    m_inputLayer->setX(m_outputLayer->x() - inputWidth * 2);
    m_inputLayer->setY((height - inputHeight) / 2);
}
