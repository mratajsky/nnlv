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
#include "supervisederrorchartwidget.h"
#include "ui_supervisederrorchartwidget.h"

#include "supervisednetwork.h"

SupervisedErrorChartWidget::SupervisedErrorChartWidget(Network* network, QWidget *parent) :
    NetworkVisualWidget(network, parent),
    ui(new Ui::SupervisedErrorChartWidget),
    m_chartView(new SupervisedErrorChartView(network, this))
{
    ui->setupUi(this);
    init();
}

SupervisedErrorChartWidget::~SupervisedErrorChartWidget()
{
    delete ui;
}

void SupervisedErrorChartWidget::init()
{
    ui->verticalLayout->insertWidget(0, m_chartView);

    m_chartView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    m_supervisedNetwork = dynamic_cast<SupervisedNetwork*>(m_network);

    const auto& store = m_network->trainingTableModel()->store();

    connect(&store, &TrainingSampleStore::samplesChanged, this, [this] {
        if (m_update)
            updateLabel();
    });

    connect(m_network, &Network::neuronWeightChanged, this, [this] {
        //
        // Update the error in the label when some weight changes manually
        //
        if (m_update && !m_network->isTraining())
            updateLabel();
    });
    connect(m_network, &Network::trainingSampleDone, this, [this] {
        if (m_update)
            updateLabel();
    });

    updateLabel();
}

void SupervisedErrorChartWidget::updateLabel()
{
    updateLabel(m_supervisedNetwork->error());
}

void SupervisedErrorChartWidget::updateLabel(double error)
{
    auto errorValueType = m_supervisedNetwork->errorValueType();

    if (errorValueType == SupervisedNetwork::ErrorValueType::IntValue)
        ui->labelError->setText(QString::number(static_cast<int>(error)));
    else
        ui->labelError->setText(QString::number(error, 'f', 6));
}

void SupervisedErrorChartWidget::pauseUpdates()
{
    m_update = false;
    m_chartView->pauseUpdates();
}

void SupervisedErrorChartWidget::unpauseUpdates(bool resetInterface)
{
    m_update = true;
    m_chartView->unpauseUpdates();
    if (resetInterface)
        m_chartView->reset();

    updateLabel();
}

void SupervisedErrorChartWidget::reset()
{
    m_chartView->reset();
}
