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
// toolwindow.h
#ifndef TOOLWINDOW_H
#define TOOLWINDOW_H

#include <QDockWidget>
#include <QVector>
#include <QPushButton>
#include <QMenu>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>

class ToolWindow : public QDockWidget
{
    Q_OBJECT

public:
    enum LayoutType {
        HorizontalLayout,
        VerticalLayout
    };

    explicit ToolWindow(const QString &name, LayoutType layoutType = HorizontalLayout, QWidget *parent = nullptr);
    void addItem(const QString &text, const QString &scriptPath = QString());
    QStringList getItemNames() const;
    QStringList getRemovedItemNames() const;
    QString setScriptPath(const QString &itemName, const QString &scriptPath);
    QString getScriptPath(const QString &itemName) const;
    LayoutType getLayoutType() const { return layoutType; }

signals:
    void itemClicked(const QString &itemText, const QString &scriptPath);
    void editScriptRequested(const QString &itemText, const QString &scriptPath);
    void deleteItemRequested(const QString &itemText);
    void configurationChanged();

protected:
    //void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void showItemContextMenu(const QPoint &pos);
    void renameItem(QPushButton* button);
    void editItemScript(QPushButton* button);
    void deleteItem(QPushButton* button);

private:
    QWidget *containerWidget;
    QVector<QPushButton*> items;
    QPushButton *addButton;
    QMap<QString, QString> itemScripts;
    QStringList removedItemNames;
    QBoxLayout *layout;
    LayoutType layoutType;

    void setupUI();
    void setupItemContextMenu(QPushButton *button);
};

#endif
