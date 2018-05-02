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
#include "resetweightsdialog.h"
#include "ui_resetweightsdialog.h"

#include <QCheckBox>
#include <QStringBuilder>

ResetWeightsDialog::ResetWeightsDialog(Network* network, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ResetWeightsDialog),
    m_network(network)
{
    ui->setupUi(this);
    init();
}

ResetWeightsDialog::~ResetWeightsDialog()
{
    delete ui;
}

void ResetWeightsDialog::init()
{
    const auto& map = m_network->infoMap();

    //
    // Weight value is preferred
    //
    if (map.contains(NetworkInfo::Key::InitialWeightsValue)) {
        ui->editValue->setValue(map[NetworkInfo::Key::InitialWeightsValue].toDouble());
        ui->radioValue->setChecked(true);
    } else {
        double min = -1; // default
        if (map.contains(NetworkInfo::Key::InitialWeightsRandomMin))
            min = map[NetworkInfo::Key::InitialWeightsRandomMin].toDouble();
        double max = 1;  // default
        if (map.contains(NetworkInfo::Key::InitialWeightsRandomMax))
            max = map[NetworkInfo::Key::InitialWeightsRandomMax].toDouble();
        if (min > max)
            min = max;
        ui->editRangeMin->setValue(min);
        ui->editRangeMax->setValue(max);
        ui->radioRange->setChecked(true);
    }

    for (int i = 0; i < m_network->layerCount() - 1; i++) {
        auto* layer1 = m_network->layer(i);
        auto* layer2 = m_network->layer(i + 1);
        //
        // The layer might not have resettable weights, in that case keep the
        // checkbox, but disable it
        //
        auto* checkBox = new QCheckBox(this);
        if (layer1->outConnectionWeightsSettable())
            checkBox->setChecked(true);
        else
            checkBox->setEnabled(false);
        checkBox->setText(layer1->name() % " → " % layer2->name());

        ui->formLayout->addRow(checkBox);
        connect(checkBox, &QCheckBox::clicked, this, [this, i](bool checked) {
            m_layers[i] = checked;
        });
        m_layers[i] = true;
    }
}

void ResetWeightsDialog::accept()
{
    QVector<int> layers;
    QMapIterator<int, bool> i(m_layers);
    while (i.hasNext()) {
        i.next();
        if (i.value())
            layers.append(i.key());
    }
    if (!layers.empty()) {
        //
        // Update the weight values, this will also update the NetworkInfo::Map
        //
        if (ui->radioRange->isChecked())
            m_network->setConnectionWeights(ui->editRangeMin->value(),
                                            ui->editRangeMax->value(), layers);
        else
            m_network->setConnectionWeights(ui->editValue->value(), layers);
    }
    QDialog::accept();
}

void ResetWeightsDialog::on_radioRange_toggled(bool checked)
{
    if (checked) {
        ui->editRangeMin->setEnabled(true);
        ui->editRangeMax->setEnabled(true);
        ui->editValue->setEnabled(false);
        ui->editRangeMin->setFocus();
    }
}

void ResetWeightsDialog::on_radioValue_toggled(bool checked)
{
    if (checked) {
        ui->editRangeMin->setEnabled(false);
        ui->editRangeMax->setEnabled(false);
        ui->editValue->setEnabled(true);
        ui->editValue->setFocus();
    }
}
