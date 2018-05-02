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
#include "kohonenoutputlayer.h"

#include <cmath>

#include "graphicsutilities.h"
#include "kohonenoutputneuron.h"

KohonenOutputLayer::KohonenOutputLayer(int neurons, Network *parent) :
    KohonenLayer(NetworkLayerInfo::Type::Output, parent)
{
    // Default layer name
    setName(tr("Output Layer"));

    int neuronSideCount = std::sqrt(neurons);
    for (int y = 0; y < neuronSideCount; y++)
        for (int x = 0; x < neuronSideCount; x++)
            addNeuron(new KohonenOutputNeuron(x, y, this), true);
    init();
}

KohonenOutputLayer::KohonenOutputLayer(SavedNetworkLayer* layer, Network* parent) :
    KohonenLayer(layer->infoMap(), parent)
{
    const auto& savedNeurons = layer->savedNeurons();
    for (auto* savedNeuron : savedNeurons)
        addNeuron(new KohonenOutputNeuron(savedNeuron, this));

    init();
}

void KohonenOutputLayer::init()
{
    //
    // fwhm = 2.3548 sigma
    // http://mathworld.wolfram.com/GaussianFunction.html
    //
    double radius = std::sqrt(neuronCount()) / 2;
    m_sigma = radius / 1.1774;
}

void KohonenOutputLayer::computeAndSet()
{
    updateBMU();

    // Assign the value 1 to the BMU and 0 to all other output neurons
    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = kohonenOutputNeuron(i);
        if (neuron == m_bmu)
            neuron->setValue(1);
        else
            neuron->setValue(0);
    }
}

void KohonenOutputLayer::updateBMU(double* bmuDistance)
{
    m_bmu = nullptr;

    double minDistance = qInf();
    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = kohonenOutputNeuron(i);

        double distance = 0;
        const auto& connections = neuron->inConnections();
        for (auto* conn : connections) {
            double value = conn->weight() - conn->neuron1()->value();
            distance += value * value;
        }
        distance = std::sqrt(distance);
        if (distance < minDistance) {
            m_bmu = neuron;
            minDistance = distance;
        }
    }
    if (bmuDistance != nullptr)
        *bmuDistance = minDistance;
}

KohonenOutputNeuron* KohonenOutputLayer::currentBMU() const
{
    return m_bmu;
}

QString KohonenOutputLayer::defaultNeuronName(int index) const
{
    const auto& map = neuron(index)->infoMap();

    return QString("Neuron %1/%2")
            .arg(map[NetworkNeuronInfo::Key::PositionY].toInt() + 1)
            .arg(map[NetworkNeuronInfo::Key::PositionX].toInt() + 1);
}

KohonenOutputNeuron* KohonenOutputLayer::kohonenOutputNeuron(int index) const
{
    return qobject_cast<KohonenOutputNeuron*>(neuron(index));
}

void KohonenOutputLayer::setLearningRate(double learningRate)
{
    for (int i = 0; i < neuronCount(); i++)
        kohonenOutputNeuron(i)->setLearningRate(learningRate);
}

void KohonenOutputLayer::train(int currentTrainingEpoch, int trainingEpochs)
{
    updateBMU();

    double sigma = m_sigma - ((m_sigma * currentTrainingEpoch) / trainingEpochs);
    double twoSigmaSquare = 2 * sigma * sigma;

    for (int i = 0; i < neuronCount(); i++) {
        auto* neuron = kohonenOutputNeuron(i);

        int dx = qAbs(m_bmu->position().x() - neuron->position().x());
        int dy = qAbs(m_bmu->position().y() - neuron->position().y());
        double dxSquared = dx * dx;
        double dySquared = dy * dy;
        double neighborhoodValue = std::exp(-(dxSquared + dySquared) / twoSigmaSquare);
        neuron->setValue(neighborhoodValue);
        neuron->updateWeights();
    }
}

void KohonenOutputLayer::updateScenePosition(double inputWidth)
{
    int height = GraphicsUtilities::sceneViewHeight(scene());
    int width  = GraphicsUtilities::sceneViewWidth(scene());
    if (height <= 0 || width <= 0)
        return;

    int neuronSideCount = std::sqrt(neuronCount());

    auto neuronSize = (qMin(width - inputWidth, height * 1.0) / neuronSideCount) * .3;
    auto neuronRect = QRectF(-neuronSize / 2, -neuronSize / 2, neuronSize, neuronSize);

    auto offset = qMax(0.0, (qMin(width - inputWidth, height * 1.0) - neuronSideCount * neuronSize) / (neuronSideCount + 1));
    for (int i = 0; i < neuronCount(); i++) {
        auto n = kohonenOutputNeuron(i);
        auto row = i / neuronSideCount;
        auto col = i % neuronSideCount;
        auto x = col*offset + col*neuronSize + neuronSize / 2;
        auto y = row*offset + row*neuronSize + neuronSize / 2;
        n->setX(x);
        n->setY(y);
        n->setBoundingRect(neuronRect);
        QMetaObject::invokeMethod(n, "updateConnectionPaths", Qt::QueuedConnection);
    }

    const auto* first = neurons().first();
    const auto* last = neurons().last();
    QRectF rect(QPointF(first->x(), first->y()),
           QPointF(last->x() + neuronSize, last->y() + neuronSize));

    setBoundingRect(rect);
}
