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

#include <QDir>
#include <QLabel>
#include <QMainWindow>
#include <QSettings>

#include "maindockwidget.h"
#include "network.h"
#include "networkinfo.h"
#include "networkviewwidget.h"
#include "savednetwork.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    explicit MainWindow(NetworkViewWidget* widget, QWidget* parent = nullptr);
    explicit MainWindow(const QString& fileName, QWidget* parent = nullptr);
    ~MainWindow();

    bool hasNetworkWidget() const;

public slots:
    void showNewNetworkWizard();

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionClose_triggered();
    void on_actionQuit_triggered();
    void on_actionConfigureTrainingSet_triggered();
    void on_actionComputeCustomInput_triggered();
    void on_actionRenameNetwork_triggered();
    void on_actionResetWeights_triggered();
    void on_actionShowConnectionWeights_triggered(bool checked);
    void on_actionShowInputValues_triggered(bool checked);
    void on_actionShowNeuronValues_triggered(bool checked);
    void on_actionResetNetworkView_triggered();
    void on_actionFullScreen_triggered();
    void on_actionOptions_triggered();
    void on_actionHelp_triggered();
    void on_actionAbout_triggered();
    void resizeDockWidgets();
    void updateNetworkChangeConnections();
    void updateStatusBarMessage();
    void updateWindowTitle();

private:
    void init();
    bool activateWindowForFile(const QString& filePath);
    QString createStatusBarMessage();
    NetworkViewWidget* createNetworkViewWidget(Network* network);
    NetworkViewWidget* createNetworkViewWidget(const NetworkInfo::Map& map);
    NetworkViewWidget* createNetworkViewWidget(const SavedNetwork& savedNetwork);
    Network* currentNetwork() const;
    NetworkViewWidget*currentNetworkWidget() const;
    bool hasNewNetworkWidget() const;
    bool hasOtherMainWindow() const;
    bool loadNetwork();
    bool loadNetwork(const QString& filePath);
    bool loadNetwork(const QString& filePath, SavedNetwork& savedNetwork, bool show = true);
    bool loadNetwork(SavedNetwork& savedNetwork, bool show = true);
    void readProgramSettings();
    void readWindowSettings();
    bool saveNetwork(const QString& filePath);
    void setDockWidgets(const QVector<MainDockWidget*>& widgets);
    void setModified(bool modified);
    void setNetworkWidget(NetworkViewWidget* widget);
    void showSavedNetwork(SavedNetwork& savedNetwork, const QString& filePath = QString(), bool enableSave = false);
    void writeProgramSettings();
    void writeWindowSettings();

    static QDir m_currentDir;

    Ui::MainWindow* ui;
    bool m_isModified = false;
    bool m_saveModified = false;
    bool m_networkConnectionsCreated = false;
    bool m_newNetworkWizardPending = false;
    bool m_markModifiedWeights = false;
    QMap<QString, QSize> m_dockSizes;
    QVector<MainDockWidget*> m_dockWidgets;
    QMetaObject::Connection m_markModifiedWeightsConnection;
    Qt::WindowStates m_savedWindowState;
    QSettings m_settings;
    QLabel* m_statusLabel;
};
