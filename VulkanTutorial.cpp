#include "VulkanUtils.h"
#include "EasyImage.h"
#include "DepthImage.h"
#include "Vertex.h"
#include "Mesh.h"

#include <chrono>

VkInstance instance;
std::vector<VkPhysicalDevice> physicalDevices;
uint32_t amountOfQueueFamilies;
VkSurfaceKHR surface;
VkDevice device;
VkSwapchainKHR swapchain = VK_NULL_HANDLE;
VkImageView *imageViews;
VkFramebuffer *framebuffers;
VkShaderModule shaderModuleVert;
VkShaderModule shaderModuleFrag;
VkPipelineLayout pipelineLayout;
VkRenderPass renderPass;
VkPipeline pipeline;
VkCommandPool commandPool;
VkCommandBuffer *commandBuffers;
VkSemaphore semaphoreImageAvailable;
VkSemaphore semaphoreRenderingDone;
VkQueue queue;
VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferDeviceMemory;
VkBuffer indexBuffer;
VkDeviceMemory indexBufferDeviceMemory;
VkBuffer uniformBuffer;
VkDeviceMemory uniformBufferMemory;
uint32_t amountOfImagesInSwapchain = 0;
GLFWwindow *window;

uint32_t width = 400;
uint32_t height = 300;
const VkFormat ourFormat = VK_FORMAT_B8G8R8A8_UNORM; // TODO: civ

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 lightPosition;
};

UniformBufferObject ubo;

VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorPool descriptorPool;
VkDescriptorSet descriptorSet;

EasyImage pika;
DepthImage depthImage;
Mesh dragon;


std::vector<Vertex> vertices = {};

std::vector<uint32_t> indices = {};

void printStats(VkPhysicalDevice &device) {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);

	std::cout << "Name:                          " << properties.deviceName << std::endl;
	uint32_t apiVersion = properties.apiVersion;
	std::cout << "API Version:                   " << VK_VERSION_MAJOR(apiVersion) << "." << VK_VERSION_MINOR(apiVersion) << "." << VK_VERSION_PATCH(apiVersion) << std::endl;
	uint32_t driverVersion = properties.driverVersion;
	std::cout << "Driver Version:                " << VK_VERSION_MAJOR(driverVersion) << "." << VK_VERSION_MINOR(driverVersion) << "." << VK_VERSION_PATCH(driverVersion) << std::endl;
	std::cout << "Vendor ID:                     " << properties.vendorID << std::endl;
	std::cout << "Device ID:                     " << properties.deviceID << std::endl;
	std::cout << "Device Type:                   " << properties.deviceType << std::endl;
	std::cout << "Discrete Queue Priorities:     " << properties.limits.discreteQueuePriorities << std::endl;

	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);
	std::cout << "Geometry Shader:               " << features.geometryShader << std::endl;

	VkPhysicalDeviceMemoryProperties memProp;
	vkGetPhysicalDeviceMemoryProperties(device, &memProp);

	amountOfQueueFamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, NULL);
	VkQueueFamilyProperties *familyProperties = new VkQueueFamilyProperties[amountOfQueueFamilies];
	vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, familyProperties);

	std::cout << "Amount of Queue Families:      " << amountOfQueueFamilies << std::endl;

	for (int i = 0; i < amountOfQueueFamilies; i++) {
		std::cout << std::endl;
		std::cout << "Queue Family #" << i << std::endl;
		std::cout << "VK_QUEUE_GRAPHICS_BIT       " << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
		std::cout << "VK_QUEUE_COMPUTE_BIT        " << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
		std::cout << "VK_QUEUE_TRANSFER_BIT       " << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
		std::cout << "VK_QUEUE_SPARSE_BINDING_BIT " << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
		std::cout << "Queue Count: " << familyProperties[i].queueCount << std::endl;
		std::cout << "Timestamp Valid Bits: " << familyProperties[i].timestampValidBits << std::endl;
		uint32_t width = familyProperties[i].minImageTransferGranularity.width;
		uint32_t height = familyProperties[i].minImageTransferGranularity.height;
		uint32_t depth = familyProperties[i].minImageTransferGranularity.depth;
		std::cout << "Min Image Timestamp Granularity: " << width << ", " << height << ", " << depth << std::endl;
		
	}

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);
	std::cout << "Surface Capabilites:       " << std::endl;
	std::cout << "\tminImageCount:           " << surfaceCapabilities.minImageCount << std::endl;
	std::cout << "\tmaxImageCount:           " << surfaceCapabilities.maxImageCount << std::endl;
	std::cout << "\tcurrentExtent:           " << surfaceCapabilities.currentExtent.width << "/" << surfaceCapabilities.currentExtent.height << std::endl;
	std::cout << "\tminImageExtent:          " << surfaceCapabilities.minImageExtent.width << "/" << surfaceCapabilities.minImageExtent.height << std::endl;
	std::cout << "\tmaxImageExtent:          " << surfaceCapabilities.maxImageExtent.width << "/" << surfaceCapabilities.maxImageExtent.height << std::endl;
	std::cout << "\tmaxImageArrayLayers:     " << surfaceCapabilities.maxImageArrayLayers << std::endl;
	std::cout << "\tsupportedTransforms:     " << surfaceCapabilities.supportedTransforms << std::endl;
	std::cout << "\tcurrentTransform:        " << surfaceCapabilities.currentTransform << std::endl;
	std::cout << "\tsupportedCompositeAlpha: " << surfaceCapabilities.supportedCompositeAlpha << std::endl;
	std::cout << "\tsupportedUsageFlags:     " << surfaceCapabilities.supportedUsageFlags << std::endl;

	uint32_t amountOfFormats = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, nullptr);
	VkSurfaceFormatKHR *surfaceFormats = new VkSurfaceFormatKHR[amountOfFormats];
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, surfaceFormats);

	std::cout << std::endl;
	std::cout << "Amount of Formats: " << amountOfFormats << std::endl;

	for (unsigned int i = 0; i < amountOfFormats; i++) {
		std::cout << "Format: " << surfaceFormats[i].format << std::endl;
	}

	uint32_t amountOfPresentationModes = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOfPresentationModes, nullptr);
	VkPresentModeKHR *presentModes = new VkPresentModeKHR[amountOfPresentationModes];
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOfPresentationModes, presentModes);

	std::cout << std::endl;
	std::cout << "Amount of Presentation Modes: " << amountOfPresentationModes << std::endl;
	for (unsigned int i = 0; i < amountOfPresentationModes; i++) {
		std::cout << "Supported Presentation Mode: " << presentModes[i] << std::endl;
	}


	std::cout << std::endl;
	delete[] familyProperties;
	delete[] surfaceFormats;
	delete[] presentModes;
}

void createShaderModule(const std::vector<char>& code, VkShaderModule *shaderModule) {
	VkShaderModuleCreateInfo shaderCreateInfo;
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.pNext = nullptr;
	shaderCreateInfo.flags = 0;
	shaderCreateInfo.codeSize = code.size();
	shaderCreateInfo.pCode = (uint32_t*)code.data();

	VkResult result = vkCreateShaderModule(device, &shaderCreateInfo, nullptr, shaderModule);
	ASSERT_VULKAN(result);


}

void recreateSwapchain();

void onWindowResized(GLFWwindow *window, int w, int h) {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevices[0], surface, &surfaceCapabilities);

	if (w > surfaceCapabilities.maxImageExtent.width) w = surfaceCapabilities.maxImageExtent.width;
	if (h > surfaceCapabilities.maxImageExtent.height) h = surfaceCapabilities.maxImageExtent.height;

	if (w == 0 || h == 0) {
		return;
	}

	width = w;
	height = h;
	
	recreateSwapchain();
}

void startGlfw() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, "Vulkan Test", nullptr, nullptr);
	glfwSetWindowSizeCallback(window, onWindowResized);
}

void createInstance() {
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "Vulkan Tutorial";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.pEngineName = "Kakka";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	uint32_t amountOfGlfwExtensions = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);

	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = validationLayers.size();
	instanceInfo.ppEnabledLayerNames = validationLayers.data();
	instanceInfo.enabledExtensionCount = amountOfGlfwExtensions;
	instanceInfo.ppEnabledExtensionNames = glfwExtensions;

	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
	ASSERT_VULKAN(result);
}

void printInstanceLayers() {
	uint32_t amountOfLayers = 0;
	vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
	VkLayerProperties *layers = new VkLayerProperties[amountOfLayers];
	vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);

	std::cout << "Amount of Instance Layers: " << amountOfLayers << std::endl;
	for (int i = 0; i < amountOfLayers; i++) {
		std::cout << std::endl;
		std::cout << "Name:         " << layers[i].layerName << std::endl;
		std::cout << "Spec Version: " << layers[i].specVersion << std::endl;
		std::cout << "Impl Version: " << layers[i].implementationVersion << std::endl;
		std::cout << "Description:  " << layers[i].description << std::endl;
	}
	delete[] layers;
}

void printInstanceExtensions() {
	uint32_t amountOfExtensions = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);
	VkExtensionProperties *extensions = new VkExtensionProperties[amountOfExtensions];
	vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, extensions);

	std::cout << std::endl;
	std::cout << "Amount of Extensions: " << amountOfExtensions << std::endl;
	for (int i = 0; i < amountOfExtensions; i++) {
		std::cout << std::endl;
		std::cout << "Name: " << extensions[i].extensionName << std::endl;
		std::cout << "Spec Version: " << extensions[i].specVersion << std::endl;
	}
	delete[] extensions;
}

void createGlfwWindowSurface() {
	VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
	ASSERT_VULKAN(result);
}

std::vector<VkPhysicalDevice> getAllPhysicalDevices() {
	uint32_t amountOfPhysicalDevices = 0;
	VkResult result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr);
	ASSERT_VULKAN(result);

	std::vector<VkPhysicalDevice> _physicalDevices;
	_physicalDevices.resize(amountOfPhysicalDevices);
	//VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];
	result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, _physicalDevices.data());
	ASSERT_VULKAN(result);

	return _physicalDevices;
}

void printStatsOfAllPhysicalDevices() {
	
	for (int i = 0; i < physicalDevices.size(); i++)
		printStats(physicalDevices.at(i));
}

void createLogicalDevice() {
	float queuePrios[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	VkDeviceQueueCreateInfo deviceQueueCreateInfo;
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.pNext = nullptr;
	deviceQueueCreateInfo.flags = 0;
	deviceQueueCreateInfo.queueFamilyIndex = 0; // TODO Choose correct family index
	deviceQueueCreateInfo.queueCount = amountOfQueueFamilies; // TODO Check if amount is valid
	deviceQueueCreateInfo.pQueuePriorities = queuePrios;

	VkPhysicalDeviceFeatures usedFeatures = {};
	usedFeatures.samplerAnisotropy = VK_TRUE;



	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &usedFeatures;

	// TODO pick "best device" instead of first device
	VkResult result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device);
	ASSERT_VULKAN(result);
}

void createQueue() {
	vkGetDeviceQueue(device, 0, 0, &queue);
}

void checkSurfaceSupport() {
	VkBool32 surfaceSupport = false;
	VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], 0, surface, &surfaceSupport);
	ASSERT_VULKAN(result);

	if (!surfaceSupport) {
		std::cerr << "Surface Not Supported" << std::endl;
	}
}

void createSwapchain() {
	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = nullptr;
	swapchainCreateInfo.flags = 0;;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = 3; // TODO: civ
	swapchainCreateInfo.imageFormat = ourFormat; // TODO: civ
	swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // TODO: civ
	swapchainCreateInfo.imageExtent = VkExtent2D{ width, height };
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: civ
	swapchainCreateInfo.queueFamilyIndexCount = 0;
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // TODO: civ
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = swapchain;

	VkResult result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);
	ASSERT_VULKAN(result);
}

void createImageViews() {
	vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, nullptr);
	VkImage *swapchainImages = new VkImage[amountOfImagesInSwapchain];
	VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, swapchainImages);
	ASSERT_VULKAN(result);

	imageViews = new VkImageView[amountOfImagesInSwapchain];
	for (int i = 0; i < amountOfImagesInSwapchain; i++) {

		createImageView(device, swapchainImages[i], ourFormat, VK_IMAGE_ASPECT_COLOR_BIT, imageViews[i]);

		
	}
	delete[] swapchainImages;
}

void createRenderPass() {
	VkAttachmentDescription attachmentDescription;
	attachmentDescription.flags = 0;
	attachmentDescription.format = ourFormat;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachmentReference;
	attachmentReference.attachment = 0;
	attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment = DepthImage::getDepthAttachment(physicalDevices[0]);

	VkAttachmentReference depthAttachmentReference;
	depthAttachmentReference.attachment = 1;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription;
	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &attachmentReference;
	subpassDescription.pResolveAttachments = nullptr;
	subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;

	VkSubpassDependency subpassDependency;
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependency.dependencyFlags = 0;

	std::vector<VkAttachmentDescription> attachments;
	attachments.push_back(attachmentDescription);
	attachments.push_back(depthAttachment);

	VkRenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.flags = 0;
	renderPassCreateInfo.attachmentCount = attachments.size();
	renderPassCreateInfo.pAttachments = attachments.data();
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpassDescription;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &subpassDependency;

	VkResult result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);
	ASSERT_VULKAN(result);
}

void createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
	descriptorSetLayoutBinding.binding = 0;
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerDescriptorSetLayoutBinding;
	samplerDescriptorSetLayoutBinding.binding = 1;
	samplerDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerDescriptorSetLayoutBinding.descriptorCount = 1;
	samplerDescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> descriptorSets;
	descriptorSets.push_back(descriptorSetLayoutBinding);
	descriptorSets.push_back(samplerDescriptorSetLayoutBinding);

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = nullptr;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = descriptorSets.size();
	descriptorSetLayoutCreateInfo.pBindings = descriptorSets.data();

	VkResult result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
	ASSERT_VULKAN(result);
}

void createPipeline() {
    //system("./runCompiler.sh");
	std::vector<char> shaderCodeVert = readFile("vert.spv");
	std::vector<char> shaderCodeFrag = readFile("frag.spv");

	createShaderModule(shaderCodeVert, &shaderModuleVert);
	createShaderModule(shaderCodeFrag, &shaderModuleFrag);

	VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert;
	shaderStageCreateInfoVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfoVert.pNext = nullptr;
	shaderStageCreateInfoVert.flags = 0;
	shaderStageCreateInfoVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStageCreateInfoVert.module = shaderModuleVert;
	shaderStageCreateInfoVert.pName = "main";
	shaderStageCreateInfoVert.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo shaderStageCreateInfoFrag;
	shaderStageCreateInfoFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfoFrag.pNext = nullptr;
	shaderStageCreateInfoFrag.flags = 0;
	shaderStageCreateInfoFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStageCreateInfoFrag.module = shaderModuleFrag;
	shaderStageCreateInfoFrag.pName = "main";
	shaderStageCreateInfoFrag.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo shaderStages[] = { shaderStageCreateInfoVert, shaderStageCreateInfoFrag };

	auto vertexBindingDescription = Vertex::getBindingDescription();
	auto vertexAttributeDescriptions = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo;
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.pNext = nullptr;
	vertexInputCreateInfo.flags = 0;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
	vertexInputCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.pNext = nullptr;
	inputAssemblyCreateInfo.flags = 0;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scrissor;
	scrissor.offset = { 0, 0 };
	scrissor.extent = { width, height };

	VkPipelineViewportStateCreateInfo viewportCreateInfo;
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.pNext = nullptr;
	viewportCreateInfo.flags = 0;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pViewports = &viewport;
	viewportCreateInfo.scissorCount = 1;
	viewportCreateInfo.pScissors = &scrissor;

	VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo;
	rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationCreateInfo.pNext = nullptr;
	rasterizationCreateInfo.flags = 0;
	rasterizationCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationCreateInfo.depthBiasClamp = 0.0f;
	rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
	rasterizationCreateInfo.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo;
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.pNext = nullptr;
	multisampleCreateInfo.flags = 0;
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleCreateInfo.minSampleShading = 1.0f;
	multisampleCreateInfo.pSampleMask = nullptr;
	multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = DepthImage::getDepthStencilCreateInfoOpaque();
		 
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo;
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.pNext = nullptr;
	colorBlendCreateInfo.flags = 0;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
	colorBlendCreateInfo.blendConstants[0] = 0.0f;
	colorBlendCreateInfo.blendConstants[1] = 0.0f;
	colorBlendCreateInfo.blendConstants[2] = 0.0f;
	colorBlendCreateInfo.blendConstants[3] = 0.0f;

	VkDynamicState dynamicStates[] {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.pNext = nullptr;
	dynamicStateCreateInfo.flags = 0;
	dynamicStateCreateInfo.dynamicStateCount = 2;
	dynamicStateCreateInfo.pDynamicStates = dynamicStates;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);

	ASSERT_VULKAN(result);
	VkGraphicsPipelineCreateInfo PipelineCreateInfo;
	PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineCreateInfo.pNext = nullptr;
	PipelineCreateInfo.flags = 0;
	PipelineCreateInfo.stageCount = 2;
	PipelineCreateInfo.pStages = shaderStages;
	PipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
	PipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	PipelineCreateInfo.pTessellationState = nullptr;
	PipelineCreateInfo.pViewportState = &viewportCreateInfo;
	PipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
	PipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
	PipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
	PipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
	PipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
	PipelineCreateInfo.layout = pipelineLayout;
	PipelineCreateInfo.renderPass = renderPass;
	PipelineCreateInfo.subpass = 0;
	PipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	PipelineCreateInfo.basePipelineIndex = -1;

	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &PipelineCreateInfo, nullptr, &pipeline);
	ASSERT_VULKAN(result);
}

void createFramebuffers() {
	framebuffers = new VkFramebuffer[amountOfImagesInSwapchain];

	for (size_t i = 0; i < amountOfImagesInSwapchain; i++) {

		std::vector<VkImageView> attachmentViews;
		attachmentViews.push_back(imageViews[i]);
		attachmentViews.push_back(depthImage.getImageView());

		VkFramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = attachmentViews.size();
		framebufferCreateInfo.pAttachments = attachmentViews.data();
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &(framebuffers[i]));
		ASSERT_VULKAN(result);
	}
}

void createCommandPool() {
	VkCommandPoolCreateInfo commandPoolCreateInfo;
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = nullptr;
	commandPoolCreateInfo.flags = 0;
	commandPoolCreateInfo.queueFamilyIndex = 0; //TODO civ -get correct queue with VK_QUEUE_GRAPHICS_BIT

	VkResult result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
	ASSERT_VULKAN(result);
}

void createDepthImage() {
	depthImage.create(device, physicalDevices[0], commandPool, queue, width, height);
}

void createCommandBuffers() {
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = amountOfImagesInSwapchain;

	commandBuffers = new VkCommandBuffer[amountOfImagesInSwapchain];
	VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers);
	ASSERT_VULKAN(result);
}




void loadTexture() {
	pika.load("images/40f.png");
	pika.upload(device, physicalDevices[0], commandPool, queue);
}

void loadMesh() {
	dragon.create("meshes/testi.obj");
	vertices = dragon.getVertices();
	indices = dragon.getIndices();
}

void createVertexBuffer() {
	createAndUploadBuffer(device, physicalDevices[0], queue, commandPool, vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer, vertexBufferDeviceMemory);
}

void createIndexBuffer() {
	createAndUploadBuffer(device, physicalDevices[0], queue, commandPool, indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer, indexBufferDeviceMemory);
}

void createUniformBuffer() {
	VkDeviceSize bufferSize = sizeof(ubo);
	createBuffer(device, physicalDevices[0], bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, uniformBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBufferMemory);
}

void createDescriptorPool() {
	VkDescriptorPoolSize descriptorPoolSize;
	descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSize.descriptorCount = 1;

	VkDescriptorPoolSize samplerPoolSize;
	samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerPoolSize.descriptorCount = 1;

	std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
	descriptorPoolSizes.push_back(descriptorPoolSize);
	descriptorPoolSizes.push_back(samplerPoolSize);

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = nullptr;
	descriptorPoolCreateInfo.flags = 0;
	descriptorPoolCreateInfo.maxSets = 1;
	descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizes.size();
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

	VkResult result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
	ASSERT_VULKAN(result);
}

void createDescriptorSet() {
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

	VkResult result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet);
	ASSERT_VULKAN(result);

	VkDescriptorBufferInfo descriptorBufferInfo;
	descriptorBufferInfo.buffer = uniformBuffer;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = sizeof(ubo);

	VkWriteDescriptorSet descriptorWrite;
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.pNext = nullptr;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pBufferInfo = &descriptorBufferInfo;
	descriptorWrite.pTexelBufferView = nullptr;

	VkDescriptorImageInfo descriptorImageInfo;
	descriptorImageInfo.sampler = pika.getSampler();
	descriptorImageInfo.imageView = pika.getImageView();
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet descriptorSampler;
	descriptorSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorSampler.pNext = nullptr;
	descriptorSampler.dstSet = descriptorSet;
	descriptorSampler.dstBinding = 1;
	descriptorSampler.dstArrayElement = 0;
	descriptorSampler.descriptorCount = 1;
	descriptorSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSampler.pImageInfo = &descriptorImageInfo;
	descriptorSampler.pBufferInfo = nullptr;
	descriptorSampler.pTexelBufferView = nullptr;

	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	writeDescriptorSets.push_back(descriptorWrite);
	writeDescriptorSets.push_back(descriptorSampler);

	vkUpdateDescriptorSets(device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
}

void recordCommandBuffers() {
	VkCommandBufferBeginInfo commandBufferBegininfo;
	commandBufferBegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBegininfo.pNext = nullptr;
	commandBufferBegininfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	commandBufferBegininfo.pInheritanceInfo = nullptr;

	for (size_t i = 0; i < amountOfImagesInSwapchain; i++) {
		VkResult result = vkBeginCommandBuffer(commandBuffers[i], &commandBufferBegininfo);
		ASSERT_VULKAN(result);

		VkRenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = framebuffers[i];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { width, height };
		VkClearValue clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
		VkClearValue depthClearValue = { 1.0f, 0 };

		std::vector<VkClearValue> clearValues;
		clearValues.push_back(clearValue);
		clearValues.push_back(depthClearValue);

		renderPassBeginInfo.clearValueCount = clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = width;
		viewport.height = height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);

		VkRect2D scissor;
		scissor.offset = { 0, 0 };
		scissor.extent = { width, height };
		vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer, offsets);
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		//vkCmdDraw(commandBuffers[i], vertices.size(), 1, 0, 0);
		vkCmdDrawIndexed(commandBuffers[i], indices.size(), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		result = vkEndCommandBuffer(commandBuffers[i]);
		ASSERT_VULKAN(result);
	}
}

void createSemaphores() {
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	VkResult result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreImageAvailable);
	ASSERT_VULKAN(result);
	result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreRenderingDone);
	ASSERT_VULKAN(result);
}

void startVulkan() {
	createInstance();
	physicalDevices = getAllPhysicalDevices();
	printInstanceLayers();
	printInstanceExtensions();
	createGlfwWindowSurface();
	printStatsOfAllPhysicalDevices();
	createLogicalDevice();
	createQueue();
	checkSurfaceSupport();
	createSwapchain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createPipeline();
	createCommandPool();
	createDepthImage();
	createFramebuffers();
	createCommandBuffers();
	loadTexture();
	loadMesh();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffer();
	createDescriptorPool();
	createDescriptorSet();
	recordCommandBuffers();
	createSemaphores();
}

void recreateSwapchain() {
	vkDeviceWaitIdle(device);

	depthImage.destroy();

	vkFreeCommandBuffers(device, commandPool, amountOfImagesInSwapchain, commandBuffers);
	delete[] commandBuffers;

	for (size_t i = 0; i < amountOfImagesInSwapchain; i++) {
		vkDestroyFramebuffer(device, framebuffers[i], nullptr);
	}
	delete[] framebuffers;
	
	vkDestroyRenderPass(device, renderPass, nullptr);
	for (int i = 0; i < amountOfImagesInSwapchain; i++) {
		vkDestroyImageView(device, imageViews[i], nullptr);
	}
	delete[] imageViews;

	VkSwapchainKHR oldSwapchain = swapchain;

	createSwapchain();
	createImageViews();
	createRenderPass();
	createDepthImage();
	createFramebuffers();
	createCommandBuffers();
	recordCommandBuffers();

	vkDestroySwapchainKHR(device, oldSwapchain, nullptr);
}

void drawFrame() {
	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(), semaphoreImageAvailable, VK_NULL_HANDLE, &imageIndex);
	
	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &semaphoreImageAvailable;
	VkPipelineStageFlags waitStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = waitStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(commandBuffers[imageIndex]);
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &semaphoreRenderingDone;

	VkResult result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	ASSERT_VULKAN(result);

	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &semaphoreRenderingDone;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(queue, &presentInfo);
	ASSERT_VULKAN(result);
}

auto gameStartTime = std::chrono::high_resolution_clock::now();
void updateMVP() {
	auto frameTime = std::chrono::high_resolution_clock::now();
	
	float timeSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(frameTime - gameStartTime).count();

	glm::mat4 model = glm::translate(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f)), timeSinceStart * glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, 0.0f, -2.0f));
	glm::mat4 view = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), width / (float)height, 0.01f, 10.0f);
	projection[1][1] *= -1;


	ubo.lightPosition = /*glm::rotate(glm::mat4(), timeSinceStart * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * */glm::vec4(0.0f, 3.0f, 1.0f, 0.0f);
	ubo.model = model;
	ubo.view = view;
	ubo.projection = projection;

	void* data;
	vkMapMemory(device, uniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, uniformBufferMemory);
}

void gameLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		updateMVP();

		drawFrame();
	}
}

void shutDownVulkan() {
	vkDeviceWaitIdle(device);

	depthImage.destroy();

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	vkFreeMemory(device, uniformBufferMemory, nullptr);
	vkDestroyBuffer(device, uniformBuffer, nullptr);

	vkFreeMemory(device, indexBufferDeviceMemory, nullptr);
	vkDestroyBuffer(device, indexBuffer, nullptr);

	vkFreeMemory(device, vertexBufferDeviceMemory, nullptr);
	vkDestroyBuffer(device, vertexBuffer, nullptr);

	pika.destroy();

	vkDestroySemaphore(device, semaphoreImageAvailable, nullptr);
	vkDestroySemaphore(device, semaphoreRenderingDone, nullptr);

	vkFreeCommandBuffers(device, commandPool, amountOfImagesInSwapchain, commandBuffers);
	delete[] commandBuffers;
	vkDestroyCommandPool(device, commandPool, nullptr);

	for (size_t i = 0; i < amountOfImagesInSwapchain; i++) {
		vkDestroyFramebuffer(device, framebuffers[i], nullptr);
	}
	delete[] framebuffers;

	vkDestroyPipeline(device, pipeline, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);
	for (int i = 0; i < amountOfImagesInSwapchain; i++) {
		vkDestroyImageView(device, imageViews[i], nullptr);
	}
	delete[] imageViews;
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyShaderModule(device, shaderModuleVert, nullptr);
	vkDestroyShaderModule(device, shaderModuleFrag, nullptr);
	vkDestroySwapchainKHR(device, swapchain, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void shutDownGlfw() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main()
{
	startGlfw();
	startVulkan();
	gameLoop();
	shutDownVulkan();
	shutDownGlfw();

	return 0;
}