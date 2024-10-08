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
#include "xml/xml_read_marker.h"
#include "base/base_log.h"
#include <QDebug>

namespace PspNsProcessXml {
    XmlReadMarker::XmlReadMarker() {

    }

    XmlReadMarker *XmlReadMarker::GetInstance() {
        static XmlReadMarker xmlReadMarker;
        return &xmlReadMarker;
    }

    bool XmlReadMarker::initFile(const QString &file) {
        mFile.setFileName(file);

        if (!mFile.open(QFile::ReadOnly | QFile::Text)) {
            return false;
        } else {
            setDevice(&mFile);
            return true;
        }
    }

    void XmlReadMarker::closeFile() {
        mFile.close();
    }

    void XmlReadMarker::readMarker() {
        mMarkPoints.clear();

        while (!atEnd()) {
            TokenType type = readNext();

            switch (type) {
                case StartDocument: {
                    break;
                }
                case StartElement: {
                    if (attributes().hasAttribute("id")) {
                        if (attributes().value("id") == "x") {
                            readX();
                        } else if (attributes().value("id") == "y") {
                            readY();
                        } else if (attributes().value("id") == "active") {
                            readActive();
                        } else if (attributes().value("id") == "id") {
                            readId();
                        } else if (attributes().value("id") == "width") {
                            readWidth();
                        }
                    } else if (name() == "NodesNumber") {
                        readNodeNumber();
                    }

                }
            }
        }

        closeFile();
    }

    void XmlReadMarker::readX() {
        TokenType type = readNext();

        if (type == Characters && !isWhitespace()) {
            QString x = *text().string();
            x.replace('\n', ' ');
            Log::getInstance()->writeLog(LogLevel::eDebug, QString("xml read markers x: " + x));
            QStringList listX = x.split(" ", QString::SkipEmptyParts);

            if (listX.size() == mMarkPoints.size()) {
                for (size_t i = 0; i < listX.size(); i++) {
                    mMarkPoints[i].setX(listX[i].toDouble());
                }
            }
        }
    }

    void XmlReadMarker::readY() {
        TokenType type = readNext();

        if (type == Characters && !isWhitespace()) {
            QString y = *text().string();
            y.replace('\n', ' ');
            Log::getInstance()->writeLog(LogLevel::eDebug, QString("xml read markers y: " + y));
            QStringList listY = y.split(" ", QString::SkipEmptyParts);

            if (listY.size() == mMarkPoints.size()) {
                for (size_t i = 0; i < listY.size(); i++) {
                    mMarkPoints[i].setY(listY[i].toDouble());
                }
            }
        }
    }

    void XmlReadMarker::readNodeNumber() {
        TokenType type = readNext();

        if (type == Characters && !isWhitespace()) {
            QString nodeNumber = *text().string();

            Log::getInstance()->writeLog(LogLevel::eDebug, QString("xml read markers nodesNumber: " + nodeNumber));

            mMarkPoints.resize((size_t) nodeNumber.toInt());
        }
    }

    void XmlReadMarker::readActive() {
        TokenType type = readNext();

        if (type == Characters && !isWhitespace()) {
            QString actives = *text().string();
            actives.replace('\n', ' ');
            Log::getInstance()->writeLog(LogLevel::eDebug, QString("xml read markers actives: " + actives));
            QStringList listActives = actives.split(" ", QString::SkipEmptyParts);

            if (listActives.size() == mMarkPoints.size()) {
                for (size_t i = 0; i < listActives.size(); i++) {
                    mMarkPoints[i].setActive(listActives[i].toInt());
                }
            }
        }
    }

    void XmlReadMarker::readId() {
        TokenType type = readNext();

        if (type == Characters && !isWhitespace()) {
            QString ids = *text().string();
            ids.replace('\n', ' ');
            Log::getInstance()->writeLog(LogLevel::eDebug, QString("xml read markers ids: " + ids));
            QStringList listIds = ids.split(" ", QString::SkipEmptyParts);

            if (listIds.size() == mMarkPoints.size()) {
                for (size_t i = 0; i < listIds.size(); i++) {
                    mMarkPoints[i].setIndex(listIds[i].toInt());
                }
            }
        }
    }

    void XmlReadMarker::readWidth() {
        TokenType type = readNext();

        if (type == Characters && !isWhitespace()) {
            QString widths = *text().string();
            widths.replace('\n', ' ');
            Log::getInstance()->writeLog(LogLevel::eDebug, QString("xml read markers widths: ") + widths);

            QStringList listWidths = widths.split(" ", QString::SkipEmptyParts);

            if (listWidths.size() == mMarkPoints.size()) {
                for (size_t i = 0; i < listWidths.size(); i++) {
                    mMarkPoints[i].setDetectHalfWidth(listWidths[i].toInt() / 2);
                }
            }
        }
    }

    vector<MarkerPoint> XmlReadMarker::getMarker() const {
        return mMarkPoints;
    }
}
