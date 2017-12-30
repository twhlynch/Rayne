//
//  RNPhysXDynamicBody.cpp
//  Rayne-PhysX
//
//  Copyright 2017 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNPhysXDynamicBody.h"
#include "RNPhysXWorld.h"
#include "PxPhysicsAPI.h"

namespace RN
{
	RNDefineMeta(PhysXDynamicBody, PhysXCollisionObject)
		
		PhysXDynamicBody::PhysXDynamicBody(PhysXShape *shape, float mass) :
		_shape(shape->Retain()),
		_actor(nullptr)
	{
		physx::PxPhysics *physics = PhysXWorld::GetSharedInstance()->GetPhysXInstance();
		_actor = physics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));

		if(shape->IsKindOfClass(PhysXCompoundShape::GetMetaClass()))
		{
			PhysXCompoundShape *compound = shape->Downcast<PhysXCompoundShape>();
			for(PhysXShape *tempShape : compound->_shapes)
			{
				_actor->attachShape(*tempShape->GetPhysXShape());
			}
		}
		else
		{
			_actor->attachShape(*shape->GetPhysXShape());
		}
		
		physx::PxRigidBodyExt::updateMassAndInertia(*_actor, mass);

		_actor->userData = this;
	}
		
	PhysXDynamicBody::~PhysXDynamicBody()
	{
		_actor->release();
		_shape->Release();
	}

	void PhysXDynamicBody::SetCollisionFilter(uint32 group, uint32 mask)
	{
		PhysXCollisionObject::SetCollisionFilter(group, mask);

		physx::PxFilterData filterData;
		filterData.word0 = _collisionFilterGroup;
		filterData.word1 = _collisionFilterMask;

		if(_shape->IsKindOfClass(PhysXCompoundShape::GetMetaClass()))
		{
			PhysXCompoundShape *compound = _shape->Downcast<PhysXCompoundShape>();
			for(PhysXShape *tempShape : compound->_shapes)
			{
				tempShape->GetPhysXShape()->setSimulationFilterData(filterData);
				tempShape->GetPhysXShape()->setQueryFilterData(filterData);
			}
		}
		else
		{
			_shape->GetPhysXShape()->setSimulationFilterData(filterData);
			_shape->GetPhysXShape()->setQueryFilterData(filterData);
		}


/*		const uint32 numShapes = _actor->getNbShapes();
		physx::PxShape** shapes = (physx::PxShape**)malloc(sizeof(physx::PxShape*)*numShapes);
		_actor->getShapes(shapes, numShapes);
		for(uint32 i = 0; i < numShapes; i++)
		{
			physx::PxShape* shape = shapes[i];
			shape->setSimulationFilterData(filterData);
		}
		free(shapes);*/
	}
	
		
	PhysXDynamicBody *PhysXDynamicBody::WithShape(PhysXShape *shape, float mass)
	{
		PhysXDynamicBody *body = new PhysXDynamicBody(shape, mass);
		return body->Autorelease();
	}
	
/*	btCollisionObject *PhysXDynamicBody::GetBulletCollisionObject() const
	{
		return _rigidBody;
	}*/
		
	void PhysXDynamicBody::SetMass(float mass)
	{
		physx::PxRigidBodyExt::updateMassAndInertia(*_actor, mass);
	}

	void PhysXDynamicBody::SetLinearVelocity(const Vector3 &velocity)
	{
		_actor->setLinearVelocity(physx::PxVec3(velocity.x, velocity.y, velocity.z));
	}
	void PhysXDynamicBody::SetAngularVelocity(const Vector3 &velocity)
	{
		_actor->setAngularVelocity(physx::PxVec3(velocity.x, velocity.y, velocity.z));
	}
/*	void PhysXDynamicBody::SetCCDMotionThreshold(float threshold)
	{
		_rigidBody->setCcdMotionThreshold(threshold);
	}
	void PhysXDynamicBody::SetCCDSweptSphereRadius(float radius)
	{
		_rigidBody->setCcdSweptSphereRadius(radius);
	}
		
	void PhysXDynamicBody::SetGravity(const RN::Vector3 &gravity)
	{
		_rigidBody->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
	}*/
		
	void PhysXDynamicBody::SetDamping(float linear, float angular)
	{
		_actor->setLinearDamping(linear);
		_actor->setAngularDamping(angular);
		_actor->setMaxAngularVelocity(PX_MAX_F32);
	}

	void PhysXDynamicBody::SetMaxAngularVelocity(float max)
	{
		_actor->setMaxAngularVelocity(max);
	}
		
	Vector3 PhysXDynamicBody::GetLinearVelocity() const
	{
		const physx::PxVec3& velocity = _actor->getLinearVelocity();
		return Vector3(velocity.x, velocity.y, velocity.z);
	}
	Vector3 PhysXDynamicBody::GetAngularVelocity() const
	{
		const physx::PxVec3& velocity = _actor->getAngularVelocity();
		return Vector3(velocity.x, velocity.y, velocity.z);
	}
		
		
/*	Vector3 PhysXDynamicBody::GetCenterOfMass() const
	{
		const btVector3& center = _rigidBody->getCenterOfMassPosition();
		return Vector3(center.x(), center.y(), center.z());
	}
	Matrix PhysXDynamicBody::GetCenterOfMassTransform() const
	{
		const btTransform& transform = _rigidBody->getCenterOfMassTransform();
			
		btQuaternion rotation = transform.getRotation();
		btVector3 position    = transform.getOrigin();
			
		Matrix matrix;
			
		matrix.Translate(Vector3(position.x(), position.y(), position.z()));
		matrix.Rotate(Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w()));
			
		return matrix;
	}
		
		
	void PhysXDynamicBody::ApplyForce(const Vector3 &force)
	{
		_rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
	}
	void PhysXDynamicBody::ApplyForce(const Vector3 &force, const Vector3 &origin)
	{
		_rigidBody->applyForce(btVector3(force.x, force.y, force.z), btVector3(origin.x, origin.y, origin.z));
	}
	void PhysXDynamicBody::ClearForces()
	{
		_rigidBody->clearForces();
	}
		
	void PhysXDynamicBody::ApplyTorque(const Vector3 &torque)
	{
		_rigidBody->applyTorque(btVector3(torque.x, torque.y, torque.z));
	}
	void PhysXDynamicBody::ApplyTorqueImpulse(const Vector3 &torque)
	{
		_rigidBody->applyTorqueImpulse(btVector3(torque.x, torque.y, torque.z));
	}
	void PhysXDynamicBody::ApplyImpulse(const Vector3 &impulse)
	{
		_rigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
	}
	void PhysXDynamicBody::ApplyImpulse(const Vector3 &impulse, const Vector3 &origin)
	{
		_rigidBody->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), btVector3(origin.x, origin.y, origin.z));
	}
		
		
		
	void PhysXDynamicBody::DidUpdate(SceneNode::ChangeSet changeSet)
	{
		BulletCollisionObject::DidUpdate(changeSet);
			
		if(changeSet & SceneNode::ChangeSet::Position)
		{
			btTransform transform;
				
			_motionState->getWorldTransform(transform);
			_rigidBody->setCenterOfMassTransform(transform);
		}
	}
	void PhysXDynamicBody::UpdateFromMaterial(BulletMaterial *material)
	{
		_rigidBody->setFriction(material->GetFriction());
		_rigidBody->setRollingFriction(material->GetRollingFriction());
		_rigidBody->setSpinningFriction(material->GetSpinningFriction());
		_rigidBody->setRestitution(material->GetRestitution());
		_rigidBody->setDamping(material->GetLinearDamping(), material->GetAngularDamping());
	}*/

	void PhysXDynamicBody::UpdatePosition()
	{
		const physx::PxTransform &transform = _actor->getGlobalPose();
		GetParent()->SetWorldPosition(Vector3(transform.p.x, transform.p.y, transform.p.z));
		GetParent()->SetWorldRotation(Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
	}
		
		
	void PhysXDynamicBody::InsertIntoWorld(PhysXWorld *world)
	{
		PhysXCollisionObject::InsertIntoWorld(world);
		physx::PxScene *scene = world->GetPhysXScene();
		scene->addActor(*_actor);

		const Vector3 &position = GetParent()->GetWorldPosition();
		const Quaternion &rotation = GetParent()->GetWorldRotation();
		physx::PxTransform transform;
		transform.p.x = position.x;
		transform.p.y = position.y;
		transform.p.z = position.z;
		transform.q.x = rotation.x;
		transform.q.y = rotation.y;
		transform.q.z = rotation.z;
		transform.q.w = rotation.w;
		_actor->setGlobalPose(transform);
	}
		
	void PhysXDynamicBody::RemoveFromWorld(PhysXWorld *world)
	{
		PhysXCollisionObject::RemoveFromWorld(world);
			
		physx::PxScene *scene = world->GetPhysXScene();
		scene->removeActor(*_actor);
	}

/*	void PhysXDynamicBody::SetPositionOffset(RN::Vector3 offset)
	{
		BulletCollisionObject::SetPositionOffset(offset);
		_motionState->SetPositionOffset(offset);
	}*/
}
