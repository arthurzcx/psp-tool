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

#include "base_global.h"
#include "base_sort_points.h"
#include <QString>
#include <vector>

using namespace std;

namespace PspNsBase {

    class BASE_SHARED SystemParam {
        SystemParam();
    public:
        static SystemParam *GetInstance();

        bool IsDrawTestPaintCalibration() const;
        bool IsDrawTestImageAlignment() const;
        bool IsDrawTestMarkers() const;
        bool IsDrawTestPsptoolMsg() const;

        void AddTestInfo(const QString &info);
        const vector<QString> &GetTestInfo() const;

        bool IsDebugMode();

        int GetBackgroundColor();
        void SetBackgroundColor(int color);

        int GetAssistantColor();

        bool IsShowMarkers();
        void SetIsShowMarkers(bool show);

        bool IsShowPseudoColor();
        void SetIsShowPseudoColor(bool pseudoColor);

        unsigned int GetDrawMarkerPointHalfWidth() const;
        void SetDrawMarkerPointHalfWidth(unsigned int halfWidth);

        eBaseSortPoints GetSortPointType();
        void SetSortPointType(eBaseSortPoints type);

        unsigned short GetServerPythonPort() const;
        void SetServerPythonPort(unsigned short port);

        unsigned short GetServerPspPort() const;
        void SetServerPspPort(unsigned short port);

    private:
        static bool mIsDrawTestPaintCalibration;
        static bool mIsDrawTestImageAlignment;
        static bool mIsDrawTestMarkers;
        static bool mIsDrawTestPsptoolMsg;

        vector<QString> mTestInfo;

        static bool mIsDebugMode;

        static int mBackgroundColor;
        static int mAssistantColor;
        static bool mIsShowMarkers;
        static bool mIsShowPseudoColor;
        static unsigned int mDrawMarkerPointHalfWidth;
        static eBaseSortPoints mSortPointType;
        static unsigned short mServerPythonPort;
        static unsigned short mServerPspPort;
    };
}
