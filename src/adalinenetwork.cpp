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
#include "adalinenetwork.h"

#include "graphicsutilities.h"

AdalineNetwork::AdalineNetwork(const NetworkInfo::Map& map, QObject* parent) :
    SupervisedNetwork(map, parent)
{
    init();

    auto weightFunction = createInitialWeightFunction();
    //
    // Create layers
    //
    int inputCount = m_infoMap.value(NetworkInfo::Key::InputNeuronCount).toInt();
    m_inputLayer = new AdalineInputLayer(inputCount, this);
    m_inputLayer->setInitialWeightFunction(weightFunction);
    addLayer(m_inputLayer);

    int outputCount = m_infoMap.value(NetworkInfo::Key::OutputNeuronCount).toInt();
    m_outputLayer = new AdalineOutputLayer(outputCount, this);
    m_outputLayer->setLearningRate(learningRate());
    addLayer(m_outputLayer);

    m_inputLayer->connectTo(m_outputLayer);
}

AdalineNetwork::AdalineNetwork(const SavedNetwork& savedNetwork, QObject* parent) :
    SupervisedNetwork(savedNetwork.infoMap(), parent)
{
    init();

    auto weightFunction = createInitialWeightFunction();
    //
    // Create layers
    //
    const auto& savedLayers = savedNetwork.savedLayers();
    m_inputLayer = new AdalineInputLayer(savedLayers.at(0), this);
    m_inputLayer->setInitialWeightFunction(weightFunction);
    addLayer(m_inputLayer);

    m_outputLayer = new AdalineOutputLayer(savedLayers.at(1), this);
    m_outputLayer->setLearningRate(learningRate());
    addLayer(m_outputLayer);

    m_inputLayer->connectTo(m_outputLayer);
}

void AdalineNetwork::init()
{
    auto* model = trainingTableModel();

    model->setInitialValue(-1);
    model->setStoreFieldValidator([model](int index, double value, QString& err)->bool {
        if (index >= model->store().inputCount()) {
            double ipart;
            double fpart = std::modf(value, &ipart);
            if (qFuzzyIsNull(fpart)) {
                int ipartInt = static_cast<int>(ipart);
                if (ipartInt == -1 || ipartInt == 1)
                    return true;
            }
            err = tr("Output value must be either -1 or 1, given: %1").arg(value);
            return false;
        }
        // Validation is not needed for input values
        return true;
    });

    model->setStoreSampleValidator([model](const TrainingSample& sample, QString& err)->bool {
        int errIndex = -1;
        for (int i = 0; i < sample.outputCount(); i++) {
            double ipart;
            double fpart = std::modf(sample.output(i), &ipart);
            if (!qFuzzyIsNull(fpart)) {
                errIndex = i;
                break;
            }
            int ipartInt = static_cast<int>(ipart);
            if (ipartInt != -1 && ipartInt != 1) {
                errIndex = i;
                break;
            }
        }
        if (errIndex > -1) {
            err = tr("Output value must be either -1 or 1, given: %1")
                  .arg(sample.output(errIndex));
            return false;
        }
        return true;
    });

    connect(this, &Network::infoChanged, this, [this] {
        m_outputLayer->setLearningRate(learningRate());
    });
}

QVector<double> AdalineNetwork::compute(const QVector<double>& input) const
{
    return m_outputLayer->compute(input);
}

void AdalineNetwork::computeAndSet(const QVector<double>& input)
{
    m_inputLayer->setValues(input);
    m_outputLayer->computeAndSet(input);
}

void AdalineNetwork::train(const TrainingSample& sample)
{
    m_inputLayer->setValues(sample.inputs());
    m_outputLayer->train(sample);
}

void AdalineNetwork::updateScenePosition()
{
    auto rect = GraphicsUtilities::neuronRect(scene(), layerCount(), neuronMaxCount());
    if (!rect.isValid())
        return;
    auto sceneWidth = GraphicsUtilities::sceneViewWidth(scene());
    auto size = rect.width();
    auto move = qMax(0.0, (sceneWidth - layerCount() * size) / (layerCount() + 1));

    m_inputLayer->setX(move + size / 2);
    m_inputLayer->updateScenePosition(rect);

    m_outputLayer->setX(2 * move + 1.5 * size);
    m_outputLayer->updateScenePosition(rect);
}
