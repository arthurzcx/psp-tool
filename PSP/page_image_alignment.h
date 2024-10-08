/*
 * Copyright (C) 2024 Arthur Zhang <arthur.zhang.cx@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <QDialog>
#include <QQuickItem>
#include <QQuickWidget>

namespace Ui {
    class PageImageAlignment;
}

class LogicPageImageAlignment;
class PageImageAlignment : public QDialog {
Q_OBJECT

public:
    explicit PageImageAlignment(QWidget *parent = nullptr);
    ~PageImageAlignment();

private:
    Ui::PageImageAlignment *ui;

    QQuickItem *mQuickItem;

    LogicPageImageAlignment *logicPageImageAlignment;
};

class LogicPageImageAlignment : public QObject {
Q_OBJECT

public:
    LogicPageImageAlignment();
    ~LogicPageImageAlignment();

    Q_INVOKABLE void selectReferenceImage();
    Q_INVOKABLE void selectAlignmentImage();

    Q_INVOKABLE void emitReject();
    Q_INVOKABLE void emitAccept();

signals:
    void updateReferenceImage(const QString &referenceImagePath);
    void updateAlignmentImage(const QString &alignmentImagePath);

    void reject();
    void accept();

private:
    bool selectImage(bool isReference);

    void alignment();

private:
    QString mReferenceImagePath;
    QString mReferenceMarkerPath;

    QString mAlignmentImagePath;
    QString mAlignmentMarkerPath;

    QString mPlatformPrex;
};
