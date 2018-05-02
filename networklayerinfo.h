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

class NetworkLayerInfo {
    Q_GADGET
public:
    virtual ~NetworkLayerInfo() {}

    enum class Key {
        Type,
        Name
    };
    Q_ENUM(Key)

    enum class Type {
        //
        // The "Unknown" layer type is used as an error indicator
        //
        Unknown,
        Input,
        Hidden,
        Output
    };
    Q_ENUM(Type)

    using Map = QMap<Key, QVariant>;

    virtual const Map& infoMap() const = 0;

    //
    // Retrieve layer type from the map
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
    // Convert network layer type string to enum constant
    //
    static Type typeFromString(const QString& typeString) {
        QString typeLower = typeString.toLower();
        if (typeLower == "input")
            return Type::Input;
        else if (typeLower == "hidden")
            return Type::Hidden;
        else if (typeLower == "output")
            return Type::Output;
        return Type::Unknown;
    }
    //
    // Convert network type constant to string
    //
    static QString typeToString(Type type) {
        switch (type) {
        case Type::Input:
            return "input";
        case Type::Hidden:
            return "hidden";
        case Type::Output:
            return "output";
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
};

inline QDebug operator<<(QDebug debug, NetworkLayerInfo::Type type) {
    debug << NetworkLayerInfo::typeToString(type);
    return debug;
}
