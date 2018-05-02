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
#include "mlpnetwork.h"

#include "graphicsutilities.h"
#include "mlphiddenlayer.h"
#include "mlpinputlayer.h"
#include "mlpoutputlayer.h"

MLPNetwork::MLPNetwork(const NetworkInfo::Map& map, QObject* parent) :
    SupervisedNetwork(map, parent)
{
    init();

    auto weightFunction = createInitialWeightFunction();
    //
    // Create layers
    //
    int inputCount = m_infoMap.value(NetworkInfo::Key::InputNeuronCount).toInt();
    m_inputLayer = new MLPInputLayer(inputCount, this);
    m_inputLayer->setInitialWeightFunction(weightFunction);
    addLayer(m_inputLayer);

    // Current layer that will be connected to the next layer
    MLPLayer* lastLayer = m_inputLayer;
    auto hiddenCountList = m_infoMap
                           .value(NetworkInfo::Key::HiddenNeuronCount)
                           .value<QVector<int>>();
    for (int i = 0; i < hiddenCountList.size(); i++) {
        auto* hiddenLayer = new MLPHiddenLayer(hiddenCountList[i], i + 1, this);

        hiddenLayer->setInitialWeightFunction(weightFunction);
        hiddenLayer->setLearningRate(learningRate());
        addLayer(hiddenLayer);

        lastLayer->connectTo(hiddenLayer);
        lastLayer = hiddenLayer;
    }

    int outputCount = m_infoMap.value(NetworkInfo::Key::OutputNeuronCount).toInt();
    m_outputLayer = new MLPOutputLayer(outputCount, this);
    m_outputLayer->setLearningRate(learningRate());
    addLayer(m_outputLayer);

    lastLayer->connectTo(m_outputLayer);
}

MLPNetwork::MLPNetwork(const SavedNetwork& savedNetwork, QObject* parent) :
    SupervisedNetwork(savedNetwork.infoMap(), parent)
{
    init();

    auto weightFunction = createInitialWeightFunction();
    //
    // Create layers
    //
    auto savedLayers = savedNetwork.savedLayers();

    m_inputLayer = new MLPInputLayer(savedLayers.at(0), this);
    m_inputLayer->setInitialWeightFunction(weightFunction);
    addLayer(m_inputLayer);

    // Current layer that will be connected to the next layer
    MLPLayer* lastLayer = m_inputLayer;
    for (int i = 1; i < savedLayers.size() - 1; i++) {
        auto* hiddenLayer = new MLPHiddenLayer(savedLayers.at(i), this);

        hiddenLayer->setInitialWeightFunction(weightFunction);
        hiddenLayer->setLearningRate(learningRate());
        addLayer(hiddenLayer);

        lastLayer->connectTo(hiddenLayer);
        lastLayer = hiddenLayer;
    }

    m_outputLayer = new MLPOutputLayer(savedLayers.last(), this);
    m_outputLayer->setLearningRate(learningRate());
    addLayer(m_outputLayer);

    lastLayer->connectTo(m_outputLayer);
}

void MLPNetwork::init()
{
    if (m_infoMap.contains(NetworkInfo::Key::ActivationFunction))
        setActivationFunction(MLPActivation::functionFromMap(m_infoMap));
    else
        m_infoMap[NetworkInfo::Key::ActivationFunction] =
                QVariant::fromValue(MLPActivation::Function::Sigmoid);

    connect(this, &Network::infoChanged, this, [this] {
        //
        // Set the learning rate in all non-input layers
        //
        auto rate = learningRate();
        for (int i = 0; i < layerCount(); i++) {
            auto* layer = mlpLayer(i);
            if (layer->infoType() == NetworkLayerInfo::Type::Input)
                continue;
            layer->setLearningRate(rate);
        }
    });
}

NetworkInfo::Map MLPNetwork::createDefaultInfoMap() const
{
    auto map = Network::createDefaultInfoMap();

    // XXX rename to include MLP in the name?
    map[NetworkInfo::Key::ActivationFunction] =
            QVariant::fromValue(MLPActivation::Function::Sigmoid);

    return map;
}

QVector<double> MLPNetwork::compute(const QVector<double>& input) const
{
    QVector<double> vector = input;
    for (int i = 1; i < layerCount(); i++)
        vector = mlpLayer(i)->compute(vector);

    return vector;
}

void MLPNetwork::computeAndSet(const QVector<double>& input)
{
    QVector<double> vector = input;

    // Compute and use the result as input for the next layer
    m_inputLayer->setValues(input);
    for (int i = 1; i < layerCount(); i++) {
        auto* layer = mlpLayer(i);

        layer->computeAndSet(vector);
        if (layer->infoType() == NetworkLayerInfo::Type::Output)
            break;
        vector = layer->values(true);
    }
}

MLPLayer* MLPNetwork::mlpLayer(int index) const
{
    return qobject_cast<MLPLayer*>(layer(index));
}

void MLPNetwork::train(const TrainingSample& sample)
{
    m_inputLayer->setValues(sample.inputs());

    for (int i = 1; i < layerCount(); i++)
        mlpLayer(i)->forward();

    m_outputLayer->updateDelta(sample.outputs());

    for (int i = layerCount() - 2; i > 0; i--)
        mlpLayer(i)->updateDelta();
    for (int i = layerCount() - 1; i > 0; i--)
        mlpLayer(i)->updateWeights();
}

void MLPNetwork::setActivationFunction(MLPActivation::Function function)
{
    if (function != MLPActivation::functionFromMap(m_infoMap)) {
        for (int i = 1; i < layerCount(); i++)
            mlpLayer(i)->setActivationFunction(function);

        m_infoMap[NetworkInfo::Key::ActivationFunction] = QVariant::fromValue(function);
        emit infoChanged();
    }
}

void MLPNetwork::updateScenePosition()
{
    auto rect = GraphicsUtilities::neuronRect(scene(), layerCount(), neuronMaxCount());
    if (!rect.isValid())
        return;
    auto size = rect.width();
    auto move = qMax(0.0, (GraphicsUtilities::sceneViewWidth(scene()) - layerCount() * size) / (layerCount()+1));

    for (int i = 0; i < layerCount(); i++) {
        auto* layer = mlpLayer(i);
        //
        // Move the layer and after that request the neurons to be moved, connections
        // will be fixed while while the neurons are moved
        //
        layer->setX((i+1)*move + i*size + size / 2);
        layer->updateScenePosition(rect);
    }
}
