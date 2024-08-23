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
// databasemanager.h
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariant>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool openDatabase(const QString &path);
    void closeDatabase();

    QVariant getValue(const QString &key);
    bool setValue(const QString &key, const QVariant &value);
    bool removeValue(const QString &key);

    QStringList getChildKeys(const QString &parentKey);

private:
    QSqlDatabase db;

    bool initTables();
};

#endif // DATABASEMANAGER_H
