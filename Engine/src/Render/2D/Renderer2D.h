#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Components.h"

namespace Engine::render
{

class Renderer2D
{
public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const glm::mat4& projection, const glm::mat4& view);
	static void EndScene();

	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
};

} // namespace Engine::render
