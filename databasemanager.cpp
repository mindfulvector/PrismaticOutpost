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
// databasemanager.cpp
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

bool DatabaseManager::openDatabase(const QString &path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open()) {
        qDebug() << "Error: connection with database failed";
        return false;
    }

    return initTables();
}

void DatabaseManager::closeDatabase()
{
    db.close();
}

bool DatabaseManager::initTables()
{
    QSqlQuery query;
    return query.exec("CREATE TABLE IF NOT EXISTS nodes "
                      "(key TEXT PRIMARY KEY, parent TEXT, value BLOB)");
}

QVariant DatabaseManager::getValue(const QString &key)
{
    QSqlQuery query;
    query.prepare("SELECT value FROM nodes WHERE key = :key");
    query.bindValue(":key", key);

    if (query.exec() && query.next()) {
        return query.value(0);
    }

    return QVariant();
}

bool DatabaseManager::setValue(const QString &key, const QVariant &value)
{
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO nodes (key, parent, value) "
                  "VALUES (:key, :parent, :value)");
    query.bindValue(":key", key);
    query.bindValue(":parent", key.section('.', 0, -2));
    query.bindValue(":value", value);

    return query.exec();
}

bool DatabaseManager::removeValue(const QString &key)
{
    QSqlQuery query;
    query.prepare("DELETE FROM nodes WHERE key = :key OR key LIKE :key_prefix");
    query.bindValue(":key", key);
    query.bindValue(":key_prefix", key + ".%");

    return query.exec();
}

QStringList DatabaseManager::getChildKeys(const QString &parentKey)
{
    QStringList children;
    QSqlQuery query;
    query.prepare("SELECT key FROM nodes WHERE parent = :parent");
    query.bindValue(":parent", parentKey);

    if (query.exec()) {
        while (query.next()) {
            children << query.value(0).toString();
        }
    }

    return children;
}
