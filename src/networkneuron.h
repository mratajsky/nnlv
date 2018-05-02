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
#include <QColor>
#include <QGraphicsItem>
#include <QObject>
#include <QString>
#include <QSize>

class NetworkNeuron;

#include "networkconnection.h"
#include "networklayer.h"
#include "networkneuroninfo.h"

class NetworkNeuron : public QObject, public QGraphicsItem, public NetworkNeuronInfo
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    NetworkNeuron(NetworkLayer* parent);
    NetworkNeuron(QVector<double> initialInputWeights, NetworkLayer* parent);
    NetworkNeuron(const NetworkNeuronInfo::Map& map, QVector<double> initialInputWeights,
                           NetworkLayer* parent);

    const NetworkNeuronInfo::Map& infoMap() const;

    QString name() const;
    void setName(const QString& name);

    NetworkLayer* layer();

    virtual double value() const;
    virtual void setValue(double value);

    NetworkConnection* inConnection(int index) const;
    int inConnectionCount() const;
    NetworkConnection* outConnection(int index) const;
    int outConnectionCount() const;

    void setInConnectionWeights(double value);
    void setInConnectionWeights(std::function<double()>& fn);
    void setInConnectionWeights(NetworkNeuron* neuron);

    void setOutConnectionWeights(double value);
    void setOutConnectionWeights(std::function<double()>& fn);
    void setOutConnectionWeights(NetworkNeuron* neuron);

    virtual void addInConnection(NetworkConnection* conn);
    virtual void addOutConnection(NetworkConnection* conn);

    const QVector<NetworkConnection*>& inConnections() const;
    const QVector<NetworkConnection*>& outConnections() const;

    bool isBias() const;
    void setIsBias(bool isBias);

    const QVector<double>& initialInputWeights() const;
    void setInitialInputWeights(const QVector<double>& weights);

    virtual void resetValue();

    bool showValue() const;
    void setShowValue(bool enabled);
    void setShowConnectionWeights(bool enabled, bool setForInputs = true, bool setForOutputs = true);

public slots:
    void updateConnectionPaths(bool updateInputs = true, bool updateOutputs = true);

signals:
    void inWeightChanged(int index);
    void outWeightChanged(int index);
    void nameChanged(const QString& name);
    void valueChanged(double value, double oldValue);
    void statusMessage(const QString& name);

protected:
    QColor colorValue(double min, double max) const;

    NetworkNeuronInfo::Map m_infoMap;

private:
    void setConnectionWeights(const QVector<NetworkConnection*>& connections,
                              double value);
    void setConnectionWeights(const QVector<NetworkConnection*>& connections,
                              std::function<double()> &fn);
    void setConnectionWeights(const QVector<NetworkConnection*>& connections,
                              const QVector<NetworkConnection*>& otherConnections);

    double m_value = 0.0;
    bool m_isBias = false;
    bool m_isReset = true;
    bool m_showValue = true;
    bool m_showConnectionInputs = true;
    bool m_showConnectionOutputs = true;
    bool m_showConnectionInputWeights = true;
    bool m_showConnectionOutputWeights = true;
    QVector<NetworkConnection*> m_inConnections;
    QVector<NetworkConnection*> m_outConnections;
    QVector<double> m_initialInputWeights;
};
