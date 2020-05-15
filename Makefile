CC=g++
STD=11
OPENCV_INSTALL_PATH=/usr/local
CFLAGS=`pkg-config --cflags ${OPENCV_INSTALL_PATH}/lib/pkgconfig/opencv.pc` -std=c++${STD} -Wall
LIBS=`pkg-config --libs ${OPENCV_INSTALL_PATH}/lib/pkgconfig/opencv.pc`

executable: src/blurOpt.cpp src/convolution.cpp src/crop.cpp
	$(CC) src/blurOpt.cpp src/convolution.cpp src/crop.cpp -o blur $(LIBS) $(CFLAGS)
