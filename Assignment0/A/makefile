CXX = g++
# INCLUDE = -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/
# LIBFLAGS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_stitching
OPTFLAGS = -std=c++11 -o colorizer -O3

all: src/*.cpp src/*.hpp
	$(CXX) $(OPTFLAGS) src/*.cpp `pkg-config --libs opencv`
	
clean:
	rm colorizer