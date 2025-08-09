#pragma once

#include <functional>
#include <string>

namespace Engine::render
{

struct WindowProps
{
	std::string Title;
	unsigned Width;
	unsigned Height;
};

class Window final
{
public:
	Window(WindowProps props);

	~Window();

	void OnUpdate();

	unsigned Width() const;

	unsigned Height() const;

	void SetTitle(std::string const& title);

	void* GetNativeWindow() const;

	bool ShouldClose() const;

private:
	void Init();

	void Shutdown();

private:
	void* m_window;

	WindowProps m_props;
};

} // namespace Engine::render
