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

ToolWindow::ToolWindow(const QString &name, QWidget *parent)
    : ItemWindow(name, parent)
{
    setupUI();
}

void ToolWindow::setupUI()
{
    containerWidget = new QWidget(this);
    layout = new QHBoxLayout(containerWidget);
    layout->setSpacing(2);
    layout->setContentsMargins(2, 2, 2, 2);

    addButton = new QPushButton("+", containerWidget);
    addButton->setFixedSize(24, 24);
    layout->addWidget(addButton);
    layout->setAlignment(Qt::AlignLeading);

    connect(addButton, &QPushButton::clicked, [this]() {
        // Add a new button without any script bound to it
        addItem(QStringLiteral("Btn#%1").arg(items.count()), "");
    });

    setWidget(containerWidget);
}

void ToolWindow::addItem(const QString &text, const QString &scriptPath)
{
    // Only add the item as a button to this window if it doesn't already exist
    bool shouldAddButton = !itemScripts.contains(text);

    ItemWindow::addItem(text, scriptPath);

    if(shouldAddButton) {
        QPushButton *item = items.last();
        item->setFixedSize(100, 24);
        layout->insertWidget(layout->count() - 1, item);
        layout->setStretch(layout->count() - 1, 0);
    }
}
