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

/* Define a 3D point */
typedef struct {
  vec3 pos;
  vec3 color;
  vec3 v;
  float m;
} Point;

typedef struct {
  // data index
  GLuint v1, v2, v3, v4;
  GLuint vt1, vt2, vt3, vt4;
  GLuint vn1, vn2, vn3, vn4;
} Face;

class Mesh {
public:
  // mesh data
  vector<vec3> vertices;
  vector<vec2> uvs;
  vector<vec3> faceNormals;
  vector<Face> faces;

  // opengl data
  GLuint vboVtxs, vboUvs, vboNormals;
  GLuint vao;
  GLuint shader;
  GLuint tboBase, tboNormal, tboHeight;
  GLint uniModel, uniView, uniProjection;
  GLint uniEyePoint, uniLightColor, uniLightPosition;
  GLint uniTexBase, uniTexNormal, uniTexHeight;

  // aabb
  vec3 min, max;

  mat4 model, view, projection;
  int faceType;

  /* Constructors */
  Mesh(const string, int);
  ~Mesh();

  /* Member functions */
  void loadObj(const string);
  void loadObjQuad(const string);
  void initBuffers();
  void initBuffersQuad();
  void initShader();
  void initUniform();
  void draw(mat4, mat4, mat4, vec3, vec3, vec3, int, int, int);
  void setTexture(GLuint &, int, const string, FREE_IMAGE_FORMAT);

  void translate(vec3);
  void scale(vec3);
  void rotate(vec3);
  void findAABB();
};

class Quad {
public:
  vector<vec3> vtxs;
  vector<vec2> uvs;
  vector<vec3> nms;

  // opengl data
  GLuint vboVtxs, vboUvs, vboNormals;
  GLuint vao;
  GLuint shader;
  GLuint tboBase, tboNormal, tboHeight;
  GLint uniModel, uniView, uniProjection;
  GLint uniEyePoint, uniLightColor, uniLightPosition;
  GLint uniTexBase, uniTexNormal, uniTexHeight;
  GLint uniNumQuads, uniQuadIdx;

  mat4 model, view, projection;

  Quad();
  ~Quad();

  void initData();
  void initBuffers();
  void initShader();
  void initUniform();
  void draw(mat4, mat4, mat4, vec3, vec3, vec3, int, int, int);
  void setTexture(GLuint &, int, const string, FREE_IMAGE_FORMAT);
};

string readFile(const string);
void printLog(GLuint &);
GLint myGetUniformLocation(GLuint &, string);
GLuint buildShader(string, string, string, string);
GLuint compileShader(string, GLenum);
GLuint linkShader(GLuint, GLuint, GLuint, GLuint);
void drawBox(vec3, vec3);
void drawPoints(vector<Point> &);
