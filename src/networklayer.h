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
#include <random>
#include <QVector>
#include <QObject>
#include <QString>
#include <QGraphicsWidget>
#include <QGraphicsItemGroup>

class NetworkLayer;

#include "network.h"
#include "networklayerinfo.h"
#include "networkneuron.h"

class NetworkLayer : public QObject, public QGraphicsItemGroup, public NetworkLayerInfo
{
    Q_OBJECT
public:
    NetworkLayer(NetworkLayerInfo::Type type, Network* parent);
    NetworkLayer(const NetworkLayerInfo::Map& map, Network* parent);

    const NetworkLayerInfo::Map& infoMap() const;

    QString name() const;
    void setName(const QString& name);

    Network* network();

    const QVector<NetworkNeuron*>& neurons() const;
    NetworkNeuron* neuron(int index) const;
    virtual int neuronCount(bool skipBias = false) const;

    virtual void connectTo(const NetworkLayer *nextLayer);

    void addNeuron(NetworkNeuron *neuron, bool setDefaultNeuronName = false);

    void copyWeights(NetworkLayer* otherLayer, bool updateRange);

    bool hasBias() const;
    void resetNeuronValues();

    QVector<double> values(bool skipBias = false) const;
    void setValues(const QVector<double>& values);

    void setInConnectionWeights(double value);
    void setInConnectionWeights(std::function<double()>& fn, bool updateRange = true);

    void setOutConnectionWeights(double value);
    void setOutConnectionWeights(std::function<double()>& fn, bool updateRange = true);

    QStringList neuronNameList(bool skipBias = false) const;
    void setInitialWeightFunction(std::function<double()>& fn);

    void setDefaultNeuronNames();

    void setShowConnectionWeights(bool enabled, bool setForInputs = true, bool setForOutputs = true);
    void setShowNeuronValues(bool enabled);

    virtual bool inConnectionWeightsSettable() const;
    virtual bool outConnectionWeightsSettable() const;

public slots:
    void updateInConnectionRange();
    void updateOutConnectionRange();

signals:
    void nameChanged(const QString& name);
    void statusMessage(const QString& message);
    void neuronNameChanged(int index);
    void neuronValueChanged(int index);
    void neuronWeightChanged(int index);
    void neuronStatusMessage(int index, const QString& message);

protected:
    virtual QString defaultNeuronName(int index) const;

    NetworkLayerInfo::Map m_infoMap;

private:
    bool m_changing = false;
    std::function<double()> m_initialWeight;
    QVector<NetworkNeuron *> m_neurons;
};
