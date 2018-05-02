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

#include <QScatterSeries>

#include "networkinteractivechartview.h"
#include "rbfhiddenlayer.h"
#include "resettable.h"

class RBFWeightChartView : public NetworkInteractiveChartView, public Resettable
{
    Q_OBJECT
public:
    explicit RBFWeightChartView(Network* network, QWidget* parent = nullptr);

    enum class ColoringType {
        Basic,
        Classes,
        OutputIndex
    };
    Q_ENUM(ColoringType)

    void pauseUpdates();
    void unpauseUpdates();
    void reset() override;
    void resetInteractiveView() override;
    void setColoringType(ColoringType coloringType);
    void setColoringOutputIndex(int index);
    void setWeightIndex1(int index);
    void setWeightIndex2(int index);

private:
    void init();
    void initChart();
    void populateInputs();
    void populateNeurons();
    void createInputSeriesBasic();
    void createInputSeriesClasses();
    void createInputSeriesClusters();
    void createInputSeriesOutput(int index);
    void updateCurrentInput(const TrainingSample& sample);
    void updateCurrentInput(const QVector<double>& input);
    void updateInputSeries();
    void updateRange();

    Network* m_network;
    RBFHiddenLayer* m_hiddenLayer;
    ColoringType m_coloringType = ColoringType::Classes;
    int m_coloringOutputIndex = 0;
    int m_weightIndex1 = 0;
    int m_weightIndex2 = 1;
    bool m_update = true;
    double m_inputMinX;
    double m_inputMaxX;
    double m_inputMinY;
    double m_inputMaxY;
    double m_neuronMinX;
    double m_neuronMaxX;
    double m_neuronMinY;
    double m_neuronMaxY;
    QVector<double> m_currentInput;
    QPointF m_currentInputPoint;
    QScatterSeries* m_neuronSeries;
    QScatterSeries* m_currentInputSeries;
    QHash<QPair<double, double>, int> m_neuronPointMap;
    QMap<double, QScatterSeries*> m_inputSeriesMap;
    QMap<QVector<double>, double> m_inputClassSeriesMap;
};
