//
//  RNOctree.h
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_OCTREE_H__
#define __RAYNE_OCTREE_H__

#include "RNBase.h"
#include "RNSceneNode.h"
#include "RNAABB.h"

namespace RN
{
	class Octree : public Object
	{
	public:
		Octree();
		virtual ~Octree();
		
		void AddNode(SceneNode *node);
		void RemoveNode(SceneNode *node);
		
	private:
		class OctreeNode
		{
		public:
			OctreeNode(OctreeNode *parent, const Vector3& min, const Vector3& max) :
				_min(min),
				_max(max)
			{
				_parent = parent;
				
				for(int i=0; i<8; i++)
				{
					_childs[i] = 0;
				}
			}
			
			~OctreeNode()
			{
				for(int i=0; i<8; i++)
				{
					if(_childs[i])
						delete _childs[i];
				}
			}
			
			
			Vector3 _min;
			Vector3 _max;
			float _size;
			
			Array<SceneNode *> _nodes;
			OctreeNode *_childs[8];
			OctreeNode *_parent;
		};
		
		
		OctreeNode *_node;
	};
}

#endif /* __RAYNE_OCTREE_H__ */
