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
// prismaticoutpost.h
#ifndef PRISMATICOUTPOST_H
#define PRISMATICOUTPOST_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMap>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "toolwindow.h"
#include "menuwindow.h"
#include "databasemanager.h"

class ScriptEditor;

QT_BEGIN_NAMESPACE
namespace Ui { class PrismaticOutpost; }
QT_END_NAMESPACE

class PrismaticOutpost : public QMainWindow
{
    Q_OBJECT

public:
    PrismaticOutpost(QWidget *parent = nullptr);
    ~PrismaticOutpost();

private slots:
    void createNewToolWindow();
    void createNewMenuWindow();
    void openScriptEditor(const QString &itemName, const QString &scriptPath);
    void executeScript(const QString &itemName, const QString &scriptPath);
    void saveConfiguration();
    void loadConfiguration();

private:
    Ui::PrismaticOutpost *ui;
    QDockWidget dock1;
    QMdiArea *mdiArea;
    QMap<QString, ToolWindow*> toolWindows;
    QMap<QString, MenuWindow*> menuWindows;
    DatabaseManager dbManager;

    void centerOnScreen();
    void setupMdiArea();
    void createActions();
    void setupDatabase();
    QString getScriptPath(const QString &itemName, ItemWindow *window);
};

#endif // PRISMATICOUTPOST_H
