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
#include <QJsonArray>
#include <vector>

namespace Ui {
    class PagePaintCalibration;
}

class LogicPagePaintCalibration;
class PagePaintCalibration : public QDialog {
Q_OBJECT

public:
    explicit PagePaintCalibration(QWidget *parent = nullptr);
    ~PagePaintCalibration();

private:
    Ui::PagePaintCalibration *ui;

    QQuickItem *mQuickItem;

    LogicPagePaintCalibration *mLogicPagePaintCalibration;
};

class LogicPagePaintCalibration : public QObject {
Q_OBJECT

public:
    LogicPagePaintCalibration();
    ~LogicPagePaintCalibration();

    void reset();
    bool isValidProject();

    Q_INVOKABLE bool selectDirectory();
    Q_INVOKABLE QString getFileBaseName(const QString &pathName);

    Q_INVOKABLE QString getModelImage();
    Q_INVOKABLE QString getModelParameter();

    bool readImages();
    bool readRoi();
    bool readParameter();

    void updateModel();

    Q_INVOKABLE void setModelImage(const QString &modelStr);

    void fit();

private:
    QString mSeletectDirectory;

    std::vector<QString> mImagesPath;
    std::vector<QString> mImagesName;
    std::vector<QPointF> mRoiPoints;
    std::vector<QString> mPressures;
    std::vector<QString> mTemperatures;

    QString mModelImages;
    QString mModelParameters;

    QString mPlatformPrex;
};
