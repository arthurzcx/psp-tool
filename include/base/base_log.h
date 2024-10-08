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

#include "base/base_global.h"
#include <QObject>
#include <QString>

namespace PspNsBase {
    enum LogLevel {
        eTrace = 0,
        eDebug,
        eInfo,
        eWarning,
        eError
    };

    class BASE_SHARED Log : public QObject {
    Q_OBJECT

    private:
        Log() {}

    public:
        static Log *getInstance() {
            static Log GlobalLog;
            return &GlobalLog;
        }

        void writeLog(LogLevel level, const QString &log = QString());

    signals:
        void sendLogMsg(const QString &logMsg);


    private:
        void writeLogFile(const QString &log);
    };

#define LOG_CLIENT(x) Log::getInstance()->writeLog(LogLevel::eInfo, x)
#define LOG_WARNING(x) Log::getInstance()->writeLog(LogLevel::eWarning, x)
#define LOG_ERROR(x) Log::getInstance()->writeLog(LogLevel::eError, x)
}

