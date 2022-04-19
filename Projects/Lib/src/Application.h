#pragma once

#include "Window.h"
#include "Shader.h"
#include "PipelineLayout.h"

struct GLFWwindow;

namespace Lib
{
	class Application
	{
	public:
		Application();
		~Application();

		void Run();

	private:
		void MainLoop();

	private:
		std::shared_ptr<Window> m_Window;
		std::shared_ptr<GraphicsContext> m_GraphicsContext;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<PipelineLayout> m_PipelineLayout;
	};

	Application* CreateApplication();
}