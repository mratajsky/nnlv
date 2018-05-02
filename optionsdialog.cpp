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
#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    init();
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::init()
{
    if (m_settings.value("program/save-modified", true).toBool())
        ui->checkBoxSaveModified->setChecked(true);

    if (m_settings.value("program/mark-modified-weights", false).toBool())
        ui->checkBoxMarkModifiedWeights->setChecked(true);
}

bool OptionsDialog::isModified()
{
    return m_modified;
}

void OptionsDialog::on_checkBoxSaveModified_clicked(bool checked)
{
    m_settings.setValue("program/save-modified", checked);
    m_modified = true;
}

void OptionsDialog::on_checkBoxMarkModifiedWeights_clicked(bool checked)
{
    m_settings.setValue("program/mark-modified-weights", checked);
    m_modified = true;
}
