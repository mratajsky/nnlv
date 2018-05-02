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

#include <QObject>
#include <QVector>

#include "networkinfo.h"
#include "savednetworklayer.h"
#include "trainingsamplestore.h"

class SavedNetwork : public QObject, public NetworkInfo
{
    Q_OBJECT
public:
    explicit SavedNetwork(QObject* parent = nullptr);

    NetworkInfo::Map& infoMap();
    const NetworkInfo::Map& infoMap() const override;

    TrainingSampleStore& trainingSampleStore();
    const TrainingSampleStore& trainingSampleStore() const;

    QVector<SavedNetworkLayer*>& savedLayers();
    const QVector<SavedNetworkLayer*>& savedLayers() const;

    bool verify();
    QString verifyError() const;

private:
    NetworkInfo::Map m_infoMap;
    TrainingSampleStore m_store;
    QVector<SavedNetworkLayer*> m_layers;
    QString m_verifyError;
};
