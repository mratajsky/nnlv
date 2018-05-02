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
#include "custominputdialog.h"
#include "ui_custominputdialog.h"

#include <limits>
#include <QDoubleSpinBox>

CustomInputDialog::CustomInputDialog(Network* network, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::CustomInputDialog),
    m_network(network)
{
    ui->setupUi(this);
    init();
}

CustomInputDialog::~CustomInputDialog()
{
    delete ui;
}

void CustomInputDialog::init()
{
    auto* inputLayer = m_network->inputLayer();
    //
    // Create a spin box for every input neuron and add it to the frame layout
    //
    const auto& neurons = inputLayer->neurons();
    for (const auto* neuron : neurons) {
        auto* spinBox = new QDoubleSpinBox(this);
        spinBox->setDecimals(6);
        spinBox->setRange(std::numeric_limits<double>::lowest(),
                          std::numeric_limits<double>::max());
        spinBox->setValue(neuron->value());
        spinBox->setMinimumWidth(300);
        if (neuron->isBias())
            spinBox->setReadOnly(true);

        ui->formLayout->addRow(neuron->name() + ":", spinBox);
    }
    ui->formLayout->itemAt(0, QFormLayout::FieldRole)->widget()->setFocus();
}

void CustomInputDialog::accept()
{
    QVector<double> input;

    for (int i = 0; i < ui->formLayout->rowCount(); i++) {
        auto* widget = ui->formLayout->itemAt(i, QFormLayout::FieldRole)->widget();
        auto* spinBox = qobject_cast<QDoubleSpinBox*>(widget);
        Q_ASSERT(spinBox != nullptr);
        // The input vector does not include bias
        if (spinBox->isReadOnly())
            continue;

        input.append(spinBox->value());
    }
    m_network->computeAndSet(input);
    QDialog::accept();
}
