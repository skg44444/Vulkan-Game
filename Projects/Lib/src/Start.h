#pragma once

#include "Application.h"

#include <stdexcept>
#include <cstdlib>

extern Lib::Application* Lib::CreateApplication();

int main()
{
	auto app = Lib::CreateApplication();
	try
	{
		app->Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	delete app;
	std::cin.get();
	return EXIT_SUCCESS;
}
