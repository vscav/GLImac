#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/Image.hpp>
#include <glimac/FreeflyCamera.hpp>

using namespace glimac;

struct EarthProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uEarthTexture;
    GLint uCloudTexture;

    EarthProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/pointlight.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uEarthTexture = glGetUniformLocation(m_Program.getGLId(), "uEarthTexture");
        uCloudTexture = glGetUniformLocation(m_Program.getGLId(), "uCloudTexture");
    }
};

struct MoonProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    MoonProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/pointlight.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
    }
};

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;
    
    // Shaders
    FilePath applicationPath(argv[0]);
    EarthProgram earthProgram(applicationPath);
    MoonProgram moonProgram(applicationPath);

    Program program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
								applicationPath.dirPath() + "shaders/pointlight.fs.glsl"));

    GLint uLightIntensity = glGetUniformLocation(program.getGLId(), "uLightIntensity");
    GLint uLightPos_vs = glGetUniformLocation(program.getGLId(), "uLightPos_vs");
    GLint uKd = glGetUniformLocation(program.getGLId(), "uKd");
    GLint uKs = glGetUniformLocation(program.getGLId(), "uKs");
    GLint uShininess = glGetUniformLocation(program.getGLId(), "uShininess");

    // Activate GPU's depth test
    glEnable(GL_DEPTH_TEST);

    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), 800 / 600.f, 0.1f, 100.f);
    glm::mat4 MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    // Create a sphere (using glimac class Sphere)
    Sphere sphere(1, 32, 16);

    // Create a trackball camera (using the default constructor)
    FreeflyCamera camera;

    // VBO creation
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // VBO binding
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Send data (related to the sphere) into the VBO
    glBufferData(GL_ARRAY_BUFFER,  sphere.getVertexCount() * sizeof(ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);

    // Unbind the VBO (to avoid errors)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAO creation
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // VAO binding
    glBindVertexArray(vao);

    // Vertex attributs position
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_NORMAL = 1;
    const GLuint VERTEX_ATTR_TEXCOORD = 2;

    // Enable vertex attributes array
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORD);

    // Activation of vertex attributs
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Define arrays of attribute data
    glVertexAttribPointer(
        VERTEX_ATTR_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(ShapeVertex),
        (const GLvoid*) offsetof(ShapeVertex, position)
    );
    glVertexAttribPointer(
        VERTEX_ATTR_NORMAL,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(ShapeVertex),
        (const GLvoid*) offsetof(ShapeVertex, normal)
    );
    glVertexAttribPointer(
        VERTEX_ATTR_TEXCOORD,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(ShapeVertex),
        (const GLvoid*) offsetof(ShapeVertex, texCoords)
    );

    // Unbind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind the VAO
    glBindVertexArray(0);

    // Get a certain amount of random transformation (rotation) axes
    unsigned int moonCount = 32;
	std::vector<glm::vec3> randomTransforms;
    std::vector<float> randomColors;
	for (unsigned int i = 0; i < moonCount; i++) {
        randomTransforms.push_back(glm::sphericalRand(2.f));
        randomColors.push_back(glm::linearRand(.1f, 1.f));
    }

    // Application loop:
	bool done = false;
	while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
			switch (e.type) {
				case SDL_QUIT:
					done = true;
					break;
                // case SDL_MOUSEWHEEL:
                //     // scroll up
                //     if(e.wheel.y > 0)
                //         std::cout << "Scroll up" << std::endl;
                //     // scroll down
                //     else if(e.wheel.y < 0)
                //         std::cout << "Scroll down" << std::endl;
                //     break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
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
                    if (windowManager.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
                        if (e.motion.xrel != 0) {
                            camera.rotateLeft(-e.motion.xrel / 1.5f);
                        }
                        if (e.motion.yrel != 0) {
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
        
        // Tell OpenGL to use the earthProgram
        earthProgram.m_Program.use();

        // Get the ViewMatrix
        MVMatrix = camera.getViewMatrix();

        // Planet Earth transformations
        glm::mat4 earthMVMatrix = glm::rotate(MVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0)); // Translation * Rotation
        
        // Send matrices to the GPU
        glUniformMatrix4fv(earthProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(earthMVMatrix));
        glUniformMatrix4fv(earthProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(earthMVMatrix))));
        glUniformMatrix4fv(earthProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * earthMVMatrix));
        glm::vec3 lightPos_vs(MVMatrix * glm::vec4(1, 1, 1, 0)); 
        glUniform3f(uLightIntensity, 1, .5f, .5f);
        glUniform3fv(uLightPos_vs, 1, glm::value_ptr(lightPos_vs));
        glUniform3f(uKd, 1, 0, 1);
        glUniform3f(uKs, 0, 0, 1);
        glUniform1f(uShininess, 5);

        // Bind the VAO
        glBindVertexArray(vao);
        
        // Drawing call
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        
        // Sphere as the main light
        glm::mat4 lightMVMatrix = glm::scale(MVMatrix, glm::vec3(0, 3, 3)); // Translation * Rotation * Translation * Scale
        
        // Send matrices to the GPU
        glUniformMatrix4fv(earthProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(lightMVMatrix));
        glUniformMatrix4fv(earthProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(lightMVMatrix))));
        glUniformMatrix4fv(earthProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * lightMVMatrix));
        glUniform3f(uLightIntensity, 1, 1, 1);
        glUniform3fv(uLightPos_vs, 1, glm::value_ptr(lightPos_vs));
        glUniform3f(uKd, 1,  1, 1);
        glUniform3f(uKs, 1, 1, 1);
        glUniform1f(uShininess, 6);

        // Drawing call
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        
        // Bind the VAO
        glBindVertexArray(vao);
        
        // Tell OpenGL to use the moonProgram 
        moonProgram.m_Program.use();
        
        for (int i = 0; i < moonCount; ++i) {
            // Moons transformation 
            glm::mat4 moonMVMatrix = glm::rotate(MVMatrix, (1 + randomTransforms[i][0] + randomTransforms[i][1] + randomTransforms[i][2]) * windowManager.getTime(),glm::vec3(0, 1, 0)); // Translation * Rotation
            moonMVMatrix = glm::translate(moonMVMatrix, randomTransforms[i]); // Translation * Rotation * Translation
            moonMVMatrix = glm::scale(moonMVMatrix, glm::vec3(0.2, 0.2, 0.2)); // Translation * Rotation * Translation * Scale
            
            // Send matrices to the GPU            
            glUniformMatrix4fv(moonProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(moonMVMatrix));
            glUniformMatrix4fv(moonProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(moonMVMatrix))));
            glUniformMatrix4fv(moonProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * moonMVMatrix));
            glUniform3f(uLightIntensity, .25, .25, .25);
            glUniform3fv(uLightPos_vs, 1, glm::value_ptr(lightPos_vs));
            glUniform3f(uKd, 0, 0, 0.85);
            glUniform3f(uKs, 0, 0, 0.15);
            glUniform1f(uShininess, 1);
            
            // Drawing call
            glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
            
        }
        
        // Unbind the VAO
        glBindVertexArray(0);
        
        // Update the display
        windowManager.swapBuffers();
	}

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	return EXIT_SUCCESS;
}
