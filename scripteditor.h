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

class ScriptEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptEditor(QWidget *parent = nullptr);

private slots:
    void saveScript();
    void revertScript();
    void closeEditor();
    void updateSymbolList();
    void jumpToSymbolByIndex(int index);
    void jumpToSymbol(const QString &symbol);

private:
    QComboBox *scriptNameCombo;
    QComboBox *symbolCombo;
    QPlainTextEdit *scriptEdit;
    QPushButton *saveButton;
    QPushButton *revertButton;
    QPushButton *closeButton;


    void setupUI();
    QMdiSubWindow* getMdiParent();
    void loadScripts();
    void loadSymbols();
};

#endif // SCRIPTEDITOR_H
