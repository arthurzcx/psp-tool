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
#include "manager/manager.h"

using namespace cv;

namespace PspNsManager {
    ImgManager::ImgManager() {

    }

    const Mat &ImgManager::getBackgroundImg() {
        return mBackgroundImg;
    }

    const Mat &ImgManager::getReferenceImg() {
        return mReferenceImg;
    }

    const Mat &ImgManager::getPspImg() {
        return mPspImg;
    }

    const Mat &ImgManager::getCurImg() {
        return mCurImg;
    }

    void ImgManager::setBackgroundImg(Mat backgroundImg) {
        mBackgroundImg = backgroundImg;
    }

    void ImgManager::setReferenceImg(Mat referImg) {
        mReferenceImg = referImg;
    }

    void ImgManager::setPspImg(Mat pspImg) {
        mPspImg = pspImg;
    }

    void ImgManager::setCurImg(Mat curImg) {
        mCurImg = curImg;
    }

}
