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

    connect(addButton, &QPushButton::clicked, [this]() {
        addItem("New Item");
    });

    setWidget(containerWidget);
}

void ToolWindow::addItem(const QString &text)
{
    QPushButton *item = new QPushButton(text, containerWidget);
    item->setFixedHeight(24);
    layout->insertWidget(layout->count() - 1, item);
    items.append(item);

    connect(item, &QPushButton::clicked, [this, text]() {
        emit itemClicked(text);
    });
}
