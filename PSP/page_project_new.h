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
#include "manager/manager_doc.h"
#include "manager/manager_doc_single_component.h"
#include "manager/manager_doc_two_component.h"

using namespace PspNsManager;

namespace Ui {
    class PageProjectNew;
}

enum ProjectType {
    eSingleComponent = 0,
    eDualComponent = 1
};

class LogicProjectNew;

class PageProjectNew : public QDialog {
Q_OBJECT

public:
    explicit PageProjectNew(ProjectType type, QWidget *parent = nullptr);
    ~PageProjectNew();

    PspDocument *GetDocument();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::PageProjectNew *ui;
    ProjectType mProjectType;
    LogicProjectNew *logicProjectNew;
    QQuickItem *mQuickItem;
    PspDocument *mDocument;
};

bool ProcessSingleComponentDocument(PspDocumentSingleComponent *document);
bool ProcessDualComponentDocument(PspDocumentTwoComponent *document);

class LogicProjectNew : public QObject {
Q_OBJECT
public:
    LogicProjectNew();
    ~LogicProjectNew();

    enum ReadFileType {
        eJustImage = 0,
        eJustPrintCalibrationFile,
        eImageAndMarker
    };

    bool readFile(QString &file, ReadFileType type = eImageAndMarker);
    Q_INVOKABLE void readPathBackground();
    Q_INVOKABLE void readPathBackground2();
    Q_INVOKABLE void readPathBackground3();
    Q_INVOKABLE void readPathBackground4();
    Q_INVOKABLE void readPathWindOn0();
    Q_INVOKABLE void readPathWindOff0();
    Q_INVOKABLE void readPathWindOn1();
    Q_INVOKABLE void readPathWindOff1();
    Q_INVOKABLE void readPathPaintCalibration();

    Q_INVOKABLE int getProjectType();
    void setProjectType(int type);

    Q_INVOKABLE QString getPathBackground();
    void setPathBackground(const QString &path);

    Q_INVOKABLE QString getPathBackground2();
    void setPathBackground2(const QString &path);

    Q_INVOKABLE QString getPathBackground3();
    void setPathBackground3(const QString &path);

    Q_INVOKABLE QString getPathBackground4();
    void setPathBackground4(const QString &path);

    Q_INVOKABLE QString getPathWindOn0();
    void setPathWindOn0(const QString &path);

    Q_INVOKABLE QString getPathWindOn0Mark();
    void setPathWindOn0Mark(const QString &path);

    Q_INVOKABLE QString getPathWindOff0();
    void setPathWindOff0(const QString &path);

    Q_INVOKABLE QString getPathWindOff0Mark();
    void setPathWindOff0Mark(const QString &path);

    Q_INVOKABLE QString getPathWindOn1();
    void setPathWindOn1(const QString &path);

    Q_INVOKABLE QString getPathWindOn1Mark();
    void setPathWindOn1Mark(const QString &path);

    Q_INVOKABLE QString getPathWindOff1();
    void setPathWindOff1(const QString &path);

    Q_INVOKABLE QString getPathWindOff1Mark();
    void setPathWindOff1Mark(const QString &path);

    Q_INVOKABLE QString getPathPainCalibration();
    void setPathPainCalibration(const QString &path);

    Q_INVOKABLE QString getBaseName(const QString &file);

signals:
    void projectTypeChanged();
    void pathBackgroundChanged();
    void pathBackground2Changed();
    void pathBackground3Changed();
    void pathBackground4Changed();
    void pathWindOn0Changed();
    void pathWindOn0MarkChanged();
    void pathWindOff0Changed();
    void pathWindOff0MarkChanged();
    void pathWindOn1Changed();
    void pathWindOn1MarkChanged();
    void pathWindOff1Changed();
    void pathWindOff1MarkChanged();
    void pathPainCalibrationChanged();

private:
    ProjectType mProjectType;

    QString mPathBackground;
    QString mPathBackground2;
    QString mPathBackground3;
    QString mPathBackground4;
    QString mPathWindOn0;
    QString mPathWindOn0Mark;
    QString mPathWindOff0;
    QString mPathWindOff0Mark;
    QString mPathWindOn1;
    QString mPathWindOn1Mark;
    QString mPathWindOff1;
    QString mPathWindOff1Mark;
    QString mPathPainCalibration;

    QString mPlatformPrex;
};

