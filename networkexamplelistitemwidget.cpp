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
#include "networkexamplelistitemwidget.h"
#include "ui_networkexamplelistitemwidget.h"

NetworkExampleListItemWidget::NetworkExampleListItemWidget(
        const NetworkInfo::Map& map, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkExampleListItemWidget)
{
    ui->setupUi(this);

    ui->labelName->setText(map[NetworkInfo::Key::Name].toString());
    ui->labelDescription->setText(map[NetworkInfo::Key::Description].toString());
}

NetworkExampleListItemWidget::~NetworkExampleListItemWidget()
{
    delete ui;
}
