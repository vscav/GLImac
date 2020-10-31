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

    // Load shaders
    FilePath applicationPath(argv[0]);
    Program program = loadProgram(
        applicationPath.dirPath() + "shaders/color2D.vs.glsl",
        applicationPath.dirPath() + "shaders/color2D.fs.glsl"
    );
    // Make the program use them
    program.use();

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    // VBO creation
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Vertices array
    Vertex2DColor vertices[] = { 
        Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(1, 0, 0)),
        Vertex2DColor(glm::vec2(0.5, -0.5), glm::vec3(0, 1, 0)),
        Vertex2DColor(glm::vec2(0, 0.5), glm::vec3(0, 0, 1))
    };

    // Send data
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DColor), vertices, GL_STATIC_DRAW);

    // Unbind (to avoid errors)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAO creation
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // VAO binding
    glBindVertexArray(vao);

    // Activation of vertex attributs
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 
        2, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(Vertex2DColor), 
        offsetof(Vertex2DColor, position)/*0*/
    );
        
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(Vertex2DColor), 
        (const GLvoid*) offsetof(Vertex2DColor, color)
    );
    
    // Unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind VAO
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

        // Bind vao
        glBindVertexArray(vao);

        // Drawing call
        glDrawArrays(GL_TRIANGLES, 0 /* Because no offset */, 3);

        // Unbind vao
        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    return EXIT_SUCCESS;
}
