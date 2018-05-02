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
#include "mlpcreatenetworkwidget.h"
#include "ui_mlpcreatenetworkwidget.h"

#include <QSpinBox>

#include "mlpactivation.h"
#include "mlpnetworklimits.h"

MLPCreateNetworkWidget::MLPCreateNetworkWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MLPCreateNetworkWidget)
{
    ui->setupUi(this);
    init();
}

MLPCreateNetworkWidget::~MLPCreateNetworkWidget()
{
    delete ui;
}

void MLPCreateNetworkWidget::init()
{
    // Set limits
    ui->editHiddenLayerCount->setRange(MLPNetworkLimits::minHiddenLayers,
                                       MLPNetworkLimits::maxHiddenLayers);
    ui->editInputNeuronCount->setRange(MLPNetworkLimits::minInputNeurons,
                                       MLPNetworkLimits::maxInputNeurons);
    ui->editOutputNeuronCount->setRange(MLPNetworkLimits::minOutputNeurons,
                                        MLPNetworkLimits::maxOutputNeurons);

    // Create spinboxes for the hidden neuron counts depending on the initial value
    setHiddenLayerCount(ui->editHiddenLayerCount->value());

    // Populate the combo boxes
    ui->comboBoxSampleSelectionOrder->addItems(NetworkInfo::sampleSelectionOrderStringList());
    ui->comboBoxActivationFunction->addItems(MLPActivation::functionStringList());

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

void MLPCreateNetworkWidget::setHiddenLayerCount(int count)
{
    int currentCount = ui->hiddenNeuronLayout->count();
    if (count > currentCount) {
        // Keep track of tab order of the newly added inputs
        QWidget* lastWidget;
        if (currentCount > 0)
            lastWidget = ui->hiddenNeuronLayout
                         ->itemAt(ui->hiddenNeuronLayout->count() - 1)
                         ->widget();
        else
            lastWidget = ui->editHiddenLayerCount;
        //
        // Add new rows
        //
        auto diff = count - currentCount;
        for (int i = 0; i < diff; i++) {
            auto* spinBox = new QSpinBox(this);
            spinBox->setRange(MLPNetworkLimits::minHiddenNeurons,
                              MLPNetworkLimits::maxHiddenNeurons);
            spinBox->setValue(m_hiddenNeuronsDefault);
            ui->hiddenNeuronLayout->addWidget(spinBox);
            if (currentCount)
                ui->labelHiddenNeuronCount->setBuddy(spinBox);

            QWidget::setTabOrder(lastWidget, spinBox);
            lastWidget = spinBox;
        }
    } else {
        while (count < currentCount) {
            auto* item = ui->hiddenNeuronLayout->itemAt(ui->hiddenNeuronLayout->count() - 1);
            //
            // Remove a spinbox from the end of the layout and delete both the
            // widget and the QLayoutItem
            //
            ui->hiddenNeuronLayout->removeItem(item);
            item->widget()->deleteLater();
            delete item;
            currentCount--;
        }
    }
    if (count == 0)
        ui->labelHiddenNeuronCount->hide();
    else
        ui->labelHiddenNeuronCount->show();
}

bool MLPCreateNetworkWidget::isValid() const
{
    if (ui->editNetworkName->text().isEmpty())
        return false;
    if (ui->editWeightsRandomMin->value() > ui->editWeightsRandomMax->value())
        return false;

    return true;
}

NetworkInfo::Map MLPCreateNetworkWidget::createMap() const
{
    NetworkInfo::Map map;
    map[NetworkInfo::Key::Type] = QVariant::fromValue(NetworkInfo::Type::MLP);
    map[NetworkInfo::Key::Name] = ui->editNetworkName->text().simplified();
    map[NetworkInfo::Key::InputNeuronCount] = ui->editInputNeuronCount->value();
    map[NetworkInfo::Key::OutputNeuronCount] = ui->editOutputNeuronCount->value();
    if (ui->radioWeightsRandom->isChecked()) {
        map[NetworkInfo::Key::InitialWeightsRandomMin] = ui->editWeightsRandomMin->value();
        map[NetworkInfo::Key::InitialWeightsRandomMax] = ui->editWeightsRandomMax->value();
    } else
        map[NetworkInfo::Key::InitialWeightsValue] = ui->editWeightsValue->value();

    QVector<int> hiddenCountList;
    for (int i = 0; i < ui->hiddenNeuronLayout->count(); i++) {
        auto* spinBox = qobject_cast<QSpinBox*>(ui->hiddenNeuronLayout
                                                ->itemAt(i)
                                                ->widget());
        hiddenCountList.append(spinBox->value());
    }
    map[NetworkInfo::Key::HiddenNeuronCount] = QVariant::fromValue(hiddenCountList);

    map[NetworkInfo::Key::LearningRate] = ui->editLearningRate->value();
    map[NetworkInfo::Key::SampleSelectionOrder] =
            QVariant::fromValue(NetworkInfo::sampleSelectionOrderFromIndex(
                                    ui->comboBoxSampleSelectionOrder->currentIndex()));

    map[NetworkInfo::Key::ActivationFunction] =
            QVariant::fromValue(MLPActivation::functionFromIndex(
                                    ui->comboBoxActivationFunction->currentIndex()));

    return map;
}

void MLPCreateNetworkWidget::on_editHiddenLayerCount_valueChanged(int arg1)
{
    setHiddenLayerCount(arg1);
}

void MLPCreateNetworkWidget::on_radioWeightsRandom_clicked()
{
    ui->editWeightsRandomMin->setEnabled(true);
    ui->editWeightsRandomMax->setEnabled(true);
    ui->editWeightsRandomMin->setFocus();
    ui->editWeightsValue->setEnabled(false);
}

void MLPCreateNetworkWidget::on_radioWeightsValue_clicked()
{
    ui->editWeightsRandomMin->setEnabled(false);
    ui->editWeightsRandomMax->setEnabled(false);
    ui->editWeightsValue->setEnabled(true);
    ui->editWeightsValue->setFocus();
}
