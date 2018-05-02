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
#include "kohonencreatenetworkwidget.h"
#include "ui_kohonencreatenetworkwidget.h"

#include "kohonennetworklimits.h"

KohonenCreateNetworkWidget::KohonenCreateNetworkWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::KohonenCreateNetworkWidget)
{
    ui->setupUi(this);
    init();
}

KohonenCreateNetworkWidget::~KohonenCreateNetworkWidget()
{
    delete ui;
}

void KohonenCreateNetworkWidget::updateNeuronCounts(int value)
{
    ui->editOutputNeuronSideCount->blockSignals(true);
    ui->editOutputNeuronSideCount2->blockSignals(true);

    ui->editOutputNeuronSideCount->setValue(value);
    ui->editOutputNeuronSideCount2->setValue(value);
    ui->editOutputNeuronTotalCount->setValue(value * value);

    ui->editOutputNeuronSideCount->blockSignals(false);
    ui->editOutputNeuronSideCount2->blockSignals(false);
}

void KohonenCreateNetworkWidget::init()
{
    // Set limits
    ui->editInputNeuronCount->setRange(KohonenNetworkLimits::minInputNeurons,
                                       KohonenNetworkLimits::maxInputNeurons);
    ui->editOutputNeuronSideCount->setRange(KohonenNetworkLimits::minOutputNeuronsSide,
                                            KohonenNetworkLimits::maxOutputNeuronsSide);
    ui->editOutputNeuronSideCount2->setRange(KohonenNetworkLimits::minOutputNeuronsSide,
                                             KohonenNetworkLimits::maxOutputNeuronsSide);
    connect(ui->editOutputNeuronSideCount,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,
            &KohonenCreateNetworkWidget::updateNeuronCounts);
    connect(ui->editOutputNeuronSideCount2,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,
            &KohonenCreateNetworkWidget::updateNeuronCounts);

    // Populate the combo boxes
    ui->comboBoxSampleSelectionOrder->addItems(NetworkInfo::sampleSelectionOrderStringList());

    connect(ui->editNetworkName,
            &QLineEdit::textChanged,
            this, [this] {
        emit validityChanged();
    });
    connect(ui->editWeightsRandomMin,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, [this] {
        emit validityChanged();
    });
    connect(ui->editWeightsRandomMax,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, [this] {
        emit validityChanged();
    });
}

bool KohonenCreateNetworkWidget::isValid() const
{
    if (ui->editNetworkName->text().isEmpty())
        return false;
    if (ui->editWeightsRandomMin->value() > ui->editWeightsRandomMax->value())
        return false;

    return true;
}

NetworkInfo::Map KohonenCreateNetworkWidget::createMap() const
{
    NetworkInfo::Map map;
    map[NetworkInfo::Key::Type] = QVariant::fromValue(NetworkInfo::Type::Kohonen);
    map[NetworkInfo::Key::Name] = ui->editNetworkName->text().simplified();
    map[NetworkInfo::Key::InputNeuronCount] = ui->editInputNeuronCount->value();
    map[NetworkInfo::Key::OutputNeuronCount] = std::pow(ui->editOutputNeuronSideCount->value(), 2);
    if (ui->radioWeightsRandom->isChecked()) {
        map[NetworkInfo::Key::InitialWeightsRandomMin] = ui->editWeightsRandomMin->value();
        map[NetworkInfo::Key::InitialWeightsRandomMax] = ui->editWeightsRandomMax->value();
    } else
        map[NetworkInfo::Key::InitialWeightsValue] = ui->editWeightsValue->value();

    map[NetworkInfo::Key::LearningRate] = ui->editLearningRate->value();
    map[NetworkInfo::Key::MaxEpochs] = ui->editTrainingMaxEpochs->value();

    map[NetworkInfo::Key::SampleSelectionOrder] =
            QVariant::fromValue(NetworkInfo::sampleSelectionOrderFromIndex(
                                    ui->comboBoxSampleSelectionOrder->currentIndex()));

    return map;
}

void KohonenCreateNetworkWidget::on_radioWeightsRandom_clicked()
{
    ui->editWeightsRandomMin->setEnabled(true);
    ui->editWeightsRandomMax->setEnabled(true);
    ui->editWeightsRandomMin->setFocus();
    ui->editWeightsValue->setEnabled(false);
}

void KohonenCreateNetworkWidget::on_radioWeightsValue_clicked()
{
    ui->editWeightsRandomMin->setEnabled(false);
    ui->editWeightsRandomMax->setEnabled(false);
    ui->editWeightsValue->setEnabled(true);
    ui->editWeightsValue->setFocus();
}
