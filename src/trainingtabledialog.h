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

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QDialog>
#include <QDir>
#include <QSettings>

#include "csvworker.h"
#include "trainingtablemodel.h"
#include "xmlworker.h"

namespace Ui {
class TrainingTableDialog;
}

class TrainingTableDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TrainingTableDialog(TrainingTableModel* model, QWidget* parent = nullptr);
    ~TrainingTableDialog();

    bool readOnly() const;
    void setReadOnly(bool readOnly);

public slots:
    void done(int r) override;

private slots:
    void on_buttonImport_clicked();
    void on_buttonExport_clicked();
    void on_buttonRemove_clicked();
    void on_buttonRemoveAll_clicked();
    void on_buttonBox_clicked(QAbstractButton* button);
    void updateButtonRemove();
    void updateButtonExportRemoveAll();

private:
    void init();
    void initHeaders();
    void initModel();
    bool readSamples(const QString& filePath);
    bool readSamplesFromCsv(const QString& filePath);
    bool readSamplesFromXml(const QString& filePath);
    bool writeSamples(const QString& filePath);
    bool writeSamplesToCsv(const QString& filePath);
    bool writeSamplesToXml(const QString& filePath);

    static QDir m_currentDir;
    static int m_defaultImportNameFilter;
    static int m_defaultExportNameFilter;

    Ui::TrainingTableDialog* ui;
    TrainingTableModel* m_model;
    CsvWorker m_csvWorker;
    XmlWorker m_xmlWorker;
    QAbstractItemView::EditTriggers m_editTriggers;
    bool m_readOnly = false;
    QString m_readWriteError;
    QStringList m_importExportNameFilters;
    QSettings m_settings;
};
