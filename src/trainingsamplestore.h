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

#include <functional>
#include <QObject>
#include <QStringList>
#include <QVector>

#include "trainingsample.h"

class TrainingSampleStore : public QObject
{
    Q_OBJECT
public:
    explicit TrainingSampleStore(QObject* parent = nullptr);
    explicit TrainingSampleStore(int inputs, int outputs, QObject* parent = nullptr);

    const QVector<TrainingSample>& samples() const;
    const TrainingSample& sample(int index) const;

    void beginChange();
    void endChange();

    int inputCount() const;
    int outputCount() const;

    void setFieldCount(int inputs, int outputs);

    bool addSample(const TrainingSample& sample);
    bool addSample(TrainingSample&& sample);
    bool addSample(const QStringList& fields);

    void removeSample(int index, int count = 1, bool refreshInputRange = true);
    void clearSamples();
    void swapSamples(int index1, int index2);

    QString error() const;
    double minInput() const;
    double maxInput() const;

    bool setSampleInputString(int indexSample, int indexInput, QString value,
                              bool refreshInputRangeIfNeeded = true);
    bool setSampleOutputString(int indexSample, int indexOutput, QString value);

    using FieldValidator = std::function<bool(int, double, QString&)>;
    using SampleValidator = std::function<bool(const TrainingSample&, QString&)>;

    void setFieldValidator(FieldValidator validator);
    void setSampleValidator(SampleValidator validator);

    bool replaceSamples(const TrainingSampleStore& store);
    bool replaceSamples(TrainingSampleStore&& store);

    void refreshInputRangeIfNeeded();

signals:
    //
    // Emitted when samples are modified, added or removed
    //
    void samplesChanged();

private:
    void init();
    void updateInputMinMax();
    void updateInputMinMax(const TrainingSample& sample);
    void updateInputMinMax(double inputValue, double previousValue);
    bool verifySample(const TrainingSample& sample);
    bool verifySamples(const QVector<TrainingSample>& samples);
    void signalChange();

    int m_inputs = 0;
    int m_outputs = 0;
    QVector<TrainingSample> m_samples;
    QString m_error;
    bool m_inputRangeRefreshNeeded = false;
    double m_minInput;
    double m_maxInput;
    FieldValidator m_fieldValidator;
    SampleValidator m_sampleValidator;
    bool m_changing = false;
    bool m_changePending = false;
};
