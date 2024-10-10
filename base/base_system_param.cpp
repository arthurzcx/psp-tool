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
#include "base/base_system_param.h"

namespace PspNsBase {
#ifndef _DEBUG
    bool SystemParam::mIsDrawTestPaintCalibration = false;
    bool SystemParam::mIsDrawTestImageAlignment = false;
    bool SystemParam::mIsDrawTestMarkers = false;
    bool SystemParam::mIsDrawTestPsptoolMsg = false;
#else
    bool SystemParam::mIsDrawTestPaintCalibration = true;
    bool SystemParam::mIsDrawTestImageAlignment = true;
    bool SystemParam::mIsDrawTestMarkers = true;    
    bool SystemParam::mIsDrawTestPsptoolMsg = true;
#endif

#ifdef _DEBUG
    bool SystemParam::mIsDebugMode = true;
#else
    bool SystemParam::mIsDebugMode = false;
#endif

    int SystemParam::mBackgroundColor = 0xff1a1a1a;
    int SystemParam::mAssistantColor = 0xFFFFFF00;
    bool SystemParam::mIsShowMarkers = true;
    bool SystemParam::mIsShowPseudoColor = true;
    unsigned SystemParam::mDrawMarkerPointHalfWidth = 3;
    eBaseSortPoints SystemParam::mSortPointType = eBaseSortPoints::eSortPointsLRTB;
    unsigned short SystemParam::mServerPythonPort = 14000;
    unsigned short SystemParam::mServerPspPort = 14001;

    SystemParam::SystemParam() {

    }

    SystemParam *SystemParam::GetInstance() {
        static SystemParam systemParam;
        return &systemParam;
    }

    void SystemParam::AddTestInfo(const QString &info) {
        if (mTestInfo.size() > 50) {
            mTestInfo.clear();
        }

        if (SystemParam::GetInstance()->IsDebugMode()) {
            mTestInfo.push_back(info);
        }
    }

    const vector<QString> &SystemParam::GetTestInfo() const {
        return mTestInfo;
    }

    bool SystemParam::IsDrawTestPaintCalibration() const {
        return mIsDrawTestPaintCalibration;
    }

    bool SystemParam::IsDrawTestImageAlignment() const {
        return mIsDrawTestImageAlignment;
    }

    bool SystemParam::IsDrawTestMarkers() const {
        return mIsDrawTestMarkers;
    }

    bool SystemParam::IsDrawTestPsptoolMsg() const {
        return mIsDrawTestPsptoolMsg;
    }

    bool SystemParam::IsDebugMode() {
        return mIsDebugMode;
    }

    int SystemParam::GetBackgroundColor() {
        return mBackgroundColor;
    }

    void SystemParam::SetBackgroundColor(int color) {
        mBackgroundColor = color;
    }

    int SystemParam::GetAssistantColor() {
        if (mIsShowPseudoColor) {
            return 0xFFFFFFFF;
        } else {
            return 0xFFFFFF00;
        }
    }

    bool SystemParam::IsShowMarkers() {
        return mIsShowMarkers;
    }

    void SystemParam::SetIsShowMarkers(bool show) {
        mIsShowMarkers = show;
    }

    bool SystemParam::IsShowPseudoColor() {
        return mIsShowPseudoColor;
    }

    void SystemParam::SetIsShowPseudoColor(bool pseudoColor) {
        mIsShowPseudoColor = pseudoColor;
    }

    unsigned int SystemParam::GetDrawMarkerPointHalfWidth() const {
        return mDrawMarkerPointHalfWidth;
    }
    void SystemParam::SetDrawMarkerPointHalfWidth(unsigned int halfWidth) {
        if (halfWidth >= 3) {
            mDrawMarkerPointHalfWidth = halfWidth;
        }
    }

    eBaseSortPoints SystemParam::GetSortPointType() {
        return mSortPointType;
    }

    void SystemParam::SetSortPointType(eBaseSortPoints type) {
        mSortPointType = type;
    }

    unsigned short SystemParam::GetServerPythonPort() const {
        return mServerPythonPort;
    }

    void SystemParam::SetServerPythonPort(unsigned short port) {
        mServerPythonPort = port;
    }

    unsigned short SystemParam::GetServerPspPort() const {
        return mServerPspPort;
    }

    void SystemParam::SetServerPspPort(unsigned short port) {
        mServerPspPort = port;
    }
}
