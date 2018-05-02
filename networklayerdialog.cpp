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
#include "networklayerdialog.h"
#include "ui_networklayerdialog.h"

#include <QPushButton>

#include "networklimits.h"

NetworkLayerDialog::NetworkLayerDialog(NetworkLayer* layer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkLayerDialog),
    m_layer(layer)
{
    ui->setupUi(this);
    init();
}

NetworkLayerDialog::~NetworkLayerDialog()
{
    delete ui;
}

void NetworkLayerDialog::init()
{
    ui->editName->setMaxLength(NetworkLimits::maxLayerNameLength);
    ui->editName->setText(m_layer->name());

    QString type = m_layer->infoTypeAsString();
    //
    // Show layer type with capitalized first letter
    //
    ui->labelTypeValue->setText(type.left(1).toUpper() + type.mid(1));
}

void NetworkLayerDialog::accept()
{
    m_layer->setName(ui->editName->text().simplified());
    QDialog::accept();
}

NetworkLayer* NetworkLayerDialog::layer() const
{
    return m_layer;
}

void NetworkLayerDialog::on_editName_textChanged(const QString& arg1)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!arg1.isEmpty());
}
