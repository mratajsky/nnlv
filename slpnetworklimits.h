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

#include "networklayerinfo.h"

class SLPNetworkLimits {
public:
    //
    // Neuron count limits
    //
    static constexpr int minInputNeurons = 2;
    static constexpr int maxInputNeurons = 5;
    static constexpr int minOutputNeurons = 1;
    static constexpr int maxOutputNeurons = 5;

    //
    // Verify that the given number of neurons is correct and within the limits
    // for the given layer type
    //
    static bool verifyNeuronCount(int count, NetworkLayerInfo::Type layerType) {
        switch (layerType) {
            case NetworkLayerInfo::Type::Input:
                return count >= minInputNeurons && count <= maxInputNeurons;
            case NetworkLayerInfo::Type::Output:
                return count >= minOutputNeurons && count <= maxOutputNeurons;
            default:
                break;
        }
        Q_UNREACHABLE();
        return false;
    }

    //
    // Convenience functions to retrieve neuron count limits for the given layer type
    //
    static int minNeurons(NetworkLayerInfo::Type layerType) {
        switch (layerType) {
            case NetworkLayerInfo::Type::Input:
                return minInputNeurons;
            case NetworkLayerInfo::Type::Output:
                return minOutputNeurons;
            default:
                break;
        }
        Q_UNREACHABLE();
        return -1;
    }
    static int maxNeurons(NetworkLayerInfo::Type layerType) {
        switch (layerType) {
            case NetworkLayerInfo::Type::Input:
                return maxInputNeurons;
            case NetworkLayerInfo::Type::Output:
                return maxOutputNeurons;
            default:
                break;
        }
        Q_UNREACHABLE();
        return -1;
    }
};
