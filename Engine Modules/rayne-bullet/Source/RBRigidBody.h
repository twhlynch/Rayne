//
//  RBRigidBody.h
//  rayne-bullet
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//  documentation files (the "Software"), to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
//  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
//  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#ifndef __RBULLET_RIGIDBODY_H__
#define __RBULLET_RIGIDBODY_H__

#include <RNEntity.h>
#include <btBulletDynamicsCommon.h>

#include "RBShape.h"
#include "RBPhysicsMaterial.h"

namespace RN
{
	namespace bullet
	{
		class RigidBody : public Entity, public btMotionState
		{
		public:
			RigidBody(Shape *shape, float mass);
			RigidBody(Shape *shape, float mass, const Vector3& inertia);
			RigidBody(Shape *shape, PhysicsMaterial *material, float mass);
			RigidBody(Shape *shape, PhysicsMaterial *material, float mass, const Vector3& inertia);
			virtual ~RigidBody();
			
			void SetMass(float mass);
			void SetMass(float mass, const Vector3& inertia);
			
			void SetShape(Shape *shape);
			void SetMaterial(PhysicsMaterial *material);
			
			virtual void SetPosition(const Vector3& position);
			virtual void SetRotation(const Quaternion& rotation);
			virtual void SetWorldPosition(const Vector3& position);
			virtual void SetWorldRotation(const Quaternion& rotation);
			
			Shape *Shape() const { return _shape; }
			PhysicsMaterial *Material() const { return _material; }
			
			void SetLinearVelocity(const Vector3& velocity);
			void SetAngularVelocity(const Vector3& velocity);
			
			void ApplyForce(const Vector3& force);
			void ApplyForce(const Vector3& force, const Vector3& origin);
			void ClearForces();
			
			void ApplyTorque(const Vector3& torque);
			void ApplyTorqueImpulse(const Vector3& torque);
			
			void ApplyImpulse(const Vector3& impulse);
			void ApplyImpulse(const Vector3& impulse, const Vector3& origin);
			
			btRigidBody *bulletRigidBody() const { return _rigidBody; }
			
		protected:
			virtual void getWorldTransform(btTransform& worldTrans);
			virtual void setWorldTransform(const btTransform& worldTrans);
			
			virtual void CreateRigidBody(float mass, const Vector3& inertia);
			void ApplyMaterial();
			
			class Shape *_shape;
			PhysicsMaterial *_material;
			
			btRigidBody *_rigidBody;
			
			RNDefineConstructorlessMeta(RigidBody, Entity);
		};
	}
}

#endif /* __RBULLET_RIGIDBODY_H__ */
