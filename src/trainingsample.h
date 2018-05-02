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

#include <QString>
#include <QVector>

class TrainingSample
{
public:
    //
    // Constructs an invalid sample with 0 inputs and 0 outputs.
    //
    // It allows objects of this class to be stored in a QT container.
    //
    explicit TrainingSample();
    explicit TrainingSample(int inputs, int outputs, double initialValue = 0.0);

    bool isValid() const;

    int fieldCount() const;
    int inputCount() const;
    int outputCount() const;

    const QVector<double>& inputs() const;
    const QVector<double>& outputs() const;

    double field(int index) const;
    double input(int index) const;
    double output(int index) const;

    void setField(int index, double value);
    void setInput(int index, double value);
    void setOutput(int index, double value);

private:
    QVector<double> m_inputs;
    QVector<double> m_outputs;
};

inline QDebug operator<<(QDebug debug, const TrainingSample& sample) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "Input["
                    << sample.inputs()
                    << "] Output["
                    << sample.outputs()
                    << "]";
    return debug;
}
