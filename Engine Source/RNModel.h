//
//  RNModel.h
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_MODEL_H__
#define __RAYNE_MODEL_H__

#include "RNBase.h"
#include "RNObject.h"
#include "RNMesh.h"
#include "RNMaterial.h"
#include "RNArray.h"

namespace RN
{
	class Skeleton;
	class Model : public Object
	{
	public:
		Model();
		Model(const std::string& path);
		Model(Mesh *mesh, Material *material, const std::string& name="Unnamed");
		
		virtual ~Model();
		
		static Model *WithFile(const std::string& path);
		static Model *WithMesh(Mesh *mesh, Material *material, const std::string& name="Unnamed");
		static Model *Empty();
		static Model *WithSkyCube(std::string up, std::string down, std::string left, std::string right, std::string front, std::string back, std::string shader="shader/rn_Sky");
		
		void AddMesh(Mesh *mesh, Material *material, const std::string& name="Unnamed");
		
		uint32 Meshes() const;
		uint32 Materials() const;
		
		Mesh *MeshAtIndex(uint32 index) const;
		Material *MaterialForMesh(const Mesh *mesh) const;
		
	private:
		struct MeshGroup
		{
			std::string name;
			Mesh *mesh;
			Material *material;
		};
		
		void ReadModelVersion1(File *file);
		
		Array<Material> _materials;
		std::vector<MeshGroup> _groups;
		
		RNDefineMeta(Model, Object)
	};
}

#endif /* __RAYNE_MODEL_H__ */
