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
#include "networkwizardmainpage.h"
#include "ui_networkwizardmainpage.h"

#include <QDir>
#include <QListWidgetItem>

#include "networkexamplelistitemwidget.h"
#include "networkwizard.h"
#include "savednetwork.h"
#include "xmlworker.h"

NetworkWizardMainPage::NetworkWizardMainPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::NetworkWizardMainPage)
{
    ui->setupUi(this);
    ui->radioButtonSLP->setChecked(true);

    auto networkTypeString = m_settings
            .value("program/wizard-network-type")
            .toString();
    if (!networkTypeString.isEmpty()) {
        auto networkType = NetworkInfo::typeFromString(networkTypeString);
        switch (networkType) {
            case NetworkInfo::Type::SLP:
                ui->radioButtonSLP->setChecked(true);
                break;
            case NetworkInfo::Type::Adaline:
                ui->radioButtonAdaline->setChecked(true);
                break;
            case NetworkInfo::Type::MLP:
                ui->radioButtonMLP->setChecked(true);
                break;
            case NetworkInfo::Type::Kohonen:
                ui->radioButtonKohonen->setChecked(true);
                break;
            case NetworkInfo::Type::RBF:
                ui->radioButtonRBF->setChecked(true);
                break;
            default:
                break;
        }
    }
    bool create = m_settings
            .value("program/wizard-prefer-new-network", false)
            .toBool();
    ui->checkBoxCreateNewNetwork->setChecked(create);

    auto networkType = currentNetworkType();
    if (networkType == NetworkInfo::Type::Unknown) {
        // Default network
        ui->radioButtonSLP->setChecked(true);
        networkType = NetworkInfo::Type::SLP;
    }
    loadExamples(networkType);
}

NetworkWizardMainPage::~NetworkWizardMainPage()
{
    delete ui;
}

QString NetworkWizardMainPage::currentFileName() const
{
    auto* item = ui->listExamples->currentItem();
    if (item == nullptr)
        return QString();

    return item->data(Qt::UserRole).toString();
}

NetworkInfo::Type NetworkWizardMainPage::currentNetworkType() const
{
    if (ui->radioButtonSLP->isChecked())
        return NetworkInfo::Type::SLP;
    else if (ui->radioButtonAdaline->isChecked())
        return NetworkInfo::Type::Adaline;
    else if (ui->radioButtonMLP->isChecked())
        return NetworkInfo::Type::MLP;
    else if (ui->radioButtonKohonen->isChecked())
        return NetworkInfo::Type::Kohonen;
    else if (ui->radioButtonRBF->isChecked())
        return NetworkInfo::Type::RBF;

    return NetworkInfo::Type::Unknown;
}

bool NetworkWizardMainPage::isComplete() const
{
    //
    // The form is complete if either an example is selected in the list or
    // the new network checkbox is checked
    //
    return ui->listExamples->currentRow() > -1 || ui->checkBoxCreateNewNetwork->isChecked();
}

void NetworkWizardMainPage::loadExamples(NetworkInfo::Type type)
{
    ui->listExamples->clear();

    QString folder = ":/examples";
    QDir dir(folder);
    if (!dir.exists())
        return;

    dir.setNameFilters(QStringList() << "*.xml");
    dir.setFilter(QDir::Files | QDir::Readable);
    dir.setSorting(QDir::Name);

    qDebug() << "Reading example networks from" << folder;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    XmlWorker xml;
    xml.setSchemaValidationEnabled(false);
    const auto infoList = dir.entryInfoList();
    for (const auto& info : infoList) {
        SavedNetwork network;
        if (xml.readNetwork(info.absoluteFilePath(),
                            network,
                            XmlWorker::ReadNetworkMode::ReadNetworkHeaderOnly)) {
            if (network.infoType() != type)
                continue;
            const auto& map = network.infoMap();

            auto* item = new QListWidgetItem;
            ui->listExamples->addItem(item);

            auto* widget = new NetworkExampleListItemWidget(map);
            item->setSizeHint(widget->sizeHint());
            ui->listExamples->setItemWidget(item, widget);

            item->setData(Qt::UserRole, info.absoluteFilePath());
        } else
            qWarning() << "Skipping invalid network in" << info.fileName();
    }
    QApplication::restoreOverrideCursor();

    if (ui->listExamples->count() > 0 && !ui->checkBoxCreateNewNetwork->isChecked())
        ui->listExamples->setCurrentRow(0);

    emit completeChanged();
}

int NetworkWizardMainPage::nextId() const
{
    if (ui->checkBoxCreateNewNetwork->isChecked()) {
        if (ui->radioButtonSLP->isChecked())
            return NetworkWizard::Page::SLPNetwork;
        else if (ui->radioButtonAdaline->isChecked())
            return NetworkWizard::Page::AdalineNetwork;
        else if (ui->radioButtonMLP->isChecked())
            return NetworkWizard::Page::MLPNetwork;
        else if (ui->radioButtonKohonen->isChecked())
            return NetworkWizard::Page::KohonenNetwork;
        else if (ui->radioButtonRBF->isChecked())
            return NetworkWizard::Page::RBFNetwork;
    }
    return -1;
}

void NetworkWizardMainPage::on_checkBoxCreateNewNetwork_toggled(bool checked)
{
    if (checked) {
        setFinalPage(false);
        // Deselect example
        ui->listExamples->setEnabled(false);
        ui->listExamples->setCurrentRow(-1, QItemSelectionModel::Clear);
    } else {
        setFinalPage(true);
        ui->listExamples->setEnabled(true);
        ui->listExamples->setCurrentRow(0);
    }
    emit completeChanged();
}

void NetworkWizardMainPage::on_listExamples_currentItemChanged(QListWidgetItem *current,
                                                               QListWidgetItem *previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);

    emit completeChanged();
}

void NetworkWizardMainPage::on_radioButtonSLP_clicked()
{
    loadExamples(NetworkInfo::Type::SLP);
}

void NetworkWizardMainPage::on_radioButtonAdaline_clicked()
{
    loadExamples(NetworkInfo::Type::Adaline);
}

void NetworkWizardMainPage::on_radioButtonMLP_clicked()
{
    loadExamples(NetworkInfo::Type::MLP);
}

void NetworkWizardMainPage::on_radioButtonKohonen_clicked()
{
    loadExamples(NetworkInfo::Type::Kohonen);
}

void NetworkWizardMainPage::on_radioButtonRBF_clicked()
{
    loadExamples(NetworkInfo::Type::RBF);
}

void NetworkWizardMainPage::on_listExamples_itemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);

    wizard()->accept();
}
