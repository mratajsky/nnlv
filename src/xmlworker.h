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

#include <QFile>
#include <QObject>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "network.h"
#include "savednetwork.h"
#include "savednetworklayer.h"
#include "trainingsample.h"
#include "trainingsamplestore.h"

class XmlWorker : public QObject
{
    Q_OBJECT
public:
    explicit XmlWorker(QObject* parent = nullptr);

    QString error() const;

    enum class ReadNetworkMode {
        ReadAll,
        ReadNetworkHeaderOnly,
        ReadTrainingSamplesOnly
    };
    Q_ENUM(ReadNetworkMode)

    bool readNetwork(const QString& filePath, SavedNetwork& network, ReadNetworkMode mode);
    bool writeNetwork(const QString& filePath, const Network& network);

    bool readTrainingSamples(const QString& filePath, TrainingSampleStore& store);
    bool writeTrainingSamples(const QString& filePath, const TrainingSampleStore& store);

    bool errorOnUnknownNetwork() const;
    void setErrorOnUnknownNetwork(bool enable);

    bool schemaValidationEnabled() const;
    void setSchemaValidationEnabled(bool enabled);

private:
    bool openFile(QFile& file, QIODevice::OpenMode mode);
    bool validate(QFile& file);
    bool readSample(QXmlStreamReader& xml, TrainingSampleStore& store);

    void readXmlNetworkDetails(QXmlStreamReader& xml, SavedNetwork& network);
    void readXmlNetworkInitialWeights(QXmlStreamReader& xml, SavedNetwork& network);
    void readXmlNetworkTrainingOptions(QXmlStreamReader& xml, SavedNetwork& network);
    void readXmlNetworkLayers(QXmlStreamReader& xml, SavedNetwork& network);
    void readXmlTrainingSamples(QXmlStreamReader& xml, TrainingSampleStore& store);
    void readXmlTrainingSample(QXmlStreamReader& xml, TrainingSampleStore& store);
    void readXmlNetworkLayer(QXmlStreamReader& xml, SavedNetwork& network);
    void readXmlNetworkLayerNeurons(QXmlStreamReader& xml, SavedNetworkLayer& layer);
    void readXmlNetworkLayerNeuron(QXmlStreamReader& xml, SavedNetworkLayer& layer);
    void readXmlNetworkLayerNeuronInputWeights(QXmlStreamReader& xml, SavedNetworkNeuron& neuron);

    void writeXmlTrainingSamples(QXmlStreamWriter& xml, const QVector<TrainingSample>& samples);
    void writeXmlTrainingSample(QXmlStreamWriter& xml, const TrainingSample& sample);

    QString m_error;
    bool m_errorOnUnknownNetwork = false;
    bool m_schemaValidationEnabled = true;
};
