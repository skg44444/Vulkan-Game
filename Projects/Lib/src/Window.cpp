#include "libpch.h"

#include "Window.h"

namespace Lib
{
	Window::Window()
	{
		InitWindow();
	}

	Window::~Window()
	{
		Cleanup();
	}

	void Window::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_Window = glfwCreateWindow(800, 600, "Vulkun Window", nullptr, nullptr);

		glfwSetWindowUserPointer(m_Window, &m_Data);

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				data.IsOpen = false;
			});
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
	}

	void Window::Cleanup()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
}