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
#include "rbfweightchartwidget.h"
#include "ui_rbfweightchartwidget.h"

#include <QStandardItemModel>

RBFWeightChartWidget::RBFWeightChartWidget(Network* network, QWidget *parent) :
    NetworkVisualWidget(network, parent),
    ui(new Ui::RBFWeightChartWidget),
    m_chartView(new RBFWeightChartView(network, this))
{
    ui->setupUi(this);
    init();
}

RBFWeightChartWidget::~RBFWeightChartWidget()
{
    delete ui;
}

void RBFWeightChartWidget::init()
{
    ui->verticalLayout->insertWidget(0, m_chartView);

    ui->comboBoxColoring->addItem(tr("Output classes"), m_userDataClasses);
    ui->comboBoxColoring->addItem(tr("Uniform"), m_userDataBasic);

    const auto* output = m_network->outputLayer();
    for (int i = 0; i < output->neuronCount(); i++) {
        const auto* neuron = output->neuron(i);
        ui->comboBoxColoring->addItem(tr("Values of %1").arg(neuron->name()), i);
        //
        // Handle neuron rename
        //
        connect(neuron, &NetworkNeuron::nameChanged, this,
                [this, i, neuron](const QString& name) {
            int index = ui->comboBoxColoring->findData(i);
            if (index > -1)
                ui->comboBoxColoring->setItemText(index, name);
        });
    }

    const auto& store = m_network->trainingTableModel()->store();
    //
    // Disable selection of output classes where the number of classes is too high
    //
    connect(&store, &TrainingSampleStore::samplesChanged, this, [this] {
        verifyColoringOptions();
    });
    verifyColoringOptions();

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

QMap<int, QSet<double>> RBFWeightChartWidget::outputClassMap(int maxClasses) const
{
    const auto& store = m_network->trainingTableModel()->store();

    QMap<int, QSet<double>> values;
    for (int output = 0; output < store.outputCount(); output++) {
        for (int i = 0; i < store.samples().size(); i++) {
            double value = store.sample(i).output(output);
            if (values[output].contains(value))
                continue;
            if (maxClasses < 1 || values[output].size() <= maxClasses)
                values[output].insert(value);
        }
    }
    return values;
}

void RBFWeightChartWidget::verifyColoringOptions()
{
    auto* model = qobject_cast<QStandardItemModel*>(ui->comboBoxColoring->model());

    QMap<int, QSet<double>> classMap = outputClassMap(m_maxClasses);
    bool maxReached = false;
    for (int i = 0; i < ui->comboBoxColoring->count(); i++) {
        int index = ui->comboBoxColoring->itemData(i).toInt();
        if (index < 0)
            continue;

        auto* item = model->item(i);
        auto flags = item->flags();
        if (classMap[index].size() > m_maxClasses) {
            item->setFlags(flags & ~Qt::ItemIsEnabled);
            maxReached = true;
        } else
            item->setFlags(flags | Qt::ItemIsEnabled);
    }
    auto* item = model->item(ui->comboBoxColoring->findData(m_userDataClasses));
    auto flags = item->flags();
    if (maxReached)
        item->setFlags(flags & ~Qt::ItemIsEnabled);
    else
        item->setFlags(flags | Qt::ItemIsEnabled);
    //
    // If the currently selected item got disabled, select the default one
    //
    flags = model->item(ui->comboBoxColoring->currentIndex())->flags();
    if (!(flags & Qt::ItemIsEnabled))
        ui->comboBoxColoring->setCurrentIndex(ui->comboBoxColoring->findData(m_userDataBasic));
}

void RBFWeightChartWidget::on_comboBoxColoring_currentIndexChanged(int index)
{
    int userData = ui->comboBoxColoring->itemData(index).toInt();

    if (userData == m_userDataBasic)
        m_chartView->setColoringType(RBFWeightChartView::ColoringType::Basic);
    else if (userData == m_userDataClasses)
        m_chartView->setColoringType(RBFWeightChartView::ColoringType::Classes);
    else
        m_chartView->setColoringOutputIndex(userData);
}

void RBFWeightChartWidget::on_comboBoxXAxis_currentIndexChanged(int index)
{
    if (index == ui->comboBoxYAxis->currentIndex() && ui->comboBoxYAxis->count() > 1) {
        //
        // Prevent the indexes from being the same if there are at least 2 inputs
        //
        if (index > 0)
            ui->comboBoxYAxis->setCurrentIndex(0);
        else
            ui->comboBoxYAxis->setCurrentIndex(1);
    }
    m_chartView->setWeightIndex1(index);
}

void RBFWeightChartWidget::on_comboBoxYAxis_currentIndexChanged(int index)
{
    if (index == ui->comboBoxXAxis->currentIndex() && ui->comboBoxXAxis->count() > 1) {
        //
        // Prevent the indexes from being the same if there are at least 2 inputs
        //
        if (index > 0)
            ui->comboBoxXAxis->setCurrentIndex(0);
        else
            ui->comboBoxXAxis->setCurrentIndex(1);
    }
    m_chartView->setWeightIndex2(index);
}

void RBFWeightChartWidget::pauseUpdates()
{
    m_chartView->pauseUpdates();
}

void RBFWeightChartWidget::unpauseUpdates(bool resetInterface)
{
    m_chartView->unpauseUpdates();
    if (resetInterface)
        m_chartView->reset();
}

void RBFWeightChartWidget::reset()
{
    m_chartView->reset();
}
