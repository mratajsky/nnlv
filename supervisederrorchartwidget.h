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

#include <QWidget>

#include "networkvisualwidget.h"
#include "supervisederrorchartview.h"
#include "supervisednetwork.h"

namespace Ui {
class SupervisedErrorChartWidget;
}

class SupervisedErrorChartWidget : public NetworkVisualWidget
{
    Q_OBJECT
public:
    explicit SupervisedErrorChartWidget(Network* network, QWidget* parent = nullptr);
    ~SupervisedErrorChartWidget();

    void pauseUpdates() override;
    void unpauseUpdates(bool resetInterface = false) override;
    void reset() override;

private:
    void init();
    void updateLabel();
    void updateLabel(double error);

    Ui::SupervisedErrorChartWidget* ui;
    SupervisedErrorChartView* m_chartView;
    SupervisedNetwork* m_supervisedNetwork;
    bool m_update = true;
};
