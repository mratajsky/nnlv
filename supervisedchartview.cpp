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
#include "supervisedchartview.h"

#include <QValueAxis>

#include "supervisedchart.h"
#include "trainingsamplestore.h"

const double SupervisedChartView::m_highAxisValue = 10e12;

SupervisedChartView::SupervisedChartView(Network* network, QWidget* parent) :
    NetworkInteractiveChartView(network, new SupervisedChart, parent)
{
    m_chart = chart();
    init();
}

SupervisedChartView::~SupervisedChartView()
{
    delete m_chart;
}

void SupervisedChartView::init()
{
    setRenderHint(QPainter::Antialiasing);

    const auto& store = network()->trainingTableModel()->store();
    connect(&store, &TrainingSampleStore::samplesChanged, this, [this] {
        reset();
    });
    connect(network(), &Network::trainingSampleDone, this, [this] {
        if (!m_update)
            return;
        refreshLines();
    });

    /*
     * TODO: mark current input, also when training
    connect(m_network, &Network::neuronValueChanged, this, [this] {
        //
        // Handle input changes when the network is not being trained. This happens
        // when it's changed manually by the user and it should be reflected by
        // marking the active neuron and marking the input location.
        //
        if (m_update && !m_network->isTraining()) {
            m_currentInputSeries->setColor(Colors::ColorWeightChartMark);
            updateCurrentInput(m_network->inputLayer()->values(true));
            updateRange();
        }
    });
    */
}

void SupervisedChartView::refreshLine(int neuronIndex)
{
    if (m_layerIndex < 0
            || m_xIndex < 0
            || m_yIndex < 0)
        return;

    //
    // TODO: this is really ugly, it should be more flexible
    //
    auto neuron = network()->layer(m_layerIndex+1)->neuron(neuronIndex);
    auto series = m_lineSeriesMap[neuronIndex];

    double wb = neuron->inConnection(0)->weight();
    double wx = neuron->inConnection(m_xIndex)->weight();
    double wy = neuron->inConnection(m_yIndex)->weight();
    QPointF p1, p2;
    if (!qFuzzyIsNull(wy)) {
        p1 = QPointF(-m_highAxisValue, -(wb + wx * -m_highAxisValue) / wy);
        p2 = QPointF(m_highAxisValue, -(wb + wx * m_highAxisValue) / wy);
    } else if (!qFuzzyIsNull(wx)) {
        p1 = QPointF(-(wb + wy * -m_highAxisValue) / wx, -m_highAxisValue);
        p2 = QPointF(-(wb + wy * m_highAxisValue) / wx, m_highAxisValue);
    } else {
        p1 = QPointF(-m_highAxisValue, 0);
        p2 = QPointF(m_highAxisValue, 0);
    }

    if (series->points().isEmpty()) {
        series->append(p1);
        series->append(p2);
    } else {
        series->replace(0, p1);
        series->replace(1, p2);
    }

    if (m_seriesMap.isEmpty())
        series->hide();
    else
        series->show();

    chart()->update();
}

void SupervisedChartView::refreshLines()
{
    for (auto key : m_lineSeriesMap.keys())
        refreshLine(key);
}

void SupervisedChartView::refreshSamplePoints()
{
    //
    // Remove existing series
    //
    QMutableMapIterator<double, QScatterSeries*> i(m_seriesMap);
    while (i.hasNext()) {
        i.next();
        chart()->removeSeries(i.value());
        i.remove();
    }
    if (m_xIndex < 0 || m_yIndex < 0)
        return;

    int xSampleIndex = 0;
    int ySampleIndex = 0;
    const auto* inputLayer = network()->inputLayer();
    for (int i = 0; i < inputLayer->neuronCount(); i++) {
        auto* neuron = inputLayer->neuron(i);
        if (neuron->isBias())
            continue;
        if (i < m_xIndex)
            xSampleIndex++;
        if (i < m_yIndex)
            ySampleIndex++;
    }
    const auto& store = network()->trainingTableModel()->store();

    if (xSampleIndex >= store.inputCount() || ySampleIndex >= store.inputCount())
        return;

    if (store.samples().isEmpty())
        m_minX = m_minY = m_maxX = m_maxY = 0;
    else {
        m_minX = m_minY = qInf();
        m_maxX = m_maxY = -qInf();
    }

    for (const auto& sample : store.samples()) {
        auto input  = sample.inputs();
        auto output = sample.output(m_outputIndex);

        QScatterSeries* series;
        if (m_seriesMap.contains(output))
            series = m_seriesMap[output];
        else {
            series = new QScatterSeries;
            series->setMarkerSize(12);
            series->setName(QString::number(output, 'f', 1));
            chart()->addSeries(series);

            qDebug() << "Created new scatter series for output" << output;

            m_seriesMap[output] = series;
        }
        double x = input[xSampleIndex];
        double y = input[ySampleIndex];
        series->append(x, y);
        m_minX = qMin(m_minX, x);
        m_maxX = qMax(m_maxX, x);
        m_minY = qMin(m_minY, y);
        m_maxY = qMax(m_maxY, y);
    }
    updateAxes();
}

void SupervisedChartView::updateAxes()
{
    chart()->createDefaultAxes();
    m_axisX = qobject_cast<QValueAxis*>(chart()->axisX());
    m_axisY = qobject_cast<QValueAxis*>(chart()->axisY());
}

void SupervisedChartView::updateRange()
{
    chart()->zoomReset();

    double diff;
    if (m_axisX != nullptr) {
        diff = m_maxX - m_minX;
        if (qFuzzyIsNull(diff))
            diff = 10;
        m_axisX->setRange(m_minX - (diff * 0.2), m_maxX + (diff * 0.2));
    }
    if (m_axisY != nullptr) {
        diff = m_maxY - m_minY;
        if (qFuzzyIsNull(diff))
            diff = 10;
        m_axisY->setRange(m_minY - (diff * 0.2), m_maxY + (diff * 0.2));
    }
}

void SupervisedChartView::addLine(int neuronIndex)
{
    auto* neuron = network()->layer(m_layerIndex+1)->neuron(neuronIndex);
    Q_ASSERT(neuron != nullptr);

    qDebug() << "Adding supervised chart series for neuron" << neuron->name();

    auto* series = new QLineSeries;
    series->setName(neuron->name());

    chart()->addSeries(series);
    updateAxes();

    connect(neuron, &NetworkNeuron::nameChanged, this, [neuron, series] {
        series->setName(neuron->name());
    });
    connect(neuron, &NetworkNeuron::inWeightChanged, this, [this, neuronIndex] {
        if (m_update && !network()->isTraining())
            refreshLine(neuronIndex);
    });
    m_lineSeriesMap[neuronIndex] = series;
    refreshLine(neuronIndex);
}

void SupervisedChartView::setAxes(int layerIndex, int xNeuronIndex, int yNeuronIndex)
{
    if (m_layerIndex != layerIndex
            || m_xIndex != xNeuronIndex
            || m_yIndex != yNeuronIndex) {
        m_layerIndex = layerIndex;
        m_xIndex = xNeuronIndex;
        m_yIndex = yNeuronIndex;
        refreshSamplePoints();
        refreshLines();
        updateRange();
    }
}

void SupervisedChartView::setOutputNeuronIndex(int index)
{
    if (m_outputIndex != index) {
        m_outputIndex = index;
        refreshSamplePoints();
    }
}

void SupervisedChartView::pauseUpdates()
{
    m_update = false;
}

void SupervisedChartView::unpauseUpdates()
{
    m_update = true;
    refreshLines();
}

void SupervisedChartView::reset()
{
    refreshSamplePoints();
    refreshLines();
    updateRange();
}

void SupervisedChartView::resetInteractiveView()
{
    updateRange();
}
