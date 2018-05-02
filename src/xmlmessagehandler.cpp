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
#include "xmlmessagehandler.h"

XmlMessageHandler::XmlMessageHandler(QObject* parent) :
    QAbstractMessageHandler(parent)
{
}

QString XmlMessageHandler::statusMessage() const
{
    return m_description;
}

int XmlMessageHandler::line() const
{
    return m_sourceLocation.line();
}

int XmlMessageHandler::column() const
{
    return m_sourceLocation.column();
}

void XmlMessageHandler::handleMessage(QtMsgType type,
                                      const QString& description,
                                      const QUrl& identifier,
                                      const QSourceLocation& sourceLocation)
{
    Q_UNUSED(type);
    Q_UNUSED(identifier);

    m_description = description;
    m_sourceLocation = sourceLocation;
}
