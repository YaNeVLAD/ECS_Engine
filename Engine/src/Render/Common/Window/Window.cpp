#include "Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

namespace
{
GLFWwindow* AsGLFWwindow(void* ptr)
{
	return static_cast<GLFWwindow*>(ptr);
}
} // namespace

namespace Engine::render
{

Window::Window(WindowProps props)
	: m_props(std::move(props))
{
	Init();
}

Window::~Window()
{
	Shutdown();
}

void Window::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(AsGLFWwindow(m_window));
}

unsigned Window::Width() const
{
	return m_props.Width;
}

unsigned Window::Height() const
{
	return m_props.Height;
}

void Window::SetTitle(std::string const& title)
{
	glfwSetWindowTitle(AsGLFWwindow(m_window), title.c_str());
}

void* Window::GetNativeWindow() const
{
	return m_window;
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(AsGLFWwindow(m_window));
}

void Window::Init()
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow((int)m_props.Width, (int)m_props.Height, m_props.Title.c_str(), nullptr, nullptr);
	if (m_window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(AsGLFWwindow(m_window));
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glViewport(0, 0, m_props.Width, m_props.Height);
}

void Window::Shutdown()
{
	glfwDestroyWindow(AsGLFWwindow(m_window));
	glfwTerminate();
}

} // namespace Engine::render
