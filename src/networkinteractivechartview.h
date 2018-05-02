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

#include "network.h"
#include "networkchartview.h"
#include "resettable.h"

class NetworkInteractiveChartView : public NetworkChartView
{
    Q_OBJECT
public:
    explicit NetworkInteractiveChartView(Network* network, QWidget* parent = nullptr);
    explicit NetworkInteractiveChartView(Network* network, QChart* chart, QWidget* parent = nullptr);

    virtual void resetInteractiveView();

signals:
    void zoomChanged();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    bool viewportEvent(QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    bool m_isTouching = false;
    bool m_isDragging = false;
    QPoint m_lastDragPosition;
    QCursor m_lastCursor;
};
