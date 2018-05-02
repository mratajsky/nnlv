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
#include "networkneuron.h"

#include "colors.h"

NetworkNeuron::NetworkNeuron(NetworkLayer *parent) :
    NetworkNeuron(NetworkNeuronInfo::Map(), QVector<double>(), parent)
{
}

NetworkNeuron::NetworkNeuron(QVector<double> initialInputWeights, NetworkLayer* parent) :
    NetworkNeuron(NetworkNeuronInfo::Map(), initialInputWeights, parent)
{
}

NetworkNeuron::NetworkNeuron(const NetworkNeuronInfo::Map& map,
                             QVector<double> initialInputWeights,
                             NetworkLayer* parent) :
    QObject(parent),
    m_infoMap(map),
    m_initialInputWeights(initialInputWeights)
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

const NetworkNeuronInfo::Map& NetworkNeuron::infoMap() const
{
    return m_infoMap;
}

QString NetworkNeuron::name() const
{
    return m_infoMap[NetworkNeuronInfo::Key::Name].toString();
}

void NetworkNeuron::setName(const QString& name)
{
    if (this->name() != name) {
        m_infoMap[NetworkNeuronInfo::Key::Name] = name;
        emit nameChanged(name);
    }
}

NetworkLayer* NetworkNeuron::layer()
{
    return qobject_cast<NetworkLayer*>(parent());
}

QColor NetworkNeuron::colorValue(double min, double max) const
{
    if (m_isReset || !qIsFinite(min) || !qIsFinite(max) || !(min < max))
        return Colors::ColorNeuronInitial;

    //
    // Not having the value within the [min, max] range is not an error, just
    // bound it
    //
    return Colors::colorNeuronValue(qBound(min, value(), max), min, max);
}

double NetworkNeuron::value() const
{
    if (m_isBias)
        return 1.0;

    return m_value;
}

void NetworkNeuron::setValue(double value)
{
    if (!qFuzzyCompare(value, m_value)) {
        if (m_isBias) {
            qWarning() << "Ignoring attempt to change bias value to" << value;
            return;
        }
        double previous = m_value;
        m_value = value;
        m_isReset = false;
        update();
        emit valueChanged(value, previous);
    }
}

NetworkConnection* NetworkNeuron::inConnection(int index) const
{
    return m_inConnections.at(index);
}

int NetworkNeuron::inConnectionCount() const
{
    return m_inConnections.count();
}

NetworkConnection* NetworkNeuron::outConnection(int index) const
{
    return m_outConnections.at(index);
}

int NetworkNeuron::outConnectionCount() const
{
    return m_outConnections.count();
}

void NetworkNeuron::setInConnectionWeights(double value)
{
    setConnectionWeights(m_inConnections, value);
}

void NetworkNeuron::setInConnectionWeights(std::function<double()>& fn)
{
    setConnectionWeights(m_inConnections, fn);
}

void NetworkNeuron::setInConnectionWeights(NetworkNeuron* neuron)
{
    setConnectionWeights(m_inConnections, neuron->inConnections());
}

void NetworkNeuron::setOutConnectionWeights(double value)
{
    setConnectionWeights(m_outConnections, value);
}

void NetworkNeuron::setOutConnectionWeights(std::function<double()>& fn)
{
    setConnectionWeights(m_outConnections, fn);
}

void NetworkNeuron::setOutConnectionWeights(NetworkNeuron* neuron)
{
    setConnectionWeights(m_outConnections, neuron->outConnections());
}

void NetworkNeuron::setConnectionWeights(const QVector<NetworkConnection*>& connections,
                                         double value)
{
    for (auto* conn : connections)
        conn->initializeWeight(value);
}

void NetworkNeuron::setConnectionWeights(const QVector<NetworkConnection*>& connections,
                                         std::function<double()>& fn)
{
    for (auto* conn : connections)
        conn->initializeWeight(fn());
}

void NetworkNeuron::setConnectionWeights(const QVector<NetworkConnection*>& connections,
                                         const QVector<NetworkConnection*>& otherConnections)
{
    Q_ASSERT(connections.size() == otherConnections.size());

    for (int i = 0; i < connections.size(); i++)
        connections.at(i)->initializeWeight(otherConnections.at(i)->weight());
}

void NetworkNeuron::addInConnection(NetworkConnection* conn)
{
    int index = m_inConnections.size();

    connect(conn, &NetworkConnection::weightChanged, this, [this, index] {
        emit inWeightChanged(index);
    });
    m_inConnections.append(conn);
}

void NetworkNeuron::addOutConnection(NetworkConnection* conn)
{
    int index = m_outConnections.size();

    connect(conn, &NetworkConnection::weightChanged, this, [this, index] {
        emit outWeightChanged(index);
    });
    m_outConnections.append(conn);
}

const QVector<NetworkConnection*>& NetworkNeuron::inConnections() const
{
    return m_inConnections;
}

const QVector<NetworkConnection*>& NetworkNeuron::outConnections() const
{
    return m_outConnections;
}

bool NetworkNeuron::isBias() const
{
    return m_isBias;
}

void NetworkNeuron::setIsBias(bool isBias)
{
    m_isBias = isBias;
}

const QVector<double>& NetworkNeuron::initialInputWeights() const
{
    return m_initialInputWeights;
}

void NetworkNeuron::setInitialInputWeights(const QVector<double>& weights)
{
    m_initialInputWeights = weights;
}

void NetworkNeuron::resetValue()
{
    m_value = 0.0;
    m_isReset = true;
    if (m_showValue)
        update();
}

void NetworkNeuron::setShowConnectionWeights(bool enabled, bool setForInputs, bool setForOutputs)
{
    if (setForInputs && m_showConnectionInputWeights != enabled) {
        for (auto* conn : qAsConst(m_inConnections))
            conn->setShowValue(enabled);
        m_showConnectionInputWeights = enabled;
    }
    if (setForOutputs && m_showConnectionOutputWeights != enabled) {
        for (auto* conn : qAsConst(m_outConnections))
            conn->setShowValue(enabled);
        m_showConnectionOutputWeights = enabled;
    }
}

bool NetworkNeuron::showValue() const
{
    return m_showValue;
}

void NetworkNeuron::setShowValue(bool enabled)
{
    if (m_showValue != enabled) {
        update();
        m_showValue = enabled;
    }
}

void NetworkNeuron::updateConnectionPaths(bool updateInputs, bool updateOutputs)
{
    if (updateInputs && m_showConnectionInputs) {
        const auto& connections = inConnections();
        for (auto* conn : connections)
            conn->updatePath();
    }
    if (updateOutputs && m_showConnectionOutputs) {
        const auto& connections = outConnections();
        for (auto* conn : connections)
            conn->updatePath();
    }
}
