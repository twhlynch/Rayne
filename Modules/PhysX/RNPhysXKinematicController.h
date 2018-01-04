//
//  RNPhysXKinematicController.h
//  Rayne-PhysX
//
//  Copyright 2017 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_PHYSXKINEMATICCONTROLLER_H_
#define __RAYNE_PHYSXKINEMATICCONTROLLER_H_

#include "RNPhysX.h"
#include "RNPhysXCollisionObject.h"
#include "RNPhysXShape.h"

namespace physx
{
	class PxController;
}

namespace RN
{
	class PhysXKinematicController : public PhysXCollisionObject
	{
	public:
		PXAPI PhysXKinematicController(float radius, float height, PhysXMaterial *material);
		PXAPI ~PhysXKinematicController() override;
			
		PXAPI void Move(const Vector3 &direction, float delta);
		PXAPI void Gravity(float gforce, float delta);
		PXAPI float SweepTest(const Vector3 &direction, const Vector3 &offset = Vector3()) const;

		PXAPI void SetCollisionFilter(uint32 group, uint32 mask) override;
		PXAPI Vector3 GetFeetOffset() const;

	/*	PXAPI void SetFallSpeed(float speed);
		PXAPI void SetJumpSpeed(float speed);
		PXAPI void SetMaxJumpHeight(float maxHeight);
		PXAPI void SetMaxSlope(float maxSlope);
		PXAPI void SetGravity(float gravity);*/
			
/*		PXAPI bool IsOnGround();
		PXAPI void Jump();*/
			
	protected:
		void DidUpdate(SceneNode::ChangeSet changeSet) override;
			
		physx::PxController *_controller;
		PhysXMaterial *_material;

		float _gravity;
		bool _didUpdatePosition;
			
		RNDeclareMetaAPI(PhysXKinematicController, PXAPI)
	};
}

#endif /* defined(__RAYNE_PHYSXKINEMATICCONTROLLER_H_) */
