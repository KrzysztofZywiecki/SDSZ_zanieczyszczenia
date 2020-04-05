#include "BasicInfo.h"

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

    static std::vector<const char*> GetRequiredExtensions()
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

    static bool CheckValidationSupport()
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

    VkInstance Instance::GetInstance()
    {
        return instance;
    }

    VkSurfaceKHR Instance::GetSurface()
    {
        return surface;
    }

    Instance::Instance() : instance(VK_NULL_HANDLE), surface(VK_NULL_HANDLE)
    {
    }

    void Instance::Destroy()
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void Instance::Create(Window& window)
    {
        CreateInstance();
        if(validationEnabled)
        {
            CreateDebugCallback();
        }
        CreateSurface(window);
    }

    void Instance::CreateInstance()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_MAKE_VERSION(1,1,0);
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pApplicationName = "Najlepsza aplikacja";
        appInfo.pEngineName = "Najlepszy silnik";

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        std::vector<const char*> extensions = GetRequiredExtensions();
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledExtensionCount = extensions.size();
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
        
        if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create instance");
        }
    }

    void Instance::CreateDebugCallback()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.pUserData = nullptr;
        createInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)DebugCallback;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

        if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Debug Messenger");
        }
    }

    void Instance::CreateSurface(Window& window)
    {
        glfwCreateWindowSurface(instance, window.getWindowPtr(), nullptr, &surface);
    }

}