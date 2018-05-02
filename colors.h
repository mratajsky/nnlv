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

#include <QColor>

namespace Colors {
    //
    // Hue of neuron colors, red is low, green is high
    //
    const int ColorNeuronValueLow = 0;
    const int ColorNeuronValueHigh = 120;

    //
    // Color of a bias neuron
    //
    const QColor ColorBias = QColor(0x20, 0x9f, 0xdf);
    //
    // Low value color
    //
    const QColor ColorNeuronLow = QColor::fromHsv(ColorNeuronValueLow, 255, 255);
    //
    // High value color
    //
    const QColor ColorNeuronHigh = QColor::fromHsv(ColorNeuronValueHigh, 255, 255);
    //
    // Initial neuron color
    //
    const QColor ColorNeuronInitial = ColorNeuronLow;
    //
    // Kohonen output neuron colors
    //
    const QColor ColorKohonenOutputInitial = Qt::black;
    const QColor ColorKohonenOutputColor = QColor::fromHsv(48, 255, 255);
    //
    // Neuron mark in a weight chart
    //
    const QColor ColorWeightChartNeuron = Qt::red;
    //
    // Current sample mark in a weight chart
    //
    const QColor ColorWeightChartMark = QColor::fromHsv(48, 255, 255);

    //
    // Text color
    //
    const QColor ColorText = Qt::black;

    //
    // Calculate color for value which is in the range [min, max]
    //
    QColor colorNeuronValue(double value, double min, double max);

    //
    // Calculate Kohonen output neuron color
    //
    QColor colorKohonenOutputNeuronValue(double neighborhoodValue);

    //
    // Return color for a QtChart series with the given index
    //
    QColor colorChartSeries(int index);
}
