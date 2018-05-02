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
#include "rbfweightchartview.h"

#include "colors.h"
#include "networkneurondialog.h"

RBFWeightChartView::RBFWeightChartView(Network* network, QWidget* parent) :
    NetworkInteractiveChartView(network, parent),
    m_network(network),
    m_neuronSeries(new QScatterSeries(this)),
    m_currentInputSeries(new QScatterSeries(this))
{
    init();
}

void RBFWeightChartView::init()
{
    setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setWidth(1);
    m_neuronSeries->setPen(pen);
    m_neuronSeries->setColor(Colors::ColorWeightChartNeuron);
    m_neuronSeries->setName(tr("RBF neurons"));
    //
    // Display a tooltip when user hovers over a neuron
    //
    connect(m_neuronSeries, &QScatterSeries::hovered, this, [this]
            (const QPointF& point, bool state) {
        if (state) {
            QPair<double, double> pair(point.x(), point.y());
            if (m_neuronPointMap.contains(pair)) {
                auto* neuron = m_hiddenLayer->neuron(m_neuronPointMap.value(pair));
                setToolTip(QString("%1 (%2, %3)")
                           .arg(neuron->name())
                           .arg(point.x())
                           .arg(point.y()));
            }
        } else
            setToolTip(QString());
    });
    //
    // Display a neuron dialog when user double-clicks a neuron
    //
    connect(m_neuronSeries, &QScatterSeries::doubleClicked, this, [this]
            (const QPointF& point) {
        QPair<double, double> pair(point.x(), point.y());
        if (m_neuronPointMap.contains(pair)) {
            auto* neuron = m_hiddenLayer->neuron(m_neuronPointMap.value(pair));
            NetworkNeuronDialog dialog(neuron, this);
            dialog.exec();
        }
    });

    pen.setWidthF(1.5);
    m_currentInputSeries->setPen(pen);
    m_currentInputSeries->setColor(Colors::ColorWeightChartMark);
    m_currentInputSeries->setMarkerSize(10);
    m_currentInputSeries->setName(tr("Current input"));

    connect(m_network, &Network::trainingSampleDone, this, [this](const TrainingSample& sample) {
        if (!m_update) {
            // The last selected sample will be no longer valid when unpaused
            m_currentInput.clear();
            m_currentInputPoint = QPointF();
            return;
        }
        updateCurrentInput(sample);
    });

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
    connect(m_network, &Network::neuronWeightChanged, this, [this] {
        //
        // Handle weight changes when the network is not being trained. This happens
        // when it's changed manually by the user and it should be reflected by
        // moving the output neuron.
        //
        if (m_update && !m_network->isTraining()) {
            populateNeurons();
            updateRange();
        }
    });

    //
    // The hidden layer always has the index 1
    //
    m_hiddenLayer = qobject_cast<RBFHiddenLayer*>(network()->layer(1));

    connect(m_hiddenLayer, &RBFHiddenLayer::trained, this, [this] {
        initChart();
    });
    connect(m_hiddenLayer, &RBFHiddenLayer::untrained, this, [this] {
        initChart();
    });

    const auto& store = network()->trainingTableModel()->store();
    connect(&store, &TrainingSampleStore::samplesChanged, this, [this] {
        m_currentInputSeries->clear();
        m_currentInput.clear();
        m_currentInputPoint = QPointF();
        initChart();
    });

    initChart();
}

void RBFWeightChartView::initChart()
{
    populateInputs();
    m_currentInputSeries->clear();
    if (!m_currentInput.isEmpty())
        m_currentInputSeries->append(m_currentInputPoint);

    populateNeurons();

    updateRange();
}

void RBFWeightChartView::createInputSeriesBasic()
{
    auto* series = new QScatterSeries(this);
    series->setMarkerSize(10);
    series->setName(tr("Input"));

    chart()->addSeries(series);
    m_inputSeriesMap[0] = series;
}

void RBFWeightChartView::createInputSeriesClasses()
{
    const auto& samples = network()->trainingTableModel()->store().samples();

    int index = 0;
    for (int i = 0; i < samples.size(); i++) {
        const auto& sample = samples.at(i);

        const auto& value = sample.outputs();

        if (!m_inputClassSeriesMap.contains(value)) {
            auto* series = new QScatterSeries(this);
            series->setMarkerSize(10);

            if (value.size() == 1)
                series->setName(QString::number(value.first()));
            else {
                QString name = "(";
                for (int j = 0; j < value.size(); j++) {
                    if (j > 0)
                        name += ",";
                    name += QString::number(value.at(j));
                }
                name += ")";
                series->setName(name);
            }
            chart()->addSeries(series);

            m_inputClassSeriesMap[value] = index;
            m_inputSeriesMap[index] = series;
            index++;
        }
    }
}

void RBFWeightChartView::createInputSeriesOutput(int index)
{
    const auto& samples = network()->trainingTableModel()->store().samples();

    for (int i = 0; i < samples.size(); i++) {
        const auto& sample = samples.at(i);

        double value = sample.output(index);

        if (!m_inputSeriesMap.contains(value)) {
            auto* series = new QScatterSeries(this);
            series->setMarkerSize(10);
            series->setName(QString::number(value));
            m_inputSeriesMap[value] = series;

            qDebug() << "Adding RBF output series" << value;
        }
    }

    //
    // Now add all series to the chart, this is done afterwards to sort them
    // by value
    //
    QMapIterator<double, QScatterSeries*> i(m_inputSeriesMap);
    int colorIndex = 0;
    while (i.hasNext()) {
        i.next();
        auto* series = i.value();
        series->setColor(Colors::colorChartSeries(colorIndex++));
        chart()->addSeries(series);
    }
}

void RBFWeightChartView::updateInputSeries()
{
    if (!chart()->series().isEmpty()) {
        //
        // First remove the series that will be preserved and add them back later because
        // removeSeries() doesn't delete the series and removeAllSeries() does
        //
        chart()->removeSeries(m_neuronSeries);
        chart()->removeSeries(m_currentInputSeries);
        chart()->removeAllSeries();
    }
    m_inputSeriesMap.clear();
    m_inputClassSeriesMap.clear();

    switch (m_coloringType) {
        case ColoringType::Basic:
            createInputSeriesBasic();
            break;
        case ColoringType::Classes:
            createInputSeriesClasses();
            break;
        case ColoringType::OutputIndex:
            createInputSeriesOutput(m_coloringOutputIndex);
            break;
    }
    chart()->addSeries(m_neuronSeries);
    chart()->addSeries(m_currentInputSeries);
    chart()->createDefaultAxes();
}

void RBFWeightChartView::populateInputs()
{
    updateInputSeries();

    m_inputMinX = m_inputMinY = qInf();
    m_inputMaxX = m_inputMaxY = -qInf();

    const auto& samples = m_network->trainingTableModel()->store().samples();
    for (int i = 0; i < samples.size(); i++) {
        const auto& sample = samples.at(i);

        double x = sample.input(m_weightIndex1);
        double y = sample.input(m_weightIndex2);
        m_inputMinX = qMin(m_inputMinX, x);
        m_inputMaxX = qMax(m_inputMaxX, x);
        m_inputMinY = qMin(m_inputMinY, y);
        m_inputMaxY = qMax(m_inputMaxY, y);

        switch (m_coloringType) {
            case ColoringType::Basic:
                m_inputSeriesMap[0]->append(x, y);
                break;
            case ColoringType::Classes:
                m_inputSeriesMap[m_inputClassSeriesMap.value(sample.outputs())]->append(x, y);
                break;
            case ColoringType::OutputIndex:
                m_inputSeriesMap[sample.output(m_coloringOutputIndex)]->append(x, y);
                break;
        }
    }
    updateRange();
}

void RBFWeightChartView::populateNeurons()
{
    m_neuronMinX = m_neuronMinY = qInf();
    m_neuronMaxX = m_neuronMaxY = -qInf();

    m_neuronSeries->clear();
    m_neuronPointMap.clear();

    for (int i = 0; i < m_hiddenLayer->neuronCount(); i++) {
        auto* neuron = m_hiddenLayer->neuron(i);
        if (neuron->isBias())
            continue;

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

void RBFWeightChartView::updateCurrentInput(const TrainingSample& sample)
{
    QScatterSeries* series;
    switch (m_coloringType) {
        case ColoringType::Classes:
            series = m_inputSeriesMap[m_inputClassSeriesMap.value(sample.outputs())];
            break;
        case ColoringType::OutputIndex:
            series = m_inputSeriesMap[sample.output(m_coloringOutputIndex)];
            break;
        default:
            series = nullptr;
            break;
    }
    if (series != nullptr)
        m_currentInputSeries->setColor(series->color());
    else
        m_currentInputSeries->setColor(Colors::ColorWeightChartMark);

    updateCurrentInput(sample.inputs());
}

void RBFWeightChartView::updateCurrentInput(const QVector<double>& input)
{
    m_currentInput = input;
    m_currentInputPoint.setX(m_currentInput.at(m_weightIndex1));
    m_currentInputPoint.setY(m_currentInput.at(m_weightIndex2));

    if (m_currentInputSeries->pointsVector().isEmpty())
        m_currentInputSeries->append(m_currentInputPoint);
    else
        m_currentInputSeries->replace(0, m_currentInputPoint);
}

void RBFWeightChartView::updateRange()
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

    double diffX = qAbs(maxX - minX);
    if (qFuzzyIsNull(diffX))
        diffX = 10;
    chart()->axisX()->setRange(minX - diffX * 0.1, maxX + diffX * 0.1);

    double diffY = qAbs(maxY - minY);
    if (qFuzzyIsNull(diffY))
        diffY = 10;
    chart()->axisY()->setRange(minY - diffY * 0.1, maxY + diffY * 0.1);
}

void RBFWeightChartView::pauseUpdates()
{
    m_update = false;
}

void RBFWeightChartView::unpauseUpdates()
{
    m_update = true;
}

void RBFWeightChartView::reset()
{
    m_currentInput.clear();
    m_currentInputPoint = QPointF();
    initChart();
}

void RBFWeightChartView::resetInteractiveView()
{
    updateRange();
}

void RBFWeightChartView::setColoringType(RBFWeightChartView::ColoringType coloringType)
{
    if (m_coloringType != coloringType) {
        m_coloringType = coloringType;
        //
        // Set the universal mark color for uniform coloring because the color
        // is now wrong and it's better to at least keep the mark
        //
        m_currentInputSeries->setColor(Colors::ColorWeightChartMark);
        populateInputs();
    }
}

void RBFWeightChartView::setColoringOutputIndex(int index)
{
    if (m_coloringType != ColoringType::OutputIndex || m_coloringOutputIndex != index) {
        m_coloringType = ColoringType::OutputIndex;
        m_coloringOutputIndex = index;
        populateInputs();
    }
}

void RBFWeightChartView::setWeightIndex1(int index)
{
    if (m_weightIndex1 != index) {
        m_weightIndex1 = index;
        if (m_currentInput.size() > index)
            m_currentInputPoint.setX(m_currentInput.at(index));
        initChart();
    }
}

void RBFWeightChartView::setWeightIndex2(int index)
{
    if (m_weightIndex2 != index) {
        m_weightIndex2 = index;
        if (m_currentInput.size() > index)
            m_currentInputPoint.setY(m_currentInput.at(index));
        initChart();
    }
}
