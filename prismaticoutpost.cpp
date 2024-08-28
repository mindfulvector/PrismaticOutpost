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
// pristmaticoutpost.cpp
#include "prismaticoutpost.h"
#include "scripteditor.h"

#include <QWindow>
#include <QMdiSubWindow>
#include <QInputDialog>
#include <QScreen>
#include <QGuiApplication>
#include <QStringLiteral>

PrismaticOutpost::PrismaticOutpost(QWidget *parent)
    : QMainWindow(parent)
{
    setupMdiArea();
    createActions();

    // Open the database
    if (!dbManager.openDatabase("prismaticoutpost.db")) {
        qDebug() << "Failed to open database";
    }

    loadConfiguration();

    this->show();
    this->centerOnScreen();
}

PrismaticOutpost::~PrismaticOutpost() {
    saveConfiguration();
    dbManager.closeDatabase();
}

void PrismaticOutpost::centerOnScreen()
{
    QScreen *screen = this->windowHandle()->screen();
    QRect screenGeometry = screen->geometry();

    int x = screenGeometry.x() + (screenGeometry.width() - this->width()) / 2;
    int y = screenGeometry.y() + (screenGeometry.height() - this->height()) / 2;

    this->move(x, y);
}

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
}

void PrismaticOutpost::createNewToolWindow()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("New Tool Window"),
                                         tr("Tool window name:"), QLineEdit::Normal,
                                         tr("New Tool Window"), &ok);
    if (ok && !name.isEmpty()) {
        QStringList layoutOptions;
        layoutOptions << tr("Horizontal") << tr("Vertical");
        QString layoutChoice = QInputDialog::getItem(this, tr("Layout Type"),
                                                     tr("Choose layout type:"), layoutOptions, 0, false, &ok);
        if (ok && !layoutChoice.isEmpty()) {
            ToolWindow::LayoutType layoutType = (layoutChoice == tr("Horizontal")) ?
                                                    ToolWindow::HorizontalLayout :
                                                    ToolWindow::VerticalLayout;
            ToolWindow *toolWindow = new ToolWindow(name, layoutType);
            connect(toolWindow, &ToolWindow::configurationChanged, this, &PrismaticOutpost::saveConfiguration);
            toolWindow->setAllowedAreas(Qt::AllDockWidgetAreas);
            addDockWidget(layoutType == ToolWindow::HorizontalLayout ? Qt::TopDockWidgetArea : Qt::LeftDockWidgetArea, toolWindow);
            toolWindows[name] = toolWindow;
            connect(toolWindow, &ToolWindow::itemClicked, this, &PrismaticOutpost::executeScript);
            connect(toolWindow, &ToolWindow::editScriptRequested, this, &PrismaticOutpost::openScriptEditor);
        }
    }
}

void PrismaticOutpost::saveConfiguration()
{
    // Save ToolWindows
    for (auto it = toolWindows.begin(); it != toolWindows.end(); ++it) {
        QString key = "toolwindows." + it.key();
        dbManager.setValue(key, it.value()->getItemNames());

        // Save layout type
        QString layoutKey = key + ".layout";
        dbManager.setValue(layoutKey, static_cast<int>(static_cast<ToolWindow*>(it.value())->getLayoutType()));

        // Save script paths
        QString itemsKey = key + ".items";
        for (const QString &itemName : it.value()->getItemNames()) {
            QString itemKey = itemsKey + "." + itemName;
            QString scriptPath = it.value()->getScriptPath(itemName);
            dbManager.setValue(itemKey, scriptPath);
        }

    }
}

void PrismaticOutpost::loadConfiguration()
{
    // Load ToolWindows
    QStringList toolWindowKeys = dbManager.getChildKeys("toolwindows");
    for (const QString &key : toolWindowKeys) {
        QString name = key.section('.', -1);

        // Load layout type
        QString layoutKey = "toolwindows." + name + ".layout";
        ToolWindow::LayoutType layoutType = static_cast<ToolWindow::LayoutType>(dbManager.getValue(layoutKey).toInt());

        QString scriptKey = "toolwindows." + name + ".items";
        QStringList itemKeys = dbManager.getChildKeys(scriptKey).toVector();

        ToolWindow *toolWindow = new ToolWindow(name, layoutType, this);
        connect(toolWindow, &ToolWindow::configurationChanged, this, &PrismaticOutpost::saveConfiguration);
        addDockWidget(layoutType == ToolWindow::HorizontalLayout ? Qt::TopDockWidgetArea : Qt::LeftDockWidgetArea, toolWindow);
        toolWindows[name] = toolWindow;

        for (const auto &itemKey : itemKeys) {

            // Key path for this specific button key
            QString scriptPath = dbManager.getValue(itemKey).toString();

            // Get the button label
            QString buttonLabel = itemKey.section('.', -1);

            // Add button with label from script binding map and value (script name)
            toolWindow->addItem(buttonLabel, scriptPath);
        }

        connect(toolWindow, &ToolWindow::itemClicked, this, &PrismaticOutpost::executeScript);
        connect(toolWindow, &ToolWindow::editScriptRequested, this, &PrismaticOutpost::openScriptEditor);
    }
}

void PrismaticOutpost::openScriptEditor(const QString &itemName, const QString &scriptPath)
{
    ToolWindow *window = qobject_cast<ToolWindow*>(sender());
    if(!window) {
        qDebug() << QStringLiteral("Unable to open script editor on null ToolWindow reference!\n"
                                   "Trying to load script editor for item `%1`, script path `$%2`").arg(itemName).arg(scriptPath);
    }
    QString actualScriptPath = scriptPath;
    if (actualScriptPath.isEmpty()) {
        if (window) {
            actualScriptPath = getScriptPath(itemName, window);
        }
    }

    ScriptEditor *editor = new ScriptEditor(*window, itemName, actualScriptPath, dbManager.getDatabaseDirectory(), this);
    QMdiSubWindow *subWindow = mdiArea->addSubWindow(editor);
    subWindow->resize(400, 600);
    subWindow->show();
}

QString PrismaticOutpost::getScriptPath(const QString &itemName, ToolWindow *window)
{
    QString scriptPath = window->getScriptPath(itemName);
    if (scriptPath.isEmpty()) {
        scriptPath = dbManager.getDatabaseDirectory() + "/" + itemName + ".scm";
    }
    return scriptPath;
}

void PrismaticOutpost::executeScript(const QString &itemName, const QString &scriptPath)
{
    if (scriptPath.isEmpty()) {
        qDebug() << "No script associated with item:" << itemName;
        return;
    }

    QFile file(scriptPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open script file:" << scriptPath;
        return;
    }

    QString scriptContent = file.readAll();
    file.close();

    // TODO: Implement script execution logic here
    // This could involve passing the script content to an interpreter
    // or executing it in some other way depending on your requirements
    qDebug() << "Executing script for item:" << itemName;
    qDebug() << "Script content:" << scriptContent;
}
