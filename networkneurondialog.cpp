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
#include "networkneurondialog.h"
#include "ui_networkneurondialog.h"

#include <QPushButton>

#include "networklimits.h"
#include "networkneurondialogspinbox.h"

NetworkNeuronDialog::NetworkNeuronDialog(NetworkNeuron* neuron, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::NetworkNeuronDialog),
    m_neuron(neuron)
{
    ui->setupUi(this);
    init();
}

NetworkNeuronDialog::~NetworkNeuronDialog()
{
    delete ui;
}

void NetworkNeuronDialog::init()
{
    auto* network = m_neuron->layer()->network();
    if (network->isTraining() || network->isTrainingPaused()) {
        m_readOnly = true;
        ui->editValue->setReadOnly(true);
    } else {
        if (m_neuron->layer()->infoType() == NetworkLayerInfo::Type::Input
                && !m_neuron->isBias())
            ui->editValue->setReadOnly(false);
        else
            ui->editValue->setReadOnly(true);

        m_readOnly = false;
        ui->labelReadOnly->hide();
    }

    ui->editName->setMaxLength(NetworkLimits::maxNeuronNameLength);
    ui->editName->setText(m_neuron->name());
    connect(m_neuron, &NetworkNeuron::nameChanged, this, [this](const QString& name) {
        ui->editName->setText(name);
    });

    m_initialValue = m_neuron->value();
    ui->editValue->setValue(m_initialValue);
    connect(m_neuron, &NetworkNeuron::valueChanged, this, [this](double value) {
        ui->editValue->setValue(value);
    });

    //
    // Keep track of tab order of the newly added inputs, this is the last previous
    // widget
    //
    auto* lastWidget = ui->editValue;

    int inputRow;
    ui->formLayout->getWidgetPosition(ui->labelHeadingInputWeights, &inputRow, nullptr);
    if (m_neuron->inConnectionCount() > 0) {
        for (int i = 0; i < m_neuron->inConnectionCount(); i++) {
            auto* conn = m_neuron->inConnection(i);
            auto* spinBox = new NetworkNeuronDialogSpinBox(network, conn, this);

            connect(conn, &NetworkConnection::weightChanged, this, [spinBox](double value) {
                spinBox->setValue(value);
            });
            ui->formLayout->insertRow(1 + inputRow++, conn->neuron1()->name() + ":", spinBox);

            QWidget::setTabOrder(lastWidget, spinBox);
            lastWidget = spinBox;
        }
    } else
        ui->formLayout->removeRow(inputRow);

    int outputRow;
    ui->formLayout->getWidgetPosition(ui->labelHeadingOutputWeights, &outputRow, nullptr);
    if (m_neuron->outConnectionCount() > 0) {
        for (int i = 0; i < m_neuron->outConnectionCount(); i++) {
            auto* conn = m_neuron->outConnection(i);
            auto* spinBox = new NetworkNeuronDialogSpinBox(network, conn, this);

            connect(conn, &NetworkConnection::weightChanged, this, [spinBox](double value) {
                spinBox->setValue(value);
            });
            ui->formLayout->insertRow(1 + outputRow++, conn->neuron2()->name() + ":", spinBox);

            QWidget::setTabOrder(lastWidget, spinBox);
            lastWidget = spinBox;
        }
    } else
        ui->formLayout->removeRow(outputRow);

    connect(network, &Network::trainingStarted, this, [this] {
        ui->labelReadOnly->show();
    });
    connect(network, &Network::trainingStopped, this, [this] {
        ui->labelReadOnly->hide();
    });
}

void NetworkNeuronDialog::accept()
{
    m_neuron->setName(ui->editName->text().simplified());

    //
    // Save the weights
    //
    if (!m_readOnly) {
        for (int row = 1; row < ui->formLayout->rowCount(); row++) {
            auto* item = ui->formLayout->itemAt(row, QFormLayout::FieldRole);
            if (item == nullptr)
                continue;
            auto* spinBox = qobject_cast<NetworkNeuronDialogSpinBox*>(item->widget());
            //
            // FIXME:
            // This may cause the network error to be recalculated when for each changed weight,
            // it should only be recalculated once after all weights are saved
            //
            if (spinBox != nullptr)
                spinBox->save();
        }
        auto* layer = m_neuron->layer();
        //
        // Update weight ranges in this layer
        //
        layer->updateInConnectionRange();
        layer->updateOutConnectionRange();

        if (!ui->editValue->isReadOnly()) {
            double value = ui->editValue->value();
            if (!qFuzzyCompare(value, m_initialValue)) {
                QVector<double> input = layer->values();

                input[layer->neurons().indexOf(m_neuron)] = value;
                //
                // The value vector was initially retrieved with a bias unit to
                // match the index of the neuron, but it must be removed before
                // it is computed with
                //
                if (layer->hasBias())
                    input.remove(0);

                layer->network()->computeAndSet(input);
            }
        }
    }
    QDialog::accept();
}

NetworkNeuron* NetworkNeuronDialog::neuron() const
{
    return m_neuron;
}

void NetworkNeuronDialog::on_editName_textChanged(const QString& arg1)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!arg1.isEmpty());
}
