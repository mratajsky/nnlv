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
#include "networkstatusnumberlabel.h"

NetworkStatusNumberLabel::NetworkStatusNumberLabel(QWidget* parent) :
    QLabel(parent)
{
    init();
}

NetworkStatusNumberLabel::NetworkStatusNumberLabel(const QString& text, QWidget* parent) :
    QLabel(text, parent)
{
    init();
}

void NetworkStatusNumberLabel::init()
{
    setAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

bool NetworkStatusNumberLabel::isChanged() const
{
    return m_changed;
}

void NetworkStatusNumberLabel::setChanged(bool changed)
{
    m_changed = changed;
}

void NetworkStatusNumberLabel::setNumber(double number, double precision)
{
    setText(QString::number(number, 'f', precision));
}

void NetworkStatusNumberLabel::setNumberDiff(double number)
{
    if (number == 0)
        setText(QStringLiteral(""));
    else {
        QString text;
        if (number > 0)
            text = QString("<span style='color:green'>+ %1</span>")
                   .arg(QString::number(number, 'f', 3));
        else
            text = QString("<span style='color:red'>- %1</span>")
                   .arg(QString::number(qAbs(number), 'f', 3));

        setText(text);
    }
}
