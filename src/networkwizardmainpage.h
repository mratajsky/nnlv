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

#include <QListWidgetItem>
#include <QResizeEvent>
#include <QSettings>
#include <QWizardPage>

#include "networkinfo.h"

namespace Ui {
class NetworkWizardMainPage;
}

class NetworkWizardMainPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit NetworkWizardMainPage(QWidget* parent = nullptr);
    ~NetworkWizardMainPage();

    QString currentFileName() const;
    NetworkInfo::Type currentNetworkType() const;
    bool isComplete() const override;
    int nextId() const override;

private slots:
    void on_checkBoxCreateNewNetwork_toggled(bool checked);
    void on_listExamples_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_radioButtonSLP_clicked();
    void on_radioButtonAdaline_clicked();
    void on_radioButtonMLP_clicked();
    void on_radioButtonKohonen_clicked();
    void on_radioButtonRBF_clicked();
    void on_listExamples_itemDoubleClicked(QListWidgetItem *item);

private:
    void loadExamples(NetworkInfo::Type type);

    Ui::NetworkWizardMainPage* ui;
    QSettings m_settings;
};
