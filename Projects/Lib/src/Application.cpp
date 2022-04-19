#include "libpch.h"

#include "Application.h"

namespace Lib
{
	static VkInstance instance;
	static VkDebugUtilsMessengerEXT debugMessenger;

	Application::Application()
		:m_Window(std::make_shared<Window>())
	{

		m_GraphicsContext = std::make_shared<GraphicsContext>(m_Window->GetWindowHandle());
		m_Shader = std::make_shared<Shader>("Resources/Shaders/BasicShader.glsl", m_GraphicsContext->GetLogicalDevice());
		m_PipelineLayout = std::make_shared<PipelineLayout>(m_GraphicsContext->GetLogicalDevice(), m_GraphicsContext->GetSwapChainExtent());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		MainLoop();
	}

	void Application::MainLoop()
	{
		while (m_Window->IsOpen()) {
			m_Window->OnUpdate();
		}
	}
}