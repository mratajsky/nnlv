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
#include "trainingtabledialog.h"
#include "ui_trainingtabledialog.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMap>
#include <QMessageBox>

#include "program.h"
#include "trainingtablemodel.h"
#include "utilities.h"

QDir TrainingTableDialog::m_currentDir = QDir::current();

int TrainingTableDialog::m_defaultImportNameFilter = 0;
int TrainingTableDialog::m_defaultExportNameFilter = 0;

TrainingTableDialog::TrainingTableDialog(TrainingTableModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrainingTableDialog),
    m_model(model),
    m_csvWorker(this),
    m_xmlWorker(this)
{
    ui->setupUi(this);
    init();
}

TrainingTableDialog::~TrainingTableDialog()
{
    delete ui;
}

void TrainingTableDialog::init()
{
    initModel();
    initHeaders();

    m_importExportNameFilters
            << tr("CSV files (*.csv)")
            << tr("XML files (*.xml)");
    //
    // Restore window state
    //
    QVariant value = m_settings.value("training-table/geometry").toByteArray();
    if (value.isValid())
        restoreGeometry(value.toByteArray());

    ui->labelReadOnly->setVisible(false);
    ui->tableView->setFocus();
}

void TrainingTableDialog::initHeaders()
{
    auto* header = ui->tableView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    header->setSectionsClickable(false);
    header->setSectionsMovable(false);
    //
    // FIXME: this doesn't work with clickable sections disabled, at least
    // with Qt 5.8
    //
    header->setHighlightSections(true);
    header->show();

    header = ui->tableView->verticalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionsMovable(true);
    connect(header, &QHeaderView::sectionMoved, this,
            [this, header](int, int oldIndex, int newIndex) {
        auto moveBack = false;
        if (m_model->isValidRow(oldIndex)) {
            auto index = m_model->constrainRowNumber(newIndex);
            if (oldIndex != index)
                m_model->swapRows(oldIndex, index);
            else {
                //
                // The section was moved but the old and new data positions
                // remain the same, most likely a row above the + line
                // was moved below it
                //
                moveBack = true;
            }
        } else {
            // Prevent non-data row from moving by putting it back
            moveBack = true;
        }
        if (moveBack) {
            header->blockSignals(true);
            header->moveSection(newIndex, oldIndex);
            header->blockSignals(false);
        }
    });
    header->setHighlightSections(true);
    header->show();
}

void TrainingTableDialog::initModel()
{
    ui->tableView->setModel(m_model);

    connect(m_model, &TrainingTableModel::rowsInserted, this, [this] {
        updateButtonRemove();
        updateButtonExportRemoveAll();
    });
    //
    // modelReset() is called when clearing but also when importing data
    //
    connect(m_model, &TrainingTableModel::modelReset, this, [this] {
        updateButtonRemove();
        updateButtonExportRemoveAll();
    });
    connect(m_model, &TrainingTableModel::rowsRemoved, this, [this] {
        updateButtonExportRemoveAll();
    });

    connect(m_model, &TrainingTableModel::dataChangeError, this,
            [this](int, const QString& value, const QString& error) {
        QString errMsg = tr("Could not change value to %1: %2")
                         .arg(value)
                         .arg(error);
        QMessageBox::critical(this, tr(PROGRAM_NAME), errMsg);
    });

    //
    // Buffer all changes done to the model until the dialog is closed
    //
    m_model->beginStoreChange();

    auto* selectionModel = ui->tableView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, [this] {
        updateButtonRemove();
    });

    // Select the first item at the start
    selectionModel->select(m_model->index(0, 0), QItemSelectionModel::SelectCurrent);

    // The model may contain data already
    updateButtonExportRemoveAll();
}

bool TrainingTableDialog::readOnly() const
{
    return m_readOnly;
}

void TrainingTableDialog::setReadOnly(bool readOnly)
{
    if (m_readOnly == readOnly)
        return;

    ui->buttonImport->setEnabled(!readOnly);
    ui->buttonRemove->setEnabled(!readOnly);
    ui->buttonRemoveAll->setEnabled(!readOnly);
    ui->labelReadOnly->setVisible(readOnly);

    if (readOnly) {
        m_editTriggers = ui->tableView->editTriggers();
        //
        // Prevent the table from being editable
        //
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        //
        // Hide the insert row at the end
        //
        ui->tableView->hideRow(m_model->emptyRowIndex());
        ui->tableView->verticalHeader()->setSectionsMovable(false);
    } else {
        ui->tableView->setEditTriggers(m_editTriggers);
        ui->tableView->showRow(m_model->emptyRowIndex());
        ui->tableView->verticalHeader()->setSectionsMovable(true);
    }
    m_readOnly = readOnly;
}

void TrainingTableDialog::done(int r)
{
    m_model->endStoreChange();
    m_settings.setValue("training-table/geometry", saveGeometry());
    QDialog::done(r);
}

bool TrainingTableDialog::readSamples(const QString& filePath)
{
    QFileInfo fileInfo(filePath);

    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool result;
    auto suffix = fileInfo.suffix().toLower();
    if (suffix == "xml")
        result = readSamplesFromXml(filePath);
    else
        result = readSamplesFromCsv(filePath);

    QApplication::restoreOverrideCursor();

    if (!result) {
        QString errMsg = tr("Could not read training samples from %1:"
                            "\n\n"
                            "%2")
                         .arg(fileInfo.fileName())
                         .arg(m_readWriteError);
        QMessageBox::critical(this, tr(PROGRAM_NAME), errMsg);
    }
    return result;
}

bool TrainingTableDialog::readSamplesFromCsv(const QString &filePath)
{
    TrainingSampleStore store(m_model->inputCount(), m_model->outputCount());
    bool result = m_csvWorker.readTrainingSamples(filePath, store);
    if (result) {
        result = m_model->replaceSamples(std::move(store));
        if (!result)
            m_readWriteError = m_model->store().error();
    } else
        m_readWriteError = m_csvWorker.error();
    return result;
}

bool TrainingTableDialog::readSamplesFromXml(const QString &filePath)
{
    TrainingSampleStore store(m_model->inputCount(), m_model->outputCount());
    bool result = m_xmlWorker.readTrainingSamples(filePath, store);
    if (result) {
        result = m_model->replaceSamples(std::move(store));
        if (!result)
            m_readWriteError = m_model->store().error();
    } else
        m_readWriteError = m_xmlWorker.error();
    return result;
}

bool TrainingTableDialog::writeSamples(const QString& filePath)
{
    QFileInfo fileInfo(filePath);

    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool result;
    auto suffix = fileInfo.suffix().toLower();
    if (suffix == "xml")
        result = writeSamplesToXml(filePath);
    else
        result = writeSamplesToCsv(filePath);

    QApplication::restoreOverrideCursor();

    if (!result) {
        QString errMsg = tr("Could not write training samples to %1:"
                            "\n\n"
                            "%2")
                         .arg(fileInfo.fileName())
                         .arg(m_readWriteError);
        QMessageBox::critical(this, tr(PROGRAM_NAME), errMsg);
    }
    return result;
}

//
// Write training sample list in CSV format to the given file
//
bool TrainingTableDialog::writeSamplesToCsv(const QString &filePath)
{
    bool result = m_csvWorker.writeTrainingSamples(filePath, m_model->store());
    if (!result)
        m_readWriteError = m_csvWorker.error();
    return result;
}

//
// Write training sample list in XML format to the given file
//
bool TrainingTableDialog::writeSamplesToXml(const QString &filePath)
{
    bool result = m_xmlWorker.writeTrainingSamples(filePath, m_model->store());
    if (!result)
        m_readWriteError = m_xmlWorker.error();
    return result;
}

void TrainingTableDialog::on_buttonImport_clicked()
{
    QString filter = m_importExportNameFilters[m_defaultImportNameFilter];
    QString fileName = QFileDialog::getOpenFileName(
                           this,
                           tr("Import Training Samples"),
                           m_currentDir.absolutePath(),
                           m_importExportNameFilters.join(";;"),
                           &filter);
    if (fileName.isEmpty())
        return;

    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix().toLower();
    //
    // Make the currently selected name filter the default for the next time
    //
    if (suffix == "csv")
        m_defaultImportNameFilter = 0;
    else
        m_defaultImportNameFilter = 1;

    readSamples(fileName);
    // Next time start in the last used directory
    m_currentDir = fileInfo.absoluteDir();
}

void TrainingTableDialog::on_buttonExport_clicked()
{
    QFileDialog dialog(
                this,
                tr("Export Training Samples"),
                m_currentDir.absolutePath(),
                m_importExportNameFilters.join(";;"));

    dialog.selectNameFilter(m_importExportNameFilters[m_defaultExportNameFilter]);

    //
    // Set the default suffix to make sure to get an overwrite dialog if the file
    // name is given without a suffix. With getSaveFileName() there would be no
    // confirmation, so a manually created QFileDialog is used.
    //
    dialog.setDefaultSuffix(Utilities::fileSuffixFromNameFilter(dialog.selectedNameFilter()));
    //
    // Make the default suffix match the currently selected filter, this is not done
    // automatically by Qt, at least not on Linux
    //
    connect(&dialog, &QFileDialog::filterSelected, this, [this, &dialog](const QString& filter) {
        dialog.setDefaultSuffix(Utilities::fileSuffixFromNameFilter(filter));
    });

    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    QString fileName;
    if (dialog.exec() == QDialog::Accepted) {
        const auto& fileNames = dialog.selectedFiles();
        if (!fileNames.isEmpty())
            fileName = fileNames.first();
    }
    if (fileName.isEmpty())
        return;

    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix().toLower();
    //
    // Make the currently selected name filter the default for the next time
    //
    if (suffix == "csv")
        m_defaultExportNameFilter = 0;
    else
        m_defaultExportNameFilter = 1;

    writeSamples(fileName);
    // Next time start in the last used directory
    m_currentDir = fileInfo.absoluteDir();
}

void TrainingTableDialog::on_buttonRemove_clicked()
{
    auto currentIndex = ui->tableView->currentIndex();

    QMap<int, bool> rows;
    auto selectionModel = ui->tableView->selectionModel();
    //
    // The table selects individual cells instead of full rows,
    // therefore a map is used to collect a list of rows to remove
    //
    for (auto index : selectionModel->selectedIndexes())
        rows.insert(index.row(), true);
    auto i = rows.constEnd();
    while (i != rows.constBegin()) {
        --i;
        m_model->removeRow(i.key());
    }
    //
    // Fix current position and selection
    //
    QModelIndex index = m_model->index(qMax(0, currentIndex.row() - 1), 0);

    selectionModel->select(index, QItemSelectionModel::SelectCurrent);
    selectionModel->setCurrentIndex(index, QItemSelectionModel::Current);
}

void TrainingTableDialog::on_buttonRemoveAll_clicked()
{
    int ret = QMessageBox::question(
                this,
                tr(PROGRAM_NAME),
                tr("Remove all rows in the training set?"));
    if (ret == QMessageBox::No)
        return;

    m_model->clear();
    ui->buttonExport->setEnabled(false);
    ui->buttonRemove->setEnabled(false);
    ui->buttonRemoveAll->setEnabled(false);
}

void TrainingTableDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    Q_UNUSED(button);

    m_model->refreshStoreInputRangeIfNeeded();
}

void TrainingTableDialog::updateButtonRemove()
{
    if (m_readOnly)
        return;

    bool enabled = false;
    const auto indexes = ui->tableView->selectionModel()->selectedIndexes();
    for (const auto index : indexes) {
        //
        // Disable the remove button if nothing or only the last
        // row is selected
        //
        if (m_model->isValidRow(index.row())) {
            enabled = true;
            break;
        }
    }
    ui->buttonRemove->setEnabled(enabled);
}

void TrainingTableDialog::updateButtonExportRemoveAll()
{
    if (m_readOnly)
        return;

    if (m_model->rowCount() > 1) {
        ui->buttonExport->setEnabled(true);
        ui->buttonRemoveAll->setEnabled(true);
    } else {
        ui->buttonExport->setEnabled(false);
        ui->buttonRemoveAll->setEnabled(false);
    }
}
