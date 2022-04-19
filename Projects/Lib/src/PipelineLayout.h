#pragma once

typedef struct VkPipelineLayout_T* VkPipelineLayout;

namespace Lib
{
	class PipelineLayout
	{
	public:
		PipelineLayout(VkDevice device, std::shared_ptr<VkExtent2D> extent);
		~PipelineLayout();
	private:
		VkDevice m_Device;
		std::shared_ptr<VkExtent2D> m_SwapChainExtent;

		VkPipelineLayout m_PipelineLayout;
	};
}
