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

#include <QLabel>

class NetworkStatusNumberLabel : public QLabel
{
    Q_OBJECT
public:
    explicit NetworkStatusNumberLabel(QWidget* parent = nullptr);
    explicit NetworkStatusNumberLabel(const QString& text, QWidget* parent = nullptr);

    bool isChanged() const;
    void setChanged(bool changed);

    void setNumber(double number, double precision = 6);
    void setNumberDiff(double number);

private:
    void init();

    bool m_changed = false;
};
