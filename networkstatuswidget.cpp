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
#include "networkstatuswidget.h"
#include "ui_networkstatuswidget.h"

#include <QLabel>

#include "iconlabel.h"
#include "networkstatusnumberlabel.h"
#include "supervisednetwork.h"

NetworkStatusWidget::NetworkStatusWidget(Network* network, QWidget *parent) :
    NetworkVisualWidget(network, parent),
    ui(new Ui::NetworkStatusWidget)
{
    ui->setupUi(this);
    init();
}

NetworkStatusWidget::~NetworkStatusWidget()
{
    delete ui;
}

void NetworkStatusWidget::init()
{
    int row = 0;

    // Add layers
    for (int i = 0; i < m_network->layerCount(); i++) {
        auto* layer = m_network->layer(i);
        auto* label = new QLabel(layer->name(), this);

        auto font = label->font();
        font.setBold(true);
        label->setFont(font);

        ui->gridLayout->addWidget(label, row++, 0);

        // Add neurons
        for (int j = 0; j < layer->neuronCount(); j++) {
            auto* neuron = layer->neuron(j);
            //
            // Add neuron label with an icon
            //
            auto* neuronLabel = new IconLabel(neuron->name(), this);
            if (neuron->isBias())
                neuronLabel->setIconType(IconLabel::IconType::Bias);
            else if (layer->infoType() == NetworkLayerInfo::Type::Input)
                neuronLabel->setIconType(IconLabel::IconType::Input);
            else
                neuronLabel->setIconType(IconLabel::IconType::Neuron);

            neuronLabel->textLabel()->setFont(font);
            ui->gridLayout->addWidget(neuronLabel, row, 0);

            auto* neuronNumberLabel = new NetworkStatusNumberLabel(this);
            neuronNumberLabel->setNumber(neuron->value());
            ui->gridLayout->addWidget(neuronNumberLabel, row, 1);
            row++;

            /*
            if (layer->infoType() == NetworkLayerInfo::Type::Output) {
                auto* label = new QLabel(tr("Target"), this);

                auto margins = label->contentsMargins();
                margins.setLeft(11 + 16 + neuronLabel->layout()->spacing());
                label->setContentsMargins(margins);

                ui->gridLayout->addWidget(label, row, 0);

                auto* neuronWantedLabel = new NetworkStatusNumberLabel(this);
                ui->gridLayout->addWidget(neuronWantedLabel, row, 1);

                connect(m_network, &Network::trainingSampleDone, this, [this, j, neuronWantedLabel](const TrainingSample& sample) {
                    if (!m_update)
                        return;
                    neuronWantedLabel->setNumber(sample.output(j));
                });
                row++;
            }
            */

            // Add weights
            for (int k = 0; k < neuron->outConnectionCount(); k++) {
                auto* conn = neuron->outConnection(k);
                auto* weightLabel = new QLabel(" ➜ " + conn->neuron2()->name(), this);

                auto margins = weightLabel->contentsMargins();
                margins.setLeft(11 + neuronLabel->layout()->spacing());
                weightLabel->setContentsMargins(margins);

                ui->gridLayout->addWidget(weightLabel, row, 0);

                auto* weightNumberLabel = new NetworkStatusNumberLabel(this);
                weightNumberLabel->setNumber(conn->weight());
                ui->gridLayout->addWidget(weightNumberLabel, row, 1);

                auto* weightDiffLabel = new NetworkStatusNumberLabel(this);
                ui->gridLayout->addWidget(weightDiffLabel, row++, 2);

                connect(m_network, &Network::trainingSampleDone, this, [this, weightDiffLabel] {
                    if (!m_update)
                        return;
                    if (weightDiffLabel->isChanged())
                        weightDiffLabel->setChanged(false);
                    else
                        weightDiffLabel->setText(QString());
                });

                connect(conn, &NetworkConnection::weightChanged, this,
                        [this, weightNumberLabel, weightDiffLabel](double weight, double oldWeight) {
                    if (!m_update)
                        return;
                    weightNumberLabel->setNumber(weight);
                    if (m_network->isTraining()) {
                        weightDiffLabel->setNumberDiff(weight - oldWeight);
                        weightDiffLabel->setChanged(true);
                    } else {
                        // Don't show the difference on manual updates
                        weightDiffLabel->setText(QString());
                    }
                });
                connect(conn, &NetworkConnection::weightInitialized, this,
                        [this, weightNumberLabel, weightDiffLabel](double weight) {
                    if (!m_update)
                        return;
                    weightNumberLabel->setNumber(weight);
                    //
                    // New initial weight, don't display the difference
                    //
                    weightDiffLabel->setText(QString());
                });
            }

            connect(neuron, &NetworkNeuron::nameChanged, this, [this, neuron, neuronLabel] {
                if (!m_update)
                    return;
                neuronLabel->textLabel()->setText(neuron->name());
            });
            connect(neuron, &NetworkNeuron::valueChanged, this, [this, neuronNumberLabel](double value) {
                if (!m_update)
                    return;
                neuronNumberLabel->setNumber(value);
            });
        }
    }
}

void NetworkStatusWidget::on_checkBoxPauseUpdates_clicked(bool checked)
{
    m_update = !checked;
}

void NetworkStatusWidget::pauseUpdates()
{
    m_update = false;
}

void NetworkStatusWidget::unpauseUpdates(bool resetInterface)
{
    m_update = true;
    if (resetInterface)
        reset();
}

void NetworkStatusWidget::reset()
{
    for (int i = 0; i < ui->gridLayout->rowCount(); i++) {
        auto* item = ui->gridLayout->itemAtPosition(i, 2);
        if (item == nullptr)
            continue;
        //
        // Remove the difference text on reset
        //
        auto* label = qobject_cast<NetworkStatusNumberLabel*>(item->widget());
        if (label != nullptr)
            label->setText(QString());
    }
}
