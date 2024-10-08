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
#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QThread>
#include <QTranslator>

void setOrganizationInformation() {
    QCoreApplication::setOrganizationName("XXXXXXXX");
    QCoreApplication::setApplicationName("PspTool");
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    setOrganizationInformation();

    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/res-icon/window-2.png"));
    splash->show();

    splash->showMessage(QObject::tr("PspTool is loading moudules..."), Qt::AlignLeft | Qt::AlignBottom, Qt::blue);

    QThread::sleep(2);

    MainWindow w;
    a.installTranslator(w.GetTranslator());
    w.translateUi();
    QObject::connect(&w, SIGNAL(destroyed()), &a, SLOT(quit()));
    w.show();

    w.updateStatus(QObject::tr("Welcome use PspTool! Have a nice day!"));

    splash->finish(&w);
    delete splash;

    return a.exec();
}
