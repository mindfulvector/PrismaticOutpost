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

#include <QWindow>
#include <QMdiSubWindow>
#include <QInputDialog>
#include <QScreen>
#include <QGuiApplication>

PrismaticOutpost::PrismaticOutpost(QWidget *parent)
    : QMainWindow(parent)
{
    setupMdiArea();
    createActions();

    // Open the database
    if (!dbManager.openDatabase("prismaticoutpost.db")) {
        qDebug() << "Failed to open database";
        // Handle the error appropriately
    }

    loadConfiguration();

    // Connect itemClicked signals to executeScript for all windows
    for (auto it = toolWindows.begin(); it != toolWindows.end(); ++it) {
        connect(it.value(), &ToolWindow::itemClicked, this, &PrismaticOutpost::executeScript);
        connect(it.value(), &ToolWindow::editScriptRequested, this, &PrismaticOutpost::openScriptEditor);
    }
    for (auto it = menuWindows.begin(); it != menuWindows.end(); ++it) {
        connect(it.value(), &MenuWindow::itemClicked, this, &PrismaticOutpost::executeScript);
        connect(it.value(), &MenuWindow::editScriptRequested, this, &PrismaticOutpost::openScriptEditor);
    }

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
        toolWindow->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
        addDockWidget(Qt::TopDockWidgetArea, toolWindow);
        toolWindows[name] = toolWindow;
        connect(toolWindow, &ToolWindow::itemClicked, this, &PrismaticOutpost::executeScript);
        connect(toolWindow, &ToolWindow::editScriptRequested, this, &PrismaticOutpost::openScriptEditor);
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
        menuWindow->setAllowedAreas(Qt::AllDockWidgetAreas);
        addDockWidget(Qt::LeftDockWidgetArea, menuWindow);
        menuWindows[name] = menuWindow;
        connect(menuWindow, &MenuWindow::itemClicked, this, &PrismaticOutpost::executeScript);
        connect(menuWindow, &MenuWindow::editScriptRequested, this, &PrismaticOutpost::openScriptEditor);
    }
}

void PrismaticOutpost::openScriptEditor(const QString &itemName, const QString &scriptPath)
{
    QString actualScriptPath = scriptPath;
    if (actualScriptPath.isEmpty()) {
        ItemWindow *window = qobject_cast<ItemWindow*>(sender());
        if (window) {
            actualScriptPath = getScriptPath(itemName, window);
        }
    }

    ScriptEditor *editor = new ScriptEditor(itemName, actualScriptPath, dbManager.getDatabaseDirectory(), this);
    QMdiSubWindow *subWindow = mdiArea->addSubWindow(editor);
    subWindow->resize(400, 600);
    subWindow->show();
}

QString PrismaticOutpost::getScriptPath(const QString &itemName, ItemWindow *window)
{
    QString scriptPath = window->getScriptPath(itemName);
    if (scriptPath.isEmpty()) {
        scriptPath = dbManager.getDatabaseDirectory() + "/" + itemName + ".scm";
    }
    return scriptPath;
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

    // Save script paths
    for (auto it = toolWindows.begin(); it != toolWindows.end(); ++it) {
        QString key = "toolwindows." + it.key() + ".scripts";
        QVariantMap scriptMap;
        for (const QString &itemName : it.value()->getItemNames()) {
            scriptMap[itemName] = it.value()->getScriptPath(itemName);
        }
        dbManager.setValue(key, scriptMap);
    }

    for (auto it = menuWindows.begin(); it != menuWindows.end(); ++it) {
        QString key = "menuwindows." + it.key() + ".scripts";
        QVariantMap scriptMap;
        for (const QString &itemName : it.value()->getItemNames()) {
            scriptMap[itemName] = it.value()->getScriptPath(itemName);
        }
        dbManager.setValue(key, scriptMap);
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
            // Buttons are initially added with no script bound to them
            toolWindow->addItem(button, "");
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
            // Buttons are initially added with no script bound to them
            menuWindow->addItem(item, "");
        }

        connect(menuWindow, &MenuWindow::itemClicked, this, &PrismaticOutpost::openScriptEditor);
    }

    // Load script paths
    for (const QString &key : toolWindowKeys) {
        QString name = key.section('.', -1);
        ToolWindow *toolWindow = toolWindows[name];

        QString scriptKey = "toolwindows." + name + ".scripts";
        QVariantMap scriptMap = dbManager.getValue(scriptKey).toMap();
        for (auto it = scriptMap.begin(); it != scriptMap.end(); ++it) {
            // Adding the same item key simply sets the script binding for it
            toolWindow->addItem(it.key(), it.value().toString());
        }
    }

    for (const QString &key : menuWindowKeys) {
        QString name = key.section('.', -1);
        MenuWindow *menuWindow = menuWindows[name];

        QString scriptKey = "menuwindows." + name + ".scripts";
        QVariantMap scriptMap = dbManager.getValue(scriptKey).toMap();
        for (auto it = scriptMap.begin(); it != scriptMap.end(); ++it) {
            // Adding the same item key simply sets the script binding for it
            menuWindow->addItem(it.key(), it.value().toString());
        }
    }
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
