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
// toolwindow.cpp
#include "toolwindow.h"
#include "toolwindow.h"
#include <QInputDialog>

ToolWindow::ToolWindow(const QString &name, LayoutType layoutType, QWidget *parent)
    : QDockWidget(name, parent), layoutType(layoutType)
{
    setupUI();
}

void ToolWindow::setupUI()
{
    containerWidget = new QWidget(this);

    if (layoutType == HorizontalLayout) {
        layout = new QHBoxLayout(containerWidget);
        layout->setAlignment(Qt::AlignLeading);
        addButton = new QPushButton("+", containerWidget);
        addButton->setFixedSize(24, 24);
    } else {
        layout = new QVBoxLayout(containerWidget);
        layout->setAlignment(Qt::AlignTop);
        addButton = new QPushButton("+", containerWidget);
        addButton->setFixedSize(200, 24);
    }

    layout->setSpacing(2);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(addButton);

    connect(addButton, &QPushButton::clicked, [this]() {
        QString newItemText = (layoutType == HorizontalLayout)
                                  ? QStringLiteral("Btn#%1").arg(items.count())
                                  : QStringLiteral("Button #%1").arg(items.count());
        addItem(newItemText, "");
    });

    setWidget(containerWidget);
}

void ToolWindow::addItem(const QString &text, const QString &scriptPath)
{
    if (!itemScripts.contains(text)) {
        QPushButton *item = new QPushButton(text, containerWidget);

        if (layoutType == HorizontalLayout) {
            item->setFixedSize(100, 24);
        } else {
            item->setFixedSize(200, 24);
        }

        layout->insertWidget(layout->count() - 1, item);
        layout->setStretch(layout->count() - 1, 0);

        items.append(item);

        connect(item, &QPushButton::clicked, [this, text, scriptPath]() {
            emit itemClicked(text, scriptPath);
        });

        setupItemContextMenu(item);
    }

    itemScripts[text] = scriptPath;
}

void ToolWindow::setupItemContextMenu(QPushButton *button)
{
    QMenu *contextMenu = new QMenu(button);

    QAction *renameAction = contextMenu->addAction("Rename Item");
    QAction *editAction = contextMenu->addAction("Edit Script");
    QAction *deleteAction = contextMenu->addAction("Delete Item");

    connect(renameAction, &QAction::triggered, this, [this, button]() { this->renameItem(button); });
    connect(editAction, &QAction::triggered, this, [this, button]() { this->editItemScript(button); });
    connect(deleteAction, &QAction::triggered, this, [this, button]() { this->deleteItem(button); });

    button->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(button, &QPushButton::customContextMenuRequested, this, &ToolWindow::showItemContextMenu);
}

void ToolWindow::showItemContextMenu(const QPoint &pos)
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button && button != addButton) {
        QMenu *contextMenu = button->findChild<QMenu*>();
        if (contextMenu) {
            contextMenu->exec(button->mapToGlobal(pos));
        }
    }
}

void ToolWindow::renameItem(QPushButton* button)
{
    if (button) {
        QString oldName = button->text();
        QString newName = QInputDialog::getText(this, tr("Rename Item"), tr("New name:"), QLineEdit::Normal, oldName);
        if (!newName.isEmpty() && newName != oldName) {
            button->setText(newName);
            if (itemScripts.contains(oldName)) {
                // Add to list of items to be removed from DB when saved
                removedItemNames.append(oldName);

                // Add new item name using the same script binding
                itemScripts[newName] = itemScripts[oldName];
                itemScripts.remove(oldName);

            }
        }
        emit configurationChanged();
    }
}

void ToolWindow::editItemScript(QPushButton* button)
{
    if (button) {
        QString itemName = button->text();
        QString scriptPath = itemScripts.value(itemName);
        emit editScriptRequested(itemName, scriptPath);
    }
}

void ToolWindow::deleteItem(QPushButton* button)
{
    if (button) {
        QString itemName = button->text();
        emit deleteItemRequested(itemName);
        removedItemNames.append(itemName);
        items.removeOne(button);
        itemScripts.remove(itemName);
        layout->removeWidget(button);
        button->deleteLater();
        emit configurationChanged();
    }
}

QStringList ToolWindow::getItemNames() const
{
    QStringList names;
    for (const QPushButton *item : items) {
        names << item->text();
    }
    return names;
}

QStringList ToolWindow::getRemovedItemNames() const
{
    return removedItemNames;
}


QString ToolWindow::setScriptPath(const QString &itemName, const QString &scriptPath)
{
    itemScripts.insert(itemName, scriptPath);
    return itemScripts.value(itemName);
}

QString ToolWindow::getScriptPath(const QString &itemName) const
{
    return itemScripts.value(itemName);
}
