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

#include <QObject>
#include "manager_global.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

namespace PspNsManager {
    class MANAGER_SHARED ImgManager : QObject {
    Q_OBJECT

    public:
        ImgManager();

        const cv::Mat &getBackgroundImg();
        const cv::Mat &getReferenceImg();
        const cv::Mat &getPspImg();
        const cv::Mat &getCurImg();

        void setBackgroundImg(cv::Mat backgroundImg);
        void setReferenceImg(cv::Mat referImg);
        void setPspImg(cv::Mat pspImg);
        void setCurImg(cv::Mat curImg);

    private:
        cv::Mat mBackgroundImg;
        cv::Mat mReferenceImg;
        cv::Mat mPspImg;
        cv::Mat mCurImg;
    };
}
