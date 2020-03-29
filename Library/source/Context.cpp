#include "Context.h"

#ifdef NDEBUG
    static bool validationEnabled = false;
#else
    static bool validationEnabled = true;
#endif

static std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

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
    VkPhysicalDevice            Context::physicalDevice;
    VkQueue                     Context::commandQueue;
    VkDevice                    Context::device;

    uint32_t Context::queueFamilyIndex;

    void Context::InitVulkan(GLFWwindow* window)
    {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface(window);
        PickPhysicalDevice();
        CreateDevice();
    }

    void Context::CleanUP()
    {
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

    static int IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
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
                if(properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
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
    }

    void Context::CreateSurface(GLFWwindow* window)
    {
        if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface");
        }
    }

}