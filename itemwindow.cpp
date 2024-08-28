/*
 * This file is part of the PrismaticOutpost project.
 * Copyright 2024. Isaac Raway. All rights reserved.
 * This file is licensed under the terms of the AGPL-3.0,
 * which you can find a copy of in the LICENSE.md file
 * https://www.gnu.org/licenses/agpl-3.0.md.
 * IMPORTANT: This license ALSO applies to all scripts
 * and databases packaged with this distribution of
 * PrismaticOutpost. If you wish to distribute proprietary
 * scripts or databases, then they MUST NOT be packaged
 * with this distribution or any binary distribution built
 * from this distribution or any derivative of this
 * distribution.
 */
#include "itemwindow.h"
#include <QMenu>
#include <QAction>
#include <QInputDialog>

ItemWindow::ItemWindow(const QString &name, QWidget *parent)
    : QDockWidget(name, parent)
{
}

QStringList ItemWindow::getItemNames() const
{
    QStringList names;
    for (const QPushButton *item : items) {
        names << item->text();
    }
    return names;
}

void ItemWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QWidget *child = childAt(event->pos());
    if (QPushButton *button = qobject_cast<QPushButton*>(child)) {
        if (button != addButton) {
            QMenu contextMenu(tr("Context menu"), this);
            QAction *editAction = contextMenu.addAction(tr("Edit script"));
            QAction *renameAction = contextMenu.addAction(tr("Rename"));

            connect(editAction, &QAction::triggered, this, &ItemWindow::editScript);
            connect(renameAction, &QAction::triggered, this, &ItemWindow::renameItem);

            contextMenu.exec(event->globalPos());
        }
    }
}

void ItemWindow::editScript()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender()->parent());
    if (button) {
        QString itemName = button->text();
        QString scriptPath = itemScripts.value(itemName);
        emit editScriptRequested(itemName, scriptPath);
    }
}

void ItemWindow::renameItem()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender()->parent());
    if (button) {
        QString oldName = button->text();
        QString newName = QInputDialog::getText(this, tr("Rename Item"), tr("New name:"), QLineEdit::Normal, oldName);
        if (!newName.isEmpty() && newName != oldName) {
            button->setText(newName);
            if (itemScripts.contains(oldName)) {
                itemScripts[newName] = itemScripts[oldName];
                itemScripts.remove(oldName);
            }
        }
    }
}

QString ItemWindow::getScriptPath(const QString &itemName) const
{
    return itemScripts.value(itemName);
}

void ItemWindow::showContextMenu(const QPoint &pos)
{
    QWidget *child = childAt(pos);
    if (QPushButton *button = qobject_cast<QPushButton*>(child)) {
        if (button != addButton) {
            QMenu contextMenu(this);
            QAction *editAction = contextMenu.addAction(tr("Edit script"));
            QAction *renameAction = contextMenu.addAction(tr("Rename"));

            connect(editAction, &QAction::triggered, this, [this, button]() {
                QString itemName = button->text();
                QString scriptPath = itemScripts.value(itemName);
                emit editScriptRequested(itemName, scriptPath);
            });

            connect(renameAction, &QAction::triggered, this, [this, button]() {
                QString oldName = button->text();
                QString newName = QInputDialog::getText(this, tr("Rename Item"), tr("New name:"), QLineEdit::Normal, oldName);
                if (!newName.isEmpty() && newName != oldName) {
                    button->setText(newName);
                    if (itemScripts.contains(oldName)) {
                        itemScripts[newName] = itemScripts[oldName];
                        itemScripts.remove(oldName);
                    }
                }
            });

            contextMenu.exec(mapToGlobal(pos));
        }
    }
}

void ItemWindow::addItem(const QString &text, const QString &scriptPath)
{
    // Note: Actual addition to the layout should be handled by the subclasses,
    // which should override this method and call it first in their implementation.

    // Only add the item if it doesn't already exist
    if(!itemScripts.contains(text)) {
        QPushButton *item = new QPushButton(text, containerWidget);
        item->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(item, &QPushButton::customContextMenuRequested, this, &ItemWindow::showContextMenu);

        items.append(item);

        connect(item, &QPushButton::clicked, [this, text, scriptPath]() {
            emit itemClicked(text, scriptPath);
        });
    }

    // Always set the script path, since we load configuration in two passes
    itemScripts[text] = scriptPath;

}

