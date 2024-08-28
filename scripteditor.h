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
#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMdiSubWindow>
#include <QFileSystemWatcher>

class ScriptEditor : public QWidget
{
    Q_OBJECT

    //******************************************//
    //****************** Setup *****************//
    //******************************************//

    public: explicit ScriptEditor(const QString &itemName, const QString &scriptPath, const QString &dbDir, QWidget *parent = nullptr);
    private: void setupUI();

    //******************************************//
    //********** MDI / Window Handling *********//
    //******************************************//

    private: QMdiSubWindow* getMdiParent();
    private slots: void closeEditor();
    private: void updateWindowTitle();

    //******************************************//
    //************* Script Handling ************//
    //******************************************//

    private slots: void refreshScriptList();
    private: void loadScripts();
    private: void loadScript(const QString &path);
    private: bool maybeSave();
    private slots: void saveScript();
    private slots: void revertScript();
    private slots: void scriptChanged(const QString &path);

    //******************************************//
    //********** Symbol List Handling **********//
    //******************************************//

    private slots: void updateSymbolList();
    private slots: void jumpToSymbolByIndex(int index);
    private slots: void jumpToSymbol(const QString &symbol);


    //******************************************//
    //***************** Fields *****************//
    //******************************************//

    private:
        QString itemName;
        QString currentScriptPath;
        QString dbDir;
        QComboBox *scriptNameCombo;
        QComboBox *symbolCombo;
        QPlainTextEdit *scriptEdit;
        QPushButton *saveButton;
        QPushButton *revertButton;
        QPushButton *closeButton;
        QFileSystemWatcher fileWatcher;
        bool isDirty;

};

#endif // SCRIPTEDITOR_H
