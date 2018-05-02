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
#include "maindockwidget.h"

#include "networkvisualwidget.h"

MainDockWidget::MainDockWidget(const QString& title, QWidget* parent, Qt::WindowFlags flags) :
    QDockWidget(title, parent, flags)
{
}

MainDockWidget::MainDockWidget(QWidget* parent, Qt::WindowFlags flags) :
    QDockWidget(parent, flags)
{
}

Qt::DockWidgetArea MainDockWidget::preferredArea() const
{
    return m_preferredArea;
}

void MainDockWidget::setPreferredArea(Qt::DockWidgetArea area)
{
    m_preferredArea = area;
}

int MainDockWidget::heightUnits() const
{
    return m_heightUnits;
}

void MainDockWidget::setHeightUnits(int units)
{
    Q_ASSERT(units > 0);

    m_heightUnits = units;
}

void MainDockWidget::closeEvent(QCloseEvent* event)
{
    auto* visualWidget = qobject_cast<NetworkVisualWidget*>(widget());
    if (visualWidget != nullptr) {
        visualWidget->pauseUpdates();
        m_closed = true;
    }
    QDockWidget::closeEvent(event);
}

void MainDockWidget::showEvent(QShowEvent* event)
{
    if (m_closed) {
        auto* visualWidget = qobject_cast<NetworkVisualWidget*>(widget());
        if (visualWidget != nullptr)
            visualWidget->unpauseUpdates(true);
        m_closed = false;
    }
    QDockWidget::showEvent(event);
}
