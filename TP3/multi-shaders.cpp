#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/Image.hpp>

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
                              applicationPath.dirPath() + "shaders/multiTex3D.fs.glsl")) {
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
                              applicationPath.dirPath() + "shaders/tex3D.fs.glsl")) {
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

    // Activate GPU's depth test
    glEnable(GL_DEPTH_TEST);

    // Load textures (absolute path to the texture files)
    std::unique_ptr<Image> planetEarthTexture = loadImage("/home/vincent/Documents/Github/GLImac/assets/textures/EarthMap.jpg");
    std::unique_ptr<Image> moonTexture = loadImage("/home/vincent/Documents/Github/GLImac/assets/textures/MoonMap.jpg");
    std::unique_ptr<Image> cloudTexture = loadImage("/home/vincent/Documents/Github/GLImac/assets/textures/CloudMap.jpg");

    if (planetEarthTexture == NULL) {
        std::cout << "Planet Earth texture unique ptr is null. Exit program." << std::endl;
        return 0;
    }

    if (moonTexture == NULL) {
        std::cout << "Moon texture unique ptr is null. Exit program." << std::endl;
        return 0;
    }

    if (cloudTexture == NULL) {
        std::cout << "Cloud texture unique ptr is null. Exit program." << std::endl;
        return 0;
    }

    // Number of textures used
    unsigned int texturesCount = 3;

    // Generate textures
    GLuint *textures = new GLuint[texturesCount];
    glGenTextures(texturesCount, textures);

    // Bind the first texture
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		planetEarthTexture->getWidth(),
		planetEarthTexture->getHeight(),
		0,
		GL_RGBA,
		GL_FLOAT,
		planetEarthTexture->getPixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Unbind
	glBindTexture(GL_TEXTURE_2D, 0);

    // Bind the second texture
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		moonTexture->getWidth(),
		moonTexture->getHeight(),
		0,
		GL_RGBA,
		GL_FLOAT,
		moonTexture->getPixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Unbind
	glBindTexture(GL_TEXTURE_2D, 0);

    // Bind the third texture
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		cloudTexture->getWidth(),
		cloudTexture->getHeight(),
		0,
		GL_RGBA,
		GL_FLOAT,
		cloudTexture->getPixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Unbind
	glBindTexture(GL_TEXTURE_2D, 0);

    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), 800 / 600.f, 0.1f, 100.f);
    glm::mat4 MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    // Create a sphere (using glimac class Sphere)
    Sphere sphere(1, 32, 16);

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
	for (unsigned int i = 0; i < moonCount; i++)
		randomTransforms.push_back(glm::sphericalRand(2.f));

    // Application loop:
    bool done = false;
    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true; // Leave the loop after this iteration
            }
        }

        // Clean the depth buffer on each loop
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Tell OpenGL to use the earthProgram
        earthProgram.m_Program.use();

        // Use to know on which unit the planet earth texture can be read by OpenGL
        glUniform1i(earthProgram.uEarthTexture, 0);
        // Use to know on which unit the cloud texture can be read by OpenGL
        glUniform1i(earthProgram.uCloudTexture, 1);

        glm::mat4 globalMVMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -5));

        // Planet Earth transformations
        glm::mat4 earthMVMatrix = glm::rotate(globalMVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0));

        // Send matrices to the GPU
        glUniformMatrix4fv(earthProgram.uMVMatrix, 1, GL_FALSE, 
            glm::value_ptr(earthMVMatrix));
        glUniformMatrix4fv(earthProgram.uNormalMatrix, 1, GL_FALSE, 
            glm::value_ptr(glm::transpose(glm::inverse(earthMVMatrix))));
        glUniformMatrix4fv(earthProgram.uMVPMatrix, 1, GL_FALSE, 
            glm::value_ptr(ProjMatrix * earthMVMatrix));

        // Bind the planet earth texture on the GL_TEXTURE0 unit
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        // Bind the cloud texture on the GL_TEXTURE1 unit
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[2]);

        // Drawing call
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());

        // Unbind of GL_TEXTURE1
        glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

        // Tell OpenGL to use the moonProgram
        moonProgram.m_Program.use();

        for(unsigned int i = 0; i < moonCount; i++) {
            // Moons transformation
            glm::mat4 moonMVMatrix = glm::rotate(globalMVMatrix, (1+randomTransforms[i][0]+randomTransforms[i][1]+randomTransforms[i][2]) * windowManager.getTime(), glm::vec3(0, 1, 0)); // Translation * Rotation
            moonMVMatrix = glm::translate(moonMVMatrix, randomTransforms[i]); // Translation * Rotation * Translation
            moonMVMatrix = glm::scale(moonMVMatrix, glm::vec3(0.2, 0.2, 0.2)); // Translation * Rotation * Translation * Scale

            // Send matrices to the GPU
            glUniformMatrix4fv(moonProgram.uMVMatrix, 1, GL_FALSE, 
                glm::value_ptr(moonMVMatrix));
            glUniformMatrix4fv(moonProgram.uNormalMatrix, 1, GL_FALSE, 
                glm::value_ptr(glm::transpose(glm::inverse(moonMVMatrix))));
            glUniformMatrix4fv(moonProgram.uMVPMatrix, 1, GL_FALSE, 
                glm::value_ptr(ProjMatrix * moonMVMatrix));

            // Bind the moon texture on the GL_TEXTURE0 unit
            glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[1]);

            // Drawing call
            glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        }

        // Unbind of GL_TEXTURE0 unit
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Unbind the VAO
        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    delete textures;

    return EXIT_SUCCESS;
}
