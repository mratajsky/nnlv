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
#include "helpbrowser.h"
#include "ui_helpbrowser.h"

#include <QFile>
#include <QUrl>

HelpBrowser::HelpBrowser(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::HelpBrowser)
{
    ui->setupUi(this);
    init();
}

HelpBrowser::~HelpBrowser()
{
    delete ui;
}

void HelpBrowser::init()
{
    // Restore window state
    QVariant value = m_settings.value("help-browser/geometry").toByteArray();
    if (value.isValid())
        restoreGeometry(value.toByteArray());

    populateTreeWidget();

    ui->textBrowser->setSearchPaths(QStringList(":/help"));
    ui->textBrowser->setSource(QUrl("qrc:/help/welcome.html"));
}

void HelpBrowser::done(int r)
{
    m_settings.setValue("help-browser/geometry", saveGeometry());
    QDialog::done(r);
}

void HelpBrowser::populateTreeWidget()
{
    QFile file(":/help/topics.xml");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QXmlStreamReader xml(&file);
    while (xml.readNextStartElement()) {
        if (xml.name() == "items")
            loadTopicItems(xml);
        else
            xml.skipCurrentElement();
    }
    ui->treeTopics->expandAll();
}

void HelpBrowser::loadTopicItems(QXmlStreamReader& xml, QTreeWidgetItem* parent)
{
    while (xml.readNextStartElement()) {
        if (xml.name() == "topic") {
            loadTopic(xml, parent);
        } else
            xml.skipCurrentElement();
    }
}

void HelpBrowser::loadTopic(QXmlStreamReader& xml, QTreeWidgetItem* parent)
{
    auto* item = new QTreeWidgetItem(parent);

    while (xml.readNextStartElement()) {
        if (xml.name() == "title")
            item->setText(0, xml.readElementText());
        else if (xml.name() == "file")
            item->setData(0, Qt::UserRole, xml.readElementText());
        else if (xml.name() == "items")
            loadTopicItems(xml, item);
        else
            xml.skipCurrentElement();
    }

    if (parent == nullptr)
        ui->treeTopics->addTopLevelItem(item);
}

void HelpBrowser::on_treeTopics_currentItemChanged(QTreeWidgetItem* current,
                                                   QTreeWidgetItem* previous)
{
    Q_UNUSED(previous);

    auto fileName = current->data(0, Qt::UserRole).toString();

    ui->textBrowser->setSource(QUrl(fileName));
}
