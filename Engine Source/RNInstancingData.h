//
//  RNInstancingData.h
//  Rayne
//
//  Copyright 2014 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_INSTANCINGDATA_H__
#define __RAYNE_INSTANCINGDATA_H__

#include "RNBase.h"
#include "RNObject.h"
#include "RNEntity.h"
#include "RNModel.h"
#include "RNRenderer.h"
#include "RNSpatialMap.h"
#include "RNIndexSet.h"
#include "RNRandom.h"

namespace RN
{
	class InstancingLODStage
	{
	public:
		RNAPI InstancingLODStage(Model *model, size_t stage);
		RNAPI ~InstancingLODStage();
		
		RNAPI void RemoveIndex(size_t index);
		RNAPI void AddIndex(size_t index);
		
		RNAPI void UpdateData(bool dynamic);
		RNAPI void Render(RenderingObject &object, Renderer *renderer);
		RNAPI void Clear();
		
		bool IsEmpty() const { return _indices.empty(); }
		
	private:
		Model *_model;
		size_t _stage;
		
		bool _dirty;
		
		GLuint _texture;
		GLuint _buffer;
		
		std::unordered_set<uint32> _indices;
		
		uint32 *_indicesData;
		size_t _indicesSize;
	};
	
	struct __InstancingBucket;
	typedef std::shared_ptr<__InstancingBucket> InstancingBucket;
	
	class InstancingData
	{
	public:
		RNAPI InstancingData(Model *model);
		RNAPI ~InstancingData();
		
		RNAPI void Reserve(size_t capacity);
		RNAPI void PivotMoved();
		RNAPI void SetPivot(Camera *pivot);
		RNAPI void SetClipping(bool clipping, float distance);
		RNAPI void SetCellSize(float cellSize);
		RNAPI void SetThinningRange(bool thinning, float thinRange);
		
		RNAPI void UpdateData();
		RNAPI void Render(SceneNode *node, Renderer *renderer);
		
		RNAPI void InsertEntity(Entity *entity);
		RNAPI void RemoveEntity(Entity *entity);
		RNAPI void UpdateEntity(Entity *entity);
		
		Model *GetModel() const { return _model; }
		
	private:
		void InsertEntityIntoLODStage(Entity *entity);
		void UpdateEntityLODStage(Entity *entity, const Vector3 &position);
		
		void ResignBucket(InstancingBucket &bucket);
		void ClipEntities();
		
		size_t GetIndex(Entity *entity);
		void ResignIndex(size_t index);
		
		Random::MersenneTwister _random;
		
		Model *_model;
		Camera *_pivot;
		bool _hasLODStages;
		
		GLuint _texture;
		GLuint _buffer;
		
		size_t _capacity;
		size_t _count;
		
		bool  _clipping;
		float _clipRange;
		
		SpinLock _lock;
		
		bool _dirty;
		bool _dirtyIndices;
		bool _needsClipping;
		bool _pivotMoved;
		bool _needsRecreation;
		
		float _thinRange;
		bool _thinning;
		
		std::vector<size_t> _freeList;
		std::vector<Matrix> _matrices;
		std::vector<InstancingLODStage *> _stages;
		
		std::vector<Entity *> _entities;
		std::unordered_set<Entity *> _activeEntites;
		std::list<InstancingBucket> _activeBuckets;
		stl::spatial_map<InstancingBucket> _buckets;
	};
}

#endif /* __RAYNE_INSTANCINGDATA_H__ */
