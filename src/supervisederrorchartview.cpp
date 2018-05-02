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
#include "supervisederrorchartview.h"

#include "slpoutputlayer.h"
#include "slpnetwork.h"
#include "supervisednetwork.h"

SupervisedErrorChartView::SupervisedErrorChartView(Network* network, QWidget* parent) :
    NetworkChartView(network, parent),
    m_series(new QLineSeries(this))
{
    setRenderHint(QPainter::Antialiasing);
    init();
}

void SupervisedErrorChartView::init()
{
    m_supervisedNetwork = qobject_cast<SupervisedNetwork*>(network());

    connect(m_supervisedNetwork, &Network::trainingStarted, this, [this](bool resumed) {
        if (!resumed)
            reset();
    });
    connect(m_supervisedNetwork, &Network::trainingSampleDone, this, [this] {
        if (!m_update)
            return;
        updateChart();
    });
    chart()->legend()->hide();
    reset();
}

void SupervisedErrorChartView::updateChart()
{
    auto error = m_supervisedNetwork->error();
    auto epochs = m_supervisedNetwork->trainingEpochs();

    m_series->append(epochs, error);

    auto points = m_series->pointsVector();
    if (points.size() == m_pruneLimit) {
        //
        // Each time the number of points reaches the prune limit, some points
        // are removed because only m_showEpochs of them are visible, the m_pruneLimit
        // exists to avoid shrinking the array and recalculating the axis range
        // too often
        //
        m_series->removePoints(0, points.size() - m_showEpochs);

        m_minError = qInf();
        m_maxError = -qInf();
        points = m_series->pointsVector();
        for (const auto& point : qAsConst(points)) {
            m_minError = qMin(m_minError, point.y());
            m_maxError = qMax(m_maxError, point.y());
        }
    } else {
        m_minError = qMin(m_minError, error);
        m_maxError = qMax(m_maxError, error);
    }

    chart()->axisX()->setRange(qMax(-1, epochs - m_showEpochs), epochs + 1);

    double diff = m_maxError - m_minError;
    if (qFuzzyIsNull(diff))
        diff = 10;
    chart()->axisY()->setRange(m_minError - 0.1 * diff, m_maxError + 0.1 * diff);

    //
    // Fix the series line to match the axis lines
    //
    m_axisY->applyNiceNumbers();
}

void SupervisedErrorChartView::pauseUpdates()
{
    m_update = false;
}

void SupervisedErrorChartView::unpauseUpdates()
{
    m_update = true;
}

void SupervisedErrorChartView::reset()
{
    if (!chart()->series().isEmpty()) {
        //
        // Remove and add the series back to make sure the axes are reset
        //
        chart()->removeSeries(m_series);
        m_series->clear();
    }
    chart()->addSeries(m_series);
    chart()->createDefaultAxes();

    //
    // The X-axis is the integral number of epoch
    //
    m_axisX = qobject_cast<QValueAxis*>(chart()->axisX());
    m_axisX->setLabelFormat("%d");
    m_axisY = qobject_cast<QValueAxis*>(chart()->axisY());

    auto* supervisedNetwork = dynamic_cast<SupervisedNetwork*>(network());

    auto errorValueType = supervisedNetwork->errorValueType();
    if (errorValueType == SupervisedNetwork::ErrorValueType::IntValue) {
        //
        // The Y-axis is integral for some network types
        //
        m_axisY->setLabelFormat("%d");
    }

    m_minError = qInf();
    m_maxError = -qInf();
}
