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

#include <QCursor>
#include <QLineSeries>
#include <QScatterSeries>
#include <QValueAxis>

#include "network.h"
#include "networkinteractivechartview.h"
#include "resettable.h"

class SupervisedChartView : public NetworkInteractiveChartView, public Resettable
{
    Q_OBJECT
public:
    explicit SupervisedChartView(Network* network, QWidget* parent = nullptr);
    ~SupervisedChartView();

    void addLine(int neuronIndex);
    void pauseUpdates();
    void unpauseUpdates();
    void reset() override;
    void resetInteractiveView() override;
    void setAxes(int layerIndex, int xNeuronIndex, int yNeuronIndex);
    void setOutputNeuronIndex(int index);

private:
    void init();
    void refreshLine(int neuronIndex);
    void refreshLines();
    void refreshSamplePoints();
    void updateAxes();
    void updateRange();

    static const double m_highAxisValue;

    double m_minX = 0.0;
    double m_maxX = 0.0;
    double m_minY = 0.0;
    double m_maxY = 0.0;
    int m_xIndex = -1;
    int m_yIndex = -1;
    int m_layerIndex = -1;
    int m_outputIndex = 0;
    bool m_update = true;
    QChart* m_chart;
    QValueAxis* m_axisX = nullptr;
    QValueAxis* m_axisY = nullptr;
    QMap<double, QScatterSeries*> m_seriesMap;
    QMap<int, QLineSeries*> m_lineSeriesMap;
};
