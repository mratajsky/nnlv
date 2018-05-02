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
#include "networkwizard.h"
#include "ui_networkwizard.h"

#include <QAbstractButton>

#include "networkwizardpage.h"

NetworkWizard::NetworkWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::NetworkWizard)
{
    ui->setupUi(this);
    init();
}

NetworkWizard::~NetworkWizard()
{
    delete ui;
}

void NetworkWizard::init()
{
    setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/nnlv.svg"));

    auto* customButton = button(QWizard::CustomButton1);
    auto iconThemeName = QStringLiteral("document-open");
    QIcon icon;
    if (QIcon::hasThemeIcon(iconThemeName))
        icon = QIcon::fromTheme(iconThemeName);
    else
        icon.addFile(QStringLiteral(":/icons/document-open.svg"));
    customButton->setIcon(icon);
    customButton->setText(tr("&Open From File..."));
    connect(this, &QWizard::customButtonClicked, this, [this](int which) {
        if (which == QWizard::CustomButton1)
            emit loadButtonClicked();
    });
    //
    // Create a custom button layout to place the Load button on the left side
    // and also make it appear only in the first page
    //
    m_buttonLayout
            << QWizard::Stretch
            << QWizard::BackButton
            << QWizard::NextButton
            << QWizard::FinishButton
            << QWizard::CancelButton;
    connect(this, &NetworkWizard::currentIdChanged, this, [this](int id) {
        if (id >= 0) {
            bool firstIsCustom = m_buttonLayout.first() == QWizard::CustomButton1;
            if (id == Page::Main) {
                if (!firstIsCustom)
                    m_buttonLayout.prepend(QWizard::CustomButton1);
            } else if (firstIsCustom)
                m_buttonLayout.removeFirst();

            setButtonLayout(m_buttonLayout);
        }
    });
    setButtonLayout(m_buttonLayout);

    //
    // Connect validation signals, this cannot be done in QtDesigner because it
    // doesn't allow connecting signals to signals
    //
    connect(ui->widgetSLP,
            &SLPCreateNetworkWidget::validityChanged,
            ui->wizardPageSLP,
            &SLPNetworkWizardPage::completeChanged);
    connect(ui->widgetAdaline,
            &AdalineCreateNetworkWidget::validityChanged,
            ui->wizardPageAdaline,
            &AdalineNetworkWizardPage::completeChanged);
    connect(ui->widgetMLP,
            &MLPCreateNetworkWidget::validityChanged,
            ui->wizardPageMLP,
            &MLPNetworkWizardPage::completeChanged);
    connect(ui->widgetKohonen,
            &KohonenCreateNetworkWidget::validityChanged,
            ui->wizardPageKohonen,
            &KohonenNetworkWizardPage::completeChanged);
    connect(ui->widgetRBF,
            &RBFCreateNetworkWidget::validityChanged,
            ui->wizardPageRBF,
            &RBFNetworkWizardPage::completeChanged);
}

NetworkWizard::Result NetworkWizard::result() const
{
    return m_result;
}

QString NetworkWizard::resultExampleFileName() const
{
    return m_resultExampleFileName;
}

NetworkInfo::Map NetworkWizard::resultNetworkInfoMap() const
{
    return m_resultNetworkMap;
}

void NetworkWizard::accept()
{
    NetworkInfo::Type networkType;
    //
    // When the wizard is finished, save the result depending on which page
    // was accepted from
    //
    auto* mainPage = qobject_cast<NetworkWizardMainPage*>(currentPage());
    if (mainPage != nullptr) {
        // An example network was selected from the main page
        m_result = Result::AcceptedExample;
        m_resultExampleFileName = mainPage->currentFileName();
        m_settings.setValue("program/wizard-prefer-new-network", false);
        networkType = mainPage->currentNetworkType();
    } else {
        auto* page = qobject_cast<NetworkWizardPage*>(currentPage());
        Q_ASSERT(page != nullptr);
        m_result = Result::AcceptedNewNetwork;
        m_resultNetworkMap = page->networkInfoMap();
        m_settings.setValue("program/wizard-prefer-new-network", true);
        networkType = NetworkInfo::typeFromMap(m_resultNetworkMap);
    }
    Q_ASSERT(networkType != NetworkInfo::Type::Unknown);
    m_settings.setValue("program/wizard-network-type", NetworkInfo::typeToString(networkType));

    QWizard::accept();
}
