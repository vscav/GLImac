#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>

using namespace glimac;

class Vertex2DUV
{
    public:

        glm::vec2 position;
        glm::vec2 textureCoordinates;

        Vertex2DUV(){};
        Vertex2DUV(glm::vec2 position, glm::vec2 textureCoordinates)
        : position(position), textureCoordinates(textureCoordinates)
        {};
};

glm::mat3 translate(float tx, float ty) {
    glm::mat3 M = glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(tx, ty, 1));
    return M;
}

glm::mat3 scale(float sx, float sy) {
    glm::mat3 M = glm::mat3(glm::vec3(sx, 0, 0), glm::vec3(0, sy, 0), glm::vec3(0, 0, 1));
    return M;
}

float anticlockwise = 0.0;
float clockwise = 0.0;

glm::mat3 rotate(float a) {
    float aRad = glm::radians(a);
    glm::mat3 M = glm::mat3(glm::vec3(cos(aRad), sin(aRad), 0), glm::vec3(-sin(aRad), cos(aRad), 0), glm::vec3(0, 0, 1));
    return M;
}

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    // Load texture (absolute path to the texture file)
    std::unique_ptr<Image> image = loadImage("/home/vincent/Documents/Github/GLImac/assets/textures/triforce.png");

    if (image == NULL) {
        std::cout << "Texture unique ptr is null. Exit program." << std::endl;
        return 0;
    }

    // Generate the texture
    GLuint triforceTexture;
    glGenTextures(1, &triforceTexture);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, triforceTexture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        image->getHeight(),
        image->getHeight(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image->getPixels()
    );

    // Texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Load shaders
    FilePath applicationPath(argv[0]);
    Program program = loadProgram(
        applicationPath.dirPath() + "shaders/tex2D-v2.vs.glsl",
        applicationPath.dirPath() + "shaders/tex2D-v2.fs.glsl"
    );
    // Make the program use them
    program.use();

    const GLuint programId = program.getGLId();

    GLint uModelMatrixLocation = glGetUniformLocation(programId, "uModelMatrix");

    GLint textureLocation = glGetUniformLocation(program.getGLId(), "uTexture");
    glUniform1i(textureLocation, 0);

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    // VBO creation
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // buffer binding
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Vertices array
    Vertex2DUV vertices[] = { 
        Vertex2DUV(glm::vec2(-1, -1), glm::vec2(0, 1)),
        Vertex2DUV(glm::vec2(1, -1), glm::vec2(1, 1)),
        Vertex2DUV(glm::vec2(0, 1), glm::vec2(0.5, 0))
    };

    // Send data (GL_STATIC_DRAW is used on a buffer whose data will never change)
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DUV), vertices, GL_STATIC_DRAW);

    // Unbind (to avoid errors)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAO creation (it will describe the way vertices attributs are sorted in one or more VBOs)
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
        sizeof(Vertex2DUV), 
        offsetof(Vertex2DUV, position)/*0*/
    );
        
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 
        2, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(Vertex2DUV), 
        (const GLvoid*) offsetof(Vertex2DUV, textureCoordinates)
    );
    
    // Unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind VAO
    glBindVertexArray(0);

    // Application loop:
    bool done = false;
    while(!done) {
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, triforceTexture);

        // Update rotating values
        anticlockwise -= 0.15;
        clockwise += 0.15;

        // Bind vao
        glBindVertexArray(vao);

         // Apply to uniform variable uModelMatrix
        glUniformMatrix3fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(translate(0.5, 0.5) * scale(0.25, 0.25) * rotate(clockwise)));

        // Drawing call
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Apply to uniform variable uModelMatrix
        glUniformMatrix3fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(translate(0.5, -0.5) * scale(0.25, 0.25) * rotate(anticlockwise)));

        // Drawing call
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Apply to uniform variable uModelMatrix
        glUniformMatrix3fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(translate(-0.5, -0.5) * scale(0.25, 0.25) * rotate(clockwise)));

        // Drawing call
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Apply to uniform variable uModelMatrix
        glUniformMatrix3fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(translate(-0.5, 0.5) * scale(0.25, 0.25) * rotate(anticlockwise)));

        // Drawing call
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Unbind vao
        glBindVertexArray(0);

        // Unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &triforceTexture);

    return EXIT_SUCCESS;
}