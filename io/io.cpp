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
#include "io/io.h"
#include "base/base_trace.h"
#include "algo/algo_type.h"

using namespace PspNsBase;
using namespace PspNsAlgo;
using namespace cv;

namespace PspNsImgIo {
    Img_io::Img_io() {
    }

    Img_io::~Img_io() {
    }

    Img_io::Img_io(const Img_io &imgIo) {
        mSrcImg = imgIo.mSrcImg;
    }

    Img_io &Img_io::operator=(const Img_io &imgIo) {
        mSrcImg = imgIo.mSrcImg;

        return *this;
    }

    bool Img_io::readImg(const String &fileName) {
        emit updateStatus(QString(fileName.c_str()));

        mSrcImg = imread(fileName, IMREAD_ANYDEPTH);

        if (mSrcImg.depth() != CV_16U && mSrcImg.depth() != CV_32F) {
            mSrcImg = convertTo16UC1(mSrcImg);
        }

        if (SystemParam::GetInstance()->IsDebugMode()) {
            QString srcImgInfo;
            srcImgInfo +=
                    "read image : depth " + (mSrcImg.depth() == CV_16U ? QString("CV_16U") : QString("not 16U")) + ", ";
            srcImgInfo += "channel " + QString::number(mSrcImg.channels());
            Log::getInstance()->writeLog(LogLevel::eDebug, srcImgInfo);
        }

        return true;
    }

    Mat Img_io::getSrcImg() {
        return mSrcImg;
    }
}
