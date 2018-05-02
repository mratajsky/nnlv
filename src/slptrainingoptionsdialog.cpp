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
#include "slptrainingoptionsdialog.h"
#include "ui_slptrainingoptionsdialog.h"

SLPTrainingOptionsDialog::SLPTrainingOptionsDialog(Network* network, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SLPTrainingOptionsDialog),
    m_network(network)
{
    ui->setupUi(this);
    init();
}

SLPTrainingOptionsDialog::~SLPTrainingOptionsDialog()
{
    delete ui;
}

void SLPTrainingOptionsDialog::init()
{
    const auto& map = m_network->infoMap();

    if (map.contains(NetworkInfo::Key::StopPercentage)) {
        ui->checkBoxStopPercentage->setChecked(true);
        ui->editStopPercentage->setEnabled(true);
        ui->editStopPercentage->setValue(map[NetworkInfo::Key::StopPercentage].toInt());
    }
    if (map.contains(NetworkInfo::Key::StopSamples)) {
        ui->checkBoxStopSamples->setChecked(true);
        ui->editStopSamples->setEnabled(true);
        ui->editStopSamples->setValue(map[NetworkInfo::Key::StopSamples].toInt());
    }
    ui->editLearningRate->setValue(map[NetworkInfo::Key::LearningRate].toDouble());

    if (map.contains(NetworkInfo::Key::MaxEpochs)) {
        ui->checkBoxMaxEpochs->setChecked(true);
        ui->editMaxEpochs->setEnabled(true);
        ui->editMaxEpochs->setValue(map[NetworkInfo::Key::MaxEpochs].toInt());
    }
    if (map.contains(NetworkInfo::Key::PauseAfterSample))
        ui->checkBoxPauseAfterSample->setChecked(map[NetworkInfo::Key::PauseAfterSample].toBool());

    //
    // Populate the sample selection combo box
    //
    ui->comboBoxSampleSelectionOrder->addItems(NetworkInfo::sampleSelectionOrderStringList());

    if (map.contains(NetworkInfo::Key::SampleSelectionOrder))
        ui->comboBoxSampleSelectionOrder->setCurrentIndex(NetworkInfo::sampleSelectionOrderIndexFromMap(map));
}

void SLPTrainingOptionsDialog::accept()
{
    if (ui->checkBoxStopPercentage->isChecked())
        m_network->setStopPercentage(ui->editStopPercentage->value());
    else
        m_network->setStopPercentage(0.0);
    if (ui->checkBoxStopSamples->isChecked())
        m_network->setStopSamples(ui->editStopSamples->value());
    else
        m_network->setStopSamples(0);

    m_network->setLearningRate(ui->editLearningRate->value());

    if (ui->checkBoxMaxEpochs->isChecked())
        m_network->setMaxTrainingEpochs(ui->editMaxEpochs->value());
    else
        m_network->setMaxTrainingEpochs(0);

    m_network->setPauseAfterSample(ui->checkBoxPauseAfterSample->isChecked());

    m_network->setSampleSelectionOrder(
                NetworkInfo::sampleSelectionOrderFromIndex(
                    ui->comboBoxSampleSelectionOrder->currentIndex()));

    QDialog::accept();
}

void SLPTrainingOptionsDialog::on_checkBoxMaxEpochs_clicked(bool checked)
{
    ui->editMaxEpochs->setEnabled(checked);
}

void SLPTrainingOptionsDialog::on_checkBoxStopSamples_clicked(bool checked)
{
    ui->editStopSamples->setEnabled(checked);
}

void SLPTrainingOptionsDialog::on_checkBoxStopPercentage_clicked(bool checked)
{
    ui->editStopPercentage->setEnabled(checked);
}
