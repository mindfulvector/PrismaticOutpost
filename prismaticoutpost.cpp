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
#include "prismaticoutpost.h"
#include "scripteditor.h"

#include <QMdiSubWindow>
#include <QInputDialog>

PrismaticOutpost::PrismaticOutpost(QWidget *parent)
    : QMainWindow(parent)
{
    setupMdiArea();
    createActions();
}

PrismaticOutpost::~PrismaticOutpost() {}

void PrismaticOutpost::setupMdiArea()
{
    mdiArea = new QMdiArea();
    setCentralWidget(mdiArea);
}

void PrismaticOutpost::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *newToolWindowAction = fileMenu->addAction(tr("New Tool Window"));
    connect(newToolWindowAction, &QAction::triggered, this, &PrismaticOutpost::createNewToolWindow);

    QAction *newMenuWindowAction = fileMenu->addAction(tr("New Menu Window"));
    connect(newMenuWindowAction, &QAction::triggered, this, &PrismaticOutpost::createNewMenuWindow);
}

void PrismaticOutpost::createNewToolWindow()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("New Tool Window"),
                                         tr("Tool window name:"), QLineEdit::Normal,
                                         tr("New Tool Window"), &ok);
    if (ok && !name.isEmpty()) {
        ToolWindow *toolWindow = new ToolWindow(name);
        QMdiSubWindow *subWindow = mdiArea->addSubWindow(toolWindow);
        subWindow->show();
        toolWindows[name] = toolWindow;

        connect(toolWindow, &ToolWindow::itemClicked, this, &PrismaticOutpost::openScriptEditor);
    }
}

void PrismaticOutpost::createNewMenuWindow()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("New Menu Window"),
                                         tr("Menu window name:"), QLineEdit::Normal,
                                         tr("New Menu Window"), &ok);
    if (ok && !name.isEmpty()) {
        MenuWindow *menuWindow = new MenuWindow(name);
        QMdiSubWindow *subWindow = mdiArea->addSubWindow(menuWindow);
        subWindow->show();
        menuWindows[name] = menuWindow;

        connect(menuWindow, &MenuWindow::itemClicked, this, &PrismaticOutpost::openScriptEditor);
    }
}

void PrismaticOutpost::openScriptEditor(const QString &buttonText)
{
    ScriptEditor *editor = new ScriptEditor(this);
    editor->setWindowTitle(buttonText);
    QMdiSubWindow *subWindow = mdiArea->addSubWindow(editor);
    subWindow->resize(400,600);
    subWindow->show();
}

void PrismaticOutpost::saveConfiguration()
{
    // Save ToolWindows
    for (auto it = toolWindows.begin(); it != toolWindows.end(); ++it) {
        QString key = "toolwindows." + it.key();
        dbManager.setValue(key, it.value()->getItemNames());
    }

    // Save MenuWindows
    for (auto it = menuWindows.begin(); it != menuWindows.end(); ++it) {
        QString key = "menuwindows." + it.key();
        dbManager.setValue(key, it.value()->getItemNames());
    }
}

void PrismaticOutpost::loadConfiguration()
{
    // Load ToolWindows
    QStringList toolWindowKeys = dbManager.getChildKeys("toolwindows");
    for (const QString &key : toolWindowKeys) {
        QString name = key.section('.', -1);
        ToolWindow *toolWindow = new ToolWindow(name, this);
        addDockWidget(Qt::TopDockWidgetArea, toolWindow);
        toolWindows[name] = toolWindow;

        QStringList buttons = dbManager.getValue(key).toStringList();
        for (const QString &button : buttons) {
            toolWindow->addItem(button);
        }

        connect(toolWindow, &ToolWindow::itemClicked, this, &PrismaticOutpost::openScriptEditor);
    }

    // Load MenuWindows
    QStringList menuWindowKeys = dbManager.getChildKeys("menuwindows");
    for (const QString &key : menuWindowKeys) {
        QString name = key.section('.', -1);
        MenuWindow *menuWindow = new MenuWindow(name, this);
        addDockWidget(Qt::LeftDockWidgetArea, menuWindow);
        menuWindows[name] = menuWindow;

        QStringList items = dbManager.getValue(key).toStringList();
        for (const QString &item : items) {
            menuWindow->addItem(item);
        }

        connect(menuWindow, &MenuWindow::itemClicked, this, &PrismaticOutpost::openScriptEditor);
    }
}
