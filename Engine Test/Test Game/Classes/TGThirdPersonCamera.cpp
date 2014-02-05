//
//  TGThirdPersonCamera.cpp
//  Game-osx
//
//  Created by Sidney Just on 07.04.13.
//  Copyright (c) 2013 Sidney Just. All rights reserved.
//

#include "TGThirdPersonCamera.h"

namespace TG
{
	RNDefineMeta(ThirdPersonCamera)
	
	ThirdPersonCamera::ThirdPersonCamera(RN::RenderStorage *storage, RN::Camera::Flags flags) :
	RN::Camera(RN::Vector2(), storage, flags)
	{
		_target = 0;
		_distance = 5.0f;
		_pitch = -10.0f;
	}
	
	ThirdPersonCamera::~ThirdPersonCamera()
	{
		if(_target)
			_target->Release();
	}
	
	void ThirdPersonCamera::SetTarget(RN::Entity *target)
	{
		if(_target)
			_target->Release();
		
		_target = target ? target->Retain() : 0;
	}
	
	void ThirdPersonCamera::Update(float delta)
	{
		if(_target)
		{
			RN::Quaternion rotation = _target->GetWorldRotation();
			const RN::Vector3& position = _target->GetWorldPosition();
			
			rotation *= RN::Quaternion(RN::Vector3(0.0f, 0.0f, _pitch));
			
			RN::Vector3 cameraPosition = rotation.GetRotatedVector(RN::Vector3(0.0f, 1.5f, _distance));
			cameraPosition += position;
			
			SetWorldPosition(cameraPosition);
			SetWorldRotation(rotation);
		}
		
		RN::Camera::Update(delta);
	}
	
	bool ThirdPersonCamera::CanUpdate(RN::FrameID frame)
	{
		if(!RN::Camera::CanUpdate(frame))
			return false;
		
		if(_target)
			return (_target->GetLastFrame() == frame);
		
		return true;
	}
}
