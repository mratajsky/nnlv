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
#include "slplayer.h"

#include "graphicsutilities.h"
#include "slpneuron.h"

SLPLayer::SLPLayer(NetworkLayerInfo::Type type, Network* parent) :
    SupervisedLayer(type, parent)
{
}

SLPLayer::SLPLayer(const NetworkLayerInfo::Map& map, Network* parent) :
    SupervisedLayer(map, parent)
{
}

void SLPLayer::updateScenePosition(QRectF neuronBoundingRect)
{
    if (neuronCount() == 0)
        return;

    // All neurons have the same height
    auto neuronHeight = neuron(0)->boundingRect().height();
    //
    // Single neuron Y-offset
    //
    auto height = GraphicsUtilities::sceneViewHeight(scene());
    auto offset = qMax(0.0, (height - neuronCount() * neuronHeight) / (neuronCount() + 1));

    for (auto i = 0; i < neuronCount(); i++) {
        auto n = qobject_cast<SLPNeuron*>(neuron(i));
        auto y = (i+1)*offset + i*neuronHeight + neuronHeight/2;
        n->setX(0);
        n->setY(y);
        n->setBoundingRect(neuronBoundingRect);
        QMetaObject::invokeMethod(n, "updateConnectionPaths", Qt::QueuedConnection);
    }
}
