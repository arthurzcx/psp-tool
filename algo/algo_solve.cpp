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
#include "algo/algo_solve.h"

using namespace cv;

namespace PspNsAlgo
{
    PSP_ALGO_SHARED double calAverage(const Mat& src1, const Mat& src2)
    {
        if(src1.depth() != CV_16U ||src1.size() != src2.size() || src1.depth() != src2.depth() )
        {
            assert(false);
            return DBL_MAX;
        }

        int counter = 0;
        double sum = 0.0;
        for(int h = 0; h < src1.rows; h++)
        {
            for(int w = 0; w < src1.cols; w++)
            {
                uchar* data1 = src1.data + h*src1.step + w*src1.elemSize();
                int pixel1 = (*data1) + ((*(data1+1))<<8);

                uchar* data2 = src2.data + h*src2.step + w*src2.elemSize();
                int pixel2 = (*data2) + ((*(data2+1))<<8);

                if(pixel2 == 0)continue;

                sum += pixel1/(double)pixel2;
                counter++;
            }
        }

        if(counter == 0) assert(false);
        return sum/counter;
    }

    PSP_ALGO_SHARED bool LeaseSquaresProblem(const Mat& src1, const Mat& src2, Mat& dst)
    {
        return solve(src1, src2, dst, DECOMP_SVD);
    }
}
