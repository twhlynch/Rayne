//
//  RND3D12Device.h
//  Rayne-D3D12
//
//  Copyright 2015 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_D3D12DEVICE_H__
#define __RAYNE_D3D12DEVICE_H__

#include "RND3D12.h"

#include <D3D12MemAlloc.h>

namespace RN
{
	class D3D12Device : public RenderingDevice
	{
	public:
		D3DAPI D3D12Device(IDXGIAdapter1 *adapter);
		D3DAPI ~D3D12Device();

		D3DAPI bool CreateDevice();

		IDXGIAdapter1 *GetAdapter() const { return _adapter;  }
		ID3D12Device *GetDevice() const { return _device; }
		D3D12MA::Allocator* GetMemoryAllocator() const { return _memoryAllocator; }

	private:
		IDXGIAdapter1 *_adapter;
		ID3D12Device *_device;

		D3D12MA::Allocator* _memoryAllocator;

		RNDeclareMetaAPI(D3D12Device, D3DAPI)
	};
}

#endif /* __RAYNE_D3D12DEVICE_H__ */
