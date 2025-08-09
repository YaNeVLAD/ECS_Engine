#include "Renderer2D.h"

#include <glad/glad.h>

namespace Engine::render
{

struct RendererData
{
	GLuint QuadVAO = 0;
	GLuint QuadVBO = 0;
	GLuint QuadEBO = 0;

	GLuint ShaderProgram = 0;

	GLint ProjLocation;
	GLint ViewLocation;
	GLint ModelLocation;
	GLint ColorLocation;
};

static RendererData s_Data;

void Renderer2D::Init()
{
	const char* vsSource = R"glsl(
			#version 330 core
			layout (location = 0) in vec2 aPos;

			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 projection;

			void main()
			{
				gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
			}
		)glsl";
	const char* fsSource = R"glsl(
			#version 330 core
			out vec4 FragColor;

			uniform vec4 objectColor;

			void main()
			{
				FragColor = objectColor;
			}
		)glsl";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsSource, NULL);
	glCompileShader(vertexShader);
	// TODO: Добавить проверку ошибок компиляции

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsSource, NULL);
	glCompileShader(fragmentShader);
	// TODO: Добавить проверку ошибок компиляции

	s_Data.ShaderProgram = glCreateProgram();
	glAttachShader(s_Data.ShaderProgram, vertexShader);
	glAttachShader(s_Data.ShaderProgram, fragmentShader);
	glLinkProgram(s_Data.ShaderProgram);

	// TODO: Добавить проверку ошибок линковки

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	s_Data.ProjLocation = glGetUniformLocation(s_Data.ShaderProgram, "projection");
	s_Data.ViewLocation = glGetUniformLocation(s_Data.ShaderProgram, "view");
	s_Data.ModelLocation = glGetUniformLocation(s_Data.ShaderProgram, "model");
	s_Data.ColorLocation = glGetUniformLocation(s_Data.ShaderProgram, "objectColor");

	float vertices[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &s_Data.QuadVAO);
	glGenBuffers(1, &s_Data.QuadVBO);
	glGenBuffers(1, &s_Data.QuadEBO);

	glBindVertexArray(s_Data.QuadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, s_Data.QuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.QuadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer2D::Shutdown()
{
	glDeleteVertexArrays(1, &s_Data.QuadVAO);
	glDeleteBuffers(1, &s_Data.QuadVBO);
	glDeleteBuffers(1, &s_Data.QuadEBO);
	glDeleteProgram(s_Data.ShaderProgram);
}

void Renderer2D::BeginScene(const glm::mat4& projection, const glm::mat4& view)
{
	glUseProgram(s_Data.ShaderProgram);
	glUniformMatrix4fv(s_Data.ProjLocation, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(s_Data.ViewLocation, 1, GL_FALSE, &view[0][0]);
}

void Renderer2D::EndScene()
{
	// TODO: batch flush
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(size, 1.0f));

	glUniformMatrix4fv(s_Data.ModelLocation, 1, GL_FALSE, &model[0][0]);
	glUniform4fv(s_Data.ColorLocation, 1, &color[0]);

	glBindVertexArray(s_Data.QuadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

} // namespace Engine::render
