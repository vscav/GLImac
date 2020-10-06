#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>

#include <glimac/glm.hpp>

using namespace glimac;

class Vertex2DColor
{
    public:

        glm::vec2 position;
        glm::vec3 color;

        Vertex2DColor(){};
        Vertex2DColor(glm::vec2 position, glm::vec3 color)
        {
            this->position = position;
            this->color = color;
        };
};

int main(int argc, char** argv)
{
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError)
    {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    FilePath applicationPath(argv[0]);
    Program program = loadProgram(applicationPath.dirPath() + "shaders/triangle.vs.glsl",
                                  applicationPath.dirPath() + "shaders/triangle.fs.glsl");
    program.use();

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // => Array of vertices
    Vertex2DColor vertices[] = {
        Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(1, 0, 0)), // Vertex 0
        Vertex2DColor(glm::vec2(0.5, -0.5), glm::vec3(0, 1, 0)), // Vertex 1
        Vertex2DColor(glm::vec2(0.5, 0.5), glm::vec3(0, 0, 1)), // Vertex 2
        Vertex2DColor(glm::vec2(-0.5, 0.5), glm::vec3(1, 1, 1)) // Vertex 3
    };
    // => 4 vertices instead of 6
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex2DColor), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // => IBO creation
    GLuint ibo;
    glGenBuffers(1, &ibo);

    // => Bind on GL_ELEMENT_ARRAY_BUFFER, specific target for IBOs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // => Array of indexes
    uint32_t indices[] = {
        0, 1, 2, 0, 2, 3
    };

    // => Fill IBO with indexes
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    // => We bind the IBO on GL_ELEMENT_ARRAY_BUFFER; because a VAO is currently binded,
    // it automatically save the IBO in the VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint VERTEX_ATTR_COLOR = 8;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, 
        sizeof(Vertex2DColor), (const GLvoid*) offsetof(Vertex2DColor, position));
    glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, 
        sizeof(Vertex2DColor), (const GLvoid*) offsetof(Vertex2DColor, color));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Application loop:
    bool done = false;
    while(!done)
    {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e))
        {
            if(e.type == SDL_QUIT)
            {
                done = true; // Leave the loop after this iteration
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);

        // => We use glDrawElements instead of glDrawArrays
        // It tells OpenGL that it must use the IBO currently save in the VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return EXIT_SUCCESS;
}