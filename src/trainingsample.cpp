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
#include "trainingsample.h"

TrainingSample::TrainingSample()
{
}

TrainingSample::TrainingSample(int inputs, int outputs, double initialValue) :
    m_inputs(inputs, initialValue),
    m_outputs(outputs, initialValue)
{
}

bool TrainingSample::isValid() const
{
    return m_inputs.size() >= 0 || m_outputs.size() >= 0;
}

int TrainingSample::fieldCount() const
{
    return m_inputs.size() + m_outputs.size();
}

int TrainingSample::inputCount() const
{
    return m_inputs.size();
}

int TrainingSample::outputCount() const
{
    return m_outputs.size();
}

const QVector<double>& TrainingSample::inputs() const
{
    return m_inputs;
}

const QVector<double>& TrainingSample::outputs() const
{
    return m_outputs;
}

double TrainingSample::field(int index) const
{
    if (index < m_inputs.size())
        return m_inputs.at(index);

    index -= m_inputs.size();
    Q_ASSERT(index < m_outputs.size());

    return m_outputs.at(index);
}

double TrainingSample::input(int index) const
{
    Q_ASSERT(index >= 0 && index < m_inputs.size());

    return m_inputs.at(index);
}

double TrainingSample::output(int index) const
{
    Q_ASSERT(index >= 0 && index < m_outputs.size());

    return m_outputs.at(index);
}

void TrainingSample::setField(int index, double value)
{
    if (index < m_inputs.size())
        setInput(index, value);
    else
        setOutput(index - m_inputs.size(), value);
}

void TrainingSample::setInput(int index, double value)
{
    Q_ASSERT(index >= 0 && index < m_inputs.size());

    m_inputs[index] = value;
}

void TrainingSample::setOutput(int index, double value)
{
    Q_ASSERT(index >= 0 && index < m_outputs.size());

    m_outputs[index] = value;
}
