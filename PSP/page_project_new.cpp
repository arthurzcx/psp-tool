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
#include "page_project_new.h"
#include "ui_page_project_new.h"
#include "io/io.h"
#include "algo/algo_basic_arithmatic.h"
#include "psp_utils_json_process.h"
#include <QFileDialog>
#include <QQmlContext>
#include "opencv2/core.hpp"

PageProjectNew::PageProjectNew(ProjectType type, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PageProjectNew),
        mProjectType(type),
        mDocument(nullptr) {
    ui->setupUi(this);

    if (type == ProjectType::eSingleComponent) {
        this->setWindowTitle(tr("New project - single component"));

        mDocument = new PspDocumentSingleComponent();
    } else {
        this->setWindowTitle(tr("New project - two component"));

        mDocument = new PspDocumentTwoComponent();
    }

    logicProjectNew = new LogicProjectNew();
    logicProjectNew->setProjectType((int) type);
    ui->quickWidget->rootContext()->setContextProperty("logicProjectNew", (QObject *) logicProjectNew);

    ui->quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
    ui->quickWidget->setClearColor(QColor(Qt::transparent));
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/qml/PageProjectNew.qml")));

    mQuickItem = ui->quickWidget->rootObject();
    mQuickItem->setProperty("m_width", ui->quickWidget->width());
    mQuickItem->setProperty("m_height", ui->quickWidget->height());
}

PspDocument *PageProjectNew::GetDocument() {
    return mDocument;
}

PageProjectNew::~PageProjectNew() {
    delete ui;

    if (logicProjectNew) {
        delete logicProjectNew;
        logicProjectNew = nullptr;
    }
}

QString ReplaceFilePrex(QString path) {
#ifdef PLATFORM_WINDOWS
    QString filePrex = "file:///";
#else
    QString filePrex = "file://";
#endif

    if (path.startsWith(filePrex)) {
        return path.replace(filePrex, "");
    } else {
        return path;
    }
}

void PageProjectNew::on_buttonBox_accepted() {
    if (mProjectType == ProjectType::eSingleComponent) {
        PspDocumentSingleComponent *document = dynamic_cast<PspDocumentSingleComponent *>(mDocument);

        document->SetPathBackgroundImage(ReplaceFilePrex(logicProjectNew->getPathBackground()));
        document->SetPathBackground2Image(ReplaceFilePrex(logicProjectNew->getPathBackground2()));
        document->SetPathWindOffImage(ReplaceFilePrex(logicProjectNew->getPathWindOff0()));
        document->SetPathWindOnImage(ReplaceFilePrex(logicProjectNew->getPathWindOn0()));
        document->SetPathWindOffMarker(ReplaceFilePrex(logicProjectNew->getPathWindOff0Mark()));
        document->SetPathWindOnMarker(ReplaceFilePrex(logicProjectNew->getPathWindOn0Mark()));
        document->SetPathPaintCalibrationFile(ReplaceFilePrex(logicProjectNew->getPathPainCalibration()));

        ProcessSingleComponentDocument(document);
    } else {
        PspDocumentTwoComponent *document = dynamic_cast<PspDocumentTwoComponent *>(mDocument);

        document->SetPathBackgroundImage(ReplaceFilePrex(logicProjectNew->getPathBackground()));
        document->SetPathBackground2Image(ReplaceFilePrex(logicProjectNew->getPathBackground2()));
        document->SetPathBackground3Image(ReplaceFilePrex(logicProjectNew->getPathBackground3()));
        document->SetPathBackground4Image(ReplaceFilePrex(logicProjectNew->getPathBackground4()));
        document->SetPathWindOffImage(ReplaceFilePrex(logicProjectNew->getPathWindOff0()));
        document->SetPathWindOnImage(ReplaceFilePrex(logicProjectNew->getPathWindOn0()));
        document->SetPathWindOffMarker(ReplaceFilePrex(logicProjectNew->getPathWindOff0Mark()));
        document->SetPathWindOnMarker(ReplaceFilePrex(logicProjectNew->getPathWindOn0Mark()));
        document->SetPathWindOff2Image(ReplaceFilePrex(logicProjectNew->getPathWindOff1()));
        document->SetPathWindOn2Image(ReplaceFilePrex(logicProjectNew->getPathWindOn1()));
        document->SetPathWindOff2Marker(ReplaceFilePrex(logicProjectNew->getPathWindOff1Mark()));
        document->SetPathWindOn2Marker(ReplaceFilePrex(logicProjectNew->getPathWindOn1Mark()));
        document->SetPathPaintCalibrationFile(ReplaceFilePrex(logicProjectNew->getPathPainCalibration()));

        ProcessDualComponentDocument(document);
    }

    QDialog::accept();
}

bool ProcessSingleComponentDocument(PspDocumentSingleComponent *document) {
    if (!document) {
        return false;
    }

    cv::Mat backgroundImg, background2Img, windOffImg, windOnImg;

    PspNsImgIo::Img_io io;
    if (!document->GetPathBackgroundImage().isEmpty()) {
        io.readImg(cv::String(document->GetPathBackgroundImage().toLocal8Bit().data()));
        backgroundImg = io.getSrcImg();
        document->SetBackgroundImg(backgroundImg);
    }

    if (!document->GetPathBackground2Image().isEmpty()) {
        io.readImg(cv::String(document->GetPathBackground2Image().toLocal8Bit().data()));
        background2Img = io.getSrcImg();
        document->SetBackground2Img(background2Img);
    }

    if (!document->GetPathWindOffImage().isEmpty()) {
        io.readImg(cv::String(document->GetPathWindOffImage().toLocal8Bit().data()));
        windOffImg = io.getSrcImg();
        document->SetWindOffImg(windOffImg);
    }

    if (!document->GetPathWindOnImage().isEmpty()) {
        io.readImg(cv::String(document->GetPathWindOnImage().toLocal8Bit().data()));
        windOnImg = io.getSrcImg();
        document->SetWindOnImg(windOnImg);
    }

    std::vector<MarkerPoint> mpWindOff, mpWindOn;

    if (!document->GetPathWindOffMarker().isEmpty()) {
        mpWindOff = JsonProcess::readMarkerFromFile(document->GetPathWindOffMarker());
        document->SetWindOffMarkers(mpWindOff);
    }

    if (!document->GetPathWindOnMarker().isEmpty()) {
        mpWindOn = JsonProcess::readMarkerFromFile(document->GetPathWindOnMarker());
        document->SetWindOnMarkers(mpWindOn);
    }

    if (windOffImg.empty() || windOnImg.empty() || mpWindOff.empty() || mpWindOn.empty() ||
        mpWindOff.size() != mpWindOn.size()) {
        return false;
    }

    return true;
}

bool ProcessDualComponentDocument(PspDocumentTwoComponent *document) {
    if (!document) {
        return false;
    }

    std::vector<MarkerPoint> mpWindOff, mpWindOn, mpWindOff2, mpWindOn2;

    if (!document->GetPathWindOffMarker().isEmpty()) {
        mpWindOff = JsonProcess::readMarkerFromFile(document->GetPathWindOffMarker());
        document->SetWindOffMarkers(mpWindOff);
    }

    if (!document->GetPathWindOnMarker().isEmpty()) {
        mpWindOn = JsonProcess::readMarkerFromFile(document->GetPathWindOnMarker());
        document->SetWindOnMarkers(mpWindOn);
    }

    if (!document->GetPathWindOff2Marker().isEmpty()) {
        mpWindOff2 = JsonProcess::readMarkerFromFile(document->GetPathWindOff2Marker());
        document->SetWindOff2Markers(mpWindOff2);
    }

    if (!document->GetPathWindOn2Marker().isEmpty()) {
        mpWindOn2 = JsonProcess::readMarkerFromFile(document->GetPathWindOn2Marker());
        document->SetWindOn2Markers(mpWindOn2);
    }

    if (mpWindOff.empty() || mpWindOn.empty() || mpWindOff.size() != mpWindOn.size() ||
        mpWindOff2.empty() || mpWindOn2.empty() || mpWindOff2.size() != mpWindOn2.size()) {
        return false;
    }

    return true;

}

void PageProjectNew::on_buttonBox_rejected() {

    QDialog::reject();
}

LogicProjectNew::LogicProjectNew()
        : mProjectType(ProjectType::eSingleComponent), mPathBackground(""), mPathBackground2(""), mPathBackground3(""),
          mPathBackground4(""), mPathWindOn0(""), mPathWindOn0Mark(""), mPathWindOff0(""), mPathWindOff0Mark(""),
          mPathWindOn1(""), mPathWindOn1Mark(""), mPathWindOff1(""), mPathWindOff1Mark(""), mPathPainCalibration("") {
#ifdef PLATFORM_WINDOWS
    mPlatformPrex = "file:///";
#else
    mPlatformPrex = "file://";
#endif
}

LogicProjectNew::~LogicProjectNew() {

}

bool LogicProjectNew::readFile(QString &file, ReadFileType type) {
    file.clear();

    QFileDialog fileDialog(0, QString(tr("New project")));
    fileDialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileFilters;
    if (type != ReadFileType::eJustPrintCalibrationFile) {
        fileFilters << tr("Image files (*.bmp *.jpg *.tif)");
    }
    if (type == ReadFileType::eImageAndMarker) {
        fileFilters << tr("Mark files (*.marker)");
    }
    if (type == ReadFileType::eJustPrintCalibrationFile) {
        fileFilters << tr("Calibration files (*.pcf)");
    }
    fileDialog.setNameFilters(fileFilters);

    if (fileDialog.exec() == QDialog::Accepted) {
        file = mPlatformPrex + fileDialog.selectedFiles()[0];

        return true;
    }

    return false;
}

void LogicProjectNew::readPathBackground() {
    QString tmp;
    if (readFile(tmp, ReadFileType::eJustImage)) {
        setPathBackground(tmp);
    }
}

void LogicProjectNew::readPathBackground2() {
    QString tmp;
    if (readFile(tmp, ReadFileType::eJustImage)) {
        setPathBackground2(tmp);
    }
}
void LogicProjectNew::readPathBackground3() {
    QString tmp;
    if (readFile(tmp, ReadFileType::eJustImage)) {
        setPathBackground3(tmp);
    }
}
void LogicProjectNew::readPathBackground4() {
    QString tmp;
    if (readFile(tmp, ReadFileType::eJustImage)) {
        setPathBackground4(tmp);
    }
}

void LogicProjectNew::readPathWindOn0() {
    QString tmp;
    if (readFile(tmp)) {
        if (tmp.endsWith(".marker")) {
            setPathWindOn0Mark(tmp);
        } else {
            setPathWindOn0(tmp);
        }
    }
}

void LogicProjectNew::readPathWindOff0() {
    QString tmp;
    if (readFile(tmp)) {
        if (tmp.endsWith(".marker")) {
            setPathWindOff0Mark(tmp);
        } else {
            setPathWindOff0(tmp);
        }
    }
}

void LogicProjectNew::readPathWindOn1() {
    QString tmp;
    if (readFile(tmp)) {
        if (tmp.endsWith(".marker")) {
            setPathWindOn1Mark(tmp);
        } else {
            setPathWindOn1(tmp);
        }
    }
}

void LogicProjectNew::readPathWindOff1() {
    QString tmp;
    if (readFile(tmp)) {
        if (tmp.endsWith(".marker")) {
            setPathWindOff1Mark(tmp);
        } else {
            setPathWindOff1(tmp);
        }
    }
}

void LogicProjectNew::readPathPaintCalibration() {
    QString tmp;
    if (readFile(tmp, ReadFileType::eJustPrintCalibrationFile)) {
        if (tmp.endsWith(".pcf")) {
            setPathPainCalibration(tmp);
        }
    }
}

int LogicProjectNew::getProjectType() {
    return (int) mProjectType;
}

void LogicProjectNew::setProjectType(int type) {
    mProjectType = (ProjectType) type;
    emit projectTypeChanged();
}

QString LogicProjectNew::getPathBackground() {
    return mPathBackground;
}

void LogicProjectNew::setPathBackground(const QString &path) {
    mPathBackground = path;
    emit pathBackgroundChanged();
}

QString LogicProjectNew::getPathBackground2() {
    return mPathBackground2;
}

void LogicProjectNew::setPathBackground2(const QString &path) {
    mPathBackground2 = path;
    emit pathBackground2Changed();
}

QString LogicProjectNew::getPathBackground3() {
    return mPathBackground3;
}

void LogicProjectNew::setPathBackground3(const QString &path) {
    mPathBackground3 = path;
    emit pathBackground3Changed();
}

QString LogicProjectNew::getPathBackground4() {
    return mPathBackground4;
}
void LogicProjectNew::setPathBackground4(const QString &path) {
    mPathBackground4 = path;
    emit pathBackground4Changed();
}

QString LogicProjectNew::getPathWindOn0() {
    return mPathWindOn0;
}

void LogicProjectNew::setPathWindOn0(const QString &path) {
    mPathWindOn0 = path;
    emit pathWindOn0Changed();
}

QString LogicProjectNew::getPathWindOn0Mark() {
    return mPathWindOn0Mark;
}

void LogicProjectNew::setPathWindOn0Mark(const QString &path) {
    mPathWindOn0Mark = path;
    emit pathWindOn0MarkChanged();
}

QString LogicProjectNew::getPathWindOff0() {
    return mPathWindOff0;
}

void LogicProjectNew::setPathWindOff0(const QString &path) {
    mPathWindOff0 = path;
    emit pathWindOff0Changed();
}

QString LogicProjectNew::getPathWindOff0Mark() {
    return mPathWindOff0Mark;
}

void LogicProjectNew::setPathWindOff0Mark(const QString &path) {
    mPathWindOff0Mark = path;
    emit pathWindOff0MarkChanged();
}

QString LogicProjectNew::getPathWindOn1() {
    return mPathWindOn1;
}

void LogicProjectNew::setPathWindOn1(const QString &path) {
    mPathWindOn1 = path;
    emit pathWindOn1Changed();
}

QString LogicProjectNew::getPathWindOn1Mark() {
    return mPathWindOn1Mark;
}

void LogicProjectNew::setPathWindOn1Mark(const QString &path) {
    mPathWindOn1Mark = path;
    emit pathWindOn1MarkChanged();
}

QString LogicProjectNew::getPathWindOff1() {
    return mPathWindOff1;
}

void LogicProjectNew::setPathWindOff1(const QString &path) {
    mPathWindOff1 = path;
    emit pathWindOff1Changed();
}

QString LogicProjectNew::getPathWindOff1Mark() {
    return mPathWindOff1Mark;
}

void LogicProjectNew::setPathWindOff1Mark(const QString &path) {
    mPathWindOff1Mark = path;
    emit pathWindOff1MarkChanged();
}

QString LogicProjectNew::getPathPainCalibration() {
    return mPathPainCalibration;
}

void LogicProjectNew::setPathPainCalibration(const QString &path) {
    mPathPainCalibration = path;
    emit pathPainCalibrationChanged();
}

QString LogicProjectNew::getBaseName(const QString &file) {
    QFileInfo info(file);
    return info.baseName();
}
