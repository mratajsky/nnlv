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

#include "kohonenweightchartview.h"
#include "networkvisualwidget.h"

namespace Ui {
class KohonenWeightChartWidget;
}

class KohonenWeightChartWidget : public NetworkVisualWidget
{
    Q_OBJECT
public:
    explicit KohonenWeightChartWidget(Network* network, QWidget* parent = nullptr);
    ~KohonenWeightChartWidget();

    void pauseUpdates() override;
    void unpauseUpdates(bool resetInterface = false) override;
    void reset() override;

private slots:
    void on_comboBoxXAxis_currentIndexChanged(int index);
    void on_comboBoxYAxis_currentIndexChanged(int index);

private:
    void init();

    Ui::KohonenWeightChartWidget* ui;
    KohonenWeightChartView* m_chartView;
};
