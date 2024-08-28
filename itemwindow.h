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
#ifndef ITEMWINDOW_H
#define ITEMWINDOW_H

#include <QDockWidget>
#include <QVector>
#include <QPushButton>
#include <QMenu>
#include <QContextMenuEvent>

class ItemWindow : public QDockWidget
{
    Q_OBJECT

public:
    explicit ItemWindow(const QString &name, QWidget *parent = nullptr);
    virtual void addItem(const QString &text, const QString &scriptPath = QString()) = 0;
    QStringList getItemNames() const;
    QString getScriptPath(const QString &itemName) const;

signals:
    void itemClicked(const QString &itemText, const QString &scriptPath);
    void editScriptRequested(const QString &itemText, const QString &scriptPath);

protected:
    QWidget *containerWidget;
    QVector<QPushButton*> items;
    QPushButton *addButton;
    QMap<QString, QString> itemScripts;

    virtual void setupUI() = 0;
    void contextMenuEvent(QContextMenuEvent *event) override;

public slots:
    void showContextMenu(const QPoint &pos);

private slots:
    void editScript();
    void renameItem();
};

#endif // ITEMWINDOW_H
