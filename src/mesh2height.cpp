// Generate a height map from a terrain mesh.
// It simply takes the xz-plane as the image plane,
// and takes the y-axis as the height.
// By default, the number of vertices in the xz-axis of the terrain must be equal.
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
void loadTerrainObj(const string);

// ========================================================
// Main function
// ========================================================
int main(int argc, char const *argv[])
{
    // Read mesh data into vertex list
    loadTerrainObj("./terrain.obj");

    // Create canvas
    int width = glm::sqrt(vertices.size());
    int height = width;
    canvas = Mat(height, width, CV_32FC3, Scalar(0, 0, 0));

    // Compute height for each vertex, then create height map
    for (size_t i = 0; i < vertices.size(); i++)
    {
        vec3 &vertex = vertices[i];

        // [-1, 1] to [0, 1]
        vertex = (vertex + vec3(1.f)) * 0.5f;

        // Compute row and column indices of the canvas
        int col = int(glm::min(vertex.x * float(width), float(width - 1)));
        int row = int(glm::min(vertex.z * float(height), float(height - 1)));

        // Get pixel from canvas
        Vec3f &pixel = canvas.at<Vec3f>(row, col);

        // Height -> color
        float height = vertex.y;
        float scale = 255.f;
        float color = height * scale;
        pixel[0] = color;
        pixel[1] = color;
        pixel[2] = color;
    }

    // Save canvas to image
    imwrite("test.png", canvas);

    return 0;
}

// ========================================================
// Load terrain mesh
// Parameters:
//   fileName: terrain mesh .obj file
// Remarks: Only used to read terrain mesh
// ========================================================
void loadTerrainObj(const string fileName)
{
    // Read terrain file
    ifstream fin;
    fin.open(fileName.c_str());
    if (!(fin.good()))
    {
        std::cout << "failed to open file : " << fileName << std::endl;
    }

    // Get terrain data
    while (fin.peek() != EOF)
    {
        string s;
        fin >> s;

        // Vertex coordinate
        if ("vertex" == s)
        {
            float x, y, z;
            fin >> x;
            fin >> y;
            fin >> z;
            vertices.push_back(vec3(x, y, z));
        }
        else
        {
            continue;
        }
    }

    fin.close();
}
