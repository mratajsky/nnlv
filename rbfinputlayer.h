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
#pragma once

#include "common.h"

#include "savednetworklayer.h"
#include "slpinputlayer.h"

class RBFInputLayer : public SLPInputLayer
{
    Q_OBJECT
public:
    explicit RBFInputLayer(int neurons, Network *parent = nullptr);
    explicit RBFInputLayer(SavedNetworkLayer* layer, Network* parent = nullptr);

    QString defaultNeuronName(int index) const override;

    bool outConnectionWeightsSettable() const override;
};
