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
#include "networklayoutwidget.h"
#include "ui_networklayoutwidget.h"

#include <QPainter>
#include <QPixmap>
#include <QScopedPointer>
#include <QShortcut>

#include "colors.h"
#include "networklayerdialog.h"
#include "networklimits.h"
#include "networkneurondialog.h"

NetworkLayoutWidget::NetworkLayoutWidget(NetworkViewWidget* viewWidget, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkLayoutWidget),
    m_network(viewWidget->network()),
    m_networkViewWidget(viewWidget)
{
    ui->setupUi(this);
    init();
}

NetworkLayoutWidget::~NetworkLayoutWidget()
{
    delete ui;
}

void NetworkLayoutWidget::init()
{
    initTreeLayers();
    //
    // Handle renames of layers and neurons
    //
    connect(ui->treeLayers, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item) {
        auto* parentItem = item->parent();
        if (parentItem == nullptr) {
            auto* layer = m_network->layer(item->data(0, Qt::UserRole).toInt());

            const auto& text = item->text(0);
            if (text.isEmpty()) {
                //
                // Empty name is not allowed, restore the original name
                //
                ui->treeLayers->blockSignals(true);
                item->setText(0, layer->name());
                ui->treeLayers->blockSignals(false);
                return;
            }
            //
            // The old and new name will be the same if the user changed the name
            // through a dialog
            //
            if (text != layer->name()) {
                qDebug() << "User changed layer name from"
                         << layer->name() << "to"
                         << text;
                layer->setName(text.simplified().left(NetworkLimits::maxLayerNameLength));
            }
        } else {
            auto* layer = m_network->layer(parentItem->data(0, Qt::UserRole).toInt());

            auto* neuron = layer->neuron(item->data(0, Qt::UserRole).toInt());
            const auto& text = item->text(0);
            if (text.isEmpty()) {
                //
                // Empty name is not allowed, restore the original name
                //
                ui->treeLayers->blockSignals(true);
                item->setText(0, neuron->name());
                ui->treeLayers->blockSignals(false);
                return;
            }
            if (text != layer->name()) {
                qDebug() << "User changed neuron name from"
                         << neuron->name() << "to"
                         << text;
                neuron->setName(text.simplified().left(NetworkLimits::maxNeuronNameLength));
            }
        }
    });

    initTreeMenus();
}

void NetworkLayoutWidget::initTreeLayers()
{
    ui->treeLayers->blockSignals(true);
    ui->treeLayers->clear();

    for (int i = 0; i < m_network->layerCount(); i++) {
        auto* layer = m_network->layer(i);
        auto* layerWidgetItem = new QTreeWidgetItem;
        layerWidgetItem->setData(0, Qt::UserRole, i);
        layerWidgetItem->setText(0, layer->name());
        layerWidgetItem->setFlags(Qt::ItemIsSelectable |
                                  Qt::ItemIsEditable |
                                  Qt::ItemIsEnabled);
        connect(layer, &NetworkLayer::nameChanged, this, [layer, layerWidgetItem] {
            layerWidgetItem->setText(0, layer->name());
        });
        ui->treeLayers->addTopLevelItem(layerWidgetItem);

        //
        // Find out the height of the item in the tree, which will be used to
        // determine size of neurons icons.
        //
        // This seems to only work for the top-level items, but they should all
        // be the equal size.
        //
        int height = ui->treeLayers->visualItemRect(layerWidgetItem).height();

        //
        // Add neuron items to the layer
        //
        for (int j = 0; j < layer->neuronCount(); j++) {
            auto* neuron = layer->neuron(j);
            auto* neuronWidgetItem = new QTreeWidgetItem;
            neuronWidgetItem->setData(0, Qt::UserRole, j);
            neuronWidgetItem->setText(0, neuron->name());
            neuronWidgetItem->setFlags(Qt::ItemIsSelectable |
                                       Qt::ItemIsEditable |
                                       Qt::ItemIsEnabled);
            connect(neuron, &NetworkNeuron::nameChanged, this, [neuron, neuronWidgetItem] {
                neuronWidgetItem->setText(0, neuron->name());
            });
            layerWidgetItem->addChild(neuronWidgetItem);
            //
            // Add icon to the neuron, use 75% of the item height to leave out
            // some space above and below the icons
            //
            QPixmap pixmap(height * .75, height * .75);

            if (layer->infoType() == NetworkLayerInfo::Type::Input && !neuron->isBias()) {
                pixmap.fill(Colors::ColorNeuronInitial);
            } else {
                pixmap.fill(Qt::transparent);

                QPainter painter(&pixmap);
                if (neuron->isBias()) {
                    painter.setPen(QPen(Colors::ColorBias));
                    painter.setBrush(Colors::ColorBias);
                } else {
                    painter.setPen(QPen(Colors::ColorNeuronInitial));
                    painter.setBrush(Colors::ColorNeuronInitial);
                }
                painter.drawEllipse(pixmap.rect().center(), pixmap.width()/2, pixmap.height()/2);
            }
            neuronWidgetItem->setIcon(0, QIcon(pixmap));
        }
        layerWidgetItem->setExpanded(true);
    }
    ui->treeLayers->blockSignals(false);
}

void NetworkLayoutWidget::initTreeMenus()
{
    auto* layerMenu = new QMenu(this);

    //
    // Add layer menu actions
    //
    layerMenu->addAction(tr("&Layer Properties..."), [=]() {
        showItemDialog(ui->treeLayers->currentItem());
    });
    layerMenu->addAction(tr("&Rename Layer"), [=]() {
        ui->treeLayers->editItem(ui->treeLayers->currentItem());
    }, QKeySequence(Qt::Key_F2));

    //
    // Add neuron menu actions
    //
    auto* neuronMenu = new QMenu(this);
    auto* neuronAction = neuronMenu->addAction(tr("&Neuron Properties..."), [=]() {
        showItemDialog(ui->treeLayers->currentItem());
    });
    neuronMenu->setDefaultAction(neuronAction);
    neuronMenu->addAction(tr("&Rename Neuron"), [=]() {
        ui->treeLayers->editItem(ui->treeLayers->currentItem());
    }, QKeySequence(Qt::Key_F2));

    //
    // Add custom keyboard shortcuts
    //
    QShortcut *shortcut;
    shortcut = new QShortcut(QKeySequence(Qt::Key_F2), ui->treeLayers);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(shortcut, &QShortcut::activated, this, [this] {
        ui->treeLayers->editItem(ui->treeLayers->currentItem());
    });

    //
    // Create layer tree context menu
    //
    connect(ui->treeLayers, &QTreeWidget::customContextMenuRequested, this,
            [this, neuronMenu, layerMenu](const QPoint& point) {
        auto* item = ui->treeLayers->itemAt(point);
        if (item == nullptr)
            return;
        if (item->parent() != nullptr)
            neuronMenu->exec(QCursor::pos());
        else
            layerMenu->exec(QCursor::pos());
    });
    ui->treeLayers->setContextMenuPolicy(Qt::CustomContextMenu);
}

void NetworkLayoutWidget::on_treeLayers_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);

    //
    // Layers collapse on doubleclick, so just show the properties dialog
    // on neurons
    //
    if (item->parent() != nullptr)
        showItemDialog(item);
}

void NetworkLayoutWidget::showItemDialog(QTreeWidgetItem* item)
{
    auto* parent = item->parent();

    QDialog* dialog;
    if (parent == nullptr) {
        // Layer item
        auto* layer = m_network->layer(item->data(0, Qt::UserRole).toInt());

        dialog = m_networkViewWidget->createNetworkLayerDialog(layer, this);
    } else {
        // Layer item
        auto* layer = m_network->layer(parent->data(0, Qt::UserRole).toInt());
        // Neuron item
        auto* neuron = layer->neuron(item->data(0, Qt::UserRole).toInt());

        dialog = m_networkViewWidget->createNetworkNeuronDialog(neuron, this);
    }
    dialog->exec();
    dialog->deleteLater();
}
