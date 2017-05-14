CXX = g++
# INCLUDE = -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/
# LIBFLAGS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_stitching
OPTFLAGS = -std=c++11 -o analogy -O3

all: main.cpp
	$(CXX) $(OPTFLAGS) main.cpp `pkg-config --libs opencv`

clean:
	rm analogy