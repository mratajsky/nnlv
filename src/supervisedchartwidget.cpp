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
#include "supervisedchartwidget.h"
#include "ui_supervisedchartwidget.h"

SupervisedChartWidget::SupervisedChartWidget(Network* network, QWidget* parent) :
    NetworkVisualWidget(network, parent),
    ui(new Ui::SupervisedChartWidget),
    m_chartView(new SupervisedChartView(network, this))
{
    ui->setupUi(this);
    init();
}

SupervisedChartWidget::~SupervisedChartWidget()
{
    delete ui;
}

void SupervisedChartWidget::init()
{
    ui->verticalLayout->insertWidget(0, m_chartView);

    m_chartView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    //
    // TODO: this is a safe default for now, but it should be made more flexible
    //
    m_chartView->setAxes(0, 1, 2);
    m_chartView->setOutputNeuronIndex(0);
    //
    // Remove the output neuron selection field if there is just one output neuron
    //
    if (m_network->outputLayer()->neuronCount() == 1)
        ui->formLayout->removeRow(0);
    //
    // Create a line for each neuron in the layer beside the input layer
    //
    auto* layer = m_network->layer(1);
    for (int i = 0; i < layer->neuronCount(); i++) {
        auto* neuron = layer->neuron(i);
        if (neuron->isBias())
            continue;

        m_chartView->addLine(i);
    }
    populateComboBoxes();
}

void SupervisedChartWidget::pauseUpdates()
{
    m_chartView->pauseUpdates();
}

void SupervisedChartWidget::unpauseUpdates(bool resetInterface)
{
    m_chartView->unpauseUpdates();
    if (resetInterface)
        m_chartView->reset();
}

void SupervisedChartWidget::reset()
{
    m_chartView->reset();
}

void SupervisedChartWidget::populateComboBoxes()
{
    auto* output = m_network->outputLayer();

    if (output->neuronCount() > 1) {
        for (int i = 0; i < output->neuronCount(); i++)
            ui->comboBoxOutput->addItem(output->neuron(i)->name(), i);
        //
        // Handle neuron renames
        //
        connect(output, &NetworkLayer::neuronNameChanged, this, [this, output](int index) {
            QVariant variantIndex(index);
            QString name = output->neuron(index)->name();
            int id = ui->comboBoxOutput->findData(variantIndex);
            if (id > -1)
                ui->comboBoxOutput->setItemText(id, name);
        });
    }

    auto* input = m_network->inputLayer();

    ui->comboBoxXAxis->blockSignals(true);
    ui->comboBoxYAxis->blockSignals(true);
    for (int i = 0; i < input->neuronCount(); i++) {
        if (input->neuron(i)->isBias())
            continue;
        ui->comboBoxXAxis->addItem(input->neuron(i)->name(), i);
        ui->comboBoxYAxis->addItem(input->neuron(i)->name(), i);
    }
    ui->comboBoxYAxis->setCurrentIndex(1);
    ui->comboBoxYAxis->blockSignals(false);
    ui->comboBoxXAxis->blockSignals(false);
    //
    // Handle neuron renames
    //
    connect(input, &NetworkLayer::neuronNameChanged, this, [this, input](int index) {
        QVariant variantIndex(index);
        QString name = input->neuron(index)->name();
        int xId = ui->comboBoxXAxis->findData(variantIndex);
        if (xId > -1)
            ui->comboBoxXAxis->setItemText(xId, name);
        int yId = ui->comboBoxYAxis->findData(variantIndex);
        if (yId > -1)
            ui->comboBoxYAxis->setItemText(yId, name);
    });
}

void SupervisedChartWidget::on_comboBoxOutput_currentIndexChanged(int index)
{
    m_chartView->setOutputNeuronIndex(ui->comboBoxOutput->itemData(index).toInt());
}

void SupervisedChartWidget::on_comboBoxXAxis_currentIndexChanged(int index)
{
    // Prevent the indexes from being the same if there are at least 2 inputs
    if (index == ui->comboBoxYAxis->currentIndex() && ui->comboBoxYAxis->count() > 1) {
        ui->comboBoxYAxis->blockSignals(true);
        if (index > 0)
            ui->comboBoxYAxis->setCurrentIndex(0);
        else
            ui->comboBoxYAxis->setCurrentIndex(1);
        ui->comboBoxYAxis->blockSignals(false);
    }
    updateAxes();
}

void SupervisedChartWidget::on_comboBoxYAxis_currentIndexChanged(int index)
{
    // Prevent the indexes from being the same if there are at least 2 inputs
    if (index == ui->comboBoxXAxis->currentIndex() && ui->comboBoxXAxis->count() > 1) {
        ui->comboBoxXAxis->blockSignals(true);
        if (index > 0)
            ui->comboBoxXAxis->setCurrentIndex(0);
        else
            ui->comboBoxXAxis->setCurrentIndex(1);
        ui->comboBoxXAxis->blockSignals(false);
    }
    updateAxes();
}

void SupervisedChartWidget::updateAxes()
{
    int xIndex = ui->comboBoxXAxis->itemData(ui->comboBoxXAxis->currentIndex()).toInt();
    int yIndex = ui->comboBoxYAxis->itemData(ui->comboBoxYAxis->currentIndex()).toInt();

    m_chartView->setAxes(0, xIndex, yIndex);
}
