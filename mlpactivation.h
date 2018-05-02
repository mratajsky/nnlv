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

#include <cmath>
#include <QObject>

#include "networkinfo.h"

class MLPActivation {
    Q_GADGET
public:
    enum class Function {
        //
        // The "Unknown" function is used as an error indicator
        //
        Unknown,
        Sigmoid,
        Tanh
    };
    Q_ENUM(Function)

    static double sigmoid(double x) {
        return 1.0 / (1.0 + std::exp(-x));
    }

    static double tanh(double x) {
        return std::tanh(x);
    }

    //
    // Compute value by giving the function constant as the first argument
    //
    static double function(Function function, double x) {
        switch (function) {
            case Function::Sigmoid:
                return sigmoid(x);
            case Function::Tanh:
                return tanh(x);
            default:
                Q_UNREACHABLE();
                return qSNaN();
        }
    }

    //
    // Retrieve a list of activation functions
    //
    static QStringList functionStringList() {
        auto list = QStringList();
        list << QObject::tr("Standard Sigmoid");
        list << QObject::tr("Tanh");
        return list;
    }

    //
    // Convert between activation function order and index
    //
    static Function functionFromIndex(int index) {
        switch (index) {
            case 0:
                return Function::Sigmoid;
            case 1:
                return Function::Tanh;
            default:
                break;
        }
        return Function::Unknown;
    }
    static int functionToIndex(Function function) {
        switch (function) {
            case MLPActivation::Function::Sigmoid:
                return 0;
            case MLPActivation::Function::Tanh:
                return 1;
            default:
                break;
        }
        return -1;
    }

    //
    // Convert activation function name string to enum constant
    //
    static Function functionFromString(const QString& funcString) {
        QString funcLower = funcString.toLower();
        if (funcLower == "sigmoid")
            return Function::Sigmoid;
        else if (funcLower == "tanh")
            return Function::Tanh;
        return Function::Unknown;
    }
    //
    // Convert function name constant to string
    //
    static QString functionToString(Function func) {
        switch (func) {
            case Function::Sigmoid:
                return "sigmoid";
            case Function::Tanh:
                return "tanh";
            case Function::Unknown:
                break;
        }
        return "unknown";
    }

    static Function functionFromMap(const NetworkInfo::Map& map) {
        if (map.contains(NetworkInfo::Key::ActivationFunction))
            return map.value(NetworkInfo::Key::ActivationFunction).value<Function>();

        return Function::Unknown;
    }
    static int functionIndexFromMap(const NetworkInfo::Map& map) {
        if (map.contains(NetworkInfo::Key::ActivationFunction))
            return functionToIndex(functionFromMap(map));

        return -1;
    }
};

class MLPActivationDerivative {
public:
    static double sigmoid(double x) {
        return x * (1.0 - x);
    }

    static double tanh(double x) {
        return 1.0 - x * x;
    }

    //
    // Compute value by giving the function constant as the first argument
    //
    static double function(MLPActivation::Function function, double x) {
        switch (function) {
            case MLPActivation::Function::Sigmoid:
                return sigmoid(x);
            case MLPActivation::Function::Tanh:
                return tanh(x);
            default:
                Q_UNREACHABLE();
                return qSNaN();
        }
    }
};
