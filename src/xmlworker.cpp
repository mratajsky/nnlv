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
#include "xmlworker.h"

#include <QScopedPointer>
#include <QRegExp>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

#include "kohonenoutputlayer.h"
#include "mlpactivation.h"
#include "networklimits.h"
#include "savednetwork.h"
#include "savednetworklayer.h"
#include "savednetworkneuron.h"
#include "xmlmessagehandler.h"

XmlWorker::XmlWorker(QObject* parent) :
    QObject(parent)
{
}

bool XmlWorker::validate(QFile& file)
{
    QFile schemaFile(":/resources/xmlschema.xsd");
    schemaFile.open(QIODevice::ReadOnly);

    QXmlSchema schema;
    schema.load(&schemaFile, QUrl::fromLocalFile(schemaFile.fileName()));
    if (!schema.isValid())
        qFatal("XML schema is invalid");

    QXmlSchemaValidator validator(schema);
    XmlMessageHandler messageHandler;
    validator.setMessageHandler(&messageHandler);
    bool result;
    if (validator.validate(&file, QUrl::fromLocalFile(file.fileName())))
        result = true;
    else {
        result = false;
        m_error = tr("Line %1: %2")
                  .arg(messageHandler.line())
                  .arg(messageHandler.statusMessage().remove(QRegExp("<[^>]*>")));
    }
    return result;
}

bool XmlWorker::schemaValidationEnabled() const
{
    return m_schemaValidationEnabled;
}

void XmlWorker::setSchemaValidationEnabled(bool enabled)
{
    m_schemaValidationEnabled = enabled;
}

bool XmlWorker::readNetwork(const QString& filePath, SavedNetwork& network, ReadNetworkMode mode)
{
    QFile file(filePath);
    if (!openFile(file, QIODevice::ReadOnly | QIODevice::Text))
        return false;

    if (m_schemaValidationEnabled) {
        if (!validate(file))
            return false;
        file.seek(0);
    }
    QXmlStreamReader xml(&file);
    bool ret = true;
    bool hasDetails = false;
    while (xml.readNextStartElement()) {
        if (xml.name() == "network") {
            while (xml.readNextStartElement()) {
                if (xml.name() == "network-details") {
                    readXmlNetworkDetails(xml, network);
                    hasDetails = true;
                } else if (xml.name() == "training-samples") {
                    if (!hasDetails)
                        xml.raiseError("The file does not include network information");
                    else if (mode == ReadNetworkMode::ReadAll
                            || mode == ReadNetworkMode::ReadTrainingSamplesOnly) {
                        readXmlTrainingSamples(xml, network.trainingSampleStore());
                    }
                } else
                    xml.skipCurrentElement();
                if (xml.hasError())
                    break;
            }
        } else
            xml.raiseError("Invalid root element");
        if (xml.hasError()) {
            ret = false;
            m_error = xml.errorString();
            break;
        }
    }

    if (ret == true) {
        if (!hasDetails) {
            m_error = QStringLiteral("The file does not include network information");
            ret = false;
        } else if (!network.verify()) {
            m_error = network.verifyError();
            ret = false;
        } else if (network.infoType() == NetworkInfo::Type::Unknown
                   && m_errorOnUnknownNetwork) {
            m_error = QStringLiteral("Unknown network type");
            ret = false;
        }
    }
    file.close();
    return ret;
}

bool XmlWorker::readTrainingSamples(const QString& filePath, TrainingSampleStore& store)
{
    QFile file(filePath);
    if (!openFile(file, QIODevice::ReadOnly | QIODevice::Text))
        return false;

    if (m_schemaValidationEnabled) {
        if (!validate(file))
            return false;
        file.seek(0);
    }
    QXmlStreamReader xml(&file);
    bool ret = true;
    while (xml.readNextStartElement()) {
        if (xml.name() == "network") {
            while (xml.readNextStartElement()) {
                if (xml.name() == "training-samples") {
                    readXmlTrainingSamples(xml, store);
                } else
                    xml.skipCurrentElement();
                if (xml.hasError())
                    break;
            }
        } else
            xml.raiseError("Invalid root element");
        if (xml.hasError()) {
            ret = false;
            m_error = xml.errorString();
            break;
        }
    }
    file.close();
    return ret;
}

void XmlWorker::readXmlNetworkDetails(QXmlStreamReader& xml, SavedNetwork& network)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "network-details");

    auto& map = network.infoMap();
    while (xml.readNextStartElement()) {
        if (xml.name() == "type") {
            //
            // <type>
            // The type will be set to "Unknown" in case of conversion error, but
            // deciding whether unknown type is an error is left to the caller
            //
            map[NetworkInfo::Key::Type] =
                    QVariant::fromValue(NetworkInfo::typeFromString(xml.readElementText()));
        } else if (xml.name() == "name") {
            //
            // <name>
            //
            map[NetworkInfo::Key::Name] =
                    xml.readElementText().simplified().left(NetworkLimits::maxNetworkNameLength);
        } else if (xml.name() == "description") {
            //
            // <description>
            //
            map[NetworkInfo::Key::Description] = xml.readElementText();
        } else if (xml.name() == "initial-weights") {
            //
            // <initial-weights>
            //
            readXmlNetworkInitialWeights(xml, network);
        } else if (xml.name() == "training-options") {
            //
            // <training-options>
            //
            readXmlNetworkTrainingOptions(xml, network);
        } else if (xml.name() == "layers") {
            //
            // <layers>
            //
            readXmlNetworkLayers(xml, network);
        } else
            xml.skipCurrentElement();
        if (xml.hasError())
            return;
    }
    //
    // Find out the number of inputs and outputs that will be used for training.
    //
    // It is necessary to set these in the TrainingSampleStore before training
    // samples are read as these numbers will be used for validating the samples.
    //
    int inputCount = 0;
    int outputCount = 0;
    QVector<int> hiddenCountList;
    for (const auto* layer : network.savedLayers()) {
        if (layer->infoType() == NetworkLayerInfo::Type::Input)
            inputCount = layer->savedNeurons().size();
        else if (layer->infoType() == NetworkLayerInfo::Type::Hidden)
            hiddenCountList.append(layer->savedNeurons().size());
        else if (layer->infoType() == NetworkLayerInfo::Type::Output)
            outputCount = layer->savedNeurons().size();
    }
    if (network.isSupervised())
        network.trainingSampleStore().setFieldCount(inputCount, outputCount);
    else
        network.trainingSampleStore().setFieldCount(inputCount, 0);

    map[NetworkInfo::Key::InputNeuronCount] = inputCount;
    map[NetworkInfo::Key::OutputNeuronCount] = outputCount;
    map[NetworkInfo::Key::HiddenNeuronCount] = QVariant::fromValue(hiddenCountList);
}

void XmlWorker::readXmlNetworkInitialWeights(QXmlStreamReader& xml, SavedNetwork& network)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "initial-weights");

    auto& map = network.infoMap();
    while (xml.readNextStartElement()) {
        if (xml.name() == "random-min") {
            //
            // <random-min>
            //
            map[NetworkInfo::Key::InitialWeightsRandomMin] = xml.readElementText().toDouble();
        } else if (xml.name() == "random-max") {
            //
            // <random-max>
            //
            map[NetworkInfo::Key::InitialWeightsRandomMax] = xml.readElementText().toDouble();
        } else if (xml.name() == "value") {
            //
            // <value>
            //
            map[NetworkInfo::Key::InitialWeightsValue] = xml.readElementText().toDouble();
        } else
            xml.skipCurrentElement();
        if (xml.hasError())
            break;
    }
}

void XmlWorker::readXmlNetworkTrainingOptions(QXmlStreamReader& xml, SavedNetwork& network)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "training-options");

    auto& map = network.infoMap();
    while (xml.readNextStartElement()) {
        if (xml.name() == "activation-function") {
            //
            // <activation-function>
            //
            auto value = MLPActivation::functionFromString(xml.readElementText());
            if (value == MLPActivation::Function::Unknown) {
                xml.raiseError("Invalid <activation-function> value");
                break;
            }
            map[NetworkInfo::Key::ActivationFunction] = QVariant::fromValue(value);
        } else if (xml.name() == "learning-rate") {
            //
            // <learning-rate>
            //
            map[NetworkInfo::Key::LearningRate] = xml.readElementText().toDouble();
        } else if (xml.name() == "max-epochs") {
            //
            // <max-epochs>
            //
            map[NetworkInfo::Key::MaxEpochs] = xml.readElementText().toInt();
        } else if (xml.name() == "pause-after-sample") {
            //
            // <pause-after-sample>
            //
            QString value = xml.readElementText().toLower();
            if (value == QStringLiteral("1")
                    || value == QStringLiteral("true"))
                map[NetworkInfo::Key::PauseAfterSample] = QVariant::fromValue(true);
            else if (value == QStringLiteral("0")
                     || value == QStringLiteral("false"))
                map[NetworkInfo::Key::PauseAfterSample] = QVariant::fromValue(false);
        } else if (xml.name() == "sample-selection-order") {
            //
            // <sample-selection-order>
            //
            auto value = NetworkInfo::sampleSelectionOrderFromString(xml.readElementText());
            if (value == NetworkInfo::SampleSelectionOrder::Unknown) {
                xml.raiseError("Invalid <sample-selection-order> value");
                break;
            }
            map[NetworkInfo::Key::SampleSelectionOrder] = QVariant::fromValue(value);
        } else if (xml.name() == "stop-error") {
            //
            // <stop-error>
            //
            map[NetworkInfo::Key::StopError] = xml.readElementText().toDouble();
        } else if (xml.name() == "stop-percentage") {
            //
            // <stop-percentage>
            //
            map[NetworkInfo::Key::StopPercentage] = xml.readElementText().toDouble();
        } else if (xml.name() == "stop-samples") {
            //
            // <stop-samples>
            //
            map[NetworkInfo::Key::StopSamples] = xml.readElementText().toInt();
        } else if (xml.name() == "train-rbf-layer") {
            //
            // <train-rbf-layer>
            //
            QString value = xml.readElementText().toLower();
            if (value == QStringLiteral("1")
                    || value == QStringLiteral("true"))
                map[NetworkInfo::Key::TrainRBFLayer] = QVariant::fromValue(true);
            else if (value == QStringLiteral("0")
                     || value == QStringLiteral("false"))
                map[NetworkInfo::Key::TrainRBFLayer] = QVariant::fromValue(false);
        } else
            xml.skipCurrentElement();
        if (xml.hasError())
            break;
    }
}

void XmlWorker::readXmlNetworkLayers(QXmlStreamReader& xml, SavedNetwork& network)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "layers");

    while (xml.readNextStartElement()) {
        if (xml.name() == "layer") {
            readXmlNetworkLayer(xml, network);
        } else
            xml.skipCurrentElement();
        if (xml.hasError())
            break;
    }
    if (xml.hasError())
        return;
}

void XmlWorker::readXmlNetworkLayer(QXmlStreamReader& xml, SavedNetwork& network)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "layer");

    QScopedPointer<SavedNetworkLayer> layer(new SavedNetworkLayer(&network));

    auto& map = layer->infoMap();
    while (xml.readNextStartElement()) {
        if (xml.name() == "type") {
            //
            // <type>
            // The type will be set to "Unknown" in case of conversion error
            //
            map[NetworkLayerInfo::Key::Type] =
                    QVariant::fromValue(NetworkLayerInfo::typeFromString(xml.readElementText()));
        } else if (xml.name() == "name") {
            //
            // <name>
            //
            map[NetworkLayerInfo::Key::Name] =
                    xml.readElementText().simplified().left(NetworkLimits::maxLayerNameLength);
        } else if (xml.name() == "neurons") {
            //
            // <neurons>
            //
            readXmlNetworkLayerNeurons(xml, *layer);
        } else
            xml.skipCurrentElement();
        if (xml.hasError())
            break;
    }
    if (xml.hasError())
        return;
    if (!layer->verify()) {
        xml.raiseError(layer->verifyError());
        return;
    }
    network.savedLayers().append(layer.take());
}

void XmlWorker::readXmlNetworkLayerNeurons(QXmlStreamReader& xml, SavedNetworkLayer& layer)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "neurons");

    while (xml.readNextStartElement()) {
        if (xml.name() == "neuron") {
            readXmlNetworkLayerNeuron(xml, layer);
        } else
            xml.skipCurrentElement();
        if (xml.hasError())
            break;
    }
}

void XmlWorker::readXmlNetworkLayerNeuron(QXmlStreamReader& xml, SavedNetworkLayer& layer)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "neuron");

    QScopedPointer<SavedNetworkNeuron> neuron(new SavedNetworkNeuron(&layer));

    auto& map = neuron->infoMap();
    while (xml.readNextStartElement()) {
        if (xml.name() == "name") {
            //
            // <name>
            //
            map[NetworkNeuronInfo::Key::Name] =
                    xml.readElementText().simplified().left(NetworkLimits::maxNeuronNameLength);
        } else if (xml.name() == "position-x") {
            //
            // <position-x>
            //
            map[NetworkNeuronInfo::Key::PositionX] = xml.readElementText().toInt();
        } else if (xml.name() == "position-y") {
            //
            // <position-y>
            //
            map[NetworkNeuronInfo::Key::PositionY] = xml.readElementText().toInt();
        } else if (xml.name() == "sigma") {
            //
            // <sigma>
            //
            map[NetworkNeuronInfo::Key::Sigma] = xml.readElementText().toDouble();
        } else if (xml.name() == "input-weights") {
            //
            // <input-weights>
            //
            readXmlNetworkLayerNeuronInputWeights(xml, *neuron);
        } else
            xml.skipCurrentElement();
        if (xml.hasError())
            break;
    }
    if (xml.hasError())
        return;

    layer.savedNeurons().append(neuron.take());
}

void XmlWorker::readXmlNetworkLayerNeuronInputWeights(QXmlStreamReader& xml, SavedNetworkNeuron& neuron)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "input-weights");

    QVector<double> weights;
    while (xml.readNextStartElement()) {
        if (xml.name() == "weight")
            weights.append(xml.readElementText().toDouble());
        else
            xml.skipCurrentElement();
        if (xml.hasError())
            return;
    }
    neuron.setInputWeights(weights);
}

void XmlWorker::readXmlTrainingSamples(QXmlStreamReader& xml, TrainingSampleStore& store)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "training-samples");

    while (xml.readNextStartElement()) {
        if (xml.name() == "sample") {
            readXmlTrainingSample(xml, store);
            if (xml.hasError())
                return;
        } else
            xml.skipCurrentElement();
    }
}

void XmlWorker::readXmlTrainingSample(QXmlStreamReader& xml, TrainingSampleStore& store)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "sample");

    QStringList fields;
    while (xml.readNextStartElement()) {
        if (xml.name() == "value")
            fields << xml.readElementText();
        else
            xml.skipCurrentElement();
        if (xml.hasError())
            return;
    }
    if (!store.addSample(fields))
        xml.raiseError(store.error());
}

QString XmlWorker::error() const
{
    return m_error;
}

bool XmlWorker::openFile(QFile& file, QIODevice::OpenMode mode)
{
    if (!file.open(mode)) {
        m_error = file.errorString();
        return false;
    }
    return true;
}

bool XmlWorker::writeNetwork(const QString& filePath, const Network& network)
{
    QFile file(filePath);
    if (!openFile(file, QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();

    const auto& map = network.infoMap();

    xml.writeStartElement("network");
    xml.writeStartElement("network-details");
    xml.writeTextElement("type", network.infoTypeAsString());
    xml.writeTextElement("name", network.name());
    if (map.contains(NetworkInfo::Key::Description))
        xml.writeTextElement("description", map.value(NetworkInfo::Key::Description).toString());
    //
    // <initial-weights>
    //
    xml.writeStartElement("initial-weights");
    if (map.contains(NetworkInfo::Key::InitialWeightsRandomMin))
        xml.writeTextElement("random-min", map.value(NetworkInfo::Key::InitialWeightsRandomMin).toString());
    if (map.contains(NetworkInfo::Key::InitialWeightsRandomMax))
        xml.writeTextElement("random-max", map.value(NetworkInfo::Key::InitialWeightsRandomMax).toString());
    if (map.contains(NetworkInfo::Key::InitialWeightsValue))
        xml.writeTextElement("value", map.value(NetworkInfo::Key::InitialWeightsValue).toString());

    xml.writeEndElement(); // </initial-weights>
    //
    // <training-options>
    //
    xml.writeStartElement("training-options");
    if (map.contains(NetworkInfo::Key::ActivationFunction))
        xml.writeTextElement("activation-function",
                             MLPActivation::functionToString(MLPActivation::functionFromMap(map)));

    if (map.contains(NetworkInfo::Key::LearningRate))
        xml.writeTextElement("learning-rate", map.value(NetworkInfo::Key::LearningRate).toString());
    if (map.contains(NetworkInfo::Key::MaxEpochs))
        xml.writeTextElement("max-epochs", map.value(NetworkInfo::Key::MaxEpochs).toString());
    if (map.contains(NetworkInfo::Key::PauseAfterSample))
        xml.writeTextElement("pause-after-sample", map.value(NetworkInfo::Key::PauseAfterSample).toString());
    if (map.contains(NetworkInfo::Key::SampleSelectionOrder))
        xml.writeTextElement("sample-selection-order",
                             NetworkInfo::sampleSelectionOrderToString(
                                 NetworkInfo::sampleSelectionOrderFromMap(map)));

    if (map.contains(NetworkInfo::Key::StopError))
        xml.writeTextElement("stop-error", map.value(NetworkInfo::Key::StopError).toString());
    if (map.contains(NetworkInfo::Key::StopPercentage))
        xml.writeTextElement("stop-percentage", map.value(NetworkInfo::Key::StopPercentage).toString());
    if (map.contains(NetworkInfo::Key::StopSamples))
        xml.writeTextElement("stop-samples", map.value(NetworkInfo::Key::StopSamples).toString());

    if (map.contains(NetworkInfo::Key::TrainRBFLayer))
        xml.writeTextElement("train-rbf-layer", map.value(NetworkInfo::Key::TrainRBFLayer).toString());

    xml.writeEndElement(); // </training-options>

    if (network.layerCount() > 0) {
        xml.writeStartElement("layers");
        for (int i = 0; i < network.layerCount(); i++) {
            const auto* layer = network.layer(i);
            //
            // <layer>
            //
            xml.writeStartElement("layer");
            xml.writeTextElement("type", layer->infoTypeAsString());
            xml.writeTextElement("name", layer->name());

            if (layer->neuronCount() > 0) {
                //
                // <neurons>
                //
                xml.writeStartElement("neurons");

                for (int j = 0; j < layer->neuronCount(); j++) {
                    const auto* neuron = layer->neuron(j);
                    //
                    // Bias is included in the layer automatically and is not
                    // written to an XML file
                    //
                    if (neuron->isBias())
                        continue;
                    const auto& neuronMap = neuron->infoMap();
                    //
                    // <neuron>
                    //
                    xml.writeStartElement("neuron");
                    xml.writeTextElement("name", neuron->name());
                    if (neuronMap.contains(NetworkNeuronInfo::Key::PositionX))
                        xml.writeTextElement("position-x",
                                             neuronMap.value(NetworkNeuronInfo::Key::PositionX).toString());
                    if (neuronMap.contains(NetworkNeuronInfo::Key::PositionY))
                        xml.writeTextElement("position-y",
                                             neuronMap.value(NetworkNeuronInfo::Key::PositionY).toString());
                    if (neuronMap.contains(NetworkNeuronInfo::Key::Sigma))
                        xml.writeTextElement("sigma",
                                             neuronMap.value(NetworkNeuronInfo::Key::Sigma).toString());

                    auto inputs = neuron->inConnections();
                    if (inputs.size() > 0) {
                        //
                        // <input-weights>
                        //
                        xml.writeStartElement("input-weights");

                        for (int k = 0; k < inputs.size(); k++) {
                            const auto* conn = neuron->inConnection(k);
                            xml.writeTextElement("weight", QString::number(conn->weight()));
                        }
                        xml.writeEndElement(); // </input-weights>
                    }
                    xml.writeEndElement(); // </neuron>
                }
                xml.writeEndElement(); // </neurons>
            }
            xml.writeEndElement(); // </layer>
        }
        xml.writeEndElement(); // </layers>
    }
    xml.writeEndElement(); // </network-details>

    //
    // Write training samples
    //
    const auto& samples = network.trainingTableModel()->store().samples();
    if (samples.size() > 0)
        writeXmlTrainingSamples(xml, samples);

    xml.writeEndElement(); // </network>
    xml.writeEndDocument();
    file.close();
    return true;
}

bool XmlWorker::writeTrainingSamples(const QString& filePath, const TrainingSampleStore& store)
{
    QFile file(filePath);
    if (!openFile(file, QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();

    xml.writeStartElement("network");
    writeXmlTrainingSamples(xml, store.samples());
    xml.writeEndElement(); // </network>

    xml.writeEndDocument();
    file.close();
    return true;
}

bool XmlWorker::errorOnUnknownNetwork() const
{
    return m_errorOnUnknownNetwork;
}

void XmlWorker::setErrorOnUnknownNetwork(bool enable)
{
    m_errorOnUnknownNetwork = enable;
}

void XmlWorker::writeXmlTrainingSamples(QXmlStreamWriter& xml, const QVector<TrainingSample>& samples)
{
    xml.writeStartElement("training-samples");
    //
    // Write the samples
    //
    for (const auto& sample : samples)
        writeXmlTrainingSample(xml, sample);

    xml.writeEndElement();
}

void XmlWorker::writeXmlTrainingSample(QXmlStreamWriter& xml, const TrainingSample& sample)
{
    xml.writeStartElement("sample");

    for (int i = 0; i < sample.fieldCount(); i++)
        xml.writeTextElement("value", QString::number(sample.field(i)));

    xml.writeEndElement();
}
