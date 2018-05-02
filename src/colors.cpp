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
#include "colors.h"

QColor Colors::colorNeuronValue(double value, double min, double max)
{
    double slope = 1.0 * (ColorNeuronValueHigh - ColorNeuronValueLow) / (max - min);

    return QColor::fromHsv(ColorNeuronValueLow + slope * (value - min), 255, 255);
}

QColor Colors::colorKohonenOutputNeuronValue(double neighborhoodValue)
{
    return QColor::fromHsv(ColorKohonenOutputColor.hsvHue(),
                           ColorKohonenOutputColor.hsvSaturation(),
                           255 * neighborhoodValue);
}

QColor Colors::colorChartSeries(int index)
{
    constexpr int total = 10;
    //
    // The first 5 colors are taken from the QtCharts' default (light) theme
    //
    switch (index % total) {
        case 0:
            return QColor(0x20, 0x9f, 0xdf);
        case 1:
            return QColor(0x99, 0xca, 0x53);
        case 2:
            return QColor(0xf6, 0xa6, 0x25);
        case 3:
            return QColor(0x6d, 0x5f, 0xd5);
        case 4:
            return QColor(0xbf, 0x59, 0x3e);
        case 5:
            return QColor(0x7e, 0x7e, 0x7e);
        case 6:
            return QColor(0x3e, 0xbf, 0x93);
        case 7:
            return QColor(0xbf, 0xb6, 0x3e);
        case 8:
            return QColor(0xd5, 0x5f, 0xa1);
        case 9:
            return QColor(0x00, 0x00, 0x00);
        default:
            Q_UNREACHABLE();
    }
    return QColor();
}
