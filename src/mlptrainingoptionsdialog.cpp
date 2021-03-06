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
#include "mlptrainingoptionsdialog.h"
#include "ui_mlptrainingoptionsdialog.h"

#include "mlpactivation.h"
#include "mlpnetwork.h"

MLPTrainingOptionsDialog::MLPTrainingOptionsDialog(Network* network, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::MLPTrainingOptionsDialog),
    m_network(network)
{
    ui->setupUi(this);
    init();
}

MLPTrainingOptionsDialog::~MLPTrainingOptionsDialog()
{
    delete ui;
}

void MLPTrainingOptionsDialog::init()
{
    const auto& map = m_network->infoMap();

    ui->editLearningRate->setValue(map[NetworkInfo::Key::LearningRate].toDouble());

    if (map.contains(NetworkInfo::Key::MaxEpochs)) {
        ui->checkBoxMaxEpochs->setChecked(true);
        ui->editMaxEpochs->setEnabled(true);
        ui->editMaxEpochs->setValue(map[NetworkInfo::Key::MaxEpochs].toInt());
    }
    if (map.contains(NetworkInfo::Key::StopError)) {
        ui->checkBoxStopError->setChecked(true);
        ui->editStopError->setEnabled(true);
        ui->editStopError->setValue(map[NetworkInfo::Key::StopError].toDouble());
    }
    if (map.contains(NetworkInfo::Key::PauseAfterSample))
        ui->checkBoxPauseAfterSample->setChecked(map[NetworkInfo::Key::PauseAfterSample].toBool());

    //
    // Populate the combo boxes
    //
    ui->comboBoxSampleSelectionOrder->addItems(NetworkInfo::sampleSelectionOrderStringList());
    if (map.contains(NetworkInfo::Key::SampleSelectionOrder))
        ui->comboBoxSampleSelectionOrder->setCurrentIndex(NetworkInfo::sampleSelectionOrderIndexFromMap(map));

    ui->comboBoxActivationFunction->addItems(MLPActivation::functionStringList());
    if (map.contains(NetworkInfo::Key::ActivationFunction))
        ui->comboBoxActivationFunction->setCurrentIndex(MLPActivation::functionIndexFromMap(map));
}

void MLPTrainingOptionsDialog::accept()
{
    m_network->setLearningRate(ui->editLearningRate->value());

    if (ui->checkBoxMaxEpochs->isChecked())
        m_network->setMaxTrainingEpochs(ui->editMaxEpochs->value());
    else
        m_network->setMaxTrainingEpochs(0);

    m_network->setPauseAfterSample(ui->checkBoxPauseAfterSample->isChecked());

    m_network->setSampleSelectionOrder(
                NetworkInfo::sampleSelectionOrderFromIndex(
                    ui->comboBoxSampleSelectionOrder->currentIndex()));

    auto* mlpNetwork = qobject_cast<MLPNetwork*>(m_network);
    mlpNetwork->setActivationFunction(
                MLPActivation::functionFromIndex(
                    ui->comboBoxActivationFunction->currentIndex()));

    if (ui->checkBoxStopError->isChecked())
        m_network->setStopError(ui->editStopError->value());
    else
        m_network->setStopError(0);

    QDialog::accept();
}

void MLPTrainingOptionsDialog::on_checkBoxMaxEpochs_clicked(bool checked)
{
    ui->editMaxEpochs->setEnabled(checked);
}

void MLPTrainingOptionsDialog::on_checkBoxStopError_clicked(bool checked)
{
    ui->editStopError->setEnabled(checked);
}
