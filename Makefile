CXX=llvm-g++
COMPILE=-g -c -std=c++17 \
-I/usr/local/Cellar/glew/2.2.0_1/include \
-I/usr/local/Cellar/glfw/3.3.6/include \
-I/usr/local/Cellar/freeimage/3.18.0/include \
-I/usr/local/Cellar/glm/0.9.9.8/include \
-I/usr/local/Cellar/opencv/4.5.4_1/include/opencv4 \
-I/Users/YJ-work/cpp/myGL_glfw/tessellation/header
LINK=-L/usr/local/Cellar/glew/2.2.0_1/lib -lglfw \
-L/usr/local/Cellar/glfw/3.3.6/lib -lGLEW \
-L/usr/local/Cellar/freeimage/3.18.0/lib -lfreeimage \
-L/usr/local/Cellar/opencv/4.5.4_1/lib -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_imgcodecs \
-framework GLUT -framework OpenGL -framework Cocoa
SRC_DIR=/Users/YJ-work/cpp/myGL_glfw/tessellation/src

all: main mesh2height

main: main.o common.o
	$(CXX) $(LINK) $^ -o $@

main.o: $(SRC_DIR)/main.cpp
	$(CXX) $(COMPILE) $^ -o $@

common.o: $(SRC_DIR)/common.cpp
	$(CXX) $(COMPILE) $^ -o $@

mesh2height: mesh2height.o
	$(CXX) $(LINK) $^ -o $@

mesh2height.o: $(SRC_DIR)/mesh2height.cpp
	$(CXX) $(COMPILE) $^ -o $@

.PHONY: clean

cleanObj:
	rm -vf *.o

cleanImg:
	rm -vf ./result/*
