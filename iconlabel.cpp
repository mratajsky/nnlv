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
#include "iconlabel.h"

#include <QPainter>
#include <QPixmap>
#include <QHBoxLayout>

#include "colors.h"

IconLabel::IconLabel(QWidget* parent) :
    QWidget(parent),
    m_icon(new QLabel(this)),
    m_text(new QLabel(this))
{
    init();
}

IconLabel::IconLabel(const QString& text, QWidget* parent) :
    QWidget(parent),
    m_icon(new QLabel(this)),
    m_text(new QLabel(text, this))
{
    init();
}

void IconLabel::init()
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addWidget(m_icon);
    layout->addWidget(m_text);
    m_icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setLayout(layout);
}

QLabel* IconLabel::iconLabel()
{
    return m_icon;
}

QLabel* IconLabel::textLabel()
{
    return m_text;
}

void IconLabel::setIconType(IconType iconType)
{
    QPixmap pixmap(m_icon->height() * .5, m_icon->height() * .5);

    switch (iconType) {
        case IconType::Bias: {
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            painter.setPen(QPen(Colors::ColorBias));
            painter.setBrush(Colors::ColorBias);
            painter.drawEllipse(pixmap.rect().center(),
                                pixmap.width() / 2,
                                pixmap.height() / 2);
            break;
        }
        case IconType::Neuron: {
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            painter.setPen(QPen(Colors::ColorNeuronInitial));
            painter.setBrush(Colors::ColorNeuronInitial);
            painter.drawEllipse(pixmap.rect().center(),
                                pixmap.width() / 2,
                                pixmap.height() / 2);
            break;
        }
        case IconType::Input:
            pixmap.fill(Colors::ColorNeuronInitial);
    }
    m_icon->setPixmap(pixmap);
    m_iconType = iconType;
}
