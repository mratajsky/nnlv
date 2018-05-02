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
#include "mlphiddenlayer.h"

#include "mlpbiasneuron.h"
#include "mlphiddenneuron.h"

MLPHiddenLayer::MLPHiddenLayer(int neurons, int layerNo, Network* parent) :
    MLPLayer(NetworkLayerInfo::Type::Hidden, parent)
{
    // Default layer name
    setName(tr("Hidden Layer %1").arg(layerNo));

    addNeuron(new MLPBiasNeuron(this));
    for (auto i = 0; i < neurons; i++)
        addNeuron(new MLPHiddenNeuron(this), true);
}

MLPHiddenLayer::MLPHiddenLayer(SavedNetworkLayer* layer, Network* parent) :
    MLPLayer(layer->infoMap(), parent)
{
    addNeuron(new MLPBiasNeuron(this));
    for (auto* savedNeuron : layer->savedNeurons())
        addNeuron(new MLPHiddenNeuron(savedNeuron, this));
}
