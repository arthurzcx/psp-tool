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
#include "base/base_log.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

namespace PspNsBase {

    void Log::writeLog(LogLevel level, const QString &log) {

        if (log.isEmpty()) return;

        writeLogFile(log);

        if ((int) level > (int) LogLevel::eDebug) {
            emit this->sendLogMsg(log);
        }
    }

    void Log::writeLogFile(const QString &log) {
        if (log.isEmpty()) return;

        const QDateTime dateTime = QDateTime::currentDateTime();
        const QString dt = dateTime.toString("yyyy-MM-dd hh:mm:ss");

        QString appPath = QCoreApplication::applicationDirPath();
        QString filePath = appPath + "/Psp.log";

        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            return;
        }
        QTextStream out(&file);
        out << dt + QString(" >> ") + log << "\n";
    }

}
