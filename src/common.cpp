#include "common.h"

std::string readFile(const std::string fileName)
{
    std::ifstream in;
    in.open(fileName.c_str());
    std::stringstream ss;
    ss << in.rdbuf();
    std::string sOut = ss.str();
    in.close();

    return sOut;
}

// return a shader executable
GLuint buildShader(string vsDir, string fsDir, string tcsDir = "", string tesDir = "")
{
    GLuint vs, fs, tcs = 0, tes = 0;
    GLint linkOk;
    GLuint exeShader;

    // compile
    vs = compileShader(vsDir, GL_VERTEX_SHADER);
    fs = compileShader(fsDir, GL_FRAGMENT_SHADER);

    // TCS, TES
    if (tcsDir != "" && tesDir != "")
    {
        tcs = compileShader(tcsDir, GL_TESS_CONTROL_SHADER);
        tes = compileShader(tesDir, GL_TESS_EVALUATION_SHADER);
    }

    // link
    exeShader = linkShader(vs, fs, tcs, tes);

    return exeShader;
}

GLuint compileShader(string fileName, GLenum type)
{
    /* read source code */
    string sTemp = readFile(fileName);
    string info;
    const GLchar *source = sTemp.c_str();

    switch (type)
    {
        case GL_VERTEX_SHADER:
            info = "Vertex";
            break;
        case GL_FRAGMENT_SHADER:
            info = "Fragment";
            break;
    }

    if (source == NULL)
    {
        std::cout << info << " Shader : Can't read shader source file." << std::endl;
        return 0;
    }

    const GLchar *sources[] = {source};
    GLuint objShader = glCreateShader(type);
    glShaderSource(objShader, 1, sources, NULL);
    glCompileShader(objShader);

    GLint compile_ok;
    glGetShaderiv(objShader, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE)
    {
        std::cout << info << " Shader : Fail to compile." << std::endl;
        printLog(objShader);
        glDeleteShader(objShader);
        return 0;
    }

    return objShader;
}

GLuint linkShader(GLuint vsObj, GLuint fsObj, GLuint tcsObj, GLuint tesObj)
{
    GLuint exe;
    GLint linkOk;

    exe = glCreateProgram();
    glAttachShader(exe, vsObj);
    glAttachShader(exe, fsObj);

    if (tcsObj != 0 && tesObj != 0)
    {
        glAttachShader(exe, tcsObj);
        glAttachShader(exe, tesObj);
    }

    glLinkProgram(exe);

    // check result
    glGetProgramiv(exe, GL_LINK_STATUS, &linkOk);

    if (linkOk == GL_FALSE)
    {
        std::cout << "Failed to link shader program." << std::endl;
        printLog(exe);
        glDeleteProgram(exe);

        return 0;
    }

    return exe;
}

void printLog(GLuint &object)
{
    GLint log_length = 0;
    if (glIsShader(object))
    {
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    }
    else if (glIsProgram(object))
    {
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    }
    else
    {
        cerr << "printlog: Not a shader or a program" << endl;
        return;
    }

    char *log = (char *)malloc(log_length);

    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);

    cerr << log << endl;
    free(log);
}

GLint myGetUniformLocation(GLuint &prog, string name)
{
    GLint location;
    location = glGetUniformLocation(prog, name.c_str());
    if (location == -1)
    {
        cerr << "Could not bind uniform : " << name << ". "
             << "Did you set the right name? "
             << "Or is " << name << " not used?" << endl;
    }

    return location;
}

// ================================================
// Mesh class definition
// ================================================

// ---------------------------------------------------------
// Constructor
// Parameters:
//   1. fileName: mesh file
//   2. type: face type (triangle or quad)
// ---------------------------------------------------------
Mesh::Mesh(const string fileName, int type = TRIANGLE)
{
    faceType = type;

    if (type == TRIANGLE)
    {
        loadObj(fileName);
        initBuffers();
    }
    else if (type == QUAD)
    {
        loadObjQuad(fileName);
        initBuffersQuad();
    }

    initShader();
    initUniform();
}

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
Mesh::~Mesh()
{
    glDeleteBuffers(1, &vboVtxs);
    glDeleteBuffers(1, &vboUvs);
    glDeleteBuffers(1, &vboNormals);
    glDeleteVertexArrays(1, &vao);
}

// ---------------------------------------------------------
// Initialize shaders
// ---------------------------------------------------------
void Mesh::initShader()
{
    shader =
        buildShader("./shader/vsPhong.glsl", "./shader/fsPhong.glsl", "./shader/tcsQuad.glsl", "./shader/tesQuad.glsl");
}

// ---------------------------------------------------------
// Initialize uniforms
// ---------------------------------------------------------
void Mesh::initUniform()
{
    uniModel = myGetUniformLocation(shader, "M");
    uniView = myGetUniformLocation(shader, "V");
    uniProjection = myGetUniformLocation(shader, "P");
    uniEyePoint = myGetUniformLocation(shader, "eyePoint");
    uniLightColor = myGetUniformLocation(shader, "lightColor");
    uniLightPosition = myGetUniformLocation(shader, "lightPosition");
    uniTexBase = myGetUniformLocation(shader, "texBase");
    uniTexNormal = myGetUniformLocation(shader, "texNormal");
    uniTexHeight = myGetUniformLocation(shader, "texHeight");
}

// ---------------------------------------------------------
// Load mesh .obj (for triangle face)
// Parameters:
//   fileName: mesh file
// ---------------------------------------------------------
void Mesh::loadObj(const string fileName)
{
    std::ifstream fin;
    fin.open(fileName.c_str());

    if (!(fin.good()))
    {
        std::cout << "failed to open file : " << fileName << std::endl;
    }

    while (fin.peek() != EOF)
    {
        std::string s;
        fin >> s;

        // Vertex coordinate
        if ("v" == s)
        {
            float x, y, z;
            fin >> x;
            fin >> y;
            fin >> z;
            vertices.push_back(glm::vec3(x, y, z));
        }
        // Texture coordinate
        else if ("vt" == s)
        {
            float u, v;
            fin >> u;
            fin >> v;
            uvs.push_back(glm::vec2(u, v));
        }
        // Face normal (recorded as vn in obj file)
        else if ("vn" == s)
        {
            float x, y, z;
            fin >> x;
            fin >> y;
            fin >> z;
            faceNormals.push_back(glm::vec3(x, y, z));
        }
        // Vertices contained in face, and face normal
        else if ("f" == s)
        {
            Face f;

            // v1/vt1/vn1
            fin >> f.v1;
            fin.ignore(1);
            fin >> f.vt1;
            fin.ignore(1);
            fin >> f.vn1;

            // v2/vt2/vn2
            fin >> f.v2;
            fin.ignore(1);
            fin >> f.vt2;
            fin.ignore(1);
            fin >> f.vn2;

            // v3/vt3/vn3
            fin >> f.v3;
            fin.ignore(1);
            fin >> f.vt3;
            fin.ignore(1);
            fin >> f.vn3;

            // Note:
            //  v, vt, vn in "v/vt/vn" start from 1,
            //  but indices of std::vector start from 0,
            //  so we need minus 1 for all elements
            f.v1 -= 1;
            f.vt1 -= 1;
            f.vn1 -= 1;

            f.v2 -= 1;
            f.vt2 -= 1;
            f.vn2 -= 1;

            f.v3 -= 1;
            f.vt3 -= 1;
            f.vn3 -= 1;

            faces.push_back(f);
        }
        else
        {
            continue;
        }
    }

    fin.close();
}

// ---------------------------------------------------------
// Load mesh .obj (for quad face)
// Parameters:
//   fileName: mesh file
// ---------------------------------------------------------
void Mesh::loadObjQuad(const string fileName)
{
    std::ifstream fin;
    fin.open(fileName.c_str());

    if (!(fin.good()))
    {
        std::cout << "failed to open file : " << fileName << std::endl;
    }

    while (fin.peek() != EOF)
    {
        std::string s;
        fin >> s;

        // Vertex coordinate
        if ("v" == s)
        {
            float x, y, z;
            fin >> x;
            fin >> y;
            fin >> z;
            vertices.push_back(glm::vec3(x, y, z));
        }
        // Texture coordinate
        else if ("vt" == s)
        {
            float u, v;
            fin >> u;
            fin >> v;
            uvs.push_back(glm::vec2(u, v));
        }
        // Face normal (recorded as vn in obj file)
        else if ("vn" == s)
        {
            float x, y, z;
            fin >> x;
            fin >> y;
            fin >> z;
            faceNormals.push_back(glm::vec3(x, y, z));
        }
        // Vertices contained in face, and face normal
        else if ("f" == s)
        {
            Face f;

            // v1/vt1/vn1
            fin >> f.v1;
            fin.ignore(1);
            fin >> f.vt1;
            fin.ignore(1);
            fin >> f.vn1;

            // v2/vt2/vn2
            fin >> f.v2;
            fin.ignore(1);
            fin >> f.vt2;
            fin.ignore(1);
            fin >> f.vn2;

            // v3/vt3/vn3
            fin >> f.v3;
            fin.ignore(1);
            fin >> f.vt3;
            fin.ignore(1);
            fin >> f.vn3;

            // v4/vt4/vn4
            fin >> f.v4;
            fin.ignore(1);
            fin >> f.vt4;
            fin.ignore(1);
            fin >> f.vn4;

            // Note:
            //  v, vt, vn in "v/vt/vn" start from 1,
            //  but indices of std::vector start from 0,
            //  so we need minus 1 for all elements
            f.v1 -= 1;
            f.vt1 -= 1;
            f.vn1 -= 1;

            f.v2 -= 1;
            f.vt2 -= 1;
            f.vn2 -= 1;

            f.v3 -= 1;
            f.vt3 -= 1;
            f.vn3 -= 1;

            f.v4 -= 1;
            f.vt4 -= 1;
            f.vn4 -= 1;

            faces.push_back(f);
        }
        else
        {
            continue;
        }
    }

    fin.close();
}

// ---------------------------------------------------------
// Initialize OpenGL buffers (for triangle face)
// ---------------------------------------------------------
void Mesh::initBuffers()
{
    // Write vertex coordinate to array
    int nOfFaces = faces.size();

    // 3 vertices per face, 3 float per vertex coord, 2 float per tex coord
    GLfloat *aVtxCoords = new GLfloat[nOfFaces * 3 * 3];
    GLfloat *aUvs = new GLfloat[nOfFaces * 3 * 2];
    GLfloat *aNormals = new GLfloat[nOfFaces * 3 * 3];

    for (size_t i = 0; i < nOfFaces; i++)
    {
        // vertex 1
        int vtxIdx = faces[i].v1;
        aVtxCoords[i * 9 + 0] = vertices[vtxIdx].x;
        aVtxCoords[i * 9 + 1] = vertices[vtxIdx].y;
        aVtxCoords[i * 9 + 2] = vertices[vtxIdx].z;

        // Normal for vertex 1
        int nmlIdx = faces[i].vn1;
        aNormals[i * 9 + 0] = faceNormals[nmlIdx].x;
        aNormals[i * 9 + 1] = faceNormals[nmlIdx].y;
        aNormals[i * 9 + 2] = faceNormals[nmlIdx].z;

        // Uv for vertex 1
        int uvIdx = faces[i].vt1;
        aUvs[i * 6 + 0] = uvs[uvIdx].x;
        aUvs[i * 6 + 1] = uvs[uvIdx].y;

        // Vertex 2
        vtxIdx = faces[i].v2;
        aVtxCoords[i * 9 + 3] = vertices[vtxIdx].x;
        aVtxCoords[i * 9 + 4] = vertices[vtxIdx].y;
        aVtxCoords[i * 9 + 5] = vertices[vtxIdx].z;

        // Normal for vertex 2
        nmlIdx = faces[i].vn2;
        aNormals[i * 9 + 3] = faceNormals[nmlIdx].x;
        aNormals[i * 9 + 4] = faceNormals[nmlIdx].y;
        aNormals[i * 9 + 5] = faceNormals[nmlIdx].z;

        // Uv for vertex 2
        uvIdx = faces[i].vt2;
        aUvs[i * 6 + 2] = uvs[uvIdx].x;
        aUvs[i * 6 + 3] = uvs[uvIdx].y;

        // Vertex 3
        vtxIdx = faces[i].v3;
        aVtxCoords[i * 9 + 6] = vertices[vtxIdx].x;
        aVtxCoords[i * 9 + 7] = vertices[vtxIdx].y;
        aVtxCoords[i * 9 + 8] = vertices[vtxIdx].z;

        // Normal for vertex 3
        nmlIdx = faces[i].vn3;
        aNormals[i * 9 + 6] = faceNormals[nmlIdx].x;
        aNormals[i * 9 + 7] = faceNormals[nmlIdx].y;
        aNormals[i * 9 + 8] = faceNormals[nmlIdx].z;

        // Uv for vertex 3
        uvIdx = faces[i].vt3;
        aUvs[i * 6 + 4] = uvs[uvIdx].x;
        aUvs[i * 6 + 5] = uvs[uvIdx].y;
    }

    // Vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vbo for vertex
    glGenBuffers(1, &vboVtxs);
    glBindBuffer(GL_ARRAY_BUFFER, vboVtxs);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 3, aVtxCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Vbo for texture
    glGenBuffers(1, &vboUvs);
    glBindBuffer(GL_ARRAY_BUFFER, vboUvs);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 2, aUvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Vbo for normal
    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 3, aNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Release resource
    delete[] aVtxCoords;
    delete[] aUvs;
    delete[] aNormals;
}

// ---------------------------------------------------------
// Initialize OpenGL buffers (for quad face)
// ---------------------------------------------------------
void Mesh::initBuffersQuad()
{
    // Write vertex coordinate to array
    int nOfFaces = faces.size();

    // 4 vertices per face, 3 float per vertex coord, 2 float per tex coord
    GLfloat *aVtxCoords = new GLfloat[nOfFaces * 4 * 3];
    GLfloat *aUvs = new GLfloat[nOfFaces * 4 * 2];
    GLfloat *aNormals = new GLfloat[nOfFaces * 4 * 3];

    for (size_t i = 0; i < nOfFaces; i++)
    {
        // Vertex 1
        int vtxIdx = faces[i].v1;
        aVtxCoords[i * 12 + 0] = vertices[vtxIdx].x;
        aVtxCoords[i * 12 + 1] = vertices[vtxIdx].y;
        aVtxCoords[i * 12 + 2] = vertices[vtxIdx].z;

        // Normal for vertex 1
        int nmlIdx = faces[i].vn1;
        aNormals[i * 12 + 0] = faceNormals[nmlIdx].x;
        aNormals[i * 12 + 1] = faceNormals[nmlIdx].y;
        aNormals[i * 12 + 2] = faceNormals[nmlIdx].z;

        // Uv for vertex 1
        int uvIdx = faces[i].vt1;
        aUvs[i * 8 + 0] = uvs[uvIdx].x;
        aUvs[i * 8 + 1] = uvs[uvIdx].y;

        // Vertex 2
        vtxIdx = faces[i].v2;
        aVtxCoords[i * 12 + 3] = vertices[vtxIdx].x;
        aVtxCoords[i * 12 + 4] = vertices[vtxIdx].y;
        aVtxCoords[i * 12 + 5] = vertices[vtxIdx].z;

        // Normal for vertex 2
        nmlIdx = faces[i].vn2;
        aNormals[i * 12 + 3] = faceNormals[nmlIdx].x;
        aNormals[i * 12 + 4] = faceNormals[nmlIdx].y;
        aNormals[i * 12 + 5] = faceNormals[nmlIdx].z;

        // Uv for vertex 2
        uvIdx = faces[i].vt2;
        aUvs[i * 8 + 2] = uvs[uvIdx].x;
        aUvs[i * 8 + 3] = uvs[uvIdx].y;

        // Vertex 3
        vtxIdx = faces[i].v3;
        aVtxCoords[i * 12 + 6] = vertices[vtxIdx].x;
        aVtxCoords[i * 12 + 7] = vertices[vtxIdx].y;
        aVtxCoords[i * 12 + 8] = vertices[vtxIdx].z;

        // Normal for vertex 3
        nmlIdx = faces[i].vn3;
        aNormals[i * 12 + 6] = faceNormals[nmlIdx].x;
        aNormals[i * 12 + 7] = faceNormals[nmlIdx].y;
        aNormals[i * 12 + 8] = faceNormals[nmlIdx].z;

        // Uv for vertex 3
        uvIdx = faces[i].vt3;
        aUvs[i * 8 + 4] = uvs[uvIdx].x;
        aUvs[i * 8 + 5] = uvs[uvIdx].y;

        // Vertex 4
        vtxIdx = faces[i].v4;
        aVtxCoords[i * 12 + 9] = vertices[vtxIdx].x;
        aVtxCoords[i * 12 + 10] = vertices[vtxIdx].y;
        aVtxCoords[i * 12 + 11] = vertices[vtxIdx].z;

        // Normal for vertex 4
        nmlIdx = faces[i].vn4;
        aNormals[i * 12 + 9] = faceNormals[nmlIdx].x;
        aNormals[i * 12 + 10] = faceNormals[nmlIdx].y;
        aNormals[i * 12 + 11] = faceNormals[nmlIdx].z;

        // Uv for vertex 4
        uvIdx = faces[i].vt4;
        aUvs[i * 8 + 6] = uvs[uvIdx].x;
        aUvs[i * 8 + 7] = uvs[uvIdx].y;
    }

    // Vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vbo for vertex
    glGenBuffers(1, &vboVtxs);
    glBindBuffer(GL_ARRAY_BUFFER, vboVtxs);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 4 * 3, aVtxCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Vbo for texture
    glGenBuffers(1, &vboUvs);
    glBindBuffer(GL_ARRAY_BUFFER, vboUvs);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 4 * 2, aUvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Vbo for normal
    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 4 * 3, aNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Release resource
    delete[] aVtxCoords;
    delete[] aUvs;
    delete[] aNormals;
}

// ---------------------------------------------------------
// Set Texture for the mesh
// Parameters:
//   1. tbo: texture buffer object
//   2. texUnit: textunre unit
//   3. texDir: texture image file
//   4. imgType: texture image type
// ---------------------------------------------------------
void Mesh::setTexture(GLuint &tbo, int texUnit, const string texDir, FREE_IMAGE_FORMAT imgType)
{
    // Select a texture unit
    glActiveTexture(GL_TEXTURE0 + texUnit);

    // Create texture image
    FIBITMAP *texImage = FreeImage_ConvertTo24Bits(FreeImage_Load(imgType, texDir.c_str()));

    // Bind texture image to tbo
    glGenTextures(1, &tbo);
    glBindTexture(GL_TEXTURE_2D, tbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(texImage), FreeImage_GetHeight(texImage), 0, GL_BGR,
                 GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(texImage));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Release
    FreeImage_Unload(texImage);
}

// ---------------------------------------------------------
// Draw mesh
// Parameters:
//   1. M, V, P: transformation matrices
//   2. eye: eye point
//   3. lightColor, lightPosition: lighting
//   4. uniHeight: height map uniform
// ---------------------------------------------------------
void Mesh::draw(mat4 M, mat4 V, mat4 P, vec3 eye, vec3 lightColor, vec3 lightPosition, int uniHeight)
{
    // Bind shader program
    glUseProgram(shader);

    // Update uniforms
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(M));
    glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(V));
    glUniformMatrix4fv(uniProjection, 1, GL_FALSE, value_ptr(P));
    glUniform3fv(uniEyePoint, 1, value_ptr(eye));
    glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
    glUniform3fv(uniLightPosition, 1, value_ptr(lightPosition));
    glUniform1i(uniTexHeight, uniHeight);

    // Draw mesh
    glBindVertexArray(vao);
    if (faceType == QUAD)
    {
        glDrawArrays(GL_PATCHES, 0, faces.size() * 4);
    }
    else if (faceType == TRIANGLE)
    {
        glDrawArrays(GL_PATCHES, 0, faces.size() * 3);
    }
}

// ---------------------------------------------------------
// Draw points
// Parameters:
//   pts: point list
// ---------------------------------------------------------
void drawPoints(vector<Point> &pts)
{
    int nOfPs = pts.size();
    GLfloat *aPos = new GLfloat[nOfPs * 3];
    GLfloat *aColor = new GLfloat[nOfPs * 3];

    // Implant data
    for (size_t i = 0; i < nOfPs; i++)
    {
        // Position
        Point &p = pts[i];
        aPos[i * 3 + 0] = p.pos.x;
        aPos[i * 3 + 1] = p.pos.y;
        aPos[i * 3 + 2] = p.pos.z;

        // Color
        aColor[i * 3 + 0] = p.color.r;
        aColor[i * 3 + 1] = p.color.g;
        aColor[i * 3 + 2] = p.color.b;
    }

    // Bind vao
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Bind position vbo
    GLuint vboPos;
    glGenBuffers(1, &vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glBufferData(GL_ARRAY_BUFFER, nOfPs * 3 * sizeof(GLfloat), aPos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Bind color vbo
    GLuint vboColor;
    glGenBuffers(1, &vboColor);
    glBindBuffer(GL_ARRAY_BUFFER, vboColor);
    glBufferData(GL_ARRAY_BUFFER, nOfPs * 3 * sizeof(GLfloat), aColor, GL_STREAM_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Draw points
    glDrawArrays(GL_POINTS, 0, nOfPs);

    // Release
    delete[] aPos;
    delete[] aColor;
    glDeleteBuffers(1, &vboPos);
    glDeleteBuffers(1, &vboColor);
    glDeleteVertexArrays(1, &vao);
}
