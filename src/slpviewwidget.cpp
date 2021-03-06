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
#include "slpviewwidget.h"

#include "networklayoutwidget.h"
#include "networkstatuswidget.h"
#include "slpnetwork.h"
#include "slptrainingoptionsdialog.h"
#include "supervisedchartwidget.h"
#include "supervisederrorchartwidget.h"

#include "ui_networkviewwidget.h"

SLPViewWidget::SLPViewWidget(Network* network, QWidget* parent) :
    NetworkViewWidget(network, parent)
{
    init();
}

SLPViewWidget::SLPViewWidget(const NetworkInfo::Map& map, QWidget* parent) :
    SLPViewWidget(new SLPNetwork(map), parent)
{
}

SLPViewWidget::SLPViewWidget(const SavedNetwork& savedNetwork, QWidget* parent) :
    SLPViewWidget(new SLPNetwork(savedNetwork), parent)
{
}

void SLPViewWidget::init()
{
    connect(ui->buttonOptions, &QPushButton::clicked, this, [this] {
        SLPTrainingOptionsDialog dialog(network(), this);
        dialog.exec();
    });
}

QVector<MainDockWidget*> SLPViewWidget::createDockWidgets()
{
    QVector<MainDockWidget*> dockWidgets;

    MainDockWidget* dockWidget;
    dockWidget = new MainDockWidget(tr("Network Layout"), this);
    dockWidget->setObjectName(QStringLiteral("SLPNetworkLayoutDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::LeftDockWidgetArea);
    dockWidget->setWidget(new NetworkLayoutWidget(this, this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Network Status"), this);
    dockWidget->setObjectName(QStringLiteral("SLPNetworkStatusDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::LeftDockWidgetArea);
    dockWidget->setWidget(new NetworkStatusWidget(network(), this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Input Space"), this);
    dockWidget->setObjectName(QStringLiteral("SLPSupervisedChartDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::RightDockWidgetArea);
    dockWidget->setHeightUnits(2);
    dockWidget->setWidget(new SupervisedChartWidget(network(), this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Network Error: Misclassified Samples"), this);
    dockWidget->setObjectName(QStringLiteral("SLPSupervisedErrorChartDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::RightDockWidgetArea);
    dockWidget->setWidget(new SupervisedErrorChartWidget(network(), this));
    dockWidgets.append(dockWidget);

    return dockWidgets;
}

Qt::DockWidgetArea SLPViewWidget::preferredDockWidgetArea() const
{
    return Qt::RightDockWidgetArea;
}

NetworkViewWidget::VisualizationFeatures SLPViewWidget::visualizationFeatures() const
{
    return AllFeatures;
}
