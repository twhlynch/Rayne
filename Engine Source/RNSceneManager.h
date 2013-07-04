//
//  RNSceneManager.h
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_SCENEMANAGER_H__
#define __RAYNE_SCENEMANAGER_H__

#include "RNBase.h"
#include "RNSceneNode.h"
#include "RNCamera.h"
#include "RNEntity.h"
#include "RNRenderer.h"
#include "RNHit.h"

namespace RN
{
	class SceneManager : public Object
	{
	public:
		virtual void AddSceneNode(SceneNode *node) = 0;
		virtual void RemoveSceneNode(SceneNode *node) = 0;
		virtual void UpdateSceneNode(SceneNode *node) = 0;
		
		virtual void RenderScene(Camera *camera) = 0;
		virtual Hit CastRay(const Vector3 &position, const Vector3 &direction, uint32 mask = 0x00ff) = 0;
		
	protected:
		SceneManager();
		virtual ~SceneManager();
		
		Renderer *_renderer;
		
		RNDefineMeta(SceneManager, Object)
	};
	
	class GenericSceneManager : public SceneManager
	{
	public:
		GenericSceneManager();
		virtual ~GenericSceneManager();
		
		virtual void AddSceneNode(SceneNode *node);
		virtual void RemoveSceneNode(SceneNode *node);
		virtual void UpdateSceneNode(SceneNode *node);
		
		virtual void RenderScene(Camera *camera);
		
		virtual Hit CastRay(const Vector3 &position, const Vector3 &direction, uint32 mask = 0xffff);
		
	private:
		void RenderSceneNode(Camera *camera, SceneNode *node);
		
		std::unordered_set<SceneNode *> _nodes;
		MetaClassBase *_entityClass;
		MetaClassBase *_lightClass;
		
		RNDefineMetaWithTraits(GenericSceneManager, SceneManager, MetaClassTraitCronstructable);
	};
}

#endif /* __RAYNE_SCENEMANAGER_H__ */
