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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#include "adalineviewwidget.h"
#include "custominputdialog.h"
#include "helpbrowser.h"
#include "kohonenviewwidget.h"
#include "mlpviewwidget.h"
#include "networkwizard.h"
#include "optionsdialog.h"
#include "program.h"
#include "rbfviewwidget.h"
#include "renamenetworkdialog.h"
#include "resettable.h"
#include "resetweightsdialog.h"
#include "slpviewwidget.h"
#include "supervisednetwork.h"
#include "trainingtabledialog.h"
#include "trainingsamplestore.h"
#include "version.h"

QDir MainWindow::m_currentDir = QDir::current();

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_statusLabel(new QLabel(this))
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(m_statusLabel);
    init();
}

MainWindow::MainWindow(NetworkViewWidget* widget, QWidget* parent) :
    MainWindow(parent)
{
    setNetworkWidget(widget);
}

MainWindow::MainWindow(const QString& fileName, QWidget* parent) :
    MainWindow(parent)
{
    loadNetwork(fileName);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    setAttribute(Qt::WA_DeleteOnClose);

    readProgramSettings();
    readWindowSettings();

    updateWindowTitle();
}

void MainWindow::showEvent(QShowEvent* event)
{
    if (!event->spontaneous()) {
        //
        // Save the window position to allow newly open windows to use it, the
        // delay makes it save the correct information, at least on Linux
        //
        QTimer::singleShot(100, this, [this] {
            writeWindowSettings();
        });
    }
    QMainWindow::showEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_saveModified && isWindowModified()) {
        auto result = QMessageBox::warning(
                    this,
                    tr(PROGRAM_NAME),
                    tr("The neural network has been modified."
                       "\n\n"
                       "Do you want to save your changes?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        bool ignore = false;
        switch (result) {
            case QMessageBox::Save:
                on_actionSave_triggered();
                //
                // The save action could have been cancelled, check if the
                // window is still marked as modified to find out
                //
                if (isWindowModified())
                    ignore = true;
                break;
            case QMessageBox::Cancel:
                ignore = true;
                break;
            default:
                break;
        }
        if (ignore) {
            event->ignore();
            return;
        }
    }
    writeProgramSettings();
    writeWindowSettings();

    QMainWindow::closeEvent(event);
}

//
// Create a network view widget for the given network
//
NetworkViewWidget* MainWindow::createNetworkViewWidget(Network* network)
{
    NetworkViewWidget* widget = nullptr;
    switch (network->infoType()) {
        case NetworkInfo::Type::SLP:
            widget = new SLPViewWidget(network, this);
            break;
        case NetworkInfo::Type::Adaline:
            widget = new AdalineViewWidget(network, this);
            break;
        case NetworkInfo::Type::MLP:
            widget = new MLPViewWidget(network, this);
            break;
        case NetworkInfo::Type::Kohonen:
            widget = new KohonenViewWidget(network, this);
            break;
        case NetworkInfo::Type::RBF:
            widget = new RBFViewWidget(network, this);
            break;
        case NetworkInfo::Type::Unknown:
            Q_UNREACHABLE();
            break;
    }
    return widget;
}

NetworkViewWidget* MainWindow::createNetworkViewWidget(const NetworkInfo::Map& map)
{
    NetworkViewWidget* widget = nullptr;
    switch (NetworkInfo::typeFromMap(map)) {
        case NetworkInfo::Type::SLP:
            widget = new SLPViewWidget(map, this);
            break;
        case NetworkInfo::Type::Adaline:
            widget = new AdalineViewWidget(map, this);
            break;
        case NetworkInfo::Type::MLP:
            widget = new MLPViewWidget(map, this);
            break;
        case NetworkInfo::Type::Kohonen:
            widget = new KohonenViewWidget(map, this);
            break;
        case NetworkInfo::Type::RBF:
            widget = new RBFViewWidget(map, this);
            break;
        case NetworkInfo::Type::Unknown:
            Q_UNREACHABLE();
            break;
    }
    return widget;
}

NetworkViewWidget* MainWindow::createNetworkViewWidget(const SavedNetwork& savedNetwork)
{
    NetworkViewWidget* widget = nullptr;
    switch (NetworkInfo::typeFromMap(savedNetwork.infoMap())) {
        case NetworkInfo::Type::SLP:
            widget = new SLPViewWidget(savedNetwork, this);
            break;
        case NetworkInfo::Type::Adaline:
            widget = new AdalineViewWidget(savedNetwork, this);
            break;
        case NetworkInfo::Type::MLP:
            widget = new MLPViewWidget(savedNetwork, this);
            break;
        case NetworkInfo::Type::Kohonen:
            widget = new KohonenViewWidget(savedNetwork, this);
            break;
        case NetworkInfo::Type::RBF:
            widget = new RBFViewWidget(savedNetwork, this);
            break;
        case NetworkInfo::Type::Unknown:
            Q_UNREACHABLE();
            break;
    }
    return widget;
}

//
// Return the network of the current main window network widget or nullptr
// if the main window widget is not a network one
//
Network* MainWindow::currentNetwork() const
{
    auto* widget = currentNetworkWidget();
    if (widget != nullptr)
        return widget->network();
    return nullptr;
}

//
// Return the network widget of the current main window
//
NetworkViewWidget* MainWindow::currentNetworkWidget() const
{
    return qobject_cast<NetworkViewWidget*>(centralWidget());
}

//
// Return true if the current main window widget is a network one
//
bool MainWindow::hasNetworkWidget() const
{
    return currentNetworkWidget() != nullptr;
}

//
// Show the network wizard
//
void MainWindow::showNewNetworkWizard()
{
    if (!isVisible()) {
        //
        // Prevent the wizard from being shown before the main window becomes
        // visible because it would not be centered, rather schedule it to be
        // shown later.
        //
        if (m_newNetworkWizardPending)
            return;
        m_newNetworkWizardPending = true;
        // For some reason with smaller delay this doesn't always work on Windows
        QTimer::singleShot(550, this, [this] {
            m_newNetworkWizardPending = false;
            showNewNetworkWizard();
        });
        return;
    } else
        m_newNetworkWizardPending = false;

    NetworkWizard dialog(this);
    connect(&dialog, &NetworkWizard::loadButtonClicked, this, [this, &dialog] {
        if (loadNetwork())
            dialog.close();
    });
    if (dialog.exec() != QDialog::Accepted)
        return;

    NetworkViewWidget* widget = nullptr;
    switch (dialog.result()) {
        case NetworkWizard::Result::NotAccepted:
            break;
        case NetworkWizard::Result::AcceptedExample: {
            SavedNetwork savedNetwork;
            if (loadNetwork(dialog.resultExampleFileName(), savedNetwork, false))
                showSavedNetwork(savedNetwork, QString(), true);
            break;
        }
        case NetworkWizard::Result::AcceptedNewNetwork:
            widget = createNetworkViewWidget(dialog.resultNetworkInfoMap());
            break;
    }
    if (widget == nullptr)
        return;

    if (hasNetworkWidget()) {
        //
        // If this main window already has a network widget, create a new main
        // window and put the widget into it
        //
        auto* window = new MainWindow(widget);
        window->setGeometry(geometry());
        window->setModified(true);
        window->show();
        window->activateWindow();
    } else {
        setNetworkWidget(widget);
        setModified(true);
    }
}

bool MainWindow::loadNetwork()
{
    SavedNetwork savedNetwork;
    return loadNetwork(savedNetwork);
}

bool MainWindow::loadNetwork(const QString& filePath)
{
    SavedNetwork savedNetwork;
    return loadNetwork(filePath, savedNetwork);
}

bool MainWindow::loadNetwork(SavedNetwork& savedNetwork, bool show)
{
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open Saved Network"), m_currentDir.absolutePath(),
                tr("XML files (*.xml)"));
    if (fileName.isEmpty())
        return false;

    // Next time start in the last used directory, even if loading fails
    m_currentDir = QFileInfo(fileName).absoluteDir();

    return loadNetwork(fileName, savedNetwork, show);
}

bool MainWindow::activateWindowForFile(const QString& filePath)
{
    const auto windows = QApplication::topLevelWidgets();
    for (auto* window : windows) {
        const auto* mainWindow = qobject_cast<MainWindow*>(window);
        if (mainWindow == nullptr)
            continue;
        if (filePath == window->windowFilePath()) {
            // This can even be the current window
            window->activateWindow();
            return true;
        }
    }
    return false;
}

bool MainWindow::loadNetwork(const QString& filePath, SavedNetwork& savedNetwork, bool show)
{
    // If the selected file is open in another toplevel window, just switch to it
    if (activateWindowForFile(filePath))
        return false;

    qDebug() << "Loading network from" << filePath;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    XmlWorker xml;
    xml.setErrorOnUnknownNetwork(true);
    bool result = xml.readNetwork(filePath, savedNetwork, XmlWorker::ReadNetworkMode::ReadAll);

    QApplication::restoreOverrideCursor();
    if (!result) {
        QString errMsg = tr("Could not load network from %1:"
                            "\n\n"
                            "%2")
                         .arg(QFileInfo(filePath).fileName())
                         .arg(xml.error());
        QMessageBox::critical(this, tr(PROGRAM_NAME), errMsg);
        return false;
    }
    if (show)
        showSavedNetwork(savedNetwork, filePath);
    return true;
}

void MainWindow::showSavedNetwork(SavedNetwork& savedNetwork, const QString& filePath, bool enableSave)
{
    auto* widget = createNetworkViewWidget(savedNetwork);

    // Move training samples into the fully created network
    auto* tableModel = widget->network()->trainingTableModel();
    if (!tableModel->replaceSamples(std::move(savedNetwork.trainingSampleStore()))) {
        QString errMsg = tr("Could not load training samples:"
                            "\n\n"
                            "%1")
                         .arg(tableModel->store().error());

        QMessageBox::critical(this, tr(PROGRAM_NAME), errMsg);
    }
    //
    // Create a new main window for the network if this window already has one,
    // otherwise just replace the current central widget
    //
    if (hasNetworkWidget()) {
        auto* window = new MainWindow(widget);
        window->setGeometry(geometry());
        if (!filePath.isEmpty()) {
            window->setWindowFilePath(filePath);
            window->updateWindowTitle();
        }
        if (enableSave)
            window->ui->actionSave->setEnabled(true);
        window->show();
        window->activateWindow();
    } else {
        if (!filePath.isEmpty())
            setWindowFilePath(filePath);
        setNetworkWidget(widget);
        if (enableSave)
            ui->actionSave->setEnabled(true);
    }
}

bool MainWindow::saveNetwork(const QString& filePath)
{
    // Make sure that this file is not bound to another toplevel window
    const auto windows = QApplication::topLevelWidgets();
    for (auto* window : windows) {
        const auto* mainWindow = qobject_cast<MainWindow*>(window);
        if (mainWindow == nullptr || mainWindow == this)
            continue;

        if (filePath == window->windowFilePath()) {
            QString errMsg = tr("The selected file %1 is open in another "
                                "program window.")
                             .arg(QFileInfo(filePath).fileName());
            QMessageBox::critical(this, tr(PROGRAM_NAME), errMsg);
            return false;
        }
    }

    qDebug() << "Saving network to" << filePath;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    XmlWorker xml;
    bool result = xml.writeNetwork(filePath, *currentNetwork());

    QApplication::restoreOverrideCursor();
    if (result) {
        setWindowFilePath(filePath);
        setModified(false);
        updateWindowTitle();
    } else {
        QString errMsg = tr("Could not save the network to %1:"
                            "\n\n"
                            "%2")
                         .arg(QFileInfo(filePath).fileName())
                         .arg(xml.error());
        QMessageBox::critical(this, tr(PROGRAM_NAME), errMsg);
    }
    return result;
}

void MainWindow::setModified(bool modified)
{
    ui->actionSave->setEnabled(modified);

    setWindowModified(modified);
}

void MainWindow::setDockWidgets(const QVector<MainDockWidget*>& widgets)
{
    for (auto* widget : widgets) {
        if (!restoreDockWidget(widget)) {
            addDockWidget(widget->preferredArea(), widget);
            widget->show();
        } else
            widget->hide();
    }
    m_dockWidgets = widgets;
    // Resize the dock widgets after they are realized
    QMetaObject::invokeMethod(this, "resizeDockWidgets", Qt::QueuedConnection);
}

void MainWindow::resizeDockWidgets()
{
    QMap<Qt::DockWidgetArea, int> areaCountMap;

    QList<QDockWidget*> docks;
    QList<int> widths;
    bool hasUnknownWidget = false;
    for (auto* dockWidget : qAsConst(m_dockWidgets)) {
        auto area = dockWidgetArea(dockWidget);
        if (area == Qt::NoDockWidgetArea)
            continue;
        docks.append(dockWidget);
        if (!hasUnknownWidget && m_dockSizes.contains(dockWidget->objectName())) {
            auto size = m_dockSizes.value(dockWidget->objectName());
            widths.append(size.width());
        } else
            hasUnknownWidget = true;

        if (!areaCountMap.contains(area))
            areaCountMap.insert(area, 0);

        areaCountMap[area] += dockWidget->heightUnits();
    }

    if (!hasUnknownWidget) {
        //
        // The heights were restored correctly by restoreDockWidget(), the widths
        // are set to the saved values here.
        //
        // To avoid flickering, the widgets are hidden during the resizing.
        //
        resizeDocks(docks, widths, Qt::Horizontal);
    } else {
        //
        // The default choices for dock widget sizes are not good, try to give
        // them a reasonable share of space here.
        //
        // This code assumes all widgets are placed in either left or right dock area.
        //
        Qt::DockWidgetArea preferredArea = currentNetworkWidget()->preferredDockWidgetArea();

        QMap<Qt::DockWidgetArea, int> areaWidth;
        areaWidth[Qt::LeftDockWidgetArea] =
                areaWidth[Qt::RightDockWidgetArea] = size().width() * 0.2;
        //
        // Either the left or the right area may be prefered, which gives widgets
        // in that area twice the space
        //
        if (preferredArea != Qt::NoDockWidgetArea)
            areaWidth[preferredArea] = size().width() * 0.4;

        QMapIterator<Qt::DockWidgetArea, int> i(areaCountMap);
        while (i.hasNext()) {
            i.next();

            QList<QDockWidget*> areaDocks;
            QList<int> hs;
            QList<int> ws;
            int height = size().height() / areaCountMap[i.key()];
            for (auto* dock : qAsConst(docks)) {
                if (dockWidgetArea(dock) == i.key()) {
                    areaDocks.append(dock);
                    hs.append(height * qobject_cast<MainDockWidget*>(dock)->heightUnits());
                    ws.append(areaWidth[i.key()]);
                }
            }
            resizeDocks(areaDocks, hs, Qt::Vertical);
            resizeDocks(areaDocks, ws, Qt::Horizontal);
        }
    }

    for (auto* widget : qAsConst(docks))
        widget->show();
}

void MainWindow::setNetworkWidget(NetworkViewWidget* widget)
{
    // This method is expected to be called only once
    Q_ASSERT(currentNetwork() == nullptr);

    setCentralWidget(widget);

    auto* network = currentNetwork();
    connect(network, &Network::statusMessage, this, [this](const QString& message) {
        ui->statusBar->showMessage(message);
    });
    connect(network, &Network::trainingStarted, this, [this] {
        ui->actionComputeCustomInput->setEnabled(false);
        ui->actionResetWeights->setEnabled(false);
        ui->actionResetNetworkView->setEnabled(false);
    });
    connect(network, &Network::trainingStateChanged, this, &MainWindow::updateStatusBarMessage);
    connect(network, &Network::nameChanged, this, &MainWindow::updateWindowTitle);

    updateNetworkChangeConnections();
    updateStatusBarMessage();
    updateWindowTitle();

    const auto& dockWidgets = widget->createDockWidgets();
    if (!dockWidgets.isEmpty())
        setDockWidgets(dockWidgets);

    connect(network, &Network::trainingStopped, this,
            [this, network](Network::StopTrainingReason reason) {
        QString textReason;
        auto* supervisedNetwork = qobject_cast<SupervisedNetwork*>(network);
        switch (reason) {
            case Network::StopTrainingReason::ErrorReached:
                textReason = tr("error threshold reached"
                                "\n\n"
                                "Threshold: %1"
                                "\n"
                                "Current error: %2")
                             .arg(supervisedNetwork->stopError())
                             .arg(supervisedNetwork->error());
                break;
            case Network::StopTrainingReason::PercentageReached:
                textReason = tr("%n% of samples classified correctly", "",
                                supervisedNetwork->correctPercentage());
                break;
            case Network::StopTrainingReason::SamplesReached:
                textReason = tr("%n sample(s) classified correctly", "",
                                supervisedNetwork->correctSamples());
                break;
            case Network::StopTrainingReason::MaxEpochsReached:
                textReason = tr("%n epoch(s) done", "",
                                network->trainingEpochs());
                break;
            default:
                break;
        }
        if (!textReason.isEmpty())
            QMessageBox::information(
                        this,
                        tr(PROGRAM_NAME),
                        tr("Training stopped: %1").arg(textReason));

        ui->actionComputeCustomInput->setEnabled(true);
        ui->actionResetWeights->setEnabled(true);
        ui->actionResetNetworkView->setEnabled(true);
    });

    // Enable all actions that are kept disabled until a network is either
    // created or loaded.
    ui->menuNetwork->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
    ui->actionConfigureTrainingSet->setEnabled(true);
    ui->actionComputeCustomInput->setEnabled(true);
    ui->actionRenameNetwork->setEnabled(true);
    ui->actionResetWeights->setEnabled(true);

    if (widget->visualizationFeatures() & NetworkViewWidget::ConnectionWeights) {
        ui->actionShowConnectionWeights->setEnabled(true);
        widget->setShowConnectionWeights(ui->actionShowConnectionWeights->isChecked());
    }
    if (widget->visualizationFeatures() & NetworkViewWidget::InputValues) {
        ui->actionShowInputValues->setEnabled(true);
        widget->setShowInputValues(ui->actionShowInputValues->isChecked());
    }
    if (widget->visualizationFeatures() & NetworkViewWidget::NeuronValues) {
        ui->actionShowNeuronValues->setEnabled(true);
        widget->setShowNeuronValues(ui->actionShowNeuronValues->isChecked());
    }

    ui->actionResetNetworkView->setEnabled(true);
}

void MainWindow::readProgramSettings()
{
    m_saveModified = m_settings
                     .value(QStringLiteral("program/save-modified"), true)
                     .toBool();

    updateNetworkChangeConnections();
}

void MainWindow::readWindowSettings()
{
    QVariant value;
    value = m_settings.value("window/geometry");
    if (value.isValid())
        restoreGeometry(value.toByteArray());
    value = m_settings.value("window/state");
    if (value.isValid())
        restoreState(value.toByteArray());

    value = m_settings.value("window/show-connection-weights", true);
    if (value.isValid())
        ui->actionShowConnectionWeights->setChecked(value.toBool());
    value = m_settings.value("window/show-input-values", true);
    if (value.isValid())
        ui->actionShowInputValues->setChecked(value.toBool());
    value = m_settings.value("window/show-neuron-values", true);
    if (value.isValid())
        ui->actionShowNeuronValues->setChecked(value.toBool());

    m_settings.beginGroup("dock-sizes");
    const auto keys = m_settings.childKeys();
    for (const auto key : keys)
        m_dockSizes[key] = m_settings.value(key).toSize();

    m_settings.endGroup();
}

void MainWindow::writeProgramSettings()
{
    m_settings.setValue("window/show-connection-weights",
                        ui->actionShowConnectionWeights->isChecked());
    m_settings.setValue("window/show-input-values",
                        ui->actionShowInputValues->isChecked());
    m_settings.setValue("window/show-neuron-values",
                        ui->actionShowNeuronValues->isChecked());
}

void MainWindow::writeWindowSettings()
{
    m_settings.setValue("window/geometry", saveGeometry());
    m_settings.setValue("window/state", saveState());

    m_settings.beginGroup(QStringLiteral("dock-sizes"));

    for (const auto* widget : qAsConst(m_dockWidgets))
        m_settings.setValue(widget->objectName(), widget->size());

    m_settings.endGroup();
}

void MainWindow::updateNetworkChangeConnections()
{
    auto* network = currentNetwork();
    if (network == nullptr)
        return;

    if (!m_networkConnectionsCreated) {
        //
        // Mark the network as modified if any of the following things happens:
        //  - Training samples change
        //  - NetworkInfo::Map changes - this includes name, initial weights and
        //    training options of the network
        //  - Layer name changes
        //  - Neuron name changes
        //  - Weight changes - only if monitoring this change is enabled in the
        //    program options
        //
        const auto& store = network->trainingTableModel()->store();

        connect(&store, &TrainingSampleStore::samplesChanged, this, [this] {
            setModified(true);
        });
        connect(network, &Network::infoChanged, this, [this] {
            setModified(true);
        });
        connect(network, &Network::layerNameChanged, this, [this] {
            setModified(true);
        });
        connect(network, &Network::neuronNameChanged, this, [this] {
            setModified(true);
        });
        m_networkConnectionsCreated = true;
    }
    bool markModified = m_settings.value("program/mark-modified-weights").toBool();
    if (m_markModifiedWeightsConnection)
        disconnect(m_markModifiedWeightsConnection);
    if (markModified)
        m_markModifiedWeightsConnection =
                connect(network, &Network::neuronWeightChanged, this, [this] {
                    setModified(true);
                });
    else
        m_markModifiedWeightsConnection =
                connect(network, &Network::neuronWeightChanged, this, [this] {
                    //
                    // User requested not to mark the window as modified, but the
                    // network has changes and it should be possible to save it
                    //
                    ui->actionSave->setEnabled(true);
                });
}

void MainWindow::updateStatusBarMessage()
{
    QString message;

    auto* network = currentNetwork();
    if (network != nullptr && (network->isTraining() || network->isTrainingPaused())) {
        int epochs = network->trainingEpochs();
        if (network->isTrainingPaused())
            message = tr("Training is paused. ");
        else
            message = tr("Training in progress. ");

        message += tr("%n epoch(s) done", "", epochs);

        int maxEpochs = network->maxTrainingEpochs();
        if (maxEpochs > 0)
            message += tr(" (%n total).", "", maxEpochs);
        else
            message += ".";
    } else
        message = tr("Idle.");

    m_statusLabel->setText(message);
}

void MainWindow::updateWindowTitle()
{
    auto* network = currentNetwork();

    if (network != nullptr) {
        QString title;
        const auto& name = network->name();
        if (name.isEmpty())
            title = tr("Unnamed Network");
        else
            title = name;
        title += QStringLiteral("[*]");

        QString filePath = windowFilePath();
        if (!filePath.isEmpty())
            title += QStringLiteral(" - ") + QFileInfo(filePath).fileName();

        title += QStringLiteral(" - ") + PROGRAM_TITLE;

        setWindowTitle(title);
    } else
        setWindowTitle(PROGRAM_TITLE);
}

// ===========================================================================
// Menu actions
// ===========================================================================

//
// File -> New
//
void MainWindow::on_actionNew_triggered()
{
    showNewNetworkWizard();
}

//
// File -> Open
//
void MainWindow::on_actionOpen_triggered()
{
    loadNetwork();
}

//
// File -> Save
//
void MainWindow::on_actionSave_triggered()
{
    auto fileName = windowFilePath();
    if (!fileName.isEmpty())
        saveNetwork(fileName);
    else
        on_actionSaveAs_triggered();
}

//
// File -> Save As
//
void MainWindow::on_actionSaveAs_triggered()
{
    QFileDialog dialog(
                this,
                tr("Save Network"),
                m_currentDir.absolutePath(),
                tr("XML files (*.xml)"));

    dialog.setAcceptMode(QFileDialog::AcceptSave);
    //
    // Set the default suffix to make sure to get an overwrite dialog if the file
    // name is given without a suffix. With getSaveFileName() there would be no
    // confirmation, so a manually created QFileDialog is used.
    //
    dialog.setDefaultSuffix(QStringLiteral("xml"));
    dialog.setFileMode(QFileDialog::AnyFile);
    QString fileName;
    if (dialog.exec() == QDialog::Accepted) {
        const auto& fileNames = dialog.selectedFiles();
        if (!fileNames.isEmpty())
            fileName = fileNames.first();
    }
    if (fileName.isEmpty())
        return;

    saveNetwork(fileName);

    // Next time start in the last used directory
    m_currentDir = QFileInfo(fileName).absoluteDir();
}

//
// File -> Close
//
void MainWindow::on_actionClose_triggered()
{
    close();
}

//
// File -> Quit
//
void MainWindow::on_actionQuit_triggered()
{
    qApp->closeAllWindows();
}

//
// Neural Network -> Configure Training Set
//
void MainWindow::on_actionConfigureTrainingSet_triggered()
{
    auto* network = currentNetwork();

    TrainingTableDialog dialog(network->trainingTableModel(), this);
    if (network->isTraining() || network->isTrainingPaused())
        dialog.setReadOnly(true);

    dialog.exec();
}

//
// Neural Network -> Compute Custom Input
//
void MainWindow::on_actionComputeCustomInput_triggered()
{
    CustomInputDialog dialog(currentNetwork(), this);
    dialog.exec();
}

//
// Neural Network -> Rename Network
//
void MainWindow::on_actionRenameNetwork_triggered()
{
    RenameNetworkDialog dialog(currentNetwork(), this);
    dialog.exec();
}

//
// Neural Network -> Reset Weights
//
void MainWindow::on_actionResetWeights_triggered()
{
    ResetWeightsDialog dialog(currentNetwork(), this);
    dialog.exec();
}

//
// Neural Network -> Visualize Connection Weights
//
void MainWindow::on_actionShowConnectionWeights_triggered(bool checked)
{
    auto* widget = currentNetworkWidget();

    widget->setShowConnectionWeights(checked);
    writeProgramSettings();
}

//
// Neural Network -> Visualize Input Values
//
void MainWindow::on_actionShowInputValues_triggered(bool checked)
{
    auto* widget = currentNetworkWidget();

    widget->setShowInputValues(checked);
    writeProgramSettings();
}

//
// Neural Network -> Visualize Neuron Values
//
void MainWindow::on_actionShowNeuronValues_triggered(bool checked)
{
    auto* widget = currentNetworkWidget();

    widget->setShowNeuronValues(checked);
    writeProgramSettings();
}

//
// Neural Network -> Reset Network View
//
void MainWindow::on_actionResetNetworkView_triggered()
{
    auto* widget = currentNetworkWidget();

    for (auto* dockWidget : qAsConst(m_dockWidgets)) {
        auto* resettable = dynamic_cast<Resettable*>(dockWidget->widget());
        if (resettable != nullptr)
            resettable->reset();
    }
    widget->resetDiagram();
}

//
// Window -> Full Screen
//
void MainWindow::on_actionFullScreen_triggered()
{
    auto state = windowState();
    if (ui->actionFullScreen->isChecked()) {
        m_savedWindowState = state;
        state = Qt::WindowFullScreen;
    } else
        state = m_savedWindowState;

    setWindowState(state);
}

//
// Window -> Options
//
void MainWindow::on_actionOptions_triggered()
{
    OptionsDialog dialog(this);

    dialog.exec();
    if (!dialog.isModified())
        return;

    // Read the new settings in all main windows
    const auto windows = QApplication::topLevelWidgets();
    for (auto* window : windows) {
        auto* mainWindow = qobject_cast<MainWindow*>(window);
        if (mainWindow == nullptr)
            continue;
        mainWindow->readProgramSettings();
    }
}

bool MainWindow::hasOtherMainWindow() const
{
    const auto windows = QApplication::topLevelWidgets();
    for (auto* window : windows) {
        auto* mainWindow = qobject_cast<MainWindow*>(window);
        if (mainWindow != nullptr && mainWindow != this)
            return true;
    }
    return false;
}

//
// Help -> Help
//
void MainWindow::on_actionHelp_triggered()
{
    HelpBrowser dialog(this);
    dialog.exec();
}

//
// Help -> About
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(
                this,
                tr("About %1").arg(PROGRAM_NAME),
                tr("<h3>%1</h3>"
                   "<h4>Version %2</h4>"
                   "<p>Copyright &copy; 2016-2017 Michal Ratajsky</p>"
                   "<p><a href='https://www.github.com/mratajsky/nnlv'>https://www.github.com/mratajsky/nnlv</a></p>"
                   "<p>Uses Qt version %3</p>"
                   "<p>This program is free software: you can redistribute it and/or modify "
                   "it under the terms of the GNU General Public License as published by "
                   "the Free Software Foundation, either version 3 of the License, or "
                   "(at your option) any later version.</p>")
                .arg(PROGRAM_TITLE)
                .arg(PROGRAM_VERSION)
                .arg(qVersion()));
}
