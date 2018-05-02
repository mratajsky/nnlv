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
#include "rbfviewwidget.h"

#include "networklayoutwidget.h"
#include "networkstatuswidget.h"
#include "rbfhiddenneuron.h"
#include "rbfnetwork.h"
#include "rbfnetworkneurondialog.h"
#include "rbftrainingoptionsdialog.h"
#include "rbfweightchartwidget.h"
#include "supervisederrorchartwidget.h"

#include "ui_networkviewwidget.h"

RBFViewWidget::RBFViewWidget(Network* network, QWidget* parent) :
    NetworkViewWidget(network, parent)
{
    init();
}

RBFViewWidget::RBFViewWidget(const NetworkInfo::Map& map, QWidget* parent) :
    RBFViewWidget(new RBFNetwork(map), parent)
{
}

RBFViewWidget::RBFViewWidget(const SavedNetwork& savedNetwork, QWidget* parent) :
    RBFViewWidget(new RBFNetwork(savedNetwork), parent)
{
}

void RBFViewWidget::init()
{
    connect(ui->buttonOptions, &QPushButton::clicked, this, [this] {
        auto* rbfNetwork = qobject_cast<RBFNetwork*>(network());
        RBFTrainingOptionsDialog dialog(rbfNetwork, this);
        dialog.exec();
    });
}

QVector<MainDockWidget*> RBFViewWidget::createDockWidgets()
{
    QVector<MainDockWidget*> dockWidgets;

    MainDockWidget* dockWidget;
    dockWidget = new MainDockWidget(tr("Network Layout"), this);
    dockWidget->setObjectName(QStringLiteral("RBFNetworkLayoutDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::LeftDockWidgetArea);
    dockWidget->setWidget(new NetworkLayoutWidget(this, this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Network Status"), this);
    dockWidget->setObjectName(QStringLiteral("RBFNetworkStatusDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::LeftDockWidgetArea);
    dockWidget->setWidget(new NetworkStatusWidget(network(), this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Input Space"), this);
    dockWidget->setObjectName(QStringLiteral("RBFWeightChartDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::RightDockWidgetArea);
    dockWidget->setHeightUnits(2);
    dockWidget->setWidget(new RBFWeightChartWidget(network(), this));
    dockWidgets.append(dockWidget);

    dockWidget = new MainDockWidget(tr("Network Error: MSE"), this);
    dockWidget->setObjectName(QStringLiteral("RBFSupervisedErrorChartDockWidget"));
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setPreferredArea(Qt::RightDockWidgetArea);
    dockWidget->setWidget(new SupervisedErrorChartWidget(network(), this));
    dockWidgets.append(dockWidget);

    return dockWidgets;
}

NetworkNeuronDialog* RBFViewWidget::createNetworkNeuronDialog(NetworkNeuron* neuron, QWidget* parent) const
{
    //
    // Show a dialog that includes sigma value for neurons in the RBF layer
    //
    auto* rbfHiddenNeuron = qobject_cast<RBFHiddenNeuron*>(neuron);
    if (rbfHiddenNeuron != nullptr)
        return new RBFNetworkNeuronDialog(neuron, parent);

    return NetworkViewWidget::createNetworkNeuronDialog(neuron, parent);
}

Qt::DockWidgetArea RBFViewWidget::preferredDockWidgetArea() const
{
    return Qt::RightDockWidgetArea;
}

NetworkViewWidget::VisualizationFeatures RBFViewWidget::visualizationFeatures() const
{
    return AllFeatures;
}

void RBFViewWidget::resetDiagram()
{
    //
    // Only reset the input and output neuron values
    //
    network()->inputLayer()->resetNeuronValues();
    network()->outputLayer()->resetNeuronValues();
}
