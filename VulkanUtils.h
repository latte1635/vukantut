#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"
#include "Camera.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <csignal>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>

#ifdef _MSC_VER
#define ASSERT_VULKAN(val)\
		if(val != VK_SUCCESS){\
		    __debugbreak();\
		}
#else
#define ASSERT_VULKAN(val)\
		if(val != VK_SUCCESS){\
		    raise(SIGTRAP);\
		}
#endif

void handleKeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Camera *camera = Camera::getInstance();
    float speed = 0.1f;
    if(action == GLFW_PRESS) {
        if(key == GLFW_KEY_E) {
            camera->changeFov(5.0f);
            std::cout << "pressed key: " << key << std::endl;
        }
        if(key == GLFW_KEY_Q) {
            camera->changeFov(-5.0f);
            std::cout << "pressed key: " << key << std::endl;
        }
        if(key == GLFW_KEY_W) {
            camera->move(glm::vec3(speed, speed, 0.0f));
            camera->moveTarget(glm::vec3(speed, speed, 0.0f));
            std::cout << "pressed key: " << key << std::endl;
        }
        if(key == GLFW_KEY_S) {
            camera->move(glm::vec3(-speed, -speed, 0.0f));
            camera->moveTarget(glm::vec3(-speed, -speed, 0.0f));
            std::cout << "pressed key: " << key << std::endl;
        }
        if(key == GLFW_KEY_D) {
            camera->move(glm::vec3(speed, -speed, 0.0f));
            camera->moveTarget(glm::vec3(speed, -speed, 0.0f));
            std::cout << "pressed key: " << key << std::endl;
        }
        if(key == GLFW_KEY_A) {
            camera->move(glm::vec3(-speed, speed, 0.0f));
            camera->moveTarget(glm::vec3(-speed, speed, 0.0f));
            std::cout << "pressed key: " << key << std::endl;
        }
    }
}

inline bool exists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

unsigned long getFileLength(std::ifstream& file)
{
    if(!file.good()) return 0;

    //unsigned long pos=file.tellg();
    file.seekg(0,std::ios::end);
    size_t len = file.tellg();
    file.seekg(std::ios::beg);

    return len;
}

std::vector<char> readFile(const char* filename) {
/*
    unsigned char c;
    std::vector<char> charBuffer;
    FILE *shaderFile = fopen(filename, "rb");
    if(shaderFile == NULL){
        std::cout << "Failed reading file..." << std::endl;
    }
    while(!feof(shaderFile)){
        c = fgetc(shaderFile);
        charBuffer.push_back(c);
    }
    fclose(shaderFile);
    return charBuffer;
*/
    std::cout << "Trying to read file: " << filename << "..." << std::endl;
    bool fileExists = exists(filename);
    std::cout << "File exists: " << fileExists << std::endl;

    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file) {
        //size_t fileSize = (size_t)file.tellg();
        //file.seekg(0);
        size_t fileSize = getFileLength(file);
        std::cout << "Shader file size: " << fileSize << std::endl;

        std::vector<char> fileBuffer(fileSize);

        file.read(fileBuffer.data(), fileSize);
        file.close();
        //std::cout << "File buffer size: " << fileBuffer << std::endl;
        return fileBuffer;
    }
    else {
    	throw std::runtime_error("Failed to open .spv file!!!");
    }
}

uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties); // TODO: civ
	for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	//__debugbreak();
	throw std::runtime_error("\n!!!Found no correct memory type!!!\n");
}

bool isFormatSupported(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags featureFlags) {
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

	if (tiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & featureFlags) == featureFlags) {
		return true;
	}
	else if (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & featureFlags) == featureFlags) {
		return true;
	}
	return false;
}

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, std::vector<VkFormat> &formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags) {
	for (VkFormat format : formats) {
		if (isFormatSupported(physicalDevice, format, tiling, featureFlags)) {
			return format;
		}
	}

	throw std::runtime_error("!!!No supported format found!!!");
}

bool isStencilFormat(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize deviceSize, VkBufferUsageFlags bufferUsageFlags, VkBuffer &buffer, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceMemory &deviceMemory) {
	VkBufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = deviceSize;
	bufferCreateInfo.usage = bufferUsageFlags;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;

	VkResult result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer);
	ASSERT_VULKAN(result);

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice, memoryRequirements.memoryTypeBits, memoryPropertyFlags);

	result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &deviceMemory);
	ASSERT_VULKAN(result);

	vkBindBufferMemory(device, buffer, deviceMemory, 0);
}

VkCommandBuffer startSingleTimeCommadBuffer(VkDevice device, VkCommandPool commandPool) {
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
	ASSERT_VULKAN(result);

	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;

	result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	ASSERT_VULKAN(result);

	return commandBuffer;
}

void endSingleTimeCommandBuffer(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer) {
	VkResult result = vkEndCommandBuffer(commandBuffer);
	ASSERT_VULKAN(result);

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	ASSERT_VULKAN(result);

	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void createImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage &image, VkDeviceMemory &imageMemory) {
	VkImageCreateInfo imageCreateInfo;
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = nullptr;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = format;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = tiling;
	imageCreateInfo.usage = usageFlags;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

	VkResult result = vkCreateImage(device, &imageCreateInfo, nullptr, &image);
	ASSERT_VULKAN(result);

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(device, image, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice, memoryRequirements.memoryTypeBits, propertyFlags);

	result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &imageMemory);
	ASSERT_VULKAN(result);

	vkBindImageMemory(device, image, imageMemory, 0);
}

void createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView &imageView) {
	VkImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = nullptr;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VkResult result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView);
	ASSERT_VULKAN(result);
}

void copyBuffer(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer src, VkBuffer dst, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = startSingleTimeCommadBuffer(device, commandPool);

	VkBufferCopy bufferCopy;
	bufferCopy.srcOffset = 0;
	bufferCopy.dstOffset = 0;
	bufferCopy.size = size;
	vkCmdCopyBuffer(commandBuffer, src, dst, 1, &bufferCopy);

	endSingleTimeCommandBuffer(device, queue, commandPool, commandBuffer);
}

template <typename T>
void createAndUploadBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue queue, VkCommandPool commandPool, std::vector<T> data, VkBufferUsageFlags usage, VkBuffer &buffer, VkDeviceMemory deviceMemory) {
	VkDeviceSize bufferSize = sizeof(T) * data.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);

	void *rawData;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, data.data(), bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(device, physicalDevice, bufferSize, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, deviceMemory);

	copyBuffer(device, queue, commandPool, stagingBuffer, buffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void changeImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = startSingleTimeCommadBuffer(device, commandPool);

	VkImageMemoryBarrier imageMemoryBarrier;
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else {
		throw std::invalid_argument("!!!Layout transition not yet supported!!!");
	}

	imageMemoryBarrier.oldLayout = oldLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (isStencilFormat(format)) {
			imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	endSingleTimeCommandBuffer(device, queue, commandPool, commandBuffer);
}
