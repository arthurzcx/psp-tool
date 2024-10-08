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
#include "logic_roi_io.h"
#include <QFileDialog>
#include <fstream>
#include <iomanip>
#include "manager/work_manager.h"
#include "base/base_graphic_rect.h"
#include "base/base_graphic_polygon.h"
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

LogicRoiIO::LogicRoiIO() {

}

LogicRoiIO::~LogicRoiIO() {

}

void LogicRoiIO::SaveRoi() {
    if (PspNsManager::WorkImgManager::GetInstance()->getROI().empty()) return;

    QFileDialog saveROIDialog(nullptr, QString(QObject::tr("Save ROI")));
    saveROIDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    saveROIDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << QObject::tr("ROI file (*.roi)");
    saveROIDialog.setNameFilters(fileType);

    if (saveROIDialog.exec() == QDialog::Accepted) {
        QString filePath = saveROIDialog.selectedFiles()[0];
        if (filePath.endsWith(".roi"))
            filePath.replace(".roi", "");

        const std::vector<Graphic *> &graphics = PspNsManager::WorkImgManager::GetInstance()->getROI();

        Json jGraphics;
        for (auto graphic: graphics) {
            if (const GraphicRect *pRect = dynamic_cast<const GraphicRect *>(graphic)) {
                Json j;
                to_json(j, *pRect);
                jGraphics.push_back(j);
            } else if (const GraphicPolygon *pPolygon = dynamic_cast<const GraphicPolygon *>(graphic)) {
                Json j;
                to_json(j, *pPolygon);
                jGraphics.push_back(j);
            } else {

            }
        }

        std::ofstream out(QString(filePath + ".roi").toStdString());
        out << std::setw(4) << jGraphics << std::endl;
        out.close();
    }
}

void LogicRoiIO::ReadRoi() {
    QFileDialog fileDialog(nullptr, QString(QObject::tr("Open ROI")));
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    QStringList fileFilters;
    fileFilters << QObject::tr("ROI files (*.roi)");
    fileDialog.setNameFilters(fileFilters);

    if (fileDialog.exec() == QDialog::Accepted) {
        if (fileDialog.selectedFiles().isEmpty()) return;

        QString filePath = fileDialog.selectedFiles()[0];

        if (!filePath.endsWith(".roi")) {
            return;
        }

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }

        Json j = Json::parse(QString(file.readAll()).toUtf8());
        if (j.is_array()) {
            PspNsManager::WorkImgManager::GetInstance()->clearROI();
            for (size_t i = 0; i < j.size(); i++) {
                Json jGraphic = j.at(i);
                if (jGraphic["type"] == "rect") {
                    GraphicRect *pRect = new GraphicRect();
                    from_json(jGraphic, *pRect);
                    PspNsManager::WorkImgManager::GetInstance()->addROI(pRect);
                } else if (jGraphic["type"] == "polygon") {
                    GraphicPolygon *pPolygon = new GraphicPolygon();
                    from_json(jGraphic, *pPolygon);
                    PspNsManager::WorkImgManager::GetInstance()->addROI(pPolygon);
                } else {

                }
            }
        }

        file.close();
    }
}
