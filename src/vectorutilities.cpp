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
#include "vectorutilities.h"

#include <cmath>

double VectorUtilities::distance(const QVector<double>& vec1, const QVector<double>& vec2)
{
    Q_ASSERT(vec1.size() == vec2.size());

    double distance = 0.0;
    for (int i = 0; i < vec1.size(); i++) {
        double diff = vec1.at(i) - vec2.at(i);
        distance += diff * diff;
    }
    return std::sqrt(distance);
}

void VectorUtilities::addEach(QVector<double>& vec, const QVector<double>& addend)
{
    Q_ASSERT(vec.size() == addend.size());

    for (int i = 0; i < vec.size(); i++)
        vec[i] += addend[i];
}

void VectorUtilities::divideEach(QVector<double>& vec, double divisor)
{
    for (int i = 0; i < vec.size(); i++)
        vec[i] /= divisor;
}
