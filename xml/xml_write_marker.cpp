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
#include "xml/xml_write_marker.h"
#include <QDateTime>

namespace PspNsProcessXml {

    XmlWriteMarker::XmlWriteMarker() {

    }

    XmlWriteMarker *XmlWriteMarker::GetInstance() {
        static XmlWriteMarker xmlWriteMarker;
        return &xmlWriteMarker;
    }

    bool XmlWriteMarker::initFile(const QString &file) {
        mFile.setFileName(file);

        if (!mFile.open(QFile::ReadWrite | QFile::Text)) {
            return false;
        } else {
            setDevice(&mFile);
            return true;
        }
    }

    void XmlWriteMarker::closeFile() {
        mFile.close();
    }

    void XmlWriteMarker::writeMarkers(const vector<MarkerPoint> &markPoints) {
        if (markPoints.empty()) return;

        mMarkPoints = markPoints;

        setAutoFormatting(true);

        writeStartDocument();

        writeTime();
        writeStartElement("ObjectMarkers");

        writeNumber();
        writeX();
        writeY();
        writeActive();
        writeId();
        writeWidth();

        writeEndElement();

        writeEndDocument();

        closeFile();
    }

    void XmlWriteMarker::writeTime() {
        QString curDateTime = QDateTime::currentDateTime().toString(QString("yyyy-MM-dd hh:mm:ss"));

        writeComment(curDateTime);
    }

    void XmlWriteMarker::writeNumber() {
        writeStartElement("NodesNumber");

        writeCharacters(QString::number(mMarkPoints.size()));

        writeEndElement();
    }

    void XmlWriteMarker::writeX() {
        writeStartElement("Function");
        writeAttribute("id", "x");

        QString x;
        for (auto iPoint: mMarkPoints) {
            x += QString::number(iPoint.x()) + " ";
        }

        writeCharacters(x);

        writeEndElement();
    }

    void XmlWriteMarker::writeY() {
        writeStartElement("Function");
        writeAttribute("id", "y");

        QString y;
        for (auto iPoint: mMarkPoints) {
            y += QString::number(iPoint.y()) + " ";
        }

        writeCharacters(y);

        writeEndElement();
    }

    void XmlWriteMarker::writeActive() {
        writeStartElement("Function");
        writeAttribute("id", "active");

        QString active;
        for (auto iPoint: mMarkPoints) {
            active += QString::number(iPoint.getActive()) + " ";
        }

        writeCharacters(active);

        writeEndElement();
    }

    void XmlWriteMarker::writeId() {
        writeStartElement("Function");
        writeAttribute("id", "id");

        QString ids;
        for (auto iPoint: mMarkPoints) {
            ids += QString::number(iPoint.getIndex()) + " ";
        }

        writeCharacters(ids);

        writeEndElement();
    }

    void XmlWriteMarker::writeWidth() {
        writeStartElement("Function");
        writeAttribute("id", "width");

        QString widths;
        for (auto iPoint: mMarkPoints) {
            widths += QString::number(iPoint.getDetectHalfWidth() * 2) + " ";
        }

        writeCharacters(widths);

        writeEndElement();
    }

}
