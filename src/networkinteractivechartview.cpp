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
#include "networkinteractivechartview.h"

NetworkInteractiveChartView::NetworkInteractiveChartView(Network* network, QWidget* parent) :
    NetworkChartView(network, parent)
{
}

NetworkInteractiveChartView::NetworkInteractiveChartView(Network* network, QChart* chart, QWidget* parent) :
    NetworkChartView(network, chart, parent)
{
}

void NetworkInteractiveChartView::resetInteractiveView()
{
}

void NetworkInteractiveChartView::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Left:
            chart()->scroll(-20, 0);
            break;
        case Qt::Key_Right:
            chart()->scroll(20, 0);
            break;
        case Qt::Key_Up:
            chart()->scroll(0, 20);
            break;
        case Qt::Key_Down:
            chart()->scroll(0, -20);
            break;
        case Qt::Key_Plus:
            chart()->zoom(1.2);
            break;
        case Qt::Key_Minus:
            chart()->zoom(0.8);
            break;
        case Qt::Key_Escape:
            resetInteractiveView();
            break;
    }
}

void NetworkInteractiveChartView::mousePressEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;

    switch (event->button()) {
        case Qt::LeftButton:
            m_lastDragPosition = event->pos();
            event->accept();
            break;
        case Qt::MidButton:
            resetInteractiveView();
            event->accept();
            break;
        default:
            NetworkChartView::mousePressEvent(event);
    }
}

void NetworkInteractiveChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    if (!m_lastDragPosition.isNull()) {
        if (!m_isDragging) {
            m_lastCursor = cursor();
            setCursor(Qt::OpenHandCursor);
            m_isDragging = true;
        }
        auto pos = event->pos();
        auto x = m_lastDragPosition.x() - pos.x();
        auto y = m_lastDragPosition.y() - pos.y();
        chart()->scroll(x, -y);
        m_lastDragPosition = pos;
    } else
        NetworkChartView::mouseMoveEvent(event);
}

void NetworkInteractiveChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isTouching)
        m_isTouching = false;
    if (event->button() == Qt::LeftButton) {
        if (m_isDragging) {
            setCursor(m_lastCursor);
            m_isDragging = false;
        }
        m_lastDragPosition = QPoint();
        event->accept();
    } else
        NetworkChartView::mouseReleaseEvent(event);
}

bool NetworkInteractiveChartView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        //
        // By default touch events are converted to mouse events. So
        // after this event we will get a mouse event also but we want
        // to handle touch events as gestures only. So we need this safeguard
        // to block mouse events that are actually generated from touch.
        //
        m_isTouching = true;
    }
    return QChartView::viewportEvent(event);
}

void NetworkInteractiveChartView::wheelEvent(QWheelEvent* event)
{
    if (event->orientation() == Qt::Vertical) {
        chart()->zoom(1.0 + event->delta() / 1200.0);
        zoomChanged();
    }
    event->accept();
}
