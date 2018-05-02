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

#include <QDoubleSpinBox>

#include "networkneurondialog.h"
#include "rbfhiddenneuron.h"

class RBFNetworkNeuronDialog : public NetworkNeuronDialog
{
public:
    explicit RBFNetworkNeuronDialog(NetworkNeuron* neuron, QWidget* parent = nullptr);

public slots:
    void accept() override;

private:
    void init();

    RBFHiddenNeuron* m_hiddenNeuron;
    QDoubleSpinBox* m_spinBox;
};
