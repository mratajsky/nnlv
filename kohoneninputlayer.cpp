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
#include "kohoneninputlayer.h"

#include "graphicsutilities.h"
#include "networklayerinfo.h"
#include "kohoneninputneuron.h"

KohonenInputLayer::KohonenInputLayer(int neurons, Network* parent) :
    KohonenLayer(NetworkLayerInfo::Type::Input, parent)
{
    // Default layer name
    setName(tr("Input Layer"));

    for (int i = 0; i < neurons; i++)
        addNeuron(new KohonenInputNeuron(this), true);
}

KohonenInputLayer::KohonenInputLayer(SavedNetworkLayer* layer, Network* parent) :
    KohonenLayer(layer->infoMap(), parent)
{
    const auto& savedNeurons = layer->savedNeurons();
    for (auto* savedNeuron : savedNeurons)
        addNeuron(new KohonenInputNeuron(savedNeuron, this));
}

QString KohonenInputLayer::defaultNeuronName(int index) const
{
    return tr("Input #%1").arg(index + 1);
}

KohonenInputNeuron* KohonenInputLayer::kohonenInputNeuron(int index) const
{
    return qobject_cast<KohonenInputNeuron*>(neuron(index));
}

void KohonenInputLayer::updateScenePosition()
{
    int height = GraphicsUtilities::sceneViewHeight(scene());
    int width  = GraphicsUtilities::sceneViewWidth(scene());
    if (height <= 0 || width <= 0)
        return;

    auto neuronCont = neuronCount();
    auto neuronRect = GraphicsUtilities::neuronRect(scene(), 1, neuronCont, .25);
    auto neuronSize = neuronRect.width();

    auto offset = qMax(0.0, (qMin(width, height) - neuronCont * neuronSize) / (neuronCont + 1));
    for (int i = 0; i < neuronCont; i++) {
        auto n = kohonenInputNeuron(i);
        auto y = i*offset + i*neuronSize + neuronSize / 2;
        n->setX(neuronSize / 2);
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
