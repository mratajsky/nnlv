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

#include <QDockWidget>
#include <QMenu>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QWidget>

#include "maindockwidget.h"
#include "network.h"
#include "networklayerdialog.h"
#include "networkneurondialog.h"

namespace Ui {
class NetworkViewWidget;
}

class NetworkViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkViewWidget(Network* network, QWidget* parent = nullptr);
    ~NetworkViewWidget();

    Network* network() const;
    virtual QVector<MainDockWidget*> createDockWidgets();
    virtual Qt::DockWidgetArea preferredDockWidgetArea() const;
    virtual void resetDiagram();

    enum VisualizationFeature {
        NoFeatures = 0x0,
        ConnectionWeights = 0x1,
        InputValues = 0x2,
        NeuronValues = 0x4,
        AllFeatures = ConnectionWeights | InputValues | NeuronValues
    };
    Q_DECLARE_FLAGS(VisualizationFeatures, VisualizationFeature);

    virtual VisualizationFeatures visualizationFeatures() const = 0;

    virtual bool showConnectionWeights() const;
    virtual void setShowConnectionWeights(bool enabled);

    virtual bool showInputValues() const;
    virtual void setShowInputValues(bool enabled);

    virtual bool showNeuronValues() const;
    virtual void setShowNeuronValues(bool enabled);

    virtual NetworkLayerDialog* createNetworkLayerDialog(NetworkLayer* layer,
                                                         QWidget* parent) const;
    virtual NetworkNeuronDialog* createNetworkNeuronDialog(NetworkNeuron* neuron,
                                                           QWidget* parent) const;

private slots:
    void on_buttonStart_clicked();
    void on_buttonPause_clicked();
    void on_buttonResume_clicked();
    void on_buttonStop_clicked();
    void on_buttonIntervalLeft_clicked();
    void on_buttonIntervalSkipLeft_clicked();
    void on_buttonIntervalRight_clicked();
    void on_buttonIntervalSkipRight_clicked();
    void on_sliderSampleDelay_valueChanged(int position);

protected:
    virtual bool eventFilter(QObject* obj, QEvent* event) override;

    Ui::NetworkViewWidget *ui;

private:
    void init();
    void initTree();
    void initProperties();
    void updateLayerActions(QTreeWidgetItem*  item);
    void updateNeuronActions(QTreeWidgetItem* item);
    void updateTreeLayers();

    Network* m_network = nullptr;
    QTimer* m_updateTimer;
    bool m_showConnectionWeights = true;
    bool m_showInputValues = true;
    bool m_showNeuronValues = true;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(NetworkViewWidget::VisualizationFeatures)
