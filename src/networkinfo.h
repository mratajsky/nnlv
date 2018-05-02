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

#include <QMap>
#include <QObject>
#include <QString>
#include <QVariant>

class NetworkInfo {
    Q_GADGET
public:
    virtual ~NetworkInfo() {}

    enum class Key {
        Type,
        Name,
        Description,
        InputNeuronCount,
        OutputNeuronCount,
        HiddenNeuronCount,
        //
        // Initial weight given to newly created connections.
        //
        // Use Inf or NaN to indicate that a random value in the range of
        // (InitialWeightsRandomMin, InitialWeightsRandomMax) should be used.
        //
        InitialWeightsValue,
        InitialWeightsRandomMin,
        InitialWeightsRandomMax,
        //
        // Training options
        //
        ActivationFunction,     // Uses MLPActivation::Function
        LearningRate,
        MaxEpochs,
        PauseAfterSample,
        SampleSelectionOrder,   // Uses NetworkInfo::SampleSelectionOrder
        StopError,
        StopPercentage,
        StopSamples,
        //
        // Network-specific training options
        //
        TrainRBFLayer
    };
    Q_ENUM(Key)

    enum class Type {
        //
        // The "Unknown" network type is used as an error indicator
        //
        Unknown,
        SLP,
        Adaline,
        MLP,
        Kohonen,
        RBF
    };
    Q_ENUM(Type)

    enum class ErrorType {
        //
        // The "Unknown" order is used as an error indicator
        //
        Unknown,
        SSE,
        MSE,
        RMSE
    };
    Q_ENUM(ErrorType)

    enum class SampleSelectionOrder {
        //
        // The "Unknown" order is used as an error indicator
        //
        Unknown,
        Random,
        InOrder
    };
    Q_ENUM(SampleSelectionOrder)

    using Map = QMap<Key, QVariant>;

    virtual const Map& infoMap() const = 0;

    //
    // Return true if this is a supervised learning network
    //
    bool isSupervised() const {
        Type type = infoType();
        return (type == Type::SLP || type == Type::Adaline || type == Type::MLP || type == Type::RBF);
    }

    //
    // Retrieve network type from the map
    //
    Type infoType() const {
        return typeFromMap(infoMap());
    }
    QString infoTypeAsString() const {
        return typeToString(typeFromMap(infoMap()));
    }

    // ===
    // Static methods
    // ===

    //
    // Convert network type string to enum constant
    //
    static Type typeFromString(const QString& typeString) {
        QString typeLower = typeString.toLower();
        if (typeLower == "slp")
            return Type::SLP;
        else if (typeLower == "adaline")
            return Type::Adaline;
        else if (typeLower == "mlp")
            return Type::MLP;
        else if (typeLower == "kohonen")
            return Type::Kohonen;
        else if (typeLower == "rbf")
            return Type::RBF;
        return Type::Unknown;
    }
    //
    // Convert network type constant to string
    //
    static QString typeToString(Type type) {
        switch (type) {
            case Type::SLP:
                return "slp";
            case Type::Adaline:
                return "adaline";
            case Type::MLP:
                return "mlp";
            case Type::Kohonen:
                return "kohonen";
            case Type::RBF:
                return "rbf";
            case Type::Unknown:
                break;
        }
        return "unknown";
    }

    static Type typeFromMap(const Map& map) {
        if (map.contains(Key::Type))
            return map.value(Key::Type).value<Type>();
        return Type::Unknown;
    }

    //
    // Retrieve a list of sample selection orders
    //
    static QStringList sampleSelectionOrderStringList() {
        auto list = QStringList();
        list << QObject::tr("Random");
        list << QObject::tr("As they appear in the training table");
        return list;
    }

    //
    // Convert between sample selection order and index
    //
    static SampleSelectionOrder sampleSelectionOrderFromIndex(int index) {
        switch (index) {
            case 0:
                return SampleSelectionOrder::Random;
            case 1:
                return SampleSelectionOrder::InOrder;
            default:
                break;
        }
        return SampleSelectionOrder::Unknown;
    }
    static int sampleSelectionOrderToIndex(SampleSelectionOrder sso) {
        switch (sso) {
            case SampleSelectionOrder::Random:
                return 0;
            case SampleSelectionOrder::InOrder:
                return 1;
            default:
                break;
        }
        return -1;
    }

    //
    // Convert between sample selection order and string
    //
    static SampleSelectionOrder sampleSelectionOrderFromString(const QString& ssoString) {
        QString ssoLower = ssoString.toLower();
        if (ssoLower == "random")
            return SampleSelectionOrder::Random;
        else if (ssoLower == "in-order")
            return SampleSelectionOrder::InOrder;
        return SampleSelectionOrder::Unknown;
    }
    static QString sampleSelectionOrderToString(SampleSelectionOrder sso) {
        switch (sso) {
            case SampleSelectionOrder::Random:
                return "random";
            case SampleSelectionOrder::InOrder:
                return "in-order";
            case SampleSelectionOrder::Unknown:
                break;
        }
        return "unknown";
    }

    static SampleSelectionOrder sampleSelectionOrderFromMap(const Map& map) {
        if (map.contains(Key::SampleSelectionOrder))
            return map.value(Key::SampleSelectionOrder).value<SampleSelectionOrder>();

        return SampleSelectionOrder::Unknown;
    }
    static int sampleSelectionOrderIndexFromMap(const Map& map) {
        if (map.contains(Key::SampleSelectionOrder))
            return sampleSelectionOrderToIndex(sampleSelectionOrderFromMap(map));

        return -1;
    }
};

inline QDebug operator<<(QDebug debug, NetworkInfo::Type type) {
    debug << NetworkInfo::typeToString(type);
    return debug;
}
