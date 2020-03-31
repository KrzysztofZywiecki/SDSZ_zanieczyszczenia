#include "Context.h"

#ifdef NDEBUG
    static bool validationEnabled = false;
#else
    static bool validationEnabled = true;
#endif

static std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
static std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};


static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
VkAllocationCallbacks* pAllocator,
VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(func == nullptr)
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
    else
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
VkDebugUtilsMessengerEXT debugMessenger,
VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    func(instance, debugMessenger, pAllocator);
}



static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr<<"Validation Layer: "<<pCallbackData->pMessage<<std::endl;
    return VK_FALSE;
}

namespace Library
{
    VkInstance                  Context::instance;
    VkDebugUtilsMessengerEXT    Context::debugMessenger;
    VkSurfaceKHR                Context::surface;
    SwapChainSupportDetails     Context::capabilities;
    VkPhysicalDevice            Context::physicalDevice;
    VkQueue                     Context::commandQueue;
    VkDevice                    Context::device;
    VkSwapchainKHR              Context::swapChain;
    std::vector<VkImage>        Context::swapChainImages;
    std::vector<VkImageView>    Context::swapChainImageViews;

    uint32_t                    Context::queueFamilyIndex;
    VkFormat                    Context::swapChainImageFormat;
    VkExtent2D                  Context::windowExtent;
    VkPipeline                  Context::graphicsPipeline;
    VkPipelineLayout            Context::graphicsPipelineLayout;
    VkRenderPass                Context::renderPass;

    void Context::InitVulkan(Window* window)
    {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface(window->getWindowPtr());
        PickPhysicalDevice();
        CreateDevice();
        CreateSwapChain(window);
        CreateSwapChainImageViews();
        CreatePipelineLayout();
        CreateRenderPass();
        CreateGraphicsPipeline();
    }

    void Context::CleanUP()
    {
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, graphicsPipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);
        for(auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void Context::CreateInstance()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pEngineName = "Najlepszy silnik";
        appInfo.pApplicationName = "Najlepsza aplikacja";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_MAKE_VERSION(1, 1, 0);

        std::vector<const char*> extensions = GetRequiredExtensions();

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledExtensionCount = extensions.size();

        if(validationEnabled)
        {
            if(!CheckValidationSupport())
            {
                throw std::runtime_error("Layers requested, but not found");
            }
            createInfo.ppEnabledLayerNames = layers.data();
            createInfo.enabledLayerCount = layers.size();
        }
        else
        {
            createInfo.ppEnabledLayerNames = nullptr;
            createInfo.enabledLayerCount = 0;
        }
        
        if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Instance!");
        }
    }

    std::vector<const char*> Context::GetRequiredExtensions()
    {
        std::vector<const char*> names;
        uint32_t count;
        const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&count);
        names.insert(names.end(), glfwExtensionNames, glfwExtensionNames + count);

        if(validationEnabled)
        {
            names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return names;
    }

    bool Context::CheckValidationSupport()
    {
        std::vector<VkLayerProperties> properties = {};
        uint32_t propertyCount;
        vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
        properties.resize(propertyCount);
        vkEnumerateInstanceLayerProperties(&propertyCount, properties.data());

        for(auto layer : layers)
        {
            bool layerFound = false;
            for(auto property : properties)
            {
                if(strcmp(layer, property.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if(!layerFound)
            {
                return false;
            }
        }
        return true;
    }

    void Context::SetupDebugMessenger()
    {
        if(!validationEnabled)
        {
            return;
        }

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        createInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)DebugCallback;
        createInfo.pUserData = nullptr;
        CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);
    }

    void Context::CreateSurface(GLFWwindow* window)
    {
        if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    static bool checkExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t count;
        std::vector<VkExtensionProperties> properties;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
        properties.resize(count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &count, properties.data());

		for (auto name : deviceExtensions)
		{
			bool extensionFound = false;
			for (auto property : properties)
			{
				if (strcmp(property.extensionName, name) == 0)
				{
					extensionFound = true;
					break;
				}
			}
			if (!extensionFound)
			{
				return false;
			}
		}
		return true;
    }

    static int IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        if(!checkExtensionSupport(device))
        {
            return -1;
        }
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> properties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, properties.data());

        for(int i = 0; i < properties.size(); i++)
        {
            VkBool32 presentSupport;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if((properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && presentSupport)
            {
                return i;
            }
        }
        return -1;
    }

    void Context::PickPhysicalDevice()
    {
        uint32_t count;
        std::vector<VkPhysicalDevice> physicalDevices;
        vkEnumeratePhysicalDevices(instance, &count, nullptr);
        physicalDevices.resize(count);
        vkEnumeratePhysicalDevices(instance, &count, physicalDevices.data());

        for(auto iterator : physicalDevices)
        {
            int result = IsDeviceSuitable(iterator, surface);
            if(result != -1)
            {
                VkPhysicalDeviceProperties properties = {};
                vkGetPhysicalDeviceProperties(iterator, &properties);
                if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    physicalDevice = iterator;
                    queueFamilyIndex = static_cast<uint32_t>(result);
                    std::cout << properties.deviceName << std::endl;
                    return;
                }
            }
        }
        throw std::runtime_error("Failed to find suitable physical device!");
    }

    static SwapChainSupportDetails querySwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
        details.presentModes.resize(count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, details.presentModes.data());

        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
        details.formats.resize(count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, details.formats.data());
        
        return details;
    }

    static VkSurfaceFormatKHR chooseFormat(std::vector<VkSurfaceFormatKHR> formats)
    {
        for(auto format : formats)
        {
            if(format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && 
                format.format == VK_FORMAT_R8G8B8A8_SRGB)
                {
                    return format;
                }
        }
        return formats[0];
    }

    static VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR> presentModes)
    {
        for(auto presentMode : presentModes)
        {
            if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return presentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    static VkExtent2D chooseSwapChainExtent(VkSurfaceCapabilitiesKHR capabilities, Window* window)
    {
        if(capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D newExtent = {window->getWidth(), window->getHeight()};
            
            newExtent.width = std::min(capabilities.maxImageExtent.width, 
                std::max(capabilities.minImageExtent.width, newExtent.width));
        
            newExtent.height = std::min(capabilities.maxImageExtent.height, 
                std::max(capabilities.minImageExtent.width, newExtent.height));
        }
        
    }

    void Context::CreateDevice()
    {
        float priority = 1.0f;
        VkDeviceQueueCreateInfo queueInfo = {};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = queueFamilyIndex;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &priority;

        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        if(validationEnabled)
        {
            createInfo.enabledLayerCount = layers.size();
            createInfo.ppEnabledLayerNames = layers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
        }

        if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device!");
        }
        vkGetDeviceQueue(device, queueFamilyIndex, 0, &commandQueue);
        capabilities = querySwapChainSupportDetails(physicalDevice, surface);
    }

    void Context::CreateSwapChain(Window* window)
    {
        VkSurfaceFormatKHR format = chooseFormat(capabilities.formats);
        VkExtent2D extent = chooseSwapChainExtent(capabilities.capabilities, window);
        
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.minImageCount = std::min(capabilities.capabilities.minImageCount + 1, capabilities.capabilities.maxImageCount);
        createInfo.imageFormat = format.format;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.imageArrayLayers = 1;
        createInfo.imageExtent = extent;
        createInfo.clipped = VK_TRUE;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        createInfo.presentMode = choosePresentMode(capabilities.presentModes);
        createInfo.preTransform = capabilities.capabilities.currentTransform;

        vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
        swapChainImageFormat = format.format;
        windowExtent = extent;

        uint32_t count;
        vkGetSwapchainImagesKHR(device, swapChain, &count, nullptr);
        swapChainImages.resize(count);
        vkGetSwapchainImagesKHR(device, swapChain, &count, swapChainImages.data());
    }

    void Context::CreateSwapChainImageViews()
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.format = swapChainImageFormat;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.levelCount = 1;

        swapChainImageViews.resize(swapChainImages.size());
        for(int i = 0; i < swapChainImages.size(); i++)
        {
            createInfo.image = swapChainImages[i];
            if(vkCreateImageView(device, &createInfo, nullptr, swapChainImageViews.data() + i) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image view");
            }
        }
    }

    static std::vector<char> readFile(const char* path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.good())
		{
			std::string messege = "Failed to open file ";
			messege.append(path);
			throw std::runtime_error(messege);
		}
        size_t filesize = file.tellg();
        std::vector<char> fileContent(filesize);
        file.seekg(0);
        file.read(fileContent.data(), filesize);
        return fileContent;
    }

    VkShaderModule Context::CreateShaderModule(const char* path)
    {
        std::vector<char> shaderCode = readFile(path);
        VkShaderModule shaderModule;
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pCode = reinterpret_cast<uint32_t*>(shaderCode.data());
        createInfo.codeSize = shaderCode.size();
        
        if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module");
        }
        return shaderModule;
    }

    void Context::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.pushConstantRangeCount = 0;
        createInfo.pPushConstantRanges = nullptr;
        createInfo.setLayoutCount = 0;
        createInfo.pSetLayouts = nullptr;
        if(vkCreatePipelineLayout(device, &createInfo, nullptr, &graphicsPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline layout");
        }
    }

    void Context::CreateRenderPass()
    {
        VkAttachmentReference colorAttachment = {};
        colorAttachment.attachment = 0;
        colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription description = {};
        description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        description.format = swapChainImageFormat;
        description.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        description.samples = VK_SAMPLE_COUNT_1_BIT;
        description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        VkSubpassDescription subpass = {};
        subpass.colorAttachmentCount = 1;
        subpass.inputAttachmentCount = 0;
        subpass.preserveAttachmentCount = 0;
        subpass.pColorAttachments = &colorAttachment;
        subpass.pDepthStencilAttachment = nullptr;
        subpass.pPreserveAttachments = nullptr;
        subpass.pResolveAttachments = nullptr;
        subpass.pInputAttachments = nullptr;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkRenderPassCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.subpassCount = 1;
        createInfo.attachmentCount = 1;
        createInfo.dependencyCount = 0;
        createInfo.pAttachments = &description;
        createInfo.pSubpasses = &subpass;

        if(vkCreateRenderPass(device, &createInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass");
        }
    }

    void Context::CreateGraphicsPipeline()
    {
        VkShaderModule vertexShader = CreateShaderModule("Shaders/vert.spv");
        VkShaderModule fragmentShader = CreateShaderModule("Shaders/frag.spv");
        
        VkPipelineShaderStageCreateInfo vertexStage = {};
        VkPipelineShaderStageCreateInfo fragmentStage = {};

        vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexStage.pName = "main";
        vertexStage.module = vertexShader;
        
        fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentStage.pName = "main";
        fragmentStage.module = fragmentShader;

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertexStage, fragmentStage};

        VkPipelineColorBlendAttachmentState attachmentState = {};
        attachmentState.blendEnable = VK_FALSE;
        attachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlend = {};
        colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlend.attachmentCount = 1;
        colorBlend.pAttachments = &attachmentState;
        colorBlend.logicOpEnable = VK_FALSE;
        colorBlend.logicOp = VK_LOGIC_OP_COPY;
        colorBlend.blendConstants[0] = 0.0f;
        colorBlend.blendConstants[1] = 0.0f;
        colorBlend.blendConstants[2] = 0.0f;
        colorBlend.blendConstants[3] = 0.0f;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.pSampleMask = nullptr;
        multisampling.minSampleShading = 1.0f;
        multisampling.alphaToOneEnable = VK_FALSE;
        multisampling.alphaToCoverageEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterization = {};
        rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.rasterizerDiscardEnable = VK_FALSE;
        rasterization.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization.lineWidth = 1.0f;
        rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization.depthClampEnable = VK_FALSE;
        rasterization.depthBiasEnable = VK_FALSE;
        rasterization.cullMode = VK_CULL_MODE_NONE;
        rasterization.depthBiasConstantFactor = 0.0f;
        rasterization.depthBiasClamp = 0.0f;
        rasterization.depthBiasSlopeFactor = 1.0f;

        VkPipelineVertexInputStateCreateInfo vertexInput = {};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.vertexAttributeDescriptionCount = 0;
        vertexInput.vertexBindingDescriptionCount = 0;
        vertexInput.pVertexAttributeDescriptions = nullptr;
        vertexInput.pVertexBindingDescriptions = nullptr;

        VkViewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = windowExtent.width;
        viewport.height = windowExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = windowExtent;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.scissorCount = 1;
        viewportState.viewportCount = 1;
        viewportState.pScissors = &scissor;
        viewportState.pViewports = &viewport;

        VkGraphicsPipelineCreateInfo createInfo = {};
        createInfo.basePipelineHandle = graphicsPipeline;
        createInfo.basePipelineIndex = -1;
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = 2;
        createInfo.pStages = shaderStages;
        createInfo.layout = graphicsPipelineLayout;
        createInfo.renderPass = renderPass;
        createInfo.subpass = 0;
        createInfo.pColorBlendState = &colorBlend;
        createInfo.pDepthStencilState = nullptr;
        createInfo.pDynamicState = nullptr;
        createInfo.pInputAssemblyState = &inputAssembly;
        createInfo.pMultisampleState = &multisampling;
        createInfo.pRasterizationState = &rasterization;        
        createInfo.pTessellationState = nullptr;
        createInfo.pVertexInputState = &vertexInput;
        createInfo.pViewportState = &viewportState;

        if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline, debilu");
        }
        vkDestroyShaderModule(device, vertexShader, nullptr);
        vkDestroyShaderModule(device, fragmentShader, nullptr);
    }

}