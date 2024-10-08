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
#include "page_histogram.h"
#include "ui_page_histogram.h"

#include <QtCharts/QChart>
#include "qchart.h"
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

using namespace QtCharts;

PageHistogram::PageHistogram(cv::Mat srcMat, QWidget *parent) :
        QDialog(parent),
        mSrcMat(srcMat),
        ui(new Ui::PageHistogram) {
    ui->setupUi(this);

    GetHistPoints();
    InitChart();
}

PageHistogram::~PageHistogram() {
    delete ui;
}

void PageHistogram::InitChart() {
    if (mHistPoints.empty()) return;

    int maxVal = *std::max_element(mHistPoints.begin(), mHistPoints.end());

    QChart *chart = new QChart();
    chart->setTitle("Histogram");

    QSplineSeries *series = new QSplineSeries(chart);
    for (size_t i = 0; i < mHistPoints.size(); i++) {
        series->append(i, mHistPoints[i]);
    }
    chart->addSeries(series);
    chart->createDefaultAxes();

    chart->axisX()->setRange(0, (int) mHistPoints.size());
    chart->axisY()->setRange(0, maxVal + 20);

    chart->setTheme(QChart::ChartTheme::ChartThemeDark);
    chart->legend()->hide();
    QChartView *chartView = new QChartView(chart);
    ui->horizontalLayout->addWidget(chartView);
}

void PageHistogram::GetHistPoints() {
    if (mSrcMat.empty()) return;

    switch (mSrcMat.type()) {
        case CV_16UC1:
            mHistPoints.resize(65536, 0);

            for (int i = 0; i < mSrcMat.rows; i++) {
                for (int j = 0; j < mSrcMat.cols; j++) {
                    mHistPoints[mSrcMat.at<ushort>(i, j)]++;
                }
            }
            break;
        case CV_8UC1:
            mHistPoints.resize(256, 0);
            for (int i = 0; i < mSrcMat.rows; i++) {
                for (int j = 0; j < mSrcMat.cols; j++) {
                    mHistPoints[mSrcMat.at<uchar>(i, j)]++;
                }
            }
            break;
        default:
            break;
    }
}
