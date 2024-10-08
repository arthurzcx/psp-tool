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
#include "page_sectional_view.h"
#include "ui_page_sectional_view.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator>
#include <QtCharts/QBarCategoryAxis>
#include <QtWidgets/QApplication>
#include <QtCharts/QValueAxis>
#include <QDebug>
#include "nlohmann/json.hpp"

using namespace QtCharts;
using Json = nlohmann::json;

PageSectionalView::PageSectionalView(eSectionalViewType type,
                                     int minX, int maxX, int minY, int maxY,
                                     const std::vector<double> &valueX,
                                     const std::vector<double> &valueY, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PageSectionalView),
        mType(type),
        mMinX(minX),
        mMaxX(maxX),
        mMinY(minY),
        mMaxY(maxY),
        mValX(valueX),
        mValY(valueY) {
    ui->setupUi(this);

    ParseData();

    InitChart();
}

PageSectionalView::~PageSectionalView() {
    delete ui;
}

void PageSectionalView::InitChart() {
    double scaleStretch = 0.1;
    double xStretch = scaleStretch * (mMaxX - mMinX);
    double yStretch = scaleStretch * (mMaxY - mMinY);
    double valXStretch = scaleStretch * (mMaxValX - mMinValX);
    double valYStretch = scaleStretch * (mMaxValY - mMinValY);

    if (mType != eSectionalViewType::eVert) {
        QChart *chart = new QChart();
        chart->setTitle("Horiz");

        QSplineSeries *series = new QSplineSeries(chart);
        for (int i = 0; i < mMaxX - mMinX + 1; i++) {
            series->append(mMinX + i, mValY[i]);
        }
        chart->addSeries(series);
        chart->createDefaultAxes();

        chart->axisX()->setRange(mMinX - xStretch, mMaxX + xStretch);
        chart->axisY()->setRange(mMinValY - valYStretch, mMaxValY + valYStretch);

        static_cast<QValueAxis *>(chart->axisY())->setLabelFormat("%.3f  ");

        chart->setTheme(QChart::ChartTheme::ChartThemeDark);
        chart->legend()->hide();
        QChartView *chartView = new QChartView(chart);
        ui->horizontalLayout->addWidget(chartView);
    }
    if (mType != eSectionalViewType::eHoriz) {
        QChart *chart = new QChart();
        chart->setTitle("Vert");

        QSplineSeries *series = new QSplineSeries(chart);
        for (int i = 0; i < mMaxY - mMinY + 1; i++) {
            series->append(mMinY + i, mValX[i]);
        }
        chart->addSeries(series);
        chart->createDefaultAxes();

        chart->axisX()->setRange(mMinY - yStretch, mMaxY + yStretch);

        chart->axisY()->setRange(mMinValX - valXStretch, mMaxValX + valXStretch);
        chart->axisY()->setTitleText("value");

        static_cast<QValueAxis *>(chart->axisY())->setLabelFormat("%.2f  ");

        chart->setTheme(QChart::ChartTheme::ChartThemeDark);
        chart->legend()->hide();
        QChartView *chartView = new QChartView(chart);
        ui->horizontalLayout->addWidget(chartView);
    }

}

bool PageSectionalView::ParseData() {
    if (mType == eSectionalViewType::eVert || mType == eSectionalViewType::eBilateral) {
        mMaxValX = *std::max_element(mValX.begin(), mValX.end());
        mMinValX = *std::min_element(mValX.begin(), mValX.end());
    }
    if (mType == eSectionalViewType::eHoriz || mType == eSectionalViewType::eBilateral) {
        mMaxValY = *std::max_element(mValY.begin(), mValY.end());
        mMinValY = *std::min_element(mValY.begin(), mValY.end());
    }

    return true;
}
