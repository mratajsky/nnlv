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

#include <random>
#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>

#include "trainingsample.h"
#include "trainingsamplestore.h"

class TrainingTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TrainingTableModel(QObject* parent = nullptr);

    enum class Mode {
        Integer,
        IntegerAndString,
        Double
    };
    Q_ENUM(Mode)

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void clear();

    void setInitialValue(double value);

    QStringList inputs() const;
    void setInputs(const QStringList &inputs);

    QStringList outputs() const;
    void setOutputs(const QStringList &outputs);

    const TrainingSample &currentSample(bool advance = true);
    const TrainingSample &randomSample();
    const TrainingSample &sample(int index) const;

    bool isEmpty() const;
    bool isValidRow(int row) const;
    bool isValidColumn(int column) const;
    int inputCount() const;
    int outputCount() const;
    int sampleCount() const;
    int constrainRowNumber(int row) const;
    int emptyRowIndex() const;

    bool swapRows(int row1, int row2);

    bool replaceSamples(const TrainingSampleStore& store);
    bool replaceSamples(TrainingSampleStore&& store);

    void setStoreFieldValidator(TrainingSampleStore::FieldValidator validator);
    void setStoreSampleValidator(TrainingSampleStore::SampleValidator validator);

    const TrainingSampleStore& store() const;

    void beginStoreChange();
    void endStoreChange();

    void refreshStoreInputRangeIfNeeded();
signals:
    void dataChangeError(int column, const QString& value, const QString& error);

private:
    int m_currentPosition = 0;
    double m_initialValue = 0;
    QStringList m_inputs;
    QStringList m_outputs;
    TrainingSampleStore m_store;
    std::mt19937 m_generator;
};
