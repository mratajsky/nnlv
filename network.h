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
#include <QGraphicsItemGroup>
#include <QString>
#include <QTimer>
#include <QVector>

class Network;

#include "networkinfo.h"
#include "networklayer.h"
#include "trainingtablemodel.h"
#include "trainingsample.h"

class Network : public QObject, public QGraphicsItemGroup, public NetworkInfo
{
    Q_OBJECT
public:
    enum class StopTrainingReason {
        UserRequested,
        ErrorReached,
        PercentageReached,
        SamplesReached,
        MaxEpochsReached
    };
    Q_ENUM(StopTrainingReason)

    explicit Network(QObject* parent = nullptr);
    explicit Network(const NetworkInfo::Map& map, QObject* parent = nullptr);
    virtual ~Network() {}

    NetworkLayer *inputLayer() const;
    int inputLayerIndex() const;

    NetworkLayer *outputLayer() const;
    int outputLayerIndex() const;

    QString name() const;
    void setName(QString name);

    virtual QVector<double> compute(const QVector<double>& input) const;
    virtual void computeAndSet(const QVector<double>& input);

    //
    // When reimplmenting call this method to retrieve the general defaults
    //
    virtual NetworkInfo::Map createDefaultInfoMap() const;

    void setConnectionWeights(double min, double max,
                              const QVector<int>& layers = QVector<int>(),
                              bool updateNetworkInfo = true);
    void setConnectionWeights(double value,
                              const QVector<int>& layers = QVector<int>(),
                              bool updateNetworkInfo = true);

    //
    // Training control
    //
    void startTraining();
    void stopTraining();
    void pauseTraining();
    void setTrainingSampleDelay(int delay);

    //
    // Training information and options
    //
    bool isTraining() const;
    bool isTrainingPaused() const;
    int trainingEpochs() const;

    double learningRate() const;
    void setLearningRate(double learningRate);

    int maxTrainingEpochs() const;
    void setMaxTrainingEpochs(int maxEpochs);

    int stopSamples() const;
    void setStopSamples(int samples);

    double stopPercentage() const;
    void setStopPercentage(double percentage);

    double stopError() const;
    void setStopError(double error);

    bool pauseAfterSample() const;
    void setPauseAfterSample(bool pauseAfterSample);

    NetworkInfo::SampleSelectionOrder sampleSelectionOrder() const;
    void setSampleSelectionOrder(NetworkInfo::SampleSelectionOrder sso);

    virtual void train(const TrainingSample& sample) = 0;
    virtual void updateScenePosition() = 0;

    void addLayer(NetworkLayer* layer);
    NetworkLayer* layer(int index) const;
    const QVector<NetworkLayer*>& layers();

    int layerCount() const;
    int neuronMinCount() const;
    int neuronMaxCount() const;

    const TrainingTableModel* trainingTableModel() const;
    TrainingTableModel* trainingTableModel();

    const NetworkInfo::Map& infoMap() const;

signals:
    void infoChanged();
    void nameChanged(const QString& name);
    void layerNameChanged(int index);
    void layerStatusMessage(int index, const QString& message);
    void neuronNameChanged(int layerIndex, int neuronIndex);
    void neuronValueChanged(int layerIndex, int neuronIndex);
    void neuronWeightChanged(int layerIndex, int neuronIndex);
    void neuronStatusMessage(int layerIndex, int index, const QString& message);
    void statusMessage(const QString& message);
    void trainingStarted(bool resumed);
    void trainingStopped(StopTrainingReason reason);
    void trainingPaused();
    void trainingSampleDone(const TrainingSample& sample);
    void trainingStateChanged();

protected:
    std::function<double()> createInitialWeightFunction();
    std::function<double()> createInitialWeightFunction(double weight);

    //
    // This class only verifies the MaxEpoch stop conditions.
    //
    // Other networks may implement other stop conditions, and they must put
    // it into 'reason' when they return true.
    //
    virtual bool isStopConditionReached(StopTrainingReason* reason) {
        Q_UNUSED(reason);
        return false;
    }

    virtual void prepareTraining();
    virtual void cleanupTraining();

    NetworkInfo::Map m_infoMap;

private:
    void init();
    void initTrainingOptions();
    const TrainingSample& nextSample();
    void stopTraining(StopTrainingReason reason);

    std::mt19937 m_generator;
    QVector<NetworkLayer*> m_layers;
    TrainingTableModel* m_trainingTableModel;
    QTimer* m_trainingTimer;

    //
    // Training information
    //
    bool m_training = false;
    bool m_trainingPaused = false;
    bool m_trainingPrepared = false;
    int m_trainingEpochs = 0;
    int m_trainingSampleDelay = 1000;

    //
    // Training options that have their default values set in the default map
    //
    double m_learningRate;
    NetworkInfo::SampleSelectionOrder m_sampleSelectionOrder;

    //
    // These options have their default disabled values here.
    //
    // They are not included in the default info map because they may only be used
    // by some types of networks and using them requires explicitly enabling them.
    //
    int m_maxTrainingEpochs = 0;
    int m_stopSamples = 0;
    double m_stopPercentage = 0.0;
    double m_stopError = 0.0;
    bool m_pauseAfterSample = false;
};
