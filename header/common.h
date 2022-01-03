// =======================================
// Headers: order matters
// =======================================
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>
#include <FreeImage.h>

using namespace std;
using namespace glm;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define TRIANGLE 0
#define QUAD 1

// =======================================
// Define a point
// =======================================
typedef struct
{
    vec3 pos;
    vec3 color;
    vec3 v;
    float m;
} Point;

// =======================================
// Define a face of a 3D model
// =======================================
typedef struct
{
    // Vertex indices
    GLuint v1, v2, v3, v4;

    // UV coordinate indices
    GLuint vt1, vt2, vt3, vt4;

    // Vertex normal indices
    GLuint vn1, vn2, vn3, vn4;
} Face;

// =======================================
// Define a mesh
// =======================================
class Mesh
{
  public:
    // --------------------------------
    // Member variables
    // --------------------------------
    // Vertex attributes
    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<vec3> faceNormals;
    vector<Face> faces;

    // OpenGL context
    GLuint vboVtxs, vboUvs, vboNormals;
    GLuint vao;
    GLuint shader;
    GLuint tboBase, tboNormal, tboHeight;
    GLint uniModel, uniView, uniProjection;
    GLint uniEyePoint, uniLightColor, uniLightPosition;
    GLint uniTexBase, uniTexNormal, uniTexHeight;

    // Transformation matrices
    mat4 model, view, projection;

    // Face type of the mesh (triangle or quad)
    int faceType;

    // --------------------------------
    // Constructor and destructor
    // --------------------------------
    Mesh(const string, int);
    ~Mesh();

    // --------------------------------
    // Member functions
    // --------------------------------
    void loadObj(const string);
    void loadObjQuad(const string);
    void initBuffers();
    void initBuffersQuad();
    void initShader();
    void initUniform();
    void draw(mat4, mat4, mat4, vec3, vec3, vec3, int);
    void setTexture(GLuint &, int, const string, FREE_IMAGE_FORMAT);
};

// =======================================
// OpenGL utilities
// =======================================
string readFile(const string);
void printLog(GLuint &);
GLint myGetUniformLocation(GLuint &, string, bool = false);
GLuint buildShader(string, string, string, string);
GLuint compileShader(string, GLenum);
GLuint linkShader(GLuint, GLuint, GLuint, GLuint);
void drawPoints(vector<Point> &);
