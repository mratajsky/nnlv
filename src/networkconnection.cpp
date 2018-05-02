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
#include "networkconnection.h"

#include <QTimer>

#include "networkneuron.h"

NetworkConnection::NetworkConnection(NetworkNeuron* n1, NetworkNeuron* n2, double weight, QObject* parent) :
    QObject(parent),
    m_neuron1(n1),
    m_neuron2(n2),
    m_weight(weight),
    m_oldWeight(weight),
    m_minWeight(weight),
    m_maxWeight(weight),
    m_animation(new QVariantAnimation(this)),
    m_timer(new QTimer(this))
{
    init();
}

void NetworkConnection::init()
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setZValue(-1);
    updatePen();
    updatePath();

    connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        m_penWidth = value.toDouble();
        updatePen();
    });
    connect(m_animation, &QVariantAnimation::finished, this, [this] {
        //
        // Re-run if the current value has changed
        //
        updatePenThickness();
    });
    m_animation->setDuration(200);

    connect(m_timer, &QTimer::timeout, this, [this] {
        updatePenThickness();
    });
    m_timer->setInterval(100);
    m_timer->setSingleShot(true);
}

NetworkNeuron* NetworkConnection::neuron1() const
{
    return m_neuron1;
}

void NetworkConnection::setNeuron1(NetworkNeuron* neuron)
{
    m_neuron1 = neuron;
}

NetworkNeuron* NetworkConnection::neuron2() const
{
    return m_neuron2;
}

void NetworkConnection::setNeuron2(NetworkNeuron* neuron)
{
    m_neuron2 = neuron;
}

double NetworkConnection::weight() const
{
    return m_weight;
}

double NetworkConnection::oldWeight() const
{
    return m_oldWeight;
}

//
// Change the connection weight.
//
// Set adjustPenThickness to false if the weight change is going to be followed by a
// weight range adjustment.
//
void NetworkConnection::setWeight(double weight, bool adjustPenThickness)
{
    if (!qFuzzyCompare(weight, m_weight)) {
        m_oldWeight = m_weight;
        m_weight = weight;
        //
        // Make sure the new weight is within the limits, otherwise the calculation
        // in updatePenThickness() would be wrong
        //
        m_minWeight = qMin(m_minWeight, weight);
        m_maxWeight = qMax(m_maxWeight, weight);

        emit weightChanged(m_weight, m_oldWeight);

        if (adjustPenThickness && m_showValue)
            updatePenThickness();
    }
}

void NetworkConnection::initializeWeight(double weight, bool adjustPenThickness)
{
    // Set limits before calling setWeight() as it may use them to update the thickness
    m_minWeight = m_maxWeight = weight;

    if (!qFuzzyCompare(weight, m_weight))
        setWeight(weight, adjustPenThickness);
    else if (adjustPenThickness && m_showValue) {
        //
        // Make sure update the thickness for the new range even when weights remain
        // the same
        //
        updatePenThickness();
    }
    emit weightInitialized(weight);
}

double NetworkConnection::minWeight() const
{
    return m_minWeight;
}

double NetworkConnection::maxWeight() const
{
    return m_maxWeight;
}

void NetworkConnection::setWeightRange(double min, double max, bool adjustPenThickness)
{
    if (!qFuzzyCompare(min, m_minWeight) || !qFuzzyCompare(max, m_maxWeight)) {
        m_minWeight = min;
        m_maxWeight = max;

        if (adjustPenThickness && m_showValue)
            updatePenThickness();
    }
}

QColor NetworkConnection::penColor() const
{
    return m_penColor;
}

void NetworkConnection::setPenColor(QColor color)
{
    m_penColor = color;
    updatePen();
}

Qt::PenStyle NetworkConnection::penStyle() const
{
    return m_penStyle;
}

void NetworkConnection::setPenStyle(Qt::PenStyle style)
{
    m_penStyle = style;
    updatePen();
}

bool NetworkConnection::showValue() const
{
    return m_showValue;
}

void NetworkConnection::setShowValue(bool enabled)
{
    if (m_showValue != enabled) {
        m_showValue = enabled;
        if (!enabled) {
            if (m_animation->state() == QAbstractAnimation::Running)
                m_animation->stop();
            m_penWidth = 1.0;
            updatePen();
        } else
            updatePenThickness();
    }
}

void NetworkConnection::updatePen()
{
    QPen pen(m_penColor, m_penWidth, m_penStyle);
    setPen(pen);
}

//
// Update pen thickness with animation.
//
// Should be called internally when weight or weight range changes. It's possible
// to call it when a transition is in progress. Once the transition finishes, it
// will be verified whether the current thickness is correct and eventually it will
// be run again.
//
void NetworkConnection::updatePenThickness()
{
    if (!m_showValue || m_animation->state() == QAbstractAnimation::Running)
        return;
    QRectF rect = m_neuron1->boundingRect();
    if (!rect.isValid()) {
        //
        // Schedule the update for later if the neuron rectangle is not yet
        // ready
        //
        m_timer->start();
        return;
    }

    double max = rect.height() / 5;
    double min = qMax(1.0, max / 100.0);
    double diff = m_maxWeight - m_minWeight;
    double width;
    if (diff > 0)
        width = qMax(1.0, min + (max - min) * ((m_weight - m_minWeight) / diff));
    else
        width = 1.0;

    if (!qFuzzyCompare(width, m_penWidth)) {
        m_animation->setStartValue(m_penWidth);
        m_animation->setEndValue(width);
        m_animation->start();
    }
}

//
// Update line path between neurons.
//
// Should be called externally when neuron position or size changes.
//
void NetworkConnection::updatePath()
{
    QLineF line(mapFromItem(m_neuron1, 0, 0), mapFromItem(m_neuron2, 0, 0));
    setLine(line);
}
