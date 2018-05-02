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
#include "common.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QLocale>

#include "mainwindow.h"
#include "program.h"
#include "version.h"

int main(int argc, char *argv[])
{
    // Use C locale's number format
    QLocale locale = QLocale::system();
    locale.setNumberOptions(QLocale::c().numberOptions());
    QLocale::setDefault(locale);

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName(PROGRAM_ORG);
    QCoreApplication::setApplicationName(PROGRAM_NAME);
    QCoreApplication::setApplicationVersion(PROGRAM_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", QObject::tr("The file(s) to open."));
    parser.process(a);

    a.setWindowIcon(QIcon(":/icons/nnlv.svg"));

    bool hasNetwork = false;
    for (const auto& file : parser.positionalArguments()) {
        auto* window = new MainWindow(file);
        //
        // The main window will display an error dialog if loading fails.
        //
        // Delete the window after this and make sure there is always at most
        // one window without a network shown (if all files fail to load, there
        // should be a single empty window shown).
        //
        if (window->hasNetworkWidget()) {
            hasNetwork = true;
            window->show();
        } else {
            delete window;
            window = nullptr;
        }
    }
    if (!hasNetwork) {
        auto* window = new MainWindow;
        window->showNewNetworkWizard();
        window->show();
    }
    return a.exec();
}
