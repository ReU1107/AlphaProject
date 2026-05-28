#include "VulkanDevice.h"
#include <vector>
#include <string>
#include <sstream>
#include <Windows.h>
#if RHI_VK

namespace Alpha
{
	VulkanDevice::VulkanDevice()
	{
		CreateInstance();
		CreateDevice();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(mDevice, nullptr);
		
		vkDestroyInstance(mInstance, nullptr);
	}

	PFN_vkCreateDebugReportCallbackEXT	g_fCreateDebugReportCallback = VK_NULL_HANDLE;
	PFN_vkDestroyDebugReportCallbackEXT	g_fDestroyDebugReportCallback = VK_NULL_HANDLE;
	PFN_vkDebugReportMessageEXT			g_fDebugBreakCallback = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT			g_fMsgCallback;

	VkBool32 DebugMessageCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t srcObject,
		size_t location,
		int32_t msgCode,
		const char* pLayerPrefix,
		const char* pMsg,
		void* pUserData)
	{
		String message;
		{
			Stringstream buf;
			if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
				buf << "ERROR: ";
			}
			else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
				buf << "WARNING: ";
			}
			else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
				buf << "PERF: ";
			}
			else {
				return false;
			}
			buf << "[" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg;
			message = buf.str();
		}
		MessageBoxA(nullptr, message.c_str(), "ErrorMessage", MB_OK);
		

		return false;
	}

	uint32_t VulkanDevice::GetMemoryTypeIndex(uint32_t requirements, const VkMemoryPropertyFlags flag) const
	{
		uint32_t count = mProperties.memoryTypeCount;
		for (uint32_t i = 0; i < count; i++)
		{
			if ((requirements & 1) == 1)
			{
				if ((mProperties.memoryTypes[i].propertyFlags & flag) == flag)
				{
					return i;
				}
			}
			requirements >>= 1;
		}
		return 0;
	}

	void VulkanDevice::CreateInstance()
	{
		VkApplicationInfo application_info = {};
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.pNext = nullptr;
		//アプリケーションの名前
		application_info.pApplicationName = "TestVulkan";
		//アプリケーションのバージョン
		application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		//エンジンの名前
		application_info.pEngineName = "engine";
		//エンジンのバージョン;
		application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		//使用するVulkanのバージョンをVulkan 1.3にする
		application_info.apiVersion = VK_API_VERSION_1_3;
		//バリデーションレイヤーを使う
		std::vector< const char* > layers
		{
			"VK_LAYER_KHRONOS_validation"
		};

		std::vector<const char* > extensions
		{
			"VK_KHR_surface",
			"VK_KHR_win32_surface", 
			"VK_EXT_debug_report" 
		};

		VkInstanceCreateInfo instance_info = {};
		instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_info.pNext = nullptr;
		instance_info.flags = 0;
		// アプリケーションの情報を指定
		instance_info.pApplicationInfo = &application_info;
		// 使用するレイヤーを指定
		instance_info.enabledLayerCount = (uint32_t)layers.size();
		instance_info.ppEnabledLayerNames = layers.data();
		// 拡張
		instance_info.enabledExtensionCount = (uint32_t)extensions.size();
		instance_info.ppEnabledExtensionNames = extensions.data();
		VkInstance instance;
		// インスタンスを作成
		
		auto result = vkCreateInstance(&instance_info,
			nullptr,
			&instance
		);
		if (result != VK_SUCCESS)
		{

		}
		mInstance = instance;
	}
	
	void VulkanDevice::CreateDevice()
	{
		auto instance = mInstance;
		// 物理デバイス(GPUの取得)
		// デバイスの個数を取得
		uint32_t device_count = 0;
		if (vkEnumeratePhysicalDevices(instance, &device_count, nullptr) != VK_SUCCESS)
		{
		}
		// デバイスの情報を取得
		std::vector< VkPhysicalDevice > devices(device_count);

		if (vkEnumeratePhysicalDevices(instance, &device_count, devices.data()) != VK_SUCCESS)
		{

		}
		VkPhysicalDevice physical_device = devices[0];
		uint32_t queue_props_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_props_count, nullptr);
		std::vector< VkQueueFamilyProperties > queue_props(queue_props_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_props_count, queue_props.data());

		uint32_t queue_family_index = 0;
		// グラフィックス要求を受け付けるキューを探す
		for (uint32_t i = 0; i < queue_props.size(); ++i)
		{
			if (queue_props[i].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
			{
				queue_family_index = i;
			}
		}

		mGraphicsQueueFamilyIndex = queue_family_index;

		const float priority = 0.0f;
		// デバイスキュー
		VkDeviceQueueCreateInfo queue_info = {};
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.pNext = nullptr;
		queue_info.flags = 0;
		queue_info.queueFamilyIndex = queue_family_index;
		queue_info.queueCount = 1;
		queue_info.pQueuePriorities = &priority;

		
		std::vector< const char* > extension{
			"VK_KHR_swapchain",	// Swapchainを作成するのに必要
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		};
		VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering = {};
		dynamic_rendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
		dynamic_rendering.dynamicRendering = true;

		void* extension_next = &dynamic_rendering;


		// 論理デバイスを作る
		VkDeviceCreateInfo device_info = {};
		device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_info.pNext = extension_next;
		device_info.flags = 0;
		device_info.queueCreateInfoCount = 1;
		device_info.pQueueCreateInfos = &queue_info;
		device_info.enabledLayerCount = 0;
		device_info.ppEnabledLayerNames = nullptr;
		device_info.enabledExtensionCount = (uint32_t)extension.size();
		device_info.ppEnabledExtensionNames = extension.data();
		device_info.pEnabledFeatures = nullptr;
		VkDevice device = VK_NULL_HANDLE;
		auto result = vkCreateDevice(
			physical_device,
			&device_info,
			nullptr,
			&device
		);
		if (result != VK_SUCCESS)
		{

		}

		mPhysicalDevice = physical_device;
		mDevice = device;

		g_fCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		g_fDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		g_fDebugBreakCallback = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT");

		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
		dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)DebugMessageCallback;
		dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;

		VkResult err = g_fCreateDebugReportCallback(instance, &dbgCreateInfo, nullptr, &g_fMsgCallback);

		VkPhysicalDeviceMemoryProperties properties = {};
		vkGetPhysicalDeviceMemoryProperties(physical_device, &properties);

		mProperties = properties;
	}
}

#endif