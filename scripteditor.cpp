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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextBlock>
#include <QMdiSubWindow>

ScriptEditor::ScriptEditor(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadScripts();
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
    connect(symbolCombo, QOverload<int>::of(&QComboBox::activated),
            this, &ScriptEditor::jumpToSymbolByIndex);
}

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

void ScriptEditor::loadScripts()
{
    // TODO: Implement loading of existing scripts
    scriptNameCombo->addItem(tr("New Script"));
}

void ScriptEditor::saveScript()
{
    // TODO: Implement saving script
}

void ScriptEditor::revertScript()
{
    // TODO: Implement reverting script changes
}

void ScriptEditor::closeEditor()
{
    // TODO: Implement proper closing behavior
    this->getMdiParent()->close();
}

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
