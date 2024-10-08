//
//  RNVulkanGPUBuffer.h
//  Rayne
//
//  Copyright 2016 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_VULKANGPUBUFFER_H_
#define __RAYNE_VULKANGPUBUFFER_H_

#include "RNVulkan.h"
#include "RNVulkanRenderer.h"

#include <vk_mem_alloc.h>

namespace RN
{
	class VulkanGPUBuffer : public GPUBuffer
	{
	public:
		friend class VulkanRenderer;

		VKAPI void *GetBuffer() final;
		VKAPI void UnmapBuffer() final;
		VKAPI void InvalidateRange(const Range &range) final;
		VKAPI void FlushRange(const Range &range) final;
		VKAPI size_t GetLength() const final;

		VkBuffer GetVulkanBuffer() const;

	private:
		VulkanGPUBuffer(VulkanRenderer *renderer, void *data, size_t length, GPUResource::UsageOptions usageOption);
		~VulkanGPUBuffer() override;

		VulkanRenderer *_renderer;

		VkBuffer _buffer;
		VmaAllocation _allocation;

		VkBuffer _stagingBuffer;
		VmaAllocation _stagingAllocation;

		bool _isHostVisible;
		size_t _length;
		std::atomic<void *> _mappedBuffer;

		RNDeclareMetaAPI(VulkanGPUBuffer, VKAPI)
	};
}


#endif /* __RAYNE_VULKANGPUBUFFER_H_ */
