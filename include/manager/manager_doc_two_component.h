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

#include "manager_global.h"
#include "manager_doc_single_component.h"

namespace PspNsManager {
    class MANAGER_SHARED PspDocumentTwoComponent : public PspDocumentSingleComponent {
    public:
        PspDocumentTwoComponent();
        virtual ~PspDocumentTwoComponent();

        PspDocumentTwoComponent(const PspDocumentTwoComponent &document);
        PspDocumentTwoComponent &operator=(const PspDocumentTwoComponent &document);

        const QString &GetPathBackground3Image() const;
        const QString &GetPathBackground4Image() const;
        const QString &GetPathWindOff2Image() const;
        const QString &GetPathWindOn2Image() const;
        const QString &GetPathWindOff2Marker() const;
        const QString &GetPathWindOn2Marker() const;

        cv::Mat GetBackground3Img();
        cv::Mat GetBackground4Img();
        cv::Mat GetWindOn2Img();
        cv::Mat GetWindOff2Img();

        const std::vector<MarkerPoint> &GetWindOn2Markers() const;
        const std::vector<MarkerPoint> &GetWindOff2Markers() const;

        void SetPathBackground3Image(const QString &path);
        void SetPathBackground4Image(const QString &path);
        void SetPathWindOff2Image(const QString &path);
        void SetPathWindOn2Image(const QString &path);
        void SetPathWindOff2Marker(const QString &path);
        void SetPathWindOn2Marker(const QString &path);

        void SetBackground2Img(cv::Mat img);
        void SetBackground3Img(cv::Mat img);
        void SetBackground4Img(cv::Mat img);
        void SetWindOn2Img(cv::Mat img);
        void SetWindOff2Img(cv::Mat img);

        void SetWindOn2Markers(const std::vector<MarkerPoint> &markers);
        void SetWindOff2Markers(const std::vector<MarkerPoint> &markers);

    private:
        QString mPathBackground3Image;
        QString mPathBackground4Image;

        QString mPathWindOff2Image;
        QString mPathWindOn2Image;

        QString mPathWindOff2Marker;
        QString mPathWindOn2Marker;

        cv::Mat mBackground3Img;
        cv::Mat mBackground4Img;
        cv::Mat mWindOn2Img;
        cv::Mat mWindOff2Img;

        std::vector<MarkerPoint> mWindOn2Markers;
        std::vector<MarkerPoint> mWindOff2Markers;

        // Ref sens
    };

    MANAGER_SHARED void to_json(Json &j, const PspDocumentTwoComponent &document);
    MANAGER_SHARED void from_json(const Json &j, PspDocumentTwoComponent &document);
}
