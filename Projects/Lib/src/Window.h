#pragma once

#include <GraphicsContext.h>

struct GLFWwindow;

namespace Lib
{
	class Window
	{
	public:
		Window();
		~Window();

		bool IsOpen() { return m_Data.IsOpen; }
		uint32_t GetWidth() { return m_Data.width; }
		uint32_t GetHeight() { return m_Data.height; }
		GLFWwindow* GetWindowHandle() { return m_Window; }
		void OnUpdate();
	private:
		void InitWindow();
		void Cleanup();

	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			uint32_t width=800, height=600;
			bool IsOpen=true;
		};

		WindowData m_Data;
	};
}

