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
#include "manager/manager_doc_two_component.h"
#include "io/io.h"

namespace PspNsManager {
    PspDocumentTwoComponent::PspDocumentTwoComponent()
            : PspDocumentSingleComponent(), mPathBackground3Image(""), mPathBackground4Image(""),
              mPathWindOff2Image(""), mPathWindOn2Image(""), mPathWindOff2Marker(""), mPathWindOn2Marker("") {

    }

    PspDocumentTwoComponent::~PspDocumentTwoComponent() {

    }

    PspDocumentTwoComponent::PspDocumentTwoComponent(const PspDocumentTwoComponent &document)
            : PspDocumentSingleComponent(document), mPathBackground3Image(document.mPathBackground3Image),
              mPathBackground4Image(document.mPathBackground4Image), mPathWindOff2Image(document.mPathWindOff2Image),
              mPathWindOn2Image(document.mPathWindOn2Image), mPathWindOff2Marker(document.mPathWindOff2Marker),
              mPathWindOn2Marker(document.mPathWindOn2Marker), mBackground3Img(document.mBackground3Img),
              mBackground4Img(document.mBackground4Img), mWindOn2Img(document.mWindOn2Img),
              mWindOff2Img(document.mWindOff2Img), mWindOn2Markers(document.mWindOn2Markers),
              mWindOff2Markers(document.mWindOff2Markers) {

    }

    PspDocumentTwoComponent &PspDocumentTwoComponent::operator=(const PspDocumentTwoComponent &document) {
        PspDocumentSingleComponent::operator=(document);

        mPathWindOff2Image = document.mPathWindOff2Image;
        mPathWindOn2Image = document.mPathWindOn2Image;
        mPathWindOff2Marker = document.mPathWindOff2Marker;
        mPathWindOn2Marker = document.mPathWindOn2Marker;

        mBackground3Img = document.mBackground3Img;
        mBackground4Img = document.mBackground4Img;
        mWindOn2Img = document.mWindOn2Img;
        mWindOff2Img = document.mWindOff2Img;
        mWindOn2Markers = document.mWindOn2Markers;
        mWindOff2Markers = document.mWindOff2Markers;

        return *this;
    }

    const QString &PspDocumentTwoComponent::GetPathBackground3Image() const {
        return mPathBackground3Image;
    }

    const QString &PspDocumentTwoComponent::GetPathBackground4Image() const {
        return mPathBackground4Image;
    }

    const QString &PspDocumentTwoComponent::GetPathWindOff2Image() const {
        return mPathWindOff2Image;
    }

    const QString &PspDocumentTwoComponent::GetPathWindOn2Image() const {
        return mPathWindOn2Image;
    }

    const QString &PspDocumentTwoComponent::GetPathWindOff2Marker() const {
        return mPathWindOff2Marker;
    }

    const QString &PspDocumentTwoComponent::GetPathWindOn2Marker() const {
        return mPathWindOn2Marker;
    }

    cv::Mat PspDocumentTwoComponent::GetBackground3Img() {
        return mBackground3Img;
    }

    cv::Mat PspDocumentTwoComponent::GetBackground4Img() {
        return mBackground4Img;
    }

    cv::Mat PspDocumentTwoComponent::GetWindOn2Img() {
        return mWindOn2Img;
    }

    cv::Mat PspDocumentTwoComponent::GetWindOff2Img() {
        return mWindOff2Img;
    }

    const std::vector<MarkerPoint> &PspDocumentTwoComponent::GetWindOn2Markers() const {
        return mWindOn2Markers;
    }

    const std::vector<MarkerPoint> &PspDocumentTwoComponent::GetWindOff2Markers() const {
        return mWindOff2Markers;
    }

    void PspDocumentTwoComponent::SetPathWindOff2Image(const QString &path) {
        mPathWindOff2Image = path;

        PspNsImgIo::Img_io io;
        if (io.readImg(mPathWindOff2Image.toStdString())) {
            mWindOff2Img = io.getSrcImg();
        }
    }

    void PspDocumentTwoComponent::SetPathBackground3Image(const QString &path) {
        mPathBackground3Image = path;

        PspNsImgIo::Img_io io;
        if (io.readImg(mPathBackground3Image.toStdString())) {
            mBackground3Img = io.getSrcImg();
        }
    }

    void PspDocumentTwoComponent::SetPathBackground4Image(const QString &path) {
        mPathBackground4Image = path;

        PspNsImgIo::Img_io io;
        if (io.readImg(mPathBackground4Image.toStdString())) {
            mBackground4Img = io.getSrcImg();
        }
    }

    void PspDocumentTwoComponent::SetPathWindOn2Image(const QString &path) {
        mPathWindOn2Image = path;

        PspNsImgIo::Img_io io;
        if (io.readImg(mPathWindOn2Image.toStdString())) {
            mWindOn2Img = io.getSrcImg();
        }
    }

    void PspDocumentTwoComponent::SetPathWindOff2Marker(const QString &path) {
        mPathWindOff2Marker = path;
    }

    void PspDocumentTwoComponent::SetPathWindOn2Marker(const QString &path) {
        mPathWindOn2Marker = path;
    }

    void PspDocumentTwoComponent::SetBackground3Img(cv::Mat img) {
        mBackground3Img = img;
    }

    void PspDocumentTwoComponent::SetBackground4Img(cv::Mat img) {
        mBackground4Img = img;
    }

    void PspDocumentTwoComponent::SetWindOn2Img(cv::Mat img) {
        mWindOn2Img = img;
    }

    void PspDocumentTwoComponent::SetWindOff2Img(cv::Mat img) {
        mWindOff2Img = img;
    }

    void PspDocumentTwoComponent::SetWindOn2Markers(const std::vector<MarkerPoint> &markers) {
        mWindOn2Markers = markers;
    }

    void PspDocumentTwoComponent::SetWindOff2Markers(const std::vector<MarkerPoint> &markers) {
        mWindOff2Markers = markers;
    }

    void to_json(Json &j, const PspDocumentTwoComponent &document) {
        to_json(j, (PspDocumentSingleComponent &) document);

        j["type"] = "Two component document";

        j["PathBackground3Image"] = document.GetPathBackground3Image().toStdString();
        j["PathBackground4Image"] = document.GetPathBackground4Image().toStdString();

        j["PathWindOff2Image"] = document.GetPathWindOff2Image().toStdString();
        j["PathWindOn2Image"] = document.GetPathWindOn2Image().toStdString();

        j["PathWindOff2Marker"] = document.GetPathWindOff2Marker().toStdString();
        j["PathWindOn2Marker"] = document.GetPathWindOn2Marker().toStdString();

        for (auto marker: document.GetWindOn2Markers()) {
            Json jsonMarker;
            to_json(jsonMarker, marker);
            j["markersWindOn2"].push_back(jsonMarker);
        }

        for (auto marker: document.GetWindOff2Markers()) {
            Json jsonMarker;
            to_json(jsonMarker, marker);
            j["markersWindOff2"].push_back(jsonMarker);
        }
    }

    void from_json(const Json &j, PspDocumentTwoComponent &document) {
        from_json(j, (PspDocumentSingleComponent &) document);

        if (j.contains("PathBackground3Image")) {
            document.SetPathBackground3Image(QString::fromStdString(j["PathBackground3Image"]));
        }

        if (j.contains("PathBackground4Image")) {
            document.SetPathBackground4Image(QString::fromStdString(j["PathBackground4Image"]));
        }

        if (j.contains("PathWindOff2Image")) {
            document.SetPathWindOff2Image(QString::fromStdString(j["PathWindOff2Image"]));
        }
        if (j.contains("PathWindOn2Image")) {
            document.SetPathWindOn2Image(QString::fromStdString(j["PathWindOn2Image"]));
        }

        if (j.contains("PathWindOff2Marker")) {
            document.SetPathWindOff2Marker(QString::fromStdString(j["PathWindOff2Marker"]));
        }
        if (j.contains("PathWindOn2Marker")) {
            document.SetPathWindOn2Marker(QString::fromStdString(j["PathWindOn2Marker"]));
        }

        std::vector<MarkerPoint> markers;
        if (j.contains("markersWindOn2")) {
            for (auto iter = j["markersWindOn2"].begin(); iter != j["markersWindOn2"].end(); ++iter) {
                MarkerPoint marker;
                from_json(*iter, marker);
                markers.push_back(marker);
            }
            document.SetWindOn2Markers(markers);
        }

        markers.clear();
        if (j.contains("markersWindOff2")) {
            for (auto iter = j["markersWindOff2"].begin(); iter != j["markersWindOff2"].end(); ++iter) {
                MarkerPoint marker;
                from_json(*iter, marker);
                markers.push_back(marker);
            }
            document.SetWindOff2Markers(markers);
        }
    }
}
