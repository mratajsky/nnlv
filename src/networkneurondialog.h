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
#pragma once

#include "common.h"

#include <QDialog>

#include "networkneuron.h"

namespace Ui {
class NetworkNeuronDialog;
}

class NetworkNeuronDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NetworkNeuronDialog(NetworkNeuron* neuron, QWidget* parent = nullptr);
    ~NetworkNeuronDialog();

    void accept() override;
    NetworkNeuron* neuron() const;

protected:
    Ui::NetworkNeuronDialog* ui;

private slots:
    void on_editName_textChanged(const QString& arg1);

private:
    void init();

    NetworkNeuron* m_neuron;
    double m_initialValue;
    bool m_readOnly = false;
};