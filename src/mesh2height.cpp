// Generate a height map from a terrain mesh.
// It simply takes the xz-plane as the image plane,
// and takes the y-axis as the height.
// By default,
// the number of vertices in the xz-axis of the terrain must be equal.
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace glm;
using namespace std;
using namespace cv;

Mat canvas;
vector<vec3> vertices;

void loadObj(const string);

int main(int argc, char const *argv[]) {
  loadObj("./terrain.obj");

  int width, height;
  width = glm::sqrt(vertices.size());
  height = width;

  canvas = Mat(height, width, CV_32FC3, Scalar(0, 0, 0));

  for (size_t i = 0; i < vertices.size(); i++) {
    vec3 &v = vertices[i];

    // [-1, 1] to [0, 1]
    v = (v + vec3(1.f)) * 0.5f;

    // clamp to [0, 1023]
    int col = int(glm::min(v.x * float(width), float(width - 1)));
    int row = int(glm::min(v.z * float(height), float(height - 1)));

    float height = v.y;

    Vec3f &pixel = canvas.at<Vec3f>(row, col);

    float scale = 255.f;
    float color = height * scale;
    pixel[0] = color;
    pixel[1] = color;
    pixel[2] = color;
  }

  imwrite("test.png", canvas);

  return 0;
}

void loadObj(const string fileName) {
  ifstream fin;
  fin.open(fileName.c_str());

  if (!(fin.good())) {
    std::cout << "failed to open file : " << fileName << std::endl;
  }

  while (fin.peek() != EOF) { // read obj loop
    string s;
    fin >> s;

    // vertex coordinate
    if ("v" == s) {
      float x, y, z;
      fin >> x;
      fin >> y;
      fin >> z;
      vertices.push_back(vec3(x, y, z));
    } else {
      continue;
    }
  } // end read obj loop

  fin.close();
}
