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
#include "csvworker.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

CsvWorker::CsvWorker(QObject* parent) :
    QObject(parent)
{
}

//
// Read training samples from the given file into the given store
//
bool CsvWorker::readTrainingSamples(const QString& filePath, TrainingSampleStore& store)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_error = file.errorString();
        return false;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream.setAutoDetectUnicode(true);

    QStringList row;
    QString field;
    QChar quote, buffer(0);
    int line = 1;
    bool result = true;
    while (!stream.atEnd()) {
        QChar ch;
        if (buffer != QChar(0)) {
            ch = buffer;
            buffer = QChar(0);
        } else
            stream >> ch;
        if (ch != m_separator && (ch.category() == QChar::Separator_Line ||
                                  ch.category() == QChar::Separator_Paragraph ||
                                  ch.category() == QChar::Other_Control)) {
            row << field.simplified();
            field.clear();
            if (!row.isEmpty()) {
                result = store.addSample(row);
                if (!result) {
                    if (line != 1 || !isProbablyHeader(row, store))
                        break;
                    // If the first line is most likely a CSV header, just skip over it
                    result = true;
                }
            }
            row.clear();
            line++;
        } else if ((m_quoteMode & DoubleQuote && ch == '"') ||
                   (m_quoteMode & SingleQuote && ch == '\'')) {
            quote = ch;
            do {
                stream >> ch;
                if(ch == '\\' && m_quoteMode & BackslashEscape) {
                    stream >> ch;
                } else if (ch == quote) {
                    if (m_quoteMode & TwoQuoteEscape) {
                        stream >> buffer;
                        if (buffer == quote) {
                            buffer = QChar(0);
                            field.append(ch);
                            continue;
                        }
                    }
                    break;
                }
                field.append(ch);
            } while (!stream.atEnd());
        } else if (ch == m_separator) {
            row << field.simplified();
            field.clear();
        } else
            field.append(ch);
    }
    if (result) {
        if (!field.isEmpty())
            row << field.simplified();
        if (!row.isEmpty()) {
            result = store.addSample(row);
            if (!result && line == 1 && isProbablyHeader(row, store)) {
                //
                // If the file has a single entry and that entry is probably a
                // CSV header, just treat the file as empty
                //
                result = true;
            }
        }
    }
    if (!result)
        m_error = QString("Line %1: %2").arg(line).arg(store.error());

    file.close();
    return result;
}

//
// Write training samples from the given store to the given file
//
bool CsvWorker::writeTrainingSamples(const QString &filePath, const TrainingSampleStore &store)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        m_error = file.errorString();
        return false;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");

    int cols = -1;
    for (const auto& sample : store.samples()) {
        if (cols == -1) {
            // All samples are expected to have the same number of fields
            cols = sample.fieldCount();
            Q_ASSERT(cols >= 0);
        }
        Q_ASSERT(cols == sample.fieldCount());
        //
        // Write columns
        //
        QString data;
        for (int col = 0; col < cols; col++) {
            if (col > 0)
                data += m_separator;
            data += addQuotes(QString::number(sample.field(col)));
        }
        stream << data << endl;
    }
    stream.flush();
    file.close();
    return true;
}

//
// Retrieve the last error
//
QString CsvWorker::error() const
{
    return m_error;
}

//
// Retrieve the current CSV field separator
//
QChar CsvWorker::separator() const
{
    return m_separator;
}

//
// Set the CSV field separator
//
void CsvWorker::setSeparator(const QChar& separator)
{
    m_separator = separator;
}

//
// Retrieve the current quote mode
//
CsvWorker::QuoteMode CsvWorker::quoteMode() const
{
    return m_quoteMode;
}

//
// Set the quote mode
//
void CsvWorker::setQuoteMode(QuoteMode mode)
{
    m_quoteMode = mode;
}

QString CsvWorker::addQuotes(QString field)
{
    bool addDoubleQuotes = ((m_quoteMode & CsvWorker::DoubleQuote) && field.contains('"'));
    bool addSingleQuotes = ((m_quoteMode & CsvWorker::SingleQuote) && field.contains('\''));
    bool quoteField = (m_quoteMode & CsvWorker::AlwaysQuoteOutput) || addDoubleQuotes || addSingleQuotes;
    if (quoteField && !addDoubleQuotes && !addSingleQuotes) {
        if (m_quoteMode & CsvWorker::DoubleQuote)
            addDoubleQuotes = true;
        else if (m_quoteMode & CsvWorker::SingleQuote)
            addSingleQuotes = true;
    }
    if (m_quoteMode & CsvWorker::BackslashEscape) {
        if (addDoubleQuotes)
            return '"' + field.replace("\\", "\\\\").replace("\"", "\\\"") + '"';
        if (addSingleQuotes)
            return '\'' + field.replace("\\", "\\\\").replace("'", "\\'") + '\'';
    } else {
        if (addDoubleQuotes)
            return '"' + field.replace("\"", "\"\"") + '"';
        if (addSingleQuotes)
            return '\'' + field.replace("'", "''") + '\'';
    }
    return field;
}

//
// Return true if the given row could be a CSV header
//
bool CsvWorker::isProbablyHeader(const QStringList& row, const TrainingSampleStore& store)
{
    return row.size() == (store.inputCount() + store.outputCount());
}
