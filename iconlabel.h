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
#include <QWidget>

class IconLabel : public QWidget
{
    Q_OBJECT
public:
    explicit IconLabel(QWidget* parent = nullptr);
    explicit IconLabel(const QString& text, QWidget* parent = nullptr);

    QLabel* iconLabel();
    QLabel* textLabel();

    enum class IconType {
        Bias,
        Input,
        Neuron
    };
    Q_ENUM(IconType)

    void setIconType(IconLabel::IconType iconType);

private:
    void init();

    QLabel* m_icon;
    QLabel* m_text;
    IconType m_iconType = IconType::Neuron;
};
