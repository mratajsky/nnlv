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
#include "adalineviewwidget.h"

#include "adalinenetwork.h"
#include "adalinetrainingoptionsdialog.h"
#include "networklayoutwidget.h"
#include "networkstatuswidget.h"
#include "supervisedchartwidget.h"
#include "supervisederrorchartwidget.h"

#include "ui_networkviewwidget.h"

AdalineViewWidget::AdalineViewWidget(Network* network, QWidget* parent) :
    NetworkViewWidget(network, parent)
{
    init();
}

AdalineViewWidget::AdalineViewWidget(const NetworkInfo::Map& map, QWidget* parent) :
    AdalineViewWidget(new AdalineNetwork(map), parent)
{
}

AdalineViewWidget::AdalineViewWidget(const SavedNetwork& savedNetwork, QWidget* parent) :
    AdalineViewWidget(new AdalineNetwork(savedNetwork), parent)
{
}

void AdalineViewWidget::init()
{
    connect(ui->buttonOptions, &QPushButton::clicked, this, [this] {
        AdalineTrainingOptionsDialog dialog(network(), this);
        dialog.exec();
    });
}

QVector<MainDockWidget*> AdalineViewWidget::createDockWidgets()
{
    QVector<MainDockWidget*> dockWidgets;

    MainDockWidget* dockWidget;
    dockWidget = new MainDockWidget(tr("Network Layout"), this);
    dockWidget->setObjectName(QStringLiteral("AdalineNetworkLayoutDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::LeftDockWidgetArea);
    dockWidget->setWidget(new NetworkLayoutWidget(this, this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Network Status"), this);
    dockWidget->setObjectName(QStringLiteral("AdalineNetworkStatusDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::LeftDockWidgetArea);
    dockWidget->setWidget(new NetworkStatusWidget(network(), this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Input Space"), this);
    dockWidget->setObjectName(QStringLiteral("AdalineSupervisedChartDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::RightDockWidgetArea);
    dockWidget->setHeightUnits(2);
    dockWidget->setWidget(new SupervisedChartWidget(network(), this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Network Error: MSE"), this);
    dockWidget->setObjectName(QStringLiteral("AdalineSupervisedErrorChartDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::RightDockWidgetArea);
    dockWidget->setWidget(new SupervisedErrorChartWidget(network(), this));
    dockWidgets.append(dockWidget);

    return dockWidgets;
}

Qt::DockWidgetArea AdalineViewWidget::preferredDockWidgetArea() const
{
    return Qt::RightDockWidgetArea;
}

NetworkViewWidget::VisualizationFeatures AdalineViewWidget::visualizationFeatures() const
{
    return AllFeatures;
}
