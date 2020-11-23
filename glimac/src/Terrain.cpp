#include "glimac/Terrain.hpp"

Terrain::Terrain(GLuint size, GLfloat tileSize, FastNoise::NoiseType noiseType, GLfloat noiseFrequency) : m_width(size), m_height(size), m_tileSize(tileSize), m_noiseType(noiseType), m_noiseFrequency(noiseFrequency), m_seed(rand())
{
	setDefaults();
	calculateMaxDistance();
	createTerrain();
}

Terrain::Terrain(GLuint size, GLfloat tileSize, FastNoise::NoiseType noiseType, GLfloat noiseFrequency, GLint seed) : m_width(size), m_height(size), m_tileSize(tileSize), m_noiseType(noiseType), m_noiseFrequency(noiseFrequency), m_seed(seed)
{
	setDefaults();
	calculateMaxDistance();
	createTerrain();
}

Terrain::Terrain(GLuint size, GLfloat tileSize, FastNoise::NoiseType noiseType, GLfloat noiseFrequency, GLint seed, GLint octaves, GLint magnitude, GLboolean isIsland) : m_width(size), m_height(size), m_tileSize(tileSize), m_noiseType(noiseType), m_noiseFrequency(noiseFrequency), m_seed(seed), m_octaves(octaves), m_magnitude(magnitude)
{
	createTerrain();
	calculateMaxDistance();
	if (isIsland)
	{
		makeIsland();
	}
}

std::string Terrain::getTerrainConfigString()
{
	std::string result = "";

	result += "terrain_size=" + std::to_string(m_width) + "\n";
	result += "tile_size=" + std::to_string(m_tileSize) + "\n";
	result += "noise_type=" + std::to_string(m_noise.GetNoiseType()) + "\n";
	result += "noise_m_seed=" + std::to_string(m_noise.GetSeed()) + "\n";
	result += "noise_frequency=" + std::to_string(m_noise.GetFrequency()) + "\n";
	result += "noise_m_octaves=" + std::to_string(m_noise.GetFractalOctaves()) + "\n";
	result += "noise_m_magnitude=" + std::to_string(m_magnitude) + "\n";
	result += "is_island=" + std::to_string(m_isIsland) + "\n";

	return result;
}

glm::vec3 Terrain::getFirstVertexPosition()
{
	// returns position of first vertex in grid , before the terrain is centre translated (so this will return { 0, noise(), 0 })
	return glm::vec3(m_vertices[0], m_vertices[1], m_vertices[2]);
}

void Terrain::makeIsland()
{
	// step for each vertex data set
	const int step = 9;

	// iterate rows and columns, and insert new m_height & colour values
	for (GLint row = 0; row < m_width; row++)
	{
		// find the current index offset for the row
		GLuint rowIndexOffset = row * m_width * step;
		for (GLint col = 0; col < m_height; col++)
		{
			// find the current index offset for the column
			GLuint colIndexOffset = col * step;

			// find the index for the vertex data
			GLuint vertexStartIndex = rowIndexOffset + colIndexOffset;

			// find X and Y distance from the center
			GLfloat diffX = fabsf(m_centerX - col);
			GLfloat diffY = fabsf(m_centerY - row);

			// calculate normalised distance
			GLfloat distance = sqrtf(pow(diffX, 2) + pow(diffY, 2));
			GLfloat distanceNrm = distance / m_maxDistance;

			// fetch current value
			GLfloat y = m_vertices[vertexStartIndex + 1];

			// modify value using its distance, based on m_exponential curve
			y = y * (1 - distanceNrm) + pow(M_E, (-5 * distanceNrm)) - distanceNrm;

			// if y is now less than 0, return it to 0
			if (y < 0.0f)
				y = 0.0f;

			// set index to new value
			m_vertices[vertexStartIndex + 1] = y;

			// add colour
			addColourForHeight(y);
		}
	}

	loadIntoShader();
	m_isIsland = true;
}

// void Terrain::render(GLuint& program)
void Terrain::render()
{
	glBindVertexArray(m_VAO);

	// draw fill colour
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void *)(sizeof(GLfloat) * 3));
	glDrawElements(GL_TRIANGLE_STRIP, m_indices.size(), GL_UNSIGNED_INT, 0);

	// draw polygon colour
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void *)(sizeof(GLfloat) * 3));
	// glDrawElements(GL_TRIANGLE_STRIP, m_indices.size(), GL_UNSIGNED_INT, 0);
}

void Terrain::setDefaults()
{
	m_octaves = 4;
	m_magnitude = 4;
}

void Terrain::calculateMaxDistance()
{
	// calculate the center vertex indexes
	m_centerX = roundf(m_width / 2);
	m_centerY = roundf(m_height / 2);

	// calculate the largest possible distance so distance calculations can be normalised
	m_maxDistance = sqrtf(pow(fabsf(m_centerX), 2) + pow(fabsf(m_centerY), 2));
}

void Terrain::createTerrain()
{
	generateVertices();
	generateIndices();
	initBuffers();
	loadIntoShader();
}

void Terrain::generateVertices()
{
	// set FastNoise noise properties to current terrain config
	m_noise.SetFractalOctaves(m_octaves);
	m_noise.SetNoiseType(m_noiseType);
	m_noise.SetFrequency(m_noiseFrequency);
	m_noise.SetSeed(m_seed);

	// iterate w/h of the terrain and add vertex data for each position
	for (GLint row = 0; row < m_width; row++)
	{
		// calculate the current spatial offset for the next row based on tilSize
		GLfloat rowOffset = row * m_tileSize;
		for (GLint col = 0; col < m_height; col++)
		{

			// position data
			// calculate m_height data - use FastNoise and then apply m_magnitude / m_exponent modifications
			GLfloat y = m_magnitude * m_noise.GetNoise(col, row);
			y = pow(y, m_exponent);

			// add positional data to vertex array
			m_vertices.push_back((GLfloat)col * m_tileSize);
			m_vertices.push_back(y);
			m_vertices.push_back((GLfloat)rowOffset);

			// polygon lines colour
			addColourForHeight(y);

			// polygon fill colour
			m_vertices.push_back(m_fillR);
			m_vertices.push_back(m_fillG);
			m_vertices.push_back(m_fillB);
		}
	}
}

void Terrain::generateIndices()
{
	// initialise index counter to track current index count for degenerate triangles
	GLuint indexCounter = 0;

	// iterate whilst there are still indexes left to add
	for (size_t index = 0; (index + m_width) < (m_height * m_width); index += 1)
	{
		// add indexes for current position, and the index of the vertex directly below
		m_indices.push_back(index);
		m_indices.push_back(index + m_width);

		// add 1 to index counter for this row
		indexCounter += 1;

		// if we are at the end of the row, we need to add a 'degenerate' triangle (triangle
		// with no area - automatically removed by OpenGL). This prevents triangles being drawn
		// that connect the end of one row with the start of the next.
		if (indexCounter == m_width && (index + m_width) != (m_height * m_width) - 1)
		{
			// add two indexes - one for the last vertex of this row, and one for the
			// first vertex of the next row.
			m_indices.push_back(index + m_width);
			m_indices.push_back(index + 1);

			// reset counter for next row
			indexCounter = 0;
		}
	}
}

void Terrain::initBuffers()
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
}

void Terrain::addColourForHeight(GLfloat &y)
{
	if (y < 0.06)
	{
		m_vertices.push_back(colours[0][0]);
		m_vertices.push_back(colours[0][1]);
		m_vertices.push_back(colours[0][2]);
	}
	else if (y < 1.2)
	{
		m_vertices.push_back(colours[1][0]);
		m_vertices.push_back(colours[1][1]);
		m_vertices.push_back(colours[1][2]);
	}
	else if (y < (m_magnitude - (m_magnitude * 0.1)))
	{
		m_vertices.push_back(colours[2][0]);
		m_vertices.push_back(colours[2][1]);
		m_vertices.push_back(colours[2][2]);
	}
	else
	{
		m_vertices.push_back(colours[3][0]);
		m_vertices.push_back(colours[3][1]);
		m_vertices.push_back(colours[3][2]);
	}
}

void Terrain::loadIntoShader()
{
	//model = glm::mat4(1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

	// load position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void *)0);

	// center terrain
	//model = glm::translate(model, glm::vec3(-((m_width / 2) * m_tileSize), -1.0f, -((m_height / 2) * m_tileSize)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Reset island flag, will be set to true after this function only in the 'makeIsland()' method.
	// Other update methods will not set this flag to true
	m_isIsland = false;
}
