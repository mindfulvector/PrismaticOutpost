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
// scripteditor.cpp
#include "scripteditor.h"
#include "toolwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextBlock>
#include <QMdiSubWindow>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>

//******************************************//
//****************** Setup *****************//
//******************************************//

ScriptEditor::ScriptEditor(ToolWindow &toolWindow, const QString &itemName, const QString &scriptPath, const QString &dbDir, QWidget *parent)
    : QWidget(parent), toolWindow(toolWindow), itemName(itemName), currentScriptPath(scriptPath), dbDir(dbDir), isDirty(false)
{
    setupUI();
    loadScripts();
    if (!scriptPath.isEmpty()) {
        loadScript(scriptPath);
    }
    fileWatcher.addPath(dbDir);
    connect(&fileWatcher, &QFileSystemWatcher::directoryChanged, this, &ScriptEditor::refreshScriptList);
}

void ScriptEditor::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *topLayout = new QHBoxLayout();
    scriptNameCombo = new QComboBox(this);
    symbolCombo = new QComboBox(this);
    topLayout->addWidget(scriptNameCombo);
    topLayout->addWidget(symbolCombo);

    scriptEdit = new QPlainTextEdit(this);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    saveButton = new QPushButton(tr("Save"), this);
    revertButton = new QPushButton(tr("Revert"), this);
    closeButton = new QPushButton(tr("Close"), this);
    bottomLayout->addWidget(saveButton);
    bottomLayout->addWidget(revertButton);
    bottomLayout->addWidget(closeButton);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scriptEdit);
    mainLayout->addLayout(bottomLayout);

    connect(saveButton, &QPushButton::clicked, this, &ScriptEditor::saveScript);
    connect(revertButton, &QPushButton::clicked, this, &ScriptEditor::revertScript);
    connect(closeButton, &QPushButton::clicked, this, &ScriptEditor::closeEditor);
    connect(scriptEdit, &QPlainTextEdit::textChanged, this, &ScriptEditor::updateSymbolList);
    connect(symbolCombo, QOverload<int>::of(&QComboBox::activated), this, &ScriptEditor::jumpToSymbolByIndex);

    connect(scriptNameCombo, QOverload<int>::of(&QComboBox::activated), this,
            [this](int index) {
                if (maybeSave()) {
                    QString newPath = scriptNameCombo->itemData(index).toString();
                    if (newPath.isEmpty()) {
                        QString newName = QInputDialog::getText(this, tr("New Script"), tr("Enter script name:"));
                        if (!newName.isEmpty()) {
                            newPath = dbDir + "/" + newName + ".scm";
                            QFile file(newPath);
                            file.open(QIODevice::WriteOnly);
                            file.close();
                            refreshScriptList();
                        }
                    }
                    if (!newPath.isEmpty()) {
                        toolWindow.setScriptPath(itemName, newPath);
                        loadScript(newPath);
                    }
                }
            });

    connect(scriptEdit, &QPlainTextEdit::textChanged, this,
            [this]() {
        isDirty = true;
        updateWindowTitle();
    });
}


//******************************************//
//********** MDI / Window Handling *********//
//******************************************//

QMdiSubWindow* ScriptEditor::getMdiParent()
{
    QWidget* parent = parentWidget();
    while (parent) {
        if (QMdiSubWindow* mdiParent = qobject_cast<QMdiSubWindow*>(parent)) {
            return mdiParent;
        }
        parent = parent->parentWidget();
    }
    return nullptr;
}


void ScriptEditor::closeEditor()
{
    if (maybeSave()) {
        this->getMdiParent()->close();
    }
}

void ScriptEditor::updateWindowTitle()
{
    QString title = QString("%1 - %2").arg(itemName, QFileInfo(currentScriptPath).fileName());
    if (isDirty) {
        title += " *";
    }
    QMdiSubWindow* mdiParent = this->getMdiParent();
    if (mdiParent) {
        mdiParent->setWindowTitle(title);
    } else {
        setWindowTitle(title);
        qWarning() << "ScriptEditor: MDI parent is null when updating window title";
    }
}


//******************************************//
//************* Script Handling ************//
//******************************************//

void ScriptEditor::refreshScriptList()
{
    QString currentScript = scriptNameCombo->currentData().toString();
    loadScripts();
    int index = scriptNameCombo->findData(currentScript);
    if (index != -1) {
        scriptNameCombo->setCurrentIndex(index);
    }
}

void ScriptEditor::loadScripts()
{
    scriptNameCombo->clear();
    scriptNameCombo->addItem(tr("New Script"), QString());

    QDir dir(dbDir);
    QStringList scripts = dir.entryList(QStringList() << "*.scm", QDir::Files);
    for (const QString &script : scripts) {
        scriptNameCombo->addItem(QFileInfo(script).baseName(), dbDir + "/" + script);
    }

    int index = scriptNameCombo->findData(currentScriptPath);
    if (index != -1) {
        scriptNameCombo->setCurrentIndex(index);
    }
}

void ScriptEditor::loadScript(const QString &path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        scriptEdit->setPlainText(file.readAll());
        currentScriptPath = path;
        isDirty = false;
        updateWindowTitle();
        updateSymbolList();
    }
}


bool ScriptEditor::maybeSave()
{
    if (!isDirty) {
        return true;
    }

    QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Unsaved Changes"),
                                                           tr("The script has been modified.\nDo you want to save your changes?"),
                                                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        saveScript();
        return true;
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }

    return true;
}

void ScriptEditor::saveScript()
{
    toolWindow.setScriptPath(itemName, currentScriptPath);

    QFile file(currentScriptPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << scriptEdit->toPlainText();
        isDirty = false;
        updateWindowTitle();
    }
}

void ScriptEditor::revertScript()
{
    if (QMessageBox::question(this, tr("Revert Changes"), tr("Are you sure you want to revert all changes?")) == QMessageBox::Yes) {
        loadScript(currentScriptPath);
        toolWindow.setScriptPath(itemName, currentScriptPath);
    }
}

void ScriptEditor::scriptChanged(const QString &path)
{
    if (path == currentScriptPath && !isDirty) {
        toolWindow.setScriptPath(itemName, currentScriptPath);
        loadScript(path);
    }
}


//******************************************//
//********** Symbol List Handling **********//
//******************************************//

void ScriptEditor::updateSymbolList()
{
    symbolCombo->clear();
    QStringList symbols;
    QTextBlock block = scriptEdit->document()->begin();

    while (block.isValid()) {
        QString text = block.text().trimmed();
        if (text.startsWith("(define ")) {
            int nameStart = text.indexOf("(define ") + 8;
            int nameEnd = text.indexOf(" ", nameStart);
            if (nameEnd != -1) {
                symbols << text.mid(nameStart, nameEnd - nameStart);
            }
        }
        block = block.next();
    }

    symbolCombo->addItems(symbols);
}

void ScriptEditor::jumpToSymbolByIndex(int index)
{
    if (index >= 0 && index < symbolCombo->count()) {
        jumpToSymbol(symbolCombo->itemText(index));
    }
}

void ScriptEditor::jumpToSymbol(const QString &symbol)
{
    QTextBlock block = scriptEdit->document()->begin();
    while (block.isValid()) {
        if (block.text().contains("(define " + symbol)) {
            QTextCursor cursor(block);
            scriptEdit->setTextCursor(cursor);
            scriptEdit->centerCursor();
            scriptEdit->setFocus(Qt::OtherFocusReason);
            break;
        }
        block = block.next();
    }
}
