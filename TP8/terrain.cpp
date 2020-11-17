#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <stb/stb_image.hpp>

#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/FreeflyCamera.hpp>
#include <glimac/Terrain.hpp>
#include <glimac/FastNoise.hpp>

#include <vector>
#include <iostream>

using namespace glimac;

struct TerrainProgram
{
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    TerrainProgram(const FilePath &applicationPath) : m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                                                                           applicationPath.dirPath() + "shaders/normals.fs.glsl"))
    {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
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
    TerrainProgram terrainProgram(applicationPath);

    // Activate GPU's depth test
    glEnable(GL_DEPTH_TEST);

    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), 800 / 600.f, 0.1f, 100.f);
    glm::mat4 MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    // Noise type
    FastNoise::NoiseType noiseType = static_cast<FastNoise::NoiseType>(4);

    // Create a terrain
    Terrain *t = nullptr;
    t = new Terrain(300 , 0.15, noiseType, 0.01);

    std::string config = t->getTerrainConfigString();

    std::cout << config << std::endl;

    int wireframe = 1;

    // Create a freefly camera (using the default constructor)
    FreeflyCamera camera;

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
                case SDLK_w:
                    wireframe = (wireframe == 0 ? 1 : 0);
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

        if (wireframe)
            glClearColor(.7f, .7f, .7f, .8f);
        else
            glClearColor(.25f, .25f, .25f, .8f);
        // Prevent that your skybox draws depth values, and the skybox will never be in front of anything that will be drawn later
        glDisable(GL_DEPTH_TEST);

        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_DEPTH_TEST);

        // Get the ViewMatrix
        MVMatrix = camera.getViewMatrix();

        // Planet 1 (reflection)
        terrainProgram.m_Program.use();

        glUniformMatrix4fv(terrainProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(terrainProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(MVMatrix))));
        glUniformMatrix4fv(terrainProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));

        t->render();

        // Update the display
        windowManager.swapBuffers();
    }

    delete t;

    return EXIT_SUCCESS;
}
