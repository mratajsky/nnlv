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
#include "network.h"

#include "networkdefaults.h"

Network::Network(QObject* parent) :
    Network(NetworkInfo::Map(), parent)
{
}

Network::Network(const NetworkInfo::Map& map, QObject* parent) :
    QObject(parent),
    m_infoMap(map),
    m_generator(std::random_device{}()),
    m_trainingTableModel(new TrainingTableModel(this)),
    m_trainingTimer(new QTimer(this))
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    init();
    initTrainingOptions();
}

const TrainingSample& Network::nextSample()
{
    switch (m_sampleSelectionOrder) {
        case SampleSelectionOrder::InOrder:
            return m_trainingTableModel->currentSample();
        case SampleSelectionOrder::Random:
            return m_trainingTableModel->randomSample();
        default:
            Q_UNREACHABLE();
            break;
    }
}

void Network::init()
{
    auto defaultMap = createDefaultInfoMap();
    //
    // Add default values to the map
    //
    QMapIterator<NetworkInfo::Key, QVariant> i(defaultMap);
    while (i.hasNext()) {
        i.next();
        if (!m_infoMap.contains(i.key()))
            m_infoMap.insert(i.key(), i.value());
    }

    connect(m_trainingTimer, &QTimer::timeout, this, [this] {
        if (m_trainingTimer->isSingleShot()) {
            Q_ASSERT(m_trainingSampleDelay > 0);
            //
            // Readjust the timer to the real interval after the first sample
            //
            m_trainingTimer->setInterval(m_trainingSampleDelay);
            m_trainingTimer->setSingleShot(false);
            if (!m_trainingPrepared) {
                prepareTraining();
                m_trainingPrepared = true;
                // The preparation function might have stopped or
                // paused the training
                if (!isTraining())
                    return;
            }
            m_trainingTimer->start();
        }
        const auto& sample = nextSample();
        train(sample);

        m_trainingEpochs++;
        emit trainingSampleDone(sample);
        emit trainingStateChanged();
        //
        // The training sample is done at this point, verify whether we
        // should stop the training
        //
        if (m_maxTrainingEpochs == m_trainingEpochs)
            stopTraining(StopTrainingReason::MaxEpochsReached);
        else {
            StopTrainingReason reason;
            if (isStopConditionReached(&reason))
                stopTraining(reason);
        }
        // Pause training if requested
        if (m_training && m_pauseAfterSample)
            pauseTraining();
    });
}

//
// Copy training options from the infoMap to instance variables to make
// them accessible without QMap lookups
//
void Network::initTrainingOptions()
{
    if (m_infoMap.contains(NetworkInfo::Key::LearningRate))
        m_learningRate = m_infoMap[NetworkInfo::Key::LearningRate].toDouble();

    if (m_infoMap.contains(NetworkInfo::Key::MaxEpochs))
        m_maxTrainingEpochs = m_infoMap[NetworkInfo::Key::MaxEpochs].toInt();

    if (m_infoMap.contains(NetworkInfo::Key::PauseAfterSample))
        m_pauseAfterSample = m_infoMap[NetworkInfo::Key::PauseAfterSample].toBool();

    if (m_infoMap.contains(NetworkInfo::Key::SampleSelectionOrder))
        m_sampleSelectionOrder = sampleSelectionOrderFromMap(m_infoMap);

    if (m_infoMap.contains(NetworkInfo::Key::StopError))
        m_stopError = m_infoMap[NetworkInfo::Key::StopError].toDouble();

    if (m_infoMap.contains(NetworkInfo::Key::StopPercentage))
        m_stopPercentage = m_infoMap[NetworkInfo::Key::StopPercentage].toDouble();

    if (m_infoMap.contains(NetworkInfo::Key::StopSamples))
        m_stopSamples = m_infoMap[NetworkInfo::Key::StopSamples].toDouble();
}

//
// Return the input layer or nullptr if there are no layers in the network
//
NetworkLayer* Network::inputLayer() const
{
    int index = inputLayerIndex();
    if (index >= 0)
        return m_layers.at(index);
    return nullptr;
}

int Network::inputLayerIndex() const
{
    //
    // If the input layer exists it is expected to be the first layer
    //
    if (!m_layers.isEmpty()) {
        const auto* layer = m_layers.first();
        if (layer->infoType() == NetworkLayerInfo::Type::Input)
            return 0;
    }
    return -1;
}

//
// Return the output layer or nullptr if the network does not have an output layer
//
NetworkLayer* Network::outputLayer() const
{
    int index = outputLayerIndex();
    if (index >= 0)
        return m_layers.at(index);
    return nullptr;
}

int Network::outputLayerIndex() const
{
    //
    // If the output layer exists it is expected to be the last layer
    //
    if (!m_layers.isEmpty()) {
        const auto* layer = m_layers.last();
        if (layer->infoType() == NetworkLayerInfo::Type::Output)
            return m_layers.size() - 1;
    }
    return -1;
}

QString Network::name() const
{
    return m_infoMap[NetworkInfo::Key::Name].toString();
}

void Network::setName(QString name)
{
    if (this->name() != name) {
        m_infoMap[NetworkInfo::Key::Name] = name;
        emit infoChanged();
        emit nameChanged(name);
    }
}

QVector<double> Network::compute(const QVector<double>& input) const
{
    Q_UNUSED(input);
    return QVector<double>();
}

void Network::computeAndSet(const QVector<double>& input)
{
    Q_UNUSED(input);
}

//
// Create a function which returns an initial connection weight
//
std::function<double()> Network::createInitialWeightFunction()
{
    double weight = qInf();
    if (m_infoMap.contains(NetworkInfo::Key::InitialWeightsValue))
        weight = m_infoMap[NetworkInfo::Key::InitialWeightsValue].toDouble();

    return createInitialWeightFunction(weight);
}

//
// Create a function which returns an initial connection weight.
//
// If the passed number is finite (not qInf() or qNaN()), this number is used for
// every initial weight, otherwise a random number is used.
//
std::function<double()> Network::createInitialWeightFunction(double weight)
{
    if (qIsFinite(weight))
        return [=]() { return weight; };

    const auto& map = infoMap();
    //
    // Use bounds from the info map
    //
    double min = -1; // default
    if (map.contains(NetworkInfo::Key::InitialWeightsRandomMin))
        min = map[NetworkInfo::Key::InitialWeightsRandomMin].toDouble();
    double max = 1;  // default
    if (map.contains(NetworkInfo::Key::InitialWeightsRandomMax))
        max = map[NetworkInfo::Key::InitialWeightsRandomMax].toDouble();
    if (min > max)
        min = max;

    std::uniform_real_distribution<double> dist(min, max);

    return std::bind(dist, std::ref(m_generator));
}

NetworkInfo::Map Network::createDefaultInfoMap() const
{
    NetworkInfo::Map map;

    map[NetworkInfo::Key::LearningRate] = NetworkDefaults::larningRate;
    map[NetworkInfo::Key::SampleSelectionOrder] =
            QVariant::fromValue(NetworkDefaults::sampleSelectionOrder);

    return map;
}

void Network::prepareTraining()
{
}

void Network::cleanupTraining()
{
}

//
// Set connection weights in the whole network to random ones with the given bounds
//
void Network::setConnectionWeights(double min, double max, const QVector<int>& layers, bool updateNetworkInfo)
{
    std::uniform_real_distribution<double> dist(min, max);
    std::function<double()> rnd = std::bind(dist, std::ref(m_generator));

    for (int i = 0; i < m_layers.size(); i++) {
        if (layers.isEmpty() || layers.contains(i))
            m_layers[i]->setOutConnectionWeights(rnd);
    }
    if (updateNetworkInfo) {
        m_infoMap.remove(NetworkInfo::Key::InitialWeightsValue);
        m_infoMap[NetworkInfo::Key::InitialWeightsRandomMin] = min;
        m_infoMap[NetworkInfo::Key::InitialWeightsRandomMax] = max;
        emit infoChanged();
    }
    qDebug() << "Changed all network weights to random in range" << min << max;
}

//
// Set connection weights in the whole network to the given value
//
void Network::setConnectionWeights(double value, const QVector<int>& layers, bool updateNetworkInfo)
{
    for (int i = 0; i < m_layers.size(); i++) {
        if (layers.isEmpty() || layers.contains(i))
            m_layers[i]->setOutConnectionWeights(value);
    }
    if (updateNetworkInfo) {
        m_infoMap.remove(NetworkInfo::Key::InitialWeightsRandomMin);
        m_infoMap.remove(NetworkInfo::Key::InitialWeightsRandomMax);
        m_infoMap[NetworkInfo::Key::InitialWeightsValue] = value;
        emit infoChanged();
    }
    qDebug() << "Changed all network weights to" << value;
}

//
// Add a layer to the network.
//
// It is required that the first layer is an input layer and if there is an output layer
// to be added to the network, it must be the last layer.
//
void Network::addLayer(NetworkLayer* layer)
{
    //
    // Update training table columns when input or output layer (in case of supervised
    // learning) is added
    //
    if (layer->infoType() == NetworkLayerInfo::Type::Input) {
        Q_ASSERT(m_layers.isEmpty());
        m_trainingTableModel->setInputs(layer->neuronNameList(true));
    } else {
        Q_ASSERT(!m_layers.isEmpty());
        Q_ASSERT(m_layers.last()->infoType() != NetworkLayerInfo::Type::Output);

        if (layer->infoType() == NetworkLayerInfo::Type::Output) {
            if (isSupervised())
                m_trainingTableModel->setOutputs(layer->neuronNameList(true));
            else
                m_trainingTableModel->setOutputs(QStringList());
        }
    }

    int index = m_layers.size();

    //
    // Forward layer and neuron change notifications
    //
    connect(layer, &NetworkLayer::nameChanged, this,
            [this, index] {
        emit layerNameChanged(index);
    });
    connect(layer, &NetworkLayer::statusMessage, this,
            [this, index](const QString& message) {
        emit layerStatusMessage(index, message);
    });
    connect(layer, &NetworkLayer::neuronNameChanged, this,
            [this, index](int neuronIndex) {
        emit neuronNameChanged(index, neuronIndex);
    });
    connect(layer, &NetworkLayer::neuronValueChanged, this,
            [this, index](int neuronIndex) {
        emit neuronValueChanged(index, neuronIndex);
    });
    connect(layer, &NetworkLayer::neuronWeightChanged, this,
            [this, index](int neuronIndex) {
        emit neuronWeightChanged(index, neuronIndex);
    });
    connect(layer, &NetworkLayer::neuronStatusMessage, this,
            [this, index](int neuronIndex, const QString& message) {
        emit neuronStatusMessage(index, neuronIndex, message);
    });

    m_layers.append(layer);
    addToGroup(layer);
}

NetworkLayer* Network::layer(int index) const
{
    return m_layers.at(index);
}

const QVector<NetworkLayer*>& Network::layers()
{
    return m_layers;
}

int Network::layerCount() const
{
    return m_layers.size();
}

int Network::neuronMinCount() const
{
    if (m_layers.isEmpty())
        return 0;

    int count = 999;
    for (const auto* layer : qAsConst(m_layers))
        count = qMin(count, layer->neuronCount());
    return count;
}

int Network::neuronMaxCount() const
{
    int count = 0;
    for (const auto* layer : qAsConst(m_layers))
        count = qMax(count, layer->neuronCount());
    return count;
}

bool Network::isTraining() const
{
    return m_training;
}

bool Network::isTrainingPaused() const
{
    return m_trainingPaused;
}

void Network::setTrainingSampleDelay(int delay)
{
    Q_ASSERT(delay > 0);

    m_trainingSampleDelay = delay;
    if (m_trainingTimer->isActive())
        m_trainingTimer->setInterval(delay);
}

void Network::startTraining()
{
    bool paused = m_trainingPaused;
    if (m_trainingPaused)
        m_trainingPaused = false;
    else {
        m_trainingEpochs = 0;
        m_trainingPrepared = false;
    }
    //
    // Initially set the timer to single-shot with 0 interval to make
    // it fire as soon as possible to process the first sample, after
    // the first sample it will be readjusted to the real interval.
    //
    m_trainingTimer->setInterval(0);
    m_trainingTimer->setSingleShot(true);
    m_trainingTimer->start();
    m_training = true;

    emit trainingStarted(paused);
    emit trainingStateChanged();
}

void Network::stopTraining()
{
    stopTraining(StopTrainingReason::UserRequested);
}

void Network::stopTraining(StopTrainingReason reason)
{
    m_trainingTimer->stop();
    m_training = false;
    m_trainingPaused = false;
    emit trainingStopped(reason);
    emit trainingStateChanged();
    cleanupTraining();
}

void Network::pauseTraining()
{
    m_trainingTimer->stop();
    m_training = false;
    m_trainingPaused = true;
    emit trainingPaused();
    emit trainingStateChanged();
}

int Network::stopSamples() const
{
    return m_stopSamples;
}

double Network::stopPercentage() const
{
    return m_stopPercentage;
}

double Network::stopError() const
{
    return m_stopError;
}

double Network::learningRate() const
{
    return m_learningRate;
}

void Network::setLearningRate(double learningRate)
{
    Q_ASSERT((learningRate > 0.0 && learningRate < 1.0) || qFuzzyCompare(learningRate, 1.0));

    if (!qFuzzyCompare(learningRate, m_learningRate)) {
        m_learningRate = learningRate;
        m_infoMap[NetworkInfo::Key::LearningRate] = learningRate;
        emit infoChanged();
    }
}

int Network::trainingEpochs() const
{
    return m_trainingEpochs;
}

int Network::maxTrainingEpochs() const
{
    return m_maxTrainingEpochs;
}

void Network::setMaxTrainingEpochs(int maxEpochs)
{
    Q_ASSERT(maxEpochs >= 0);

    if (m_maxTrainingEpochs != maxEpochs) {
        m_maxTrainingEpochs = maxEpochs;
        if (maxEpochs > 0)
            m_infoMap[NetworkInfo::Key::MaxEpochs] = maxEpochs;
        else
            m_infoMap.remove(NetworkInfo::Key::MaxEpochs);
        emit infoChanged();
    }
}

const TrainingTableModel* Network::trainingTableModel() const
{
    return m_trainingTableModel;
}

TrainingTableModel* Network::trainingTableModel()
{
    return m_trainingTableModel;
}

bool Network::pauseAfterSample() const
{
    return m_pauseAfterSample;
}

void Network::setPauseAfterSample(bool pauseAfterSample)
{
    if (m_pauseAfterSample != pauseAfterSample) {
        m_pauseAfterSample = pauseAfterSample;
        m_infoMap[NetworkInfo::Key::PauseAfterSample] = pauseAfterSample;
        emit infoChanged();
    }
}

NetworkInfo::SampleSelectionOrder Network::sampleSelectionOrder() const
{
    return m_sampleSelectionOrder;
}

void Network::setSampleSelectionOrder(NetworkInfo::SampleSelectionOrder sso)
{
    if (m_sampleSelectionOrder != sso) {
        m_sampleSelectionOrder = sso;
        m_infoMap[NetworkInfo::Key::SampleSelectionOrder] = QVariant::fromValue(sso);
        emit infoChanged();
    }
}

void Network::setStopPercentage(double percentage)
{
    Q_ASSERT(percentage > 0 || qFuzzyIsNull(percentage));

    if (!qFuzzyCompare(percentage, m_stopPercentage)) {
        m_stopPercentage = percentage;
        if (percentage > 0)
            m_infoMap[NetworkInfo::Key::StopPercentage] = percentage;
        else
            m_infoMap.remove(NetworkInfo::Key::StopPercentage);
        emit infoChanged();
    }
}

void Network::setStopSamples(int samples)
{
    Q_ASSERT(samples >= 0);

    if (m_stopSamples != samples) {
        m_stopSamples = samples;
        if (samples > 0)
            m_infoMap[NetworkInfo::Key::StopSamples] = samples;
        else
            m_infoMap.remove(NetworkInfo::Key::StopSamples);
        emit infoChanged();
    }
}

void Network::setStopError(double error)
{
    if (!qFuzzyCompare(error, m_stopError)) {
        m_stopError = error;
        if (!qFuzzyIsNull(error))
            m_infoMap[NetworkInfo::Key::StopError] = error;
        else
            m_infoMap.remove(NetworkInfo::Key::StopError);
        emit infoChanged();
    }
}

const NetworkInfo::Map& Network::infoMap() const
{
    return m_infoMap;
}
