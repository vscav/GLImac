#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/Image.hpp>

using namespace glimac;

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

    // Load shaders (relative to the current directory path)
    FilePath applicationPath(argv[0]);
    Program program = loadProgram(
        applicationPath.dirPath() + "shaders/3D.vs.glsl",
        applicationPath.dirPath() + "shaders/tex3D.fs.glsl"
    );
    // Make sure the program use them
    program.use();

    // Get program's ID
    const GLuint programId = program.getGLId();

    // Get uniforms location
    GLint MVPMatrixLocation = glGetUniformLocation(programId, "uMVPMatrix");
    GLint MVMatrixLocation = glGetUniformLocation(programId, "uMVMatrix");
    GLint NormalMatrixLocation = glGetUniformLocation(programId, "uNormalMatrix");
    GLint uTextureLocation = glGetUniformLocation(programId, "uTexture");

    // Activate GPU's depth test
    glEnable(GL_DEPTH_TEST);

    // Load textures (absolute path to the texture files)
    std::unique_ptr<Image> planetEarthTexture = loadImage("/home/vincent/Documents/Github/GLImac/assets/textures/EarthMap.jpg");
    std::unique_ptr<Image> moonTexture = loadImage("/home/vincent/Documents/Github/GLImac/assets/textures/MoonMap.jpg");

    if (planetEarthTexture == NULL) {
        std::cout << "Planet Earth texture unique ptr is null. Exit program." << std::endl;
        return 0;
    }

    if (moonTexture == NULL) {
        std::cout << "Moon texture unique ptr is null. Exit program." << std::endl;
        return 0;
    }

    // Number of textures used
    unsigned int texturesCount = 2;

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

        // Planet Earth transformations
        MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5)); // Translation
		MVMatrix = glm::rotate(MVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0)); // Translation * Rotation

        // Send matrices to the GPU
        glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(MVMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(NormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        // Bind the VAO
        glBindVertexArray(vao);

        // Bind the planet earth texture
		glBindTexture(GL_TEXTURE_2D, textures[0]);

        // Drawing call
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());

        // Unbind the VAO
        glBindVertexArray(0);

        // Bind the VAO
        glBindVertexArray(vao);

        // Bind the moon texture
		glBindTexture(GL_TEXTURE_2D, textures[1]);

        for(unsigned int i = 0; i < moonCount; i++) {
            // Moons transformation
            glm::mat4 MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5)); // Translation
            MVMatrix = glm::rotate(MVMatrix, (1 + randomTransforms[i][0]+randomTransforms[i][1]+randomTransforms[i][2]) * windowManager.getTime(), glm::vec3(0, 1, 0)); // Translation * Rotation
            MVMatrix = glm::translate(MVMatrix, randomTransforms[i]); // Translation * Rotation * Translation
            MVMatrix = glm::scale(MVMatrix, glm::vec3(0.2, 0.2, 0.2)); // Translation * Rotation * Translation * Scale

            // Send matrices to the GPU
            glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
            glUniformMatrix4fv(MVMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVMatrix));
            glUniformMatrix4fv(NormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

            // Drawing call
            glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        }

        // Unbind the texture
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
