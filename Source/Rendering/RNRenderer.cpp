//
//  RNRenderer.cpp
//  Rayne
//
//  Copyright 2015 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNRenderer.h"
#include "../Base/RNSettings.h"

namespace RN
{
	RNDefineMeta(Renderer, Object)

	static Renderer *_activeRenderer = nullptr;

	Renderer::Renderer()
	{}

	Renderer *Renderer::GetActiveRenderer()
	{
		RN_ASSERT(_activeRenderer, "GetActiveRenderer() called, but no renderer is currently active");
		return _activeRenderer;
	}

	void Renderer::Activate()
	{
		RN_ASSERT(!_activeRenderer, "Rayne only supports one active renderer at a time");
		_activeRenderer = this;
	}

	void Renderer::Deactivate()
	{
		_activeRenderer = nullptr;
	}
}
