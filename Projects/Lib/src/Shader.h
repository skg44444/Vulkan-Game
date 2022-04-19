#pragma once

#include <unordered_map>

typedef struct VkShaderModule_T* VkShaderModule;
typedef struct VkDevice_T* VkDevice;

struct VkExtent2D;
struct VkPipelineShaderStageCreateInfo;

namespace Lib
{
	enum ShaderType
	{
		None = 0, VertexShader, FragmentShader
	};

	class Shader
	{
	public:
		Shader(const std::string& filepath, VkDevice device);
		~Shader();
	private:
		void CreateShaderByteCode(const std::string& filepath);

		VkShaderModule CreateShaderModule(const std::vector<uint32_t>& code);
		void CreateShaderStage();

		std::string ReadFile(const std::string& filepath);
		std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source);
	private:
		VkDevice m_Device;

		std::unordered_map<ShaderType, std::vector<uint32_t>> m_VulkanSPIRV;
		std::unordered_map<ShaderType, VkShaderModule> m_ShaderModule;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
	};
}

