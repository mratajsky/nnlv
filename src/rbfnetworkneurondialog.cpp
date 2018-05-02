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
#include "rbfnetworkneurondialog.h"

#include <limits>

#include "ui_networkneurondialog.h"

RBFNetworkNeuronDialog::RBFNetworkNeuronDialog(NetworkNeuron* neuron, QWidget* parent) :
    NetworkNeuronDialog(neuron, parent)
{
    init();
}

void RBFNetworkNeuronDialog::init()
{
    m_hiddenNeuron = qobject_cast<RBFHiddenNeuron*>(neuron());

    m_spinBox = new QDoubleSpinBox(this);
    m_spinBox->setDecimals(6);
    m_spinBox->setRange(0, std::numeric_limits<double>::max());
    m_spinBox->setValue(m_hiddenNeuron->sigma());

    int row;
    ui->formLayout->getWidgetPosition(ui->editValue, &row, nullptr);
    //
    // Put the sigma spinbox just below the value spinbox
    //
    ui->formLayout->insertRow(row + 1, tr("Sigma:"), m_spinBox);
}

void RBFNetworkNeuronDialog::accept()
{
    m_hiddenNeuron->setSigma(m_spinBox->value());

    NetworkNeuronDialog::accept();
}
