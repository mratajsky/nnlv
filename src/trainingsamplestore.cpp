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
#include "trainingsamplestore.h"

TrainingSampleStore::TrainingSampleStore(QObject* parent) :
    QObject(parent)
{
    init();
}

TrainingSampleStore::TrainingSampleStore(int inputs, int outputs, QObject* parent) :
    QObject(parent),
    m_inputs(inputs),
    m_outputs(outputs)
{
    init();
}

void TrainingSampleStore::init()
{
    updateInputMinMax();
}

//
// Retrieve samples in the store
//
const QVector<TrainingSample>& TrainingSampleStore::samples() const
{
    return m_samples;
}

//
// Retrieve sample with the given index
//
//TrainingSample& TrainingSampleStore::sample(int index)
//{
//    return m_samples[index];
//}
const TrainingSample& TrainingSampleStore::sample(int index) const
{
    return m_samples[index];
}

void TrainingSampleStore::beginChange()
{
    m_changing = true;
}

void TrainingSampleStore::endChange()
{
    if (m_changePending) {
        emit samplesChanged();
        m_changePending = false;
    }
    m_changing = false;
}

void TrainingSampleStore::signalChange()
{
    if (m_changing)
        m_changePending = true;
    else
        emit samplesChanged();
}

//
// Retrieve number of fields
//
int TrainingSampleStore::inputCount() const
{
    return m_inputs;
}
int TrainingSampleStore::outputCount() const
{
    return m_outputs;
}

//
// Change number of fields, removing all samples in the process
//
void TrainingSampleStore::setFieldCount(int inputs, int outputs)
{
    Q_ASSERT(inputs >= 0 && outputs >= 0);

    if (m_inputs != inputs || m_outputs != outputs) {
        m_inputs = inputs;
        m_outputs = outputs;
        qDebug() << "Clearing sample store after field count changed to"
                 << inputs << outputs;
        clearSamples();
    }
}

//
// Add a sample to the store
//
bool TrainingSampleStore::addSample(const TrainingSample& sample)
{
    if (!verifySample(sample))
        return false;

    m_samples.append(sample);
    if (!m_inputRangeRefreshNeeded)
        updateInputMinMax(sample);

    signalChange();
    return true;
}

bool TrainingSampleStore::addSample(TrainingSample&& sample)
{
    if (!verifySample(sample))
        return false;

    m_samples.append(sample);
    if (!m_inputRangeRefreshNeeded)
        updateInputMinMax(sample);

    signalChange();
    return true;
}

bool TrainingSampleStore::addSample(const QStringList& fields)
{
    if (fields.size() != m_inputs + m_outputs) {
        m_error = QString("Expected %1 fields, but sample has %2 fields")
                  .arg(m_inputs + m_outputs)
                  .arg(fields.size());
        return false;
    }

    TrainingSample sample(m_inputs, m_outputs);
    bool ok;
    for (auto i = 0; i < m_inputs; i++) {
        // Input fields must be all numeric
        auto value = fields.at(i).toDouble(&ok);
        if (!ok) {
            m_error = QStringLiteral("Invalid input value");
            return false;
        }
        sample.setInput(i, value);
    }

    for (auto i = 0; i < m_outputs; i++) {
        auto value = fields.at(m_inputs + i).toDouble(&ok);
        if (!ok) {
            m_error = QStringLiteral("Invalid output value");
            return false;
        }
        sample.setOutput(i, value);
    }
    addSample(std::move(sample));
    signalChange();
    return true;
}

//
// Remove sample with the given index
//
void TrainingSampleStore::removeSample(int index, int count, bool refreshInputRange)
{
    if (count < 1)
        return;
    m_samples.remove(index, count);
    if (refreshInputRange)
        updateInputMinMax();
    else
        m_inputRangeRefreshNeeded = true;
    signalChange();
}

//
// Remove all samples from the store
//
void TrainingSampleStore::clearSamples()
{
    m_samples.clear();
    m_samples.squeeze();
    updateInputMinMax();
    signalChange();
}

//
// Swap samples with the given indices
//
void TrainingSampleStore::swapSamples(int index1, int index2)
{
    if (index1 == index2)
        return;

    Q_ASSERT(index1 >= 0 && index1 < m_samples.size());
    Q_ASSERT(index2 >= 0 && index2 < m_samples.size());

    std::swap(m_samples[index1], m_samples[index2]);
}

//
// Retrieve the last error
//
QString TrainingSampleStore::error() const
{
    return m_error;
}

double TrainingSampleStore::minInput() const
{
    return m_minInput;
}

double TrainingSampleStore::maxInput() const
{
    return m_maxInput;
}

//
// Change input value in a sample, performing validation and setting error if it fails
//
bool TrainingSampleStore::setSampleInputString(int indexSample, int indexInput, QString value,
                                               bool refreshInputRangeIfNeeded)
{
    if (!value.isEmpty()) {
        bool ok;
        auto doubleValue = value.toDouble(&ok);
        if (!ok) {
            m_error = QStringLiteral("Invalid input value");
            return false;
        } else {
            if (m_fieldValidator) {
                QString err;
                if (!m_fieldValidator(indexInput, doubleValue, err)) {
                    m_error = err;
                    return false;
                }
            }
            auto& sample = m_samples[indexSample];
            updateInputMinMax(doubleValue, sample.input(indexInput));
            sample.setInput(indexInput, doubleValue);
        }
    } else {
        //
        // Not validated, 0 has to be allowed
        //
        auto& sample = m_samples[indexSample];
        updateInputMinMax(0, sample.input(indexInput));
        sample.setInput(indexInput, 0);
    }
    if (refreshInputRangeIfNeeded && m_inputRangeRefreshNeeded)
        updateInputMinMax();

    signalChange();
    return true;
}

//
// Change output value in a sample, performing validation and setting error if it fails
//
bool TrainingSampleStore::setSampleOutputString(int indexSample, int indexOutput, QString value)
{
    if (!value.isEmpty()) {
        bool ok;
        auto doubleValue = value.toDouble(&ok);
        if (!ok) {
            m_error = QStringLiteral("Invalid output value");
            return false;
        } else {
            if (m_fieldValidator) {
                QString err;
                // Add number of inputs as it uses a field index
                if (!m_fieldValidator(m_inputs + indexOutput, doubleValue, err)) {
                    m_error = err;
                    return false;
                }
            }
            m_samples[indexSample].setOutput(indexOutput, doubleValue);
        }
    } else {
        //
        // Not validated, 0 has to be allowed
        //
        m_samples[indexSample].setOutput(indexOutput, 0);
    }
    signalChange();
    return true;
}

//
// Set a function to validate field changes
//
void TrainingSampleStore::setFieldValidator(FieldValidator validator)
{
    m_fieldValidator = validator;
}

//
// Set a function to validate sample additions
//
void TrainingSampleStore::setSampleValidator(SampleValidator validator)
{
    m_sampleValidator = validator;
}

//
// Remove samples from the store and replace them with samples from another store,
// also modifying the number of fields
//
bool TrainingSampleStore::replaceSamples(const TrainingSampleStore& store)
{
    if (!verifySamples(store.samples()))
        return false;

    m_inputs  = store.m_inputs;
    m_outputs = store.m_outputs;
    m_minInput = store.m_minInput;
    m_maxInput = store.m_maxInput;
    m_inputRangeRefreshNeeded = store.m_inputRangeRefreshNeeded;

    m_samples = store.m_samples;

    signalChange();
    return true;
}

bool TrainingSampleStore::replaceSamples(TrainingSampleStore&& store)
{
    if (!verifySamples(store.samples()))
        return false;

    m_inputs  = store.m_inputs;
    m_outputs = store.m_outputs;
    m_minInput = store.m_minInput;
    m_maxInput = store.m_maxInput;
    m_inputRangeRefreshNeeded = store.m_inputRangeRefreshNeeded;

    m_samples = std::move(store.m_samples);

    signalChange();
    return true;
}

//
// Recalculate the input value range.
//
// This function may be used if the recalculation was delayed when removeSample()
// or setSampleInputString() was called.
//
void TrainingSampleStore::refreshInputRangeIfNeeded()
{
    if (m_inputRangeRefreshNeeded)
        updateInputMinMax();
}

//
// Update input min/max by looking at all samples in the store
//
void TrainingSampleStore::updateInputMinMax()
{
    m_minInput = m_maxInput = qInf();

    for (const auto& sample : qAsConst(m_samples))
        updateInputMinMax(sample);

    m_inputRangeRefreshNeeded = false;
}

//
// Update input min/max with a new sample
//
void TrainingSampleStore::updateInputMinMax(const TrainingSample& sample)
{
    for (int i = 0; i < sample.inputCount(); i++) {
        double value = sample.input(i);
        if (!qIsFinite(m_minInput) || value < m_minInput)
            m_minInput = value;
        if (!qIsFinite(m_maxInput) || value > m_maxInput)
            m_maxInput = value;
    }
}

//
// Update input min/max when a single input value of a sample changes
//
void TrainingSampleStore::updateInputMinMax(double inputValue, double previousValue)
{
    if (inputValue < m_minInput)
        m_minInput = inputValue;
    else if (inputValue > m_minInput && qFuzzyCompare(previousValue, m_minInput)) {
        //
        // The lowest value increased so we don't know if it's still the lowest value
        // or some other value is now the lowest
        //
        m_inputRangeRefreshNeeded = true;
    }

    if (inputValue > m_maxInput)
        m_maxInput = inputValue;
    else if (inputValue < m_maxInput && qFuzzyCompare(previousValue, m_maxInput))
        m_inputRangeRefreshNeeded = true;
}

//
// Verify whether the sample can be added to this store
//
bool TrainingSampleStore::verifySample(const TrainingSample& sample)
{
    if (sample.fieldCount() != m_inputs + m_outputs) {
        m_error = QString("Expected %1 fields, but sample has %2 fields")
                  .arg(m_inputs + m_outputs)
                  .arg(sample.fieldCount());
        return false;
    }
    if (m_sampleValidator) {
        QString err;
        if (!m_sampleValidator(sample, err)) {
            m_error = err;
            return false;
        }
    }
    return true;
}

bool TrainingSampleStore::verifySamples(const QVector<TrainingSample>& samples)
{
    for (const auto& sample : samples) {
        if (!verifySample(sample))
            return false;
    }
    return true;
}
