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
#include "trainingtablemodel.h"

#include <QBrush>
#include <QColor>

#include <utility>

TrainingTableModel::TrainingTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_generator(std::random_device{}())
{
}

QStringList TrainingTableModel::inputs() const
{
    return m_inputs;
}

QStringList TrainingTableModel::outputs() const
{
    return m_outputs;
}

void TrainingTableModel::setInputs(const QStringList &inputs)
{
    m_store.setFieldCount(inputs.size(), m_outputs.size());
    m_inputs = inputs;
}

void TrainingTableModel::setOutputs(const QStringList &outputs)
{
    m_store.setFieldCount(m_inputs.size(), outputs.size());
    m_outputs = outputs;
}

//
// Retrieve the current item from the training set and optionally
// advance the internal pointer to make this method return the next item
// in sequence the next time it is called.
//
// The sample list must not be empty.
//
const TrainingSample& TrainingTableModel::currentSample(bool advance)
{
    const auto& samples = m_store.samples();

    Q_ASSERT(samples.size() > 0);
    Q_ASSERT(m_currentPosition < samples.size());

    const auto& item = samples.at(m_currentPosition);
    if (advance) {
        // Start over when the end of sequence is reached
        if (++m_currentPosition == samples.size())
            m_currentPosition = 0;
    }
    return item;
}

//
// Retrieve a random item from the training set
//
// The sample list must not be empty.
//
const TrainingSample& TrainingTableModel::randomSample()
{
    const auto& samples = m_store.samples();

    Q_ASSERT(samples.size() > 0);

    std::uniform_int_distribution<int> dist(0, samples.size() - 1);
    return samples.at(dist(m_generator));
}

//
// Return the sample corresponding to the given table row, counted from 0.
//
// The index must be valid.
//
const TrainingSample& TrainingTableModel::sample(int index) const
{
    Q_ASSERT(isValidRow(index));

    return m_store.samples().at(index);
}

int TrainingTableModel::inputCount() const
{
    return m_inputs.size();
}

int TrainingTableModel::outputCount() const
{
    return m_outputs.size();
}

int TrainingTableModel::sampleCount() const
{
    return m_store.samples().size();
}

int TrainingTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        //
        // When implementing a table based model,
        // rowCount() should return 0 when the parent is valid.
        //
        return 0;
    }
    // There is an extra row for new data
    return m_store.samples().size() + 1;
}

int TrainingTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        //
        // When implementing a table based model,
        // columnCount() should return 0 when the parent is valid.
        //
        return 0;
    }
    return m_inputs.size() + m_outputs.size();
}

QVariant TrainingTableModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    if (isValidRow(row) && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        //
        // The Display and Edit roles return strings as per documentation
        //
        return QString::number(sample(row).field(index.column()));
    }
    // Return invalid QVariant on unknown item or role
    return QVariant();
}

bool TrainingTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //
    // Only support the edit role which is used when user changes
    // the value in the table
    //
    if (role != Qt::EditRole)
        return false;

    const auto row = index.row();
    if (row == m_store.samples().size()) {
        qDebug() << "Creating a new sample for row" << row;
        //
        // Adding value to the + row, create a new item for it
        //
        beginInsertRows(QModelIndex(), row + 1, row + 1);
        TrainingSample item(m_inputs.size(), m_outputs.size(), m_initialValue);
        m_store.addSample(std::move(item));
        endInsertRows();
    }
    if (isValidRow(row)) {
        bool result;
        int column = index.column();
        if (column < m_store.inputCount())
            result = m_store.setSampleInputString(row, column, value.toString(), false);
        else
            result = m_store.setSampleOutputString(row, column - m_store.inputCount(), value.toString());
        if (!result) {
            auto error = m_store.error();
            emit dataChangeError(column, value.toString(), error);
            return false;
        }
        emit dataChanged(index, index, QVector<int>{ role });
        return true;
    }
    return false;
}

bool TrainingTableModel::swapRows(int row1, int row2)
{
    if (row1 == row2)
        return false;
    //
    // First swap the samples in the sample vector, then remove both
    // rows from the model and add them in reverse order
    //
    m_store.swapSamples(row1, row2);
    if (row1 > row2)
        std::swap(row1, row2);
    beginRemoveRows(QModelIndex(), row2, row2);
    endRemoveRows();
    beginRemoveRows(QModelIndex(), row1, row1);
    endRemoveRows();
    beginInsertRows(QModelIndex(), row1, row1);
    endInsertRows();
    beginInsertRows(QModelIndex(), row2, row2);
    endInsertRows();
    return true;
}

bool TrainingTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!isValidRow(row) || count < 1)
        return false;
    beginRemoveRows(parent, row, row + count - 1);
    m_store.removeSample(row, count, false);
    endRemoveRows();
    return true;
}

Qt::ItemFlags TrainingTableModel::flags(const QModelIndex &index) const
{
    if (isValidColumn(index.column()))
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    return 0;
}

//
// It returns 0 if there are no samples
//
int TrainingTableModel::constrainRowNumber(int row) const
{
    return qBound(0, row, m_store.samples().size() - 1);
}

//
// Return the index of the empty row at the end used for adding new items
//
int TrainingTableModel::emptyRowIndex() const
{
    return m_store.samples().size();
}

bool TrainingTableModel::isEmpty() const
{
    return m_store.samples().isEmpty();
}

//
// Return true if the given row is present and is not the extra
// empty row
//
bool TrainingTableModel::isValidRow(int row) const
{
    return row >= 0 && row < m_store.samples().size();
}

//
// Return true if the given column is present
//
bool TrainingTableModel::isValidColumn(int column) const
{
    return column >= 0 && column < (m_inputs.size() + m_outputs.size());
}

void TrainingTableModel::clear()
{
    beginResetModel();
    m_store.clearSamples();
    m_currentPosition = 0;
    endResetModel();
}

void TrainingTableModel::setInitialValue(double value)
{
    m_initialValue = value;
}

QVariant TrainingTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        if (orientation == Qt::Vertical) {
            if (!isValidRow(section))
                return QString("+");
            return QString::number(section + 1);
        } else {
            if (section < m_inputs.size())
                return m_inputs.at(section);
            return m_outputs.at(section - m_inputs.size());
        }
    case Qt::ForegroundRole:
        return QBrush(Qt::black);
    default:
        break;
    }
    return QVariant();
}

const TrainingSampleStore& TrainingTableModel::store() const
{
    return m_store;
}

bool TrainingTableModel::replaceSamples(const TrainingSampleStore& store)
{
    beginResetModel();
    bool result = m_store.replaceSamples(store);
    if (result)
        m_currentPosition = 0;
    endResetModel();
    return result;
}

bool TrainingTableModel::replaceSamples(TrainingSampleStore&& store)
{
    beginResetModel();
    bool result = m_store.replaceSamples(store);
    if (result)
        m_currentPosition = 0;
    endResetModel();
    return result;
}

void TrainingTableModel::setStoreFieldValidator(TrainingSampleStore::FieldValidator validator)
{
    m_store.setFieldValidator(validator);
}

void TrainingTableModel::setStoreSampleValidator(TrainingSampleStore::SampleValidator validator)
{
    m_store.setSampleValidator(validator);
}

void TrainingTableModel::beginStoreChange()
{
    m_store.beginChange();
}

void TrainingTableModel::endStoreChange()
{
    m_store.endChange();
}

//
// Call refreshInputRangeIfNeed() on the store, use this when done with a series of changes
//
void TrainingTableModel::refreshStoreInputRangeIfNeeded()
{
    m_store.refreshInputRangeIfNeeded();
}
