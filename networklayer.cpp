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
#include "networklayer.h"

NetworkLayer::NetworkLayer(NetworkLayerInfo::Type type, Network* parent) :
    NetworkLayer(NetworkLayerInfo::Map(), parent)
{
    m_infoMap[NetworkLayerInfo::Key::Type] = QVariant::fromValue(type);
}

NetworkLayer::NetworkLayer(const NetworkLayerInfo::Map& map, Network *parent) :
    QObject(parent),
    m_infoMap(map)
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

const NetworkLayerInfo::Map& NetworkLayer::infoMap() const
{
    return m_infoMap;
}

//
// Retrieve the name of the layer
//
QString NetworkLayer::name() const
{
    return m_infoMap[NetworkLayerInfo::Key::Name].toString();
}

//
// Set the name of the layer
//
void NetworkLayer::setName(const QString& name)
{
    if (this->name() != name) {
        m_infoMap[NetworkLayerInfo::Key::Name] = name;
        emit nameChanged(name);
    }
}

Network* NetworkLayer::network()
{
    auto* network = qobject_cast<Network*>(parent());
    Q_ASSERT(network != nullptr);

    return network;
}

//
// Set the generator of the initial weight which is used when neurons in this
// layer are connected to neurons in the neigbouring layer using connectTo()
//
void NetworkLayer::setInitialWeightFunction(std::function<double()>& fn)
{
    m_initialWeight = fn;
}

QString NetworkLayer::defaultNeuronName(int index) const
{
    return tr("Neuron #%1").arg(index + 1);
}

//
// Retrieve the vector of neurons in this layer
//
const QVector<NetworkNeuron*>& NetworkLayer::neurons() const
{
    return m_neurons;
}

//
// Retrieve the neuron with the given index, which must be valid
//
NetworkNeuron* NetworkLayer::neuron(int index) const
{
    return m_neurons.at(index);
}

//
// Return number of neurons in this layer
//
int NetworkLayer::neuronCount(bool skipBias) const
{
    int count = m_neurons.count();
    if (skipBias && hasBias())
        count--;

    return count;
}

//
// Retrieve a list of names of all neurons in this layer
//
QStringList NetworkLayer::neuronNameList(bool skipBias) const
{
    QStringList list;
    for (const auto* neuron : qAsConst(m_neurons)) {
        if (skipBias && neuron->isBias())
            continue;
        auto name = neuron->name();
        if (name.isEmpty())
            list << tr("Unnamed Neuron");
        else
            list << name;
    }
    return list;
}

//
// Connect every neuron in this layer to every neuron in the given
// target layer
//
void NetworkLayer::connectTo(const NetworkLayer* nextLayer)
{
    QVector<NetworkConnection*> connections;
    double min = qInf();
    double max = -qInf();

    for (int i = 0; i < m_neurons.size(); i++) {
        auto* neuron1 = m_neurons.at(i);
        for (int j = 0; j < nextLayer->neurons().size(); j++) {
            auto* neuron2 = nextLayer->neuron(j);
            //
            // Bias neurons do not have incoming weights
            //
            if (neuron2->isBias())
                continue;

            double weight;
            if (i < neuron2->initialInputWeights().size())
                weight = neuron2->initialInputWeights().at(i);
            else if (m_initialWeight)
                weight = m_initialWeight();
            else
                weight = 1;
            min = qMin(min, weight);
            max = qMax(max, weight);

            auto* conn = new NetworkConnection(neuron1, neuron2, weight, this);
            neuron1->addOutConnection(conn);
            neuron2->addInConnection(conn);

            addToGroup(conn);
            connections.append(conn);
            qDebug() << "Connected" << neuron1->name() << "to" << neuron2->name();
        }
    }
    for (auto* conn : connections)
        conn->setWeightRange(min, max);

    qDebug() << "Connected layer" << name() << "to" << nextLayer->name();
}

//
// Append neuron to the layer
//
void NetworkLayer::addNeuron(NetworkNeuron *neuron, bool setDefaultNeuronName)
{
    int index = m_neurons.size();

    m_neurons.append(neuron);
    if (setDefaultNeuronName)
        neuron->setName(defaultNeuronName(index));

    //
    // Forward neuron change notifications
    //
    connect(neuron, &NetworkNeuron::nameChanged, this,
            [this, index] {
        emit neuronNameChanged(index);
    });
    connect(neuron, &NetworkNeuron::valueChanged, this,
            [this, index] {
        emit neuronValueChanged(index);
    });
    connect(neuron, &NetworkNeuron::inWeightChanged, this,
            [this, index] {
        emit neuronWeightChanged(index);
    });
    connect(neuron, &NetworkNeuron::outWeightChanged, this,
            [this, index] {
        emit neuronWeightChanged(index);
    });
    connect(neuron, &NetworkNeuron::statusMessage, this,
            [this, index](const QString& message) {
        emit neuronStatusMessage(index, message);
    });
    addToGroup(neuron);
}

//
// Return true if the layer has a bias neuron
//
bool NetworkLayer::hasBias() const
{
    //
    // Bias unit is expect to be the first neuron in the layer
    //
    return !m_neurons.isEmpty() && m_neurons.first()->isBias();
}

void NetworkLayer::resetNeuronValues()
{
    if (hasBias()) {
        for (int i = 1; i < neuronCount(); i++)
            m_neurons.at(i)->resetValue();
    } else {
        for (int i = 0; i < neuronCount(); i++)
            m_neurons.at(i)->resetValue();
    }
}

void NetworkLayer::setDefaultNeuronNames()
{
    for (auto i = 0; i < m_neurons.size(); i++)
        m_neurons.at(i)->setName(defaultNeuronName(i));
}

QVector<double> NetworkLayer::values(bool skipBias) const
{
    QVector<double> values;

    for (auto* neuron : qAsConst(m_neurons)) {
        if (skipBias && neuron->isBias())
            continue;
        values.append(neuron->value());
    }
    return values;
}

//
// Assign values to neurons in this layer
//
// If the layer includes a bias neurons, the value vector may or may not include it.
// If bias is included, it will be skipped over.
//
void NetworkLayer::setValues(const QVector<double>& values)
{
    if (hasBias()) {
        Q_ASSERT(values.size() == neuronCount() - 1);

        for (int i = 1; i < neuronCount(); i++)
            m_neurons.at(i)->setValue(values[i - 1]);
    } else {
        Q_ASSERT(values.size() == neuronCount());

        for (int i = 0; i < neuronCount(); i++)
            m_neurons.at(i)->setValue(values[i]);
    }
}

void NetworkLayer::setInConnectionWeights(double value)
{
    if (!inConnectionWeightsSettable())
        return;

    for (int i = 0; i < m_neurons.size(); i++)
        m_neurons.at(i)->setInConnectionWeights(value);
}

void NetworkLayer::setInConnectionWeights(std::function<double()>& fn, bool updateRange)
{
    if (!inConnectionWeightsSettable())
        return;

    for (int i = 0; i < m_neurons.size(); i++)
        m_neurons.at(i)->setInConnectionWeights(fn);

    if (updateRange)
        updateInConnectionRange();
}

void NetworkLayer::setOutConnectionWeights(double value)
{
    if (!outConnectionWeightsSettable())
        return;

    for (int i = 0; i < m_neurons.size(); i++)
        m_neurons.at(i)->setOutConnectionWeights(value);
}

void NetworkLayer::setOutConnectionWeights(std::function<double()>& fn, bool updateRange)
{
    if (!outConnectionWeightsSettable())
        return;

    for (int i = 0; i < m_neurons.size(); i++)
        m_neurons.at(i)->setOutConnectionWeights(fn);

    if (updateRange)
        updateOutConnectionRange();
}

void NetworkLayer::setShowConnectionWeights(bool enabled, bool setForInputs, bool setForOutputs)
{
    const auto layerNeurons = neurons();
    for (auto* layerNeuron : layerNeurons)
        layerNeuron->setShowConnectionWeights(enabled, setForInputs, setForOutputs);
}

void NetworkLayer::setShowNeuronValues(bool enabled)
{
    const auto layerNeurons = neurons();
    for (auto* layerNeuron : layerNeurons)
        layerNeuron->setShowValue(enabled);
}

bool NetworkLayer::inConnectionWeightsSettable() const
{
    return true;
}

bool NetworkLayer::outConnectionWeightsSettable() const
{
    return true;
}

void NetworkLayer::updateInConnectionRange()
{
    QVector<NetworkConnection*> allConnections;
    double min = qInf();
    double max = -qInf();


    for (const auto* neuron : neurons()) {
        const auto inConnections = neuron->inConnections();
        for (auto* conn : inConnections) {
            min = qMin(min, conn->weight());
            max = qMax(max, conn->weight());
            allConnections.append(conn);
        }
    }
    for (auto* conn : qAsConst(allConnections))
        conn->setWeightRange(min, max);
}

void NetworkLayer::updateOutConnectionRange()
{
    QVector<NetworkConnection*> allConnections;
    double min = qInf();
    double max = -qInf();
    for (auto* neuron : neurons()) {
        const auto outConnections = neuron->outConnections();
        for (auto* conn : outConnections) {
            min = qMin(min, conn->weight());
            max = qMax(max, conn->weight());
            allConnections.append(conn);
        }
    }
    for (auto* conn : qAsConst(allConnections))
        conn->setWeightRange(min, max);
}
