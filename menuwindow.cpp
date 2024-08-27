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
#include "menuwindow.h"

MenuWindow::MenuWindow(const QString &name, QWidget *parent)
    : ItemWindow(name, parent)
{
    setupUI();
}

void MenuWindow::setupUI()
{
    containerWidget = new QWidget(this);
    layout = new QVBoxLayout(containerWidget);
    layout->setSpacing(2);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->minimumSize().setWidth(300);

    addButton = new QPushButton("+", containerWidget);
    addButton->setFixedSize(200, 24);
    layout->addWidget(addButton);
    layout->setAlignment(Qt::AlignTop);

    connect(addButton, &QPushButton::clicked, [this]() {
        addItem("New Item");
    });

    setWidget(containerWidget);
}

void MenuWindow::addItem(const QString &text)
{
    QPushButton *item = new QPushButton(text, containerWidget);
    item->setFixedHeight(24);
    layout->insertWidget(layout->count() - 1, item);
    items.append(item);
    layout->setStretch(layout->count()-1, 0);
    item->setFixedSize(200, 24);

    connect(item, &QPushButton::clicked, [this, text]() {
        emit itemClicked(text);
    });
}

