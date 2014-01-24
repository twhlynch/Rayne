//
//  RNCameraInternal.cpp
//  Rayne
//
//  Copyright 2014 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNCameraInternal.h"

namespace RN
{
	RNDeclareMeta(CubemapCamera)
	
	CubemapCamera::CubemapCamera(const Vector2 &size, RenderStorage *storage, Flags flags, float scaleFactor) :
		Camera(size, storage, flags, scaleFactor)
	{}
	
	bool CubemapCamera::InFrustum(const RN::Vector3 &position, float radius)
	{
		return (GetWorldPosition().Distance(position) < GetClipFar() + radius);
	}
}
