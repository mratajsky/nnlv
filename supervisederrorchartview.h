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

#include <QLineSeries>
#include <QValueAxis>

#include "networkchartview.h"
#include "resettable.h"
#include "supervisednetwork.h"

class SupervisedErrorChartView : public NetworkChartView, public Resettable
{
    Q_OBJECT
public:
    explicit SupervisedErrorChartView(Network* network, QWidget* parent = nullptr);

    void pauseUpdates();
    void unpauseUpdates();
    void reset() override;

private slots:
    void updateChart();

private:
    void init();

    static constexpr int m_showEpochs = 100;
    static constexpr int m_pruneLimit = 150;

    bool m_update = true;
    bool m_updatePending = false;
    double m_minError;
    double m_maxError;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;
    QLineSeries* m_series;
    SupervisedNetwork* m_supervisedNetwork;
};
