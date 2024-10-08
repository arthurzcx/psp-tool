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
#include "psp_utils_json_process.h"
#include <QFile>
#include <QIODevice>
#include <iomanip>
#include <fstream>

using namespace std;

JsonProcess::JsonProcess() {

}

JsonProcess::~JsonProcess() {

}

JsonProcess *JsonProcess::GetInstance() {
    static JsonProcess jsonProcess;
    return &jsonProcess;
}

Json JsonProcess::qstringToJson(const QString &str) {
    return Json::parse(str.toUtf8());
}

bool JsonProcess::readJsonFile(const QString &filePath, Json &j) {
    QFile file(filePath);
    if (!file.exists()) return false;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    j = Json::parse(QString(file.readAll()).toUtf8());
    file.close();

    if (j.is_null()) return false;

    return true;
}

bool JsonProcess::writeJsonFile(const QString &filePath, const Json &j) {
#if defined (PLATFORM_WINDOWS)
    ofstream osf(filePath.toStdWString());
#else
    ofstream osf(filePath.toStdString());
#endif

    osf << std::setw(4) << j << std::endl;
    osf.close();

    return true;
}

std::vector<PspNsBase::MarkerPoint> JsonProcess::readMarkerFromFile(const QString &filePath) {
    Json j;
    std::vector<PspNsBase::MarkerPoint> points;
    if (JsonProcess::readJsonFile(filePath, j) && j.is_array()) {
        for (auto iter = j.begin(); iter != j.end(); ++iter) {
            PspNsBase::MarkerPoint point;
            from_json(*iter, point);
            points.push_back(point);
        }
    }

    return points;
}
