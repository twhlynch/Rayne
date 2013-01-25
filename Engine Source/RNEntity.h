//
//  RNEntity.h
//  Rayne
//
//  Copyright 2013 by Felix Pohl, Nils Daumann and Sidney Just. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_ENTITY_H__
#define __RAYNE_ENTITY_H__

#include "RNBase.h"
#include "RNObject.h"
#include "RNRendering.h"
#include "RNTransform.h"
#include "RNMaterial.h"
#include "RNMesh.h"

namespace RN
{
	class Entity : public Object, public Transform
	{
	public:
		Entity();
		Entity(Entity *other);
		
		virtual ~Entity();
		
		virtual void Update(float delta);
		virtual void PostUpdate();
		RenderingIntent Intent();
		
		
		void SetModel(Model *_model);
		
		Model *Model() const { return _model; }
		
	private:
		class Model *_model;
	};
	
	RN_INLINE RenderingIntent Entity::Intent()
	{
		RenderingIntent intent(_model);
		intent.transform = Matrix();
		
		return intent;
	}
}

#endif /* __RAYNE_ENTITY_H__ */
