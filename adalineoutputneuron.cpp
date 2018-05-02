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
#include "adalineoutputneuron.h"

AdalineOutputNeuron::AdalineOutputNeuron(NetworkLayer *parent) :
    SLPOutputNeuron(parent)
{
}

AdalineOutputNeuron::AdalineOutputNeuron(SavedNetworkNeuron* savedNeuron, NetworkLayer* parent) :
    SLPOutputNeuron(savedNeuron, parent)
{
}

double AdalineOutputNeuron::compute(const QVector<double>& input) const
{
    double value = inConnection(0)->weight();
    for (int i = 1; i < input.size(); i++)
        value += input.at(i - 1) * inConnection(i)->weight();

    return value;
}

void AdalineOutputNeuron::train(const QVector<double>& input, int target)
{
    double value = 0.0;
    const auto& connections = inConnections();
    for (const auto* conn : connections)
        value += conn->neuron1()->value() * conn->weight();

    double rate = learningRate();
    for (int i = 0; i < inConnectionCount(); i++) {
        auto* conn = inConnection(i);

        double weight = rate * (value - target);
        if (i > 0)
            weight *= input[i - 1];

        conn->setWeight(conn->weight() - weight);
    }
    setValue(value);
}
