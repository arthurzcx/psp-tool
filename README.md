**PspTool**
A software for Pressure-sensitive paint data process.

### LICENSE
GNU

The following open source software protocols must be followed:
OpenCV,
Qt,
nlohmann/json.

#### HOW TO BUILD

First, please compile OpenCV (4.10 is recommended).

Second, install Qt5 (Qt5.12.6 is recommended).

Third, config CMAKE_PREFIX_PATH for Qt5, and config OPENCV_INC for OpenCV header files include directory.

And then, build as follows.

```
mkdir build

cd build

cmake -DCMAKE_PREFIX_PATH=/xxx/Qt5.12.6/5.12.6/gcc_64/ -DOPENCV_INC=/usr/local/include/opencv4 ..

make install -j6
```

### EXAMPLES
![Automatic Marker Detection](https://github.com/arthurzcx/psp-tool/blob/main/data/Automatic%20Marker%20Detection.PNG)
![Images Alignment](https://github.com/arthurzcx/psp-tool/blob/main/data/Images%20Alignment.PNG)
![Pressue Map](https://github.com/arthurzcx/psp-tool/blob/main/data/PressueMap.PNG)
![Pressue Map With Alignment](https://github.com/arthurzcx/psp-tool/blob/main/data/PressueMapWithAlignment.PNG)
![Paint Calibration](https://github.com/arthurzcx/psp-tool/blob/main/data/psp-paint-calibration.png)
![Graphic User Interface](https://github.com/arthurzcx/psp-tool/blob/main/data/psp-software-alpha.png)


