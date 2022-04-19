#pragma once

#include <iostream>
#include <vector>
#include <optional>

struct GLFWwindow;
struct VkSurfaceFormatKHR;
struct VkSurfaceCapabilitiesKHR;
struct VkExtent2D;

enum VkPresentModeKHR;
enum VkFormat;

typedef struct VkInstance_T* VkInstance;
typedef struct VkDebugUtilsMessengerEXT_T* VkDebugUtilsMessengerEXT;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;
typedef struct VkQueue_T* VkQueue;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VkRenderPass_T* VkRenderPass;

namespace Lib
{
	struct SwapChainSupportDetails;

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentationFamily;

		bool IsComplete()
		{
			return graphicsFamily.has_value() && presentationFamily.has_value();
		}
	};

	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* window);
		~GraphicsContext();

		VkDevice GetLogicalDevice() { return m_LogicalDevice; }

		std::shared_ptr<VkExtent2D> GetSwapChainExtent();
		uint32_t GetSwapChainExtentWidth();
		uint32_t GetSwapChainExtentHeight();
	private:
		void InitVulkun();
		void InitGraphicsContext();

		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		void SetupDebugMessenger();

		void PickPhysicalDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		void CreateLogicalDevice();

		void CreateSurface();

		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void CreateSwapChain();

		void CreateImageViews();

		void CreateRenderPass();
	private:
		GLFWwindow* m_Window;

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		VkSurfaceKHR m_Surface;

		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkDevice m_LogicalDevice;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentationQueue;

		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		std::shared_ptr<VkExtent2D> m_SwapChainExtent;

		std::vector<VkImageView> m_SwapChainImageViews;

		VkRenderPass m_RenderPass;
	};
}