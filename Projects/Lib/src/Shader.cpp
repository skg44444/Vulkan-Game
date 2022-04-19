#include "libpch.h"
#include "Shader.h"

#include <shaderc/shaderc.hpp>

namespace Lib
{
	static ShaderType ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return ShaderType::VertexShader;
		if (type == "fragment" || type == "pixel")
			return ShaderType::FragmentShader;

		
		return ShaderType::None;
	}

	static shaderc_shader_kind GLShaderStageToShaderC(ShaderType stage)
	{
		switch (stage)
		{
		case ShaderType::VertexShader:   return shaderc_glsl_vertex_shader;
		case ShaderType::FragmentShader: return shaderc_glsl_fragment_shader;
		}

		return (shaderc_shader_kind)0;
	}

	static VkShaderStageFlagBits ShaderStageToFlagBits(ShaderType stage)
	{
		switch (stage)
		{
		case ShaderType::VertexShader:   return VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderType::FragmentShader: return VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		return (VkShaderStageFlagBits)0;
	}

	static const char* GetCacheDirectory()
	{
		return "Resources/cache/shader/vulkan";
	}

	static void CreateCacheDirectoryIfNeeded()
	{
		std::string cacheDirectory = GetCacheDirectory();
		if (!std::filesystem::exists(cacheDirectory))
		{
			std::filesystem::create_directories(cacheDirectory);
		}
	}

	static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
	{
		switch (stage)
		{
		case ShaderType::VertexShader:    return ".cached_vulkan.vert";
		case ShaderType::FragmentShader:  return ".cached_vulkan.frag";
		}
		return "";
	}

	Shader::Shader(const std::string& filepath, VkDevice device)
		:m_Device(device)
	{
		CreateCacheDirectoryIfNeeded();
		CreateShaderByteCode(filepath);

		for (auto&& [stage, code] : m_VulkanSPIRV)
		{
			m_ShaderModule[stage] = CreateShaderModule(code);
		}
	}

	Shader::~Shader()
	{
		for (auto&& [stage, shaderModule] : m_ShaderModule)
		{
			vkDestroyShaderModule(m_Device, shaderModule, nullptr);
		}
	}

	void Shader::CreateShaderByteCode(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = GetCacheDirectory();
		std::filesystem::path shaderFilePath = filepath;

		auto& shaderData = m_VulkanSPIRV;

		for (auto&& [stage, source] : shaderSources) {

			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), filepath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					throw std::runtime_error(module.GetErrorMessage());
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	VkShaderModule Shader::CreateShaderModule(const std::vector<uint32_t>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size()* sizeof(uint32_t);
		createInfo.pCode = code.data();

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module!");
		}
		return shaderModule;
	}

	void Shader::CreateShaderStage()
	{
		for (auto&& [stage, shaderModule] : m_ShaderModule)
		{
			VkPipelineShaderStageCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			createInfo.stage = ShaderStageToFlagBits(stage);
			createInfo.module = shaderModule;
			createInfo.pName = "main";

			m_ShaderStages.push_back(createInfo);
		}
	}

	std::string Shader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				throw std::runtime_error("Cannot Read from shader file!");
			}
		}
		else
		{
			throw std::runtime_error("Failed to open shader file");
		}

		return result;
	}

	std::unordered_map<ShaderType, std::string> Shader::PreProcess(const std::string& source)
	{
		std::unordered_map<ShaderType, std::string> shaderSources;

		const char* typeToken = "#shader";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#shader " keyword)
			std::string type = source.substr(begin, eol - begin);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}
		return shaderSources;
	}



}