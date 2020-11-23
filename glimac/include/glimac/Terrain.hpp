#pragma once
#define _USE_MATH_DEFINES

#include "FastNoise.hpp"

#include <GL/glew.h>
#include "glm.hpp"

#include <vector>
#include <iostream>
#include <math.h>
#include <cmath>
#include <string>

class Terrain
{
public:
    Terrain(GLuint size, GLfloat tileSize, FastNoise::NoiseType noiseType, GLfloat noiseFrequency);
    Terrain(GLuint size, GLfloat tileSize, FastNoise::NoiseType noiseType, GLfloat noiseFrequency, GLint seed);
    Terrain(GLuint size, GLfloat tileSize, FastNoise::NoiseType noiseType, GLfloat noiseFrequency, GLint seed, GLint octaves, GLint magnitude, GLboolean isIsland);
    
    std::string getTerrainConfigString();

    glm::vec3 getFirstVertexPosition();

    void makeIsland();

    void render();

private:
    GLuint m_VAO;
    GLuint m_VBO, m_EBO;

    std::vector<GLfloat> m_vertices;
    std::vector<GLuint> m_indices;

    GLfloat m_fillR = 0.0f;
    GLfloat m_fillG = 0.0f;
    GLfloat m_fillB = 0.0f;

    GLuint m_width;
    GLuint m_height;

    GLint m_centerX;
    GLint m_centerY;
    GLfloat m_maxDistance;

    GLfloat m_tileSize;
    GLint m_seed;
    GLuint m_octaves;
    GLint m_magnitude;
    GLfloat m_exponent = 2;

    GLboolean m_isIsland = false;

    FastNoise m_noise;
    GLfloat m_noiseFrequency;
    FastNoise::NoiseType m_noiseType;

    std::vector<std::vector<GLfloat>> colours = {
        //  r     g     b
        {
            0.25f,
            0.36f,
            1.56f,
        }, // water
        {
            0.49f,
            0.72f,
            0.45f,
        }, // land
        {
            0.45f,
            0.72f,
            0.46f,
        }, // higher land
        {
            1.0f,
            1.0f,
            1.0f,
        } // snow
    };

    glm::mat4 model = glm::mat4(1.0f);

    void setDefaults();
    void calculateMaxDistance();

    void createTerrain();

    void generateVertices();
    void generateIndices();

    void initBuffers();

    void addColourForHeight(GLfloat &y);

    void loadIntoShader();
};
