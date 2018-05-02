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
#include "networkneurondialogspinbox.h"

#include <limits>

NetworkNeuronDialogSpinBox::NetworkNeuronDialogSpinBox(Network* network, NetworkConnection* conn, QWidget* parent) :
    QDoubleSpinBox(parent),
    m_network(network),
    m_connection(conn)
{
    init();
}

void NetworkNeuronDialogSpinBox::init()
{
    if (m_network->isTraining() || m_network->isTrainingPaused())
        setReadOnly(true);

    connect(m_network, &Network::trainingStarted, this, [this] {
        setReadOnly(true);
    });
    connect(m_network, &Network::trainingStopped, this, [this] {
        setReadOnly(false);
    });

    setDecimals(6);
    setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    setValue(m_connection->weight());
}

void NetworkNeuronDialogSpinBox::save()
{
    m_connection->setWeight(value());
}
