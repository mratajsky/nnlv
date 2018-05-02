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
#include "kohonentrainingoptionsdialog.h"
#include "ui_kohonentrainingoptionsdialog.h"

KohonenTrainingOptionsDialog::KohonenTrainingOptionsDialog(Network* network, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::KohonenTrainingOptionsDialog),
    m_network(network)
{
    ui->setupUi(this);
    init();
}

KohonenTrainingOptionsDialog::~KohonenTrainingOptionsDialog()
{
    delete ui;
}

void KohonenTrainingOptionsDialog::init()
{
    const auto& map = m_network->infoMap();

    if (m_network->isTraining() || m_network->isTrainingPaused()) {
        ui->editLearningRate->setReadOnly(true);
        ui->editMaxEpochs->setReadOnly(true);
        ui->labelTraining->show();
    } else
        ui->labelTraining->hide();

    ui->editLearningRate->setValue(map[NetworkInfo::Key::LearningRate].toDouble());

    if (map.contains(NetworkInfo::Key::MaxEpochs)) {
        ui->editMaxEpochs->setEnabled(true);
        ui->editMaxEpochs->setValue(map[NetworkInfo::Key::MaxEpochs].toInt());
    }
    if (map.contains(NetworkInfo::Key::PauseAfterSample))
        ui->checkBoxPauseAfterSample->setChecked(map[NetworkInfo::Key::PauseAfterSample].toBool());

    //
    // Populate the combo boxes
    //
    ui->comboBoxSampleSelectionOrder->addItems(NetworkInfo::sampleSelectionOrderStringList());
    if (map.contains(NetworkInfo::Key::SampleSelectionOrder))
        ui->comboBoxSampleSelectionOrder->setCurrentIndex(NetworkInfo::sampleSelectionOrderIndexFromMap(map));
}

void KohonenTrainingOptionsDialog::accept()
{
    m_network->setLearningRate(ui->editLearningRate->value());
    m_network->setMaxTrainingEpochs(ui->editMaxEpochs->value());
    m_network->setPauseAfterSample(ui->checkBoxPauseAfterSample->isChecked());

    m_network->setSampleSelectionOrder(
                NetworkInfo::sampleSelectionOrderFromIndex(
                    ui->comboBoxSampleSelectionOrder->currentIndex()));

    QDialog::accept();
}
