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
#include "io_global.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

namespace PspNsImgIo {
    class IO_SHARED Img_io : public QObject {
    Q_OBJECT
    public:
        Img_io();
        ~Img_io();

        Img_io(const Img_io &imgIo);
        Img_io &operator=(const Img_io &imgIo);

        bool readImg(const cv::String &fileName);

        cv::Mat getSrcImg();

    signals:
        void updateStatus(const QString &msg);

    private:
        cv::Mat mSrcImg;
    };
}
