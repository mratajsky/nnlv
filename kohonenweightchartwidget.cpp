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
#include "kohonenweightchartwidget.h"
#include "ui_kohonenweightchartwidget.h"

#include "kohonenweightchartview.h"

KohonenWeightChartWidget::KohonenWeightChartWidget(Network* network, QWidget *parent) :
    NetworkVisualWidget(network, parent),
    ui(new Ui::KohonenWeightChartWidget),
    m_chartView(new KohonenWeightChartView(network, this))
{
    ui->setupUi(this);
    init();
}

KohonenWeightChartWidget::~KohonenWeightChartWidget()
{
    delete ui;
}

void KohonenWeightChartWidget::init()
{
    ui->verticalLayout->insertWidget(0, m_chartView);

    auto* inputLayer = m_network->inputLayer();
    int inputCount = inputLayer->neuronCount();
    if (inputCount > 1)
        ui->comboBoxYAxis->blockSignals(true);
    for (int i = 0; i < inputCount; i++) {
        ui->comboBoxXAxis->addItem(inputLayer->neuron(i)->name(), i);
        ui->comboBoxYAxis->addItem(inputLayer->neuron(i)->name(), i);
    }
    if (inputCount > 1) {
        ui->comboBoxYAxis->blockSignals(false);
        ui->comboBoxYAxis->setCurrentIndex(1);
    }

    //
    // Handle neuron renames
    //
    connect(inputLayer, &NetworkLayer::neuronNameChanged, this, [this, inputLayer](int index) {
        QVariant variantIndex(index);
        QString name = inputLayer->neuron(index)->name();
        int xId = ui->comboBoxXAxis->findData(variantIndex);
        if (xId > -1)
            ui->comboBoxXAxis->setItemText(xId, name);
        int yId = ui->comboBoxYAxis->findData(variantIndex);
        if (yId > -1)
            ui->comboBoxYAxis->setItemText(yId, name);
    });

    m_chartView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

void KohonenWeightChartWidget::on_comboBoxXAxis_currentIndexChanged(int index)
{
    if (index == ui->comboBoxYAxis->currentIndex() && ui->comboBoxYAxis->count() > 1) {
        // Prevent the indexes from being the same if there are at least 2 inputs
        if (index > 0)
            ui->comboBoxYAxis->setCurrentIndex(0);
        else
            ui->comboBoxYAxis->setCurrentIndex(1);
    }
    m_chartView->setWeightIndex1(index);
}

void KohonenWeightChartWidget::on_comboBoxYAxis_currentIndexChanged(int index)
{
    if (index == ui->comboBoxXAxis->currentIndex() && ui->comboBoxXAxis->count() > 1) {
        // Prevent the indexes from being the same if there are at least 2 inputs
        if (index > 0)
            ui->comboBoxXAxis->setCurrentIndex(0);
        else
            ui->comboBoxXAxis->setCurrentIndex(1);
    }
    m_chartView->setWeightIndex2(index);
}

void KohonenWeightChartWidget::pauseUpdates()
{
    m_chartView->pauseUpdates();
}

void KohonenWeightChartWidget::unpauseUpdates(bool resetInterface)
{
    m_chartView->unpauseUpdates();
    if (resetInterface)
        m_chartView->reset();
}

void KohonenWeightChartWidget::reset()
{
    m_chartView->reset();
}
