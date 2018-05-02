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
 *
 * This file includes parts based on qxtcsvmodel.cpp/qxtcsvmodel.h:
 *     Copyright (c) 2006 - 2011, the LibQxt project.
 *     See the Qxt AUTHORS file for a list of authors and copyright holders.
 *     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the LibQxt project nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#pragma once

#include "common.h"

#include <QObject>

#include "trainingsamplestore.h"

class CsvWorker : public QObject
{
    Q_OBJECT
public:
    explicit CsvWorker(QObject* parent = nullptr);

    bool readTrainingSamples(const QString& filePath, TrainingSampleStore& store);
    bool writeTrainingSamples(const QString& filePath, const TrainingSampleStore& store);

    QString error() const;

    QChar separator() const;
    void setSeparator(const QChar& separator);

    enum QuoteOption {
        NoQuotes = 0,
        SingleQuote = 1,
        DoubleQuote = 2,
        BothQuotes = 3,
        NoEscape = 0,
        TwoQuoteEscape = 4,
        BackslashEscape = 8,
        AlwaysQuoteOutput = 16,
        DefaultQuoteMode = BothQuotes | BackslashEscape | AlwaysQuoteOutput
    };
    Q_DECLARE_FLAGS(QuoteMode, QuoteOption)

    QuoteMode quoteMode() const;
    void setQuoteMode(QuoteMode mode);

private:
    QString addQuotes(QString field);
    bool isProbablyHeader(const QStringList& row, const TrainingSampleStore& store);

    QChar m_separator = ',';
    QString m_error;
    QuoteMode m_quoteMode;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(CsvWorker::QuoteMode)
