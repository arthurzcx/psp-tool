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
#include "base_log.h"
#include "base_system_param.h"
#include <QString>
#include <string>

#ifdef PLATFORM_WINDOWS
#include <intrin.h>
#include <array>
#include <vector>
#endif

namespace PspNsBase {

    class BASE_SHARED PspTrace {
    public:
        explicit PspTrace(const char *file, const char *func, int line, const QString &info = QString("null"))
                : mFile(file), mFunc(func), mLine(QString::number(line, 10)), mInfo(info) {
            if (SystemParam::GetInstance()->IsDebugMode()) {
                Log::getInstance()->writeLog(LogLevel::eTrace, traceIn());
            }
        }

        const QString traceIn() {
            return QString("Trace In: File>> ") + mFile + QString(", Func>> ") + mFunc + QString(", Line>> ") + mLine +
                   QString(", Desc>> ") + mInfo;
        }

        const QString traceOut() {
            return QString("Trace Out: File>> ") + mFile + QString(", Func>> ") + mFunc + QString(", Line>> ") + mLine +
                   QString(", Desc>> ") + mInfo;
        }

        void setInfo(const QString &info) {
            mInfo = info;
        }

        ~PspTrace() {
            if (SystemParam::GetInstance()->IsDebugMode()) {
                Log::getInstance()->writeLog(LogLevel::eTrace, traceOut());
            }
        }

        static QString getCpuId() {
#ifdef PLATFORM_WINDOWS
            std::array<int, 4> cpui;
            __cpuid(cpui.data(), 0);

            std::vector<std::array<int, 4>> data_;
            for(int i = 0; i < cpui[0]; i++)
            {
                __cpuidex(cpui.data(), i, 0);
                data_.push_back(cpui);
            }

            char vendor_serialnumber[0x14] = { 0 };
            sprintf_s(vendor_serialnumber, sizeof(vendor_serialnumber), "%08X%08X", data_[1][3], data_[1][0]);

            return QString(vendor_serialnumber);
#else
            return QString("NONE");
#endif
        }

    private:
        QString mFile;
        QString mFunc;
        QString mLine;
        QString mInfo;

    };

}
