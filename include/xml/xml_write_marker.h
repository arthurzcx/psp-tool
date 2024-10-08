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

#include "xml_global.h"
#include "base/base_markerpoint.h"
#include <QXmlStreamWriter>
#include <QFile>
#include <vector>

using namespace std;
using namespace PspNsBase;

namespace PspNsProcessXml {
    class XML_SHARED XmlWriteMarker : public QXmlStreamWriter {
        XmlWriteMarker();

    public:
        static XmlWriteMarker *GetInstance();

        bool initFile(const QString &file);
        void closeFile();

        void writeMarkers(const vector<MarkerPoint> &markPoints);

    private:
        void writeTime();
        void writeNumber();
        void writeX();
        void writeY();
        void writeActive();
        void writeId();
        void writeWidth();

    private:
        QFile mFile;
        vector<MarkerPoint> mMarkPoints;
    };
}
