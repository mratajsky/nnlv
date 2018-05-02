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

#include "network.h"

class SupervisedNetwork : public Network {
    Q_OBJECT
public:
    explicit SupervisedNetwork(QObject* parent = nullptr);
    explicit SupervisedNetwork(const NetworkInfo::Map& map, QObject* parent = nullptr);

    virtual double correctPercentage();
    virtual int correctSamples();
    virtual double error();

    enum class ErrorValueType {
        IntValue,
        DoubleValue
    };
    Q_ENUM(ErrorValueType)

    virtual ErrorValueType errorValueType() { return ErrorValueType::DoubleValue; }

protected:
    bool isStopConditionReached(StopTrainingReason* reason) override;
    virtual void updateCurrentStatus();

    double m_correctPercentage = 0.0;
    int m_correctSamples = 0;
    double m_error = 0.0;

private:
    void init();

    bool m_updateNeeded = false;
};
