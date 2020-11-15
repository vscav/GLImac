#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <stb/stb_image.hpp>

#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/Image.hpp>
#include <glimac/FreeflyCamera.hpp>

#include <vector>
#include <iostream>

using namespace glimac;

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

struct PlanetProgram
{
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uLightIntensity;
    GLint uLightDir_vs;
    GLint uKd;
    GLint uKs;
    GLint uShininess;

    PlanetProgram(const FilePath &applicationPath) : m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                                                                           applicationPath.dirPath() + "shaders/directionallight.fs.glsl"))
    {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uLightIntensity = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");
        uLightDir_vs = glGetUniformLocation(m_Program.getGLId(), "uLightDir_vs");
        uKd = glGetUniformLocation(m_Program.getGLId(), "uKd");
        uKs = glGetUniformLocation(m_Program.getGLId(), "uKs");
        uShininess = glGetUniformLocation(m_Program.getGLId(), "uShininess");
    }
};

struct SkyBoxProgram
{
    Program m_Program;

    GLint uMVMatrix;
    GLint uMVPMatrix;

    SkyBoxProgram(const FilePath &applicationPath) : m_Program(loadProgram(applicationPath.dirPath() + "shaders/skybox.vs.glsl",
                                                                           applicationPath.dirPath() + "shaders/skybox.fs.glsl"))
    {
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "MVMatrix");
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "MVPMatrix");
    }
};

int main(int argc, char **argv)
{
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if (GLEW_OK != glewInitError)
    {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    // Shaders
    FilePath applicationPath(argv[0]);

    SkyBoxProgram skyBoxProgram(applicationPath);
    PlanetProgram planetProgram(applicationPath);

    // Activate GPU's depth test
    glEnable(GL_DEPTH_TEST);

    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), 800 / 600.f, 0.1f, 100.f);
    glm::mat4 MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    // Create a sphere (using glimac class Sphere)
    Sphere sphere(1, 32, 16);

    // Create a freefly camera (using the default constructor)
    FreeflyCamera camera;

    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_NORMAL = 1;
    const GLuint VERTEX_ATTR_TEXCOORD = 2;

    // Create a sphere model

    GLuint planetVAO, planetVBO;
    glGenVertexArrays(1, &planetVAO);
    glGenBuffers(1, &planetVBO);
    glBindVertexArray(planetVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planetVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORD);
    glVertexAttribPointer(
        VERTEX_ATTR_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(ShapeVertex),
        (const GLvoid *)offsetof(ShapeVertex, position));
    glVertexAttribPointer(
        VERTEX_ATTR_NORMAL,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(ShapeVertex),
        (const GLvoid *)offsetof(ShapeVertex, normal));
    glVertexAttribPointer(
        VERTEX_ATTR_TEXCOORD,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(ShapeVertex),
        (const GLvoid *)offsetof(ShapeVertex, texCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Create a SkyBox model

    GLfloat skyboxVertices[] = {
        // Positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glVertexAttribPointer(
        VERTEX_ATTR_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (GLvoid *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Cubemap (Skybox)
    std::vector<std::string> faces;

    faces.push_back("/home/vincent/Documents/Github/GLImac/assets/textures/skybox/space/right.png");
    faces.push_back("/home/vincent/Documents/Github/GLImac/assets/textures/skybox/space/left.png");
    faces.push_back("/home/vincent/Documents/Github/GLImac/assets/textures/skybox/space/top.png");
    faces.push_back("/home/vincent/Documents/Github/GLImac/assets/textures/skybox/space/bottom.png");
    faces.push_back("/home/vincent/Documents/Github/GLImac/assets/textures/skybox/space/front.png");
    faces.push_back("/home/vincent/Documents/Github/GLImac/assets/textures/skybox/space/back.png");

    GLuint cubemapTexture = loadCubemap(faces);

    // Application loop:
    bool done = false;
    while (!done)
    {
        // Event loop:
        SDL_Event e;
        while (windowManager.pollEvent(e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                done = true;
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                case SDLK_z:
                    camera.moveFront(1.f);
                    break;
                case SDLK_s:
                    camera.moveFront(-1.f);
                    break;
                case SDLK_q:
                    camera.moveLeft(1.f);
                    break;
                case SDLK_d:
                    camera.moveLeft(-1.f);
                    break;
                }
                break;
            case SDL_MOUSEMOTION:
                if (windowManager.isMouseButtonPressed(SDL_BUTTON_LEFT))
                {
                    if (e.motion.xrel != 0)
                    {
                        camera.rotateLeft(-e.motion.xrel / 1.5f);
                    }
                    if (e.motion.yrel != 0)
                    {
                        camera.rotateUp(-e.motion.yrel / 1.5f);
                    }
                    break;
                }
            default:
                break;
            }
        }

        // Clean the depth buffer on each loop
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Prevent that your skybox draws depth values, and the skybox will never be in front of anything that will be drawn later
        glDisable(GL_DEPTH_TEST);

        // Skybox
        skyBoxProgram.m_Program.use();

        // Remove the translation section of the MVMatrix
        MVMatrix = glm::mat4(glm::mat3(MVMatrix));

        glUniformMatrix4fv(skyBoxProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(skyBoxProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix));

        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Set depth function back to default
        glDepthFunc(GL_LESS);

        glEnable(GL_DEPTH_TEST);

        // Change depth function so depth test passes when values are equal to depth buffer's content
        glDepthFunc(GL_LEQUAL);

        // Get the ViewMatrix
        MVMatrix = camera.getViewMatrix();

        // Planet
        planetProgram.m_Program.use();

        glm::mat4 planetMVMatrix = glm::rotate(MVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0)); // Translation * Rotation
        glUniformMatrix4fv(planetProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(planetMVMatrix));
        glUniformMatrix4fv(planetProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(planetMVMatrix))));
        glUniformMatrix4fv(planetProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * planetMVMatrix));

        glm::mat4 lightMVMatrix = glm::rotate(MVMatrix, 4.75f, glm::vec3(-1, -1.5, 0)); // Translation * Rotation
        glm::vec3 lightDir_vs(lightMVMatrix * glm::vec4(1, 1, 1, 0));
        glUniform3f(planetProgram.uLightIntensity, 1, .5f, .5f);
        glUniform3fv(planetProgram.uLightDir_vs, 1, glm::value_ptr(lightDir_vs));
        glUniform3f(planetProgram.uKd, 0.05, 0, 1);
        glUniform3f(planetProgram.uKs, 0.15, 0.15, 1);
        glUniform1f(planetProgram.uShininess, 2);

        glBindVertexArray(planetVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planetVAO);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &skyboxVAO);

    return EXIT_SUCCESS;
}
