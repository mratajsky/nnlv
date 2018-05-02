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

#include <QGraphicsLineItem>
#include <QColor>
#include <QObject>
#include <QPen>
#include <QPointer>
#include <QVariantAnimation>

class NetworkNeuron;

class NetworkConnection : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    explicit NetworkConnection(NetworkNeuron* n1, NetworkNeuron* n2, double weight,
                               QObject* parent = nullptr);

    NetworkNeuron* neuron1() const;
    void setNeuron1(NetworkNeuron* neuron);
    NetworkNeuron* neuron2() const;
    void setNeuron2(NetworkNeuron* neuron);

    double weight() const;
    double oldWeight() const;
    void setWeight(double weight, bool adjustPenThickness = true);
    void initializeWeight(double weight, bool adjustPenThickness = true);

    double minWeight() const;
    double maxWeight() const;
    void setWeightRange(double min, double max, bool adjustPenThickness = true);

    QColor penColor() const;
    void setPenColor(QColor color);

    Qt::PenStyle penStyle() const;
    void setPenStyle(Qt::PenStyle style);

    bool showValue() const;
    void setShowValue(bool enabled);

public slots:
    void updatePath();

signals:
    void weightChanged(double weight, double oldWeight);
    //
    // The weightInitialized() signal is emitted when weight reset is issued by the user,
    // it may be preceded by the weightChanged() signal if the new weight is different from
    // the previous weight
    //
    void weightInitialized(double weight);

private slots:
    void updatePen();
    void updatePenThickness();

private:
    QPen createPen();
    void init();

    QPointer<NetworkNeuron> m_neuron1;
    QPointer<NetworkNeuron> m_neuron2;
    double m_weight;
    double m_oldWeight;
    double m_minWeight;
    double m_maxWeight;
    double m_penWidth = 1.0;
    QColor m_penColor = Qt::black;
    Qt::PenStyle m_penStyle = Qt::SolidLine;
    bool m_showValue = true;
    QVariantAnimation* m_animation;
    QTimer* m_timer;
};
