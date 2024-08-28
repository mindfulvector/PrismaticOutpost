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

#include "itemwindow.h"
#include <QHBoxLayout>

class ToolWindow : public ItemWindow
{
    Q_OBJECT

public:
    explicit ToolWindow(const QString &name, QWidget *parent = nullptr);
    void addItem(const QString &text, const QString &scriptPath) override;

private:
    QHBoxLayout *layout;

    void setupUI() override;
};

#endif // TOOLWINDOW_H
