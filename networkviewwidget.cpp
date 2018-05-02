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
#include "networkviewwidget.h"
#include "ui_networkviewwidget.h"

#include <QMessageBox>

#include "program.h"
#include "trainingtabledialog.h"

NetworkViewWidget::NetworkViewWidget(Network* network, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NetworkViewWidget),
    m_network(network),
    m_updateTimer(new QTimer(this))
{
    ui->setupUi(this);
    //
    // The network is always reparented to this widget
    //
    network->setParent(this);
    init();
}

NetworkViewWidget::~NetworkViewWidget()
{
    delete ui;
}

void NetworkViewWidget::init()
{
    ui->buttonPause->hide();
    ui->buttonResume->hide();

    auto* scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, ui->diagram->width(), ui->diagram->height());
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->addItem(m_network);

    ui->diagram->setScene(scene);

    m_updateTimer->setInterval(10);
    m_updateTimer->setSingleShot(true);

    //
    // Make all changes to training controls in callbacks as some
    // of them may happen not from within this widget
    //
    connect(m_network, &Network::trainingStarted, this, [this](int resumed) {
        if (resumed) {
            ui->buttonResume->hide();
            ui->buttonPause->show();
        } else {
            ui->buttonStart->hide();
            ui->buttonPause->show();
            ui->buttonStop->setEnabled(true);
        }
    });
    connect(m_network, &Network::trainingPaused, this, [this] {
        ui->buttonPause->hide();
        ui->buttonResume->show();
    });
    connect(m_network, &Network::trainingStopped, this, [this] {
        ui->buttonStart->show();
        ui->buttonPause->hide();
        ui->buttonResume->hide();
        ui->buttonStop->setEnabled(false);
    });

    connect(m_updateTimer, &QTimer::timeout, this, [this] {
        m_network->updateScenePosition();
    });

    ui->diagram->installEventFilter(this);

    resetDiagram();
}

NetworkLayerDialog* NetworkViewWidget::createNetworkLayerDialog(NetworkLayer* layer, QWidget* parent) const
{
    return new NetworkLayerDialog(layer, parent);
}

NetworkNeuronDialog* NetworkViewWidget::createNetworkNeuronDialog(NetworkNeuron* neuron, QWidget* parent) const
{
    return new NetworkNeuronDialog(neuron, parent);
}

Network* NetworkViewWidget::network() const
{
    return m_network;
}

QVector<MainDockWidget*> NetworkViewWidget::createDockWidgets()
{
    return QVector<MainDockWidget*>();
}

Qt::DockWidgetArea NetworkViewWidget::preferredDockWidgetArea() const
{
    return Qt::NoDockWidgetArea;
}

void NetworkViewWidget::resetDiagram()
{
    //
    // Reset neuron values in all layers
    //
    const auto& layers = m_network->layers();
    for (auto* layer : layers)
        layer->resetNeuronValues();
}

bool NetworkViewWidget::showConnectionWeights() const
{
    return m_showConnectionWeights;
}

void NetworkViewWidget::setShowConnectionWeights(bool enabled)
{
    if (m_showConnectionWeights != enabled) {
        const auto& layers = m_network->layers();
        for (auto* layer : layers)
            layer->setShowConnectionWeights(enabled);

        m_showConnectionWeights = enabled;
    }
}

bool NetworkViewWidget::showInputValues() const
{
    return m_showInputValues;
}

void NetworkViewWidget::setShowInputValues(bool enabled)
{
    if (m_showInputValues != enabled) {
        auto* layer = m_network->inputLayer();

        layer->setShowNeuronValues(enabled);
        m_showInputValues = enabled;
    }
}

bool NetworkViewWidget::showNeuronValues() const
{
    return m_showNeuronValues;
}

void NetworkViewWidget::setShowNeuronValues(bool enabled)
{
    if (m_showNeuronValues != enabled) {
        const auto& layers = m_network->layers();
        for (auto* layer : layers) {
            if (layer->infoType() == NetworkLayerInfo::Type::Input)
                continue;
            layer->setShowNeuronValues(enabled);
        }
        m_showNeuronValues = enabled;
    }
}

void NetworkViewWidget::on_buttonStart_clicked()
{
    auto* model = m_network->trainingTableModel();
    if (model->sampleCount() == 0) {
        int ret = QMessageBox::question(
                    this,
                    tr(PROGRAM_NAME),
                    tr("No training samples have been added."
                       "\n\n"
                       "Would you like to open the editor and add them now?"));
        if (ret == QMessageBox::Yes) {
            TrainingTableDialog dialog(model, this);
            dialog.exec();
        }
        return;
    }
    m_network->setTrainingSampleDelay(ui->sliderSampleDelay->value());
    m_network->startTraining();
}

void NetworkViewWidget::on_buttonPause_clicked()
{
    m_network->pauseTraining();
}

void NetworkViewWidget::on_buttonResume_clicked()
{
    m_network->startTraining();
}

void NetworkViewWidget::on_buttonStop_clicked()
{
    m_network->stopTraining();
}

void NetworkViewWidget::on_buttonIntervalLeft_clicked()
{
    int value = ui->sliderSampleDelay->value();

    value += ui->sliderSampleDelay->singleStep();

    ui->sliderSampleDelay->setValue(value);
}

void NetworkViewWidget::on_buttonIntervalSkipLeft_clicked()
{
    ui->sliderSampleDelay->setValue(ui->sliderSampleDelay->maximum());
}

void NetworkViewWidget::on_buttonIntervalRight_clicked()
{
    int value = ui->sliderSampleDelay->value();

    value -= ui->sliderSampleDelay->singleStep();

    ui->sliderSampleDelay->setValue(value);
}

void NetworkViewWidget::on_buttonIntervalSkipRight_clicked()
{
    ui->sliderSampleDelay->setValue(ui->sliderSampleDelay->minimum());
}

void NetworkViewWidget::on_sliderSampleDelay_valueChanged(int position)
{
    m_network->setTrainingSampleDelay(position);
}

bool NetworkViewWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Resize && obj == ui->diagram) {
//        qDebug()<<"size"<<dynamic_cast<QResizeEvent*>(event);
        m_updateTimer->start();
    }

    return QObject::eventFilter(obj, event);
}
