#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

using namespace glimac;

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

    // VBO creation
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Vertices array
    GLfloat vertices[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f };

    // Send data
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    // Unbind (to avoid errors)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAO creation
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // VAO binding
    glBindVertexArray(vao);

    // Activation of vertex attributs
    const GLuint VERTEX_ATTR_POSITION = 0;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glVertexAttribPointer(
        VERTEX_ATTR_POSITION, 
        2, 
        GL_FLOAT, 
        GL_FALSE, 
        2 * sizeof(GLfloat), 
        0
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
