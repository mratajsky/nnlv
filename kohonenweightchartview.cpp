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
#include "kohonenweightchartview.h"

#include "colors.h"
#include "kohonenoutputlayer.h"
#include "networkneurondialog.h"

KohonenWeightChartView::KohonenWeightChartView(Network* network, QWidget* parent) :
    NetworkInteractiveChartView(network, parent),
    m_network(network),
    m_inputSeries(new QScatterSeries(this)),
    m_neuronSeries(new QScatterSeries(this)),
    m_currentInputSeries(new QScatterSeries(this)),
    m_currentNeuronSeries(new QScatterSeries(this))
{
    init();
}

void KohonenWeightChartView::init()
{
    setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setWidth(1);
    m_neuronSeries->setPen(pen);
    m_neuronSeries->setColor(Colors::ColorWeightChartNeuron);

    m_inputSeries->setMarkerSize(10);

    //
    // Display a tooltip when user hovers over a neuron
    //
    connect(m_neuronSeries,
            &QScatterSeries::hovered,
            this,
            &KohonenWeightChartView::neuronPointHovered);
    connect(m_currentNeuronSeries,
            &QScatterSeries::hovered,
            this,
            &KohonenWeightChartView::neuronPointHovered);
    //
    // Display a neuron dialog when user double-clicks a neuron
    //
    connect(m_neuronSeries,
            &QScatterSeries::doubleClicked,
            this,
            &KohonenWeightChartView::neuronPointDoubleClicked);
    connect(m_currentNeuronSeries,
            &QScatterSeries::doubleClicked,
            this,
            &KohonenWeightChartView::neuronPointDoubleClicked);

    m_currentNeuronSeries->setColor(Colors::ColorWeightChartMark);
    m_currentNeuronSeries->setPen(pen);

    m_currentInputSeries->setColor(Colors::ColorWeightChartMark);
    m_currentInputSeries->setMarkerSize(10);
    m_currentInputSeries->setPen(pen);

    chart()->addSeries(m_inputSeries);
    chart()->addSeries(m_neuronSeries);
    chart()->addSeries(m_currentInputSeries);
    chart()->addSeries(m_currentNeuronSeries);
    chart()->createDefaultAxes();
    chart()->legend()->hide();

    connect(m_network, &Network::trainingSampleDone, this, [this]
            (const TrainingSample& sample) {
        if (!m_update) {
            // The last selected sample will be no longer valid when unpaused
            m_currentInput.clear();
            m_currentInputPoint = QPointF();
            return;
        }
        updateCurrentInput(sample);
        //
        // BMU and other neuron positions change after a trained sample
        //
        populateNeurons();
        updateCurrentNeuron();
    });

    connect(m_network, &Network::neuronValueChanged, this, [this] {
        //
        // Handle input changes when the network is not being trained. This happens
        // when it's changed manually by the user and it should be reflected by
        // marking the active neuron and the input location.
        //
        if (m_update && !m_network->isTraining()) {
            updateCurrentInput(m_network->inputLayer()->values(true));
            updateCurrentNeuron();
            updateRange();
        }
    });
    connect(m_network, &Network::neuronWeightChanged, this, [this] {
        //
        // Handle weight changes when the network is not being trained. This happens
        // when it's changed manually by the user and it should be reflected by
        // moving the output neuron.
        //
        if (m_update && !m_network->isTraining()) {
            populateNeurons();
            updateCurrentNeuron();
            updateRange();
        }
    });

    m_outputLayer = qobject_cast<KohonenOutputLayer*>(m_network->outputLayer());

    const auto& store = m_network->trainingTableModel()->store();
    connect(&store, &TrainingSampleStore::samplesChanged, this, [this] {
        initChart();
    });

    initChart();
}

void KohonenWeightChartView::initChart()
{
    populateInputs();
    m_currentInputSeries->clear();
    if (!m_currentInput.isEmpty())
        m_currentInputSeries->append(m_currentInputPoint);

    populateNeurons();
    updateCurrentNeuron();
    updateRange();
}

void KohonenWeightChartView::populateInputs()
{
    m_inputMinX = m_inputMinY = qInf();
    m_inputMaxX = m_inputMaxY = -qInf();

    m_inputSeries->clear();

    const auto& samples = network()->trainingTableModel()->store().samples();
    for (const auto& sample : samples) {
        double x = sample.input(m_weightIndex1);
        double y = sample.input(m_weightIndex2);
        m_inputMinX = qMin(m_inputMinX, x);
        m_inputMaxX = qMax(m_inputMaxX, x);
        m_inputMinY = qMin(m_inputMinY, y);
        m_inputMaxY = qMax(m_inputMaxY, y);
        m_inputSeries->append(x, y);
    }
}

void KohonenWeightChartView::populateNeurons()
{
    m_neuronMinX = m_neuronMinY = qInf();
    m_neuronMaxX = m_neuronMaxY = -qInf();

    m_neuronSeries->clear();
    m_neuronPointMap.clear();

    for (int i = 0; i < m_outputLayer->neuronCount(); i++) {
        auto* neuron = m_outputLayer->neuron(i);

        double x = neuron->inConnection(m_weightIndex1)->weight();
        double y = neuron->inConnection(m_weightIndex2)->weight();
        m_neuronMinX = qMin(m_neuronMinX, x);
        m_neuronMaxX = qMax(m_neuronMaxX, x);
        m_neuronMinY = qMin(m_neuronMinY, y);
        m_neuronMaxY = qMax(m_neuronMaxY, y);

        m_neuronPointMap[QPair<double, double>(x, y)] = i;
        m_neuronSeries->append(x, y);
    }
}

void KohonenWeightChartView::updateCurrentInput(const TrainingSample& sample)
{
    updateCurrentInput(sample.inputs());
}

void KohonenWeightChartView::updateCurrentInput(const QVector<double>& input)
{
    m_currentInput = input;
    m_currentInputPoint.setX(m_currentInput.at(m_weightIndex1));
    m_currentInputPoint.setY(m_currentInput.at(m_weightIndex2));

    m_currentInputSeries->clear();
    m_currentInputSeries->append(m_currentInputPoint);
}

void KohonenWeightChartView::updateCurrentNeuron()
{
    m_currentNeuronSeries->clear();
    // Active neuron only makes sense if there is an active input
    if (m_currentInputPoint.isNull())
        return;

    auto* bmu = m_outputLayer->currentBMU();
    if (bmu == nullptr)
        return;

    double x = bmu->inConnection(m_weightIndex1)->weight();
    double y = bmu->inConnection(m_weightIndex2)->weight();
    m_currentNeuronSeries->append(x, y);
}

void KohonenWeightChartView::updateRange()
{
    double minX = qInf();
    double maxX = -qInf();
    double minY = qInf();
    double maxY = -qInf();
    //
    // Inputs
    //
    minX = qMin(minX, m_inputMinX);
    maxX = qMax(maxX, m_inputMaxX);
    minY = qMin(minY, m_inputMinY);
    maxY = qMax(maxY, m_inputMaxY);
    // Current input
    if (!m_currentInputPoint.isNull()) {
        minX = qMin(minX, m_currentInputPoint.x());
        maxX = qMax(maxX, m_currentInputPoint.x());
        minY = qMin(minY, m_currentInputPoint.y());
        maxY = qMax(maxY, m_currentInputPoint.y());
    }

    //
    // Neurons
    //
    minX = qMin(minX, m_neuronMinX);
    maxX = qMax(maxX, m_neuronMaxX);
    minY = qMin(minY, m_neuronMinY);
    maxY = qMax(maxY, m_neuronMaxY);
    // Current neuron
    const auto& points = m_currentNeuronSeries->pointsVector();
    if (!points.isEmpty()) {
        auto point = points.first();
        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }

    double diff;
    diff = maxX - minX;
    if (qFuzzyIsNull(diff))
        diff = 10;
    chart()->axisX()->setRange(minX - diff * 0.1, maxX + diff * 0.1);

    diff = maxY - minY;
    if (qFuzzyIsNull(diff))
        diff = 10;
    chart()->axisY()->setRange(minY - diff * 0.1, maxY + diff * 0.1);
}

void KohonenWeightChartView::setWeightIndex1(int index)
{
    if (m_weightIndex1 != index) {
        m_weightIndex1 = index;
        if (m_currentInput.size() > index)
            m_currentInputPoint.setX(m_currentInput.at(index));
        initChart();
    }
}

void KohonenWeightChartView::setWeightIndex2(int index)
{
    if (m_weightIndex2 != index) {
        m_weightIndex2 = index;
        if (m_currentInput.size() > index)
            m_currentInputPoint.setY(m_currentInput.at(index));
        initChart();
    }
}

void KohonenWeightChartView::pauseUpdates()
{
    m_update = false;
}

void KohonenWeightChartView::unpauseUpdates()
{
    m_update = true;
    initChart();
}

void KohonenWeightChartView::reset()
{
    m_currentInput.clear();
    m_currentInputPoint = QPointF();
    initChart();
}

void KohonenWeightChartView::resetInteractiveView()
{
    updateRange();
}

void KohonenWeightChartView::neuronPointHovered(const QPointF& point, bool state)
{
    if (state) {
        QPair<double, double> pair(point.x(), point.y());
        if (m_neuronPointMap.contains(pair)) {
            auto* neuron = m_outputLayer->neuron(m_neuronPointMap.value(pair));
            setToolTip(QString("%1 (%2, %3)")
                       .arg(neuron->name())
                       .arg(point.x())
                       .arg(point.y()));
        }
    } else
        setToolTip(QString());
}

void KohonenWeightChartView::neuronPointDoubleClicked(const QPointF& point)
{
    QPair<double, double> pair(point.x(), point.y());
    if (m_neuronPointMap.contains(pair)) {
        auto* neuron = m_outputLayer->neuron(m_neuronPointMap.value(pair));
        NetworkNeuronDialog dialog(neuron, this);
        dialog.exec();
    }
}
