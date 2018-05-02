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
#include "kohonenviewwidget.h"

#include "kohonentrainingoptionsdialog.h"
#include "kohonenweightchartwidget.h"
#include "kohonennetwork.h"
#include "networklayoutwidget.h"
#include "networkstatuswidget.h"

#include "ui_networkviewwidget.h"

KohonenViewWidget::KohonenViewWidget(Network* network, QWidget* parent) :
    NetworkViewWidget(network, parent)
{
    init();
}

KohonenViewWidget::KohonenViewWidget(const NetworkInfo::Map& map, QWidget* parent) :
    KohonenViewWidget(new KohonenNetwork(map), parent)
{
}

KohonenViewWidget::KohonenViewWidget(const SavedNetwork& savedNetwork, QWidget* parent) :
    KohonenViewWidget(new KohonenNetwork(savedNetwork), parent)
{
}

void KohonenViewWidget::init()
{
    connect(ui->buttonOptions, &QPushButton::clicked, this, [this] {
        KohonenTrainingOptionsDialog dialog(network(), this);
        dialog.exec();
    });

    NetworkViewWidget::setShowConnectionWeights(false);
}

QVector<MainDockWidget*> KohonenViewWidget::createDockWidgets()
{
    QVector<MainDockWidget*> dockWidgets;

    MainDockWidget* dockWidget;
    dockWidget = new MainDockWidget(tr("Network Layout"), this);
    dockWidget->setObjectName(QStringLiteral("KohonenNetworkLayoutDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::LeftDockWidgetArea);
    dockWidget->setWidget(new NetworkLayoutWidget(this, this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Network Status"), this);
    dockWidget->setObjectName(QStringLiteral("KohonenNetworkStatusDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::LeftDockWidgetArea);
    dockWidget->setWidget(new NetworkStatusWidget(network(), this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Input Space"), this);
    dockWidget->setObjectName(QStringLiteral("KohonenWeightChartDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::RightDockWidgetArea);
    dockWidget->setWidget(new KohonenWeightChartWidget(network(), this));
    dockWidgets.append(dockWidget);

    return dockWidgets;
}

Qt::DockWidgetArea KohonenViewWidget::preferredDockWidgetArea() const
{
    return Qt::RightDockWidgetArea;
}

NetworkViewWidget::VisualizationFeatures KohonenViewWidget::visualizationFeatures() const
{
    return InputValues | NeuronValues;
}

bool KohonenViewWidget::showConnectionWeights() const
{
    return false;
}

void KohonenViewWidget::setShowConnectionWeights(bool enabled)
{
    Q_UNUSED(enabled);
}
