//
//  RNLight.cpp
//  Rayne
//
//  Copyright 2014 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNLight.h"
#include "RNWorld.h"
#include "RNCamera.h"
#include "RNResourceCoordinator.h"
#include "RNLightManager.h"
#include "RNCameraInternal.h"
#include "RNOpenGLQueue.h"

namespace RN
{
	RNDefineMeta(Light, SceneNode)
	
	Light::Light(Type lighttype) :
		_lightType(lighttype),
		_color("color", Color(1.0f), &Light::GetColor, &Light::SetColor),
		_intensity("intensity", 10.0f, &Light::GetIntensity, &Light::SetIntensity),
		_range("range", 10.0f, &Light::GetRange, &Light::SetRange),
		_angle("angle", 45.0f, &Light::GetAngle, &Light::SetAngle),
		_angleCos(0.797),
		_shadowTarget(nullptr),
		_suppressShadows(false),
		_dirty(false)
	{
		AddObservables({ &_color, &_intensity, &_range, &_angle });
		
		SetBoundingSphere(Sphere(Vector3(), 1.0f));
		SetBoundingBox(AABB(Vector3(), 1.0f), false);
		
		SetPriority(SceneNode::Priority::UpdateLate);
		SetCollisionGroup(25);
		
		ReCalculateColor();
		SetAngle(GetAngle());
		SetRange(GetRange());
	}
	
	Light::Light(const Light *other) :
		SceneNode(other),
		_lightType(other->_lightType),
		_color("color", Color(1.0f), &Light::GetColor, &Light::SetColor),
		_intensity("intensity", 10.0f, &Light::GetIntensity, &Light::SetIntensity),
		_range("range", 10.0f, &Light::GetRange, &Light::SetRange),
		_angle("angle", 45.0f, &Light::GetAngle, &Light::SetAngle),
		_angleCos(0.797),
		_shadowTarget(nullptr),
		_suppressShadows(false),
		_dirty(false)
	{
		AddObservables({ &_color, &_intensity, &_range, &_angle });
		
		SetBoundingSphere(Sphere(Vector3(), 1.0f));
		SetBoundingBox(AABB(Vector3(), 1.0f), false);
		
		Light *temp = const_cast<Light *>(other);
		LockGuard<Object *> lock(temp);
		
		SetColor(other->GetColor());
		SetIntensity(other->GetIntensity());
		SetAngle(other->GetAngle());
		SetRange(other->GetRange());
		
		_suppressShadows = other->_suppressShadows;
		
		if(other->HasShadows())
			ActivateShadows(other->_shadowParameter);
	}
	
	Light::~Light()
	{
		RemoveShadowCameras();
	}
	
	
	Light::Light(Deserializer *deserializer) :
		SceneNode(deserializer),
		_color("color", Color(1.0f), &Light::GetColor, &Light::SetColor),
		_intensity("intensity", 10.0f, &Light::GetIntensity, &Light::SetIntensity),
		_range("range", 10.0f, &Light::GetRange, &Light::SetRange),
		_angle("angle", 45.0f, &Light::GetAngle, &Light::SetAngle),
		_angleCos(0.797),
		_shadowTarget(nullptr),
		_suppressShadows(false),
		_dirty(false)
	{
		AddObservables({ &_color, &_intensity, &_range, &_angle });
		
		SetBoundingSphere(Sphere(Vector3(), 1.0f));
		SetBoundingBox(AABB(Vector3(), 1.0f), false);
		
		_lightType = static_cast<Type>(deserializer->DecodeInt32());
		
		Vector4 color = deserializer->DecodeVector4();
		
		SetColor(Color(color.x, color.y, color.z, color.w));
		SetIntensity(deserializer->DecodeFloat());
		SetRange(deserializer->DecodeFloat());
		SetAngle(deserializer->DecodeFloat());
		
		bool shadows = deserializer->DecodeBool();
		if(shadows)
		{
			ShadowParameter parameter;
			
			parameter.splits.clear();
			
			parameter.shadowTarget = static_cast<Camera *>(deserializer->DecodeObject());
			parameter.resolution = deserializer->DecodeInt32();
			parameter.distanceBlendFactor = deserializer->DecodeFloat();
			
			size_t splitCount = deserializer->DecodeInt32();
			for(size_t i = 0; i < splitCount; i ++)
			{
				ShadowSplit split;
				
				split.biasFactor = deserializer->DecodeFloat();
				split.biasUnits  = deserializer->DecodeFloat();
				split.updateInterval = deserializer->DecodeInt32();
				split.updateOffset   = deserializer->DecodeInt32();
				
				parameter.splits.push_back(std::move(split));
			}
			
			if(parameter.shadowTarget)
				ActivateShadows(parameter);
			else
				_shadowParameter = parameter;
		}
	}
	
	void Light::Serialize(Serializer *serializer)
	{
		SceneNode::Serialize(serializer);
		
		serializer->EncodeInt32(static_cast<int32>(_lightType));
		serializer->EncodeVector4(Vector4(_color->r, _color->g, _color->b, _color->a));
		serializer->EncodeFloat(_intensity);
		serializer->EncodeFloat(_range);
		serializer->EncodeFloat(_angle);
		
		bool shadows = HasShadows();
		serializer->EncodeBool(shadows);
		
		if(shadows)
		{
			serializer->EncodeConditionalObject(_shadowTarget);
			
			serializer->EncodeInt32(static_cast<int32>(_shadowParameter.resolution));
			serializer->EncodeFloat(_shadowParameter.distanceBlendFactor);
			serializer->EncodeInt32(static_cast<int32>(_shadowParameter.splits.size()));
			
			for(ShadowSplit &split : _shadowParameter.splits)
			{
				serializer->EncodeFloat(split.biasFactor);
				serializer->EncodeFloat(split.biasUnits);
				serializer->EncodeInt32(static_cast<int32>(split.updateInterval));
				serializer->EncodeInt32(static_cast<int32>(split.updateOffset));
			}
		}
	}
	
	
	bool Light::IsVisibleInCamera(Camera *camera)
	{
		if(_lightType == Type::DirectionalLight)
			return true;
		
		return SceneNode::IsVisibleInCamera(camera);
	}
	
	void Light::Render(Renderer *renderer, Camera *camera)
	{
		SceneNode::Render(renderer, camera);
		
		LightManager *manager = camera->GetLightManager();
		
		if(manager)
			manager->AddLight(this);
	}
	
	void Light::SetType(Type type)
	{
		_lightType = type;
		
		if(_suppressShadows || _shadowDepthCameras.GetCount() == 0)
			return;
		
		RemoveShadowCameras();
		ActivateShadows();
	}
	
	void Light::SetRange(float range)
	{
		_range = range;
		SetWorldScale(RN::Vector3(range));
	}
	
	void Light::SetColor(const Color& color)
	{
		_color = color;
		ReCalculateColor();
	}
	
	void Light::SetIntensity(float intensity)
	{
		_intensity = intensity;
		ReCalculateColor();
	}
	
	void Light::SetAngle(float angle)
	{
		_angle = angle;
		_angleCos = cosf(angle*k::DegToRad);
	}
	
	void Light::RemoveShadowCameras()
	{
		_shadowDepthCameras.Enumerate<Camera>([&](Camera *camera, size_t index, bool &stop) {
			RemoveDependency(camera);
		});
		
		_shadowDepthCameras.RemoveAllObjects();
		
		if(_shadowTarget)
		{
			RemoveDependency(_shadowTarget);
			_shadowTarget->Release();
			_shadowTarget = nullptr;
		}
	}
	
	bool Light::ActivateShadows(const ShadowParameter &parameter)
	{
		_shadowParameter = parameter;
		
		switch(_lightType)
		{
			case Type::PointLight:
				return ActivatePointShadows();
				
			case Type::SpotLight:
				return ActivateSpotShadows();
				
			case Type::DirectionalLight:
				return ActivateDirectionalShadows();

			default:
				return false;
		}
	}
	
	void Light::DeactivateShadows()
	{
		RemoveShadowCameras();
	}
	
	void Light::SetSuppressShadows(bool suppress)
	{
		_suppressShadows = suppress;
		
		if(_suppressShadows)
		{
			_shadowDepthCameras.Enumerate<Camera>([&](Camera *camera, size_t index, bool &stop) {
				camera->SetFlags(camera->GetFlags()|Camera::Flags::NoRender);
			});
		}
		else
		{
			_shadowDepthCameras.Enumerate<Camera>([&](Camera *camera, size_t index, bool &stop) {
				camera->SetFlags(camera->GetFlags()&~Camera::Flags::NoRender);
			});
		}
	}
	
	void Light::UpdateShadowParameters(const ShadowParameter &parameter)
	{
		if(!HasShadows())
		{
			ActivateShadows(parameter);
			return;
		}
		
		
		if(_shadowParameter.resolution != parameter.resolution || _shadowParameter.splits.size() != parameter.splits.size())
		{
			DeactivateShadows();
			ActivateShadows(parameter);
		}
		else
		{
			if(parameter.shadowTarget != _shadowTarget)
			{
				if(_shadowTarget)
				{
					RemoveDependency(_shadowTarget);
					_shadowTarget->Release();
					_shadowTarget = nullptr;
				}
				
				if(parameter.shadowTarget)
				{
					_shadowTarget = parameter.shadowTarget->Retain();
					AddDependency(_shadowTarget);
				}
			}
			
			_shadowDepthCameras.Enumerate<Camera>([&](Camera *camera, size_t index, bool &stop) {
				camera->GetMaterial()->SetPolygonOffsetFactor(parameter.splits[index].biasFactor);
				camera->GetMaterial()->SetPolygonOffsetUnits(parameter.splits[index].biasUnits);
			});
			
			_shadowParameter = parameter;
		}
	}
	
	
	bool Light::ActivateDirectionalShadows()
	{
		if(_shadowDepthCameras.GetCount() > 0)
			DeactivateShadows();
		
		RN_ASSERT(_shadowParameter.shadowTarget, "Directional shadows need the shadowTarget to be set to a valid value!");
		RN_ASSERT(_shadowParameter.splits.size() > 0, "The shadow parameter for directional lights needs one or more splits!");
		
		if(_shadowTarget)
		{
			RemoveDependency(_shadowTarget);
			_shadowTarget->Release();
			_shadowTarget = nullptr;
		}
		
		_shadowTarget = _shadowParameter.shadowTarget->Retain();
		AddDependency(_shadowTarget);
		
		Texture::Parameter textureParameter;
		textureParameter.wrapMode = Texture::WrapMode::Clamp;
		textureParameter.filter = Texture::Filter::Linear;
		textureParameter.format = Texture::Format::Depth24I;
		textureParameter.depthCompare = true;
		textureParameter.maxMipMaps = 0;
		
		Texture2DArray *depthtex = new Texture2DArray(textureParameter);
		depthtex->SetSize(_shadowParameter.resolution, _shadowParameter.resolution, _shadowParameter.splits.size());
		depthtex->Autorelease();
		
		Shader *depthShader = ResourceCoordinator::GetSharedInstance()->GetResourceWithName<Shader>(kRNResourceKeyDirectionalShadowDepthShader, nullptr);
		
		_shadowCameraMatrices.clear();
		
		for(uint32 i = 0; i < _shadowParameter.splits.size(); i++)
		{
			_shadowCameraMatrices.push_back(Matrix());
			
			Material *depthMaterial = new Material(depthShader);
			depthMaterial->SetPolygonOffset(true);
			depthMaterial->SetPolygonOffsetFactor(_shadowParameter.splits[i].biasFactor);
			depthMaterial->SetPolygonOffsetUnits(_shadowParameter.splits[i].biasUnits);
			depthMaterial->SetOverride(Material::Override::GroupDiscard | Material::Override::Culling);
			
			RenderStorage *storage = new RenderStorage(RenderStorage::BufferFormatDepth, 0, 1.0f);
			storage->SetDepthTarget(depthtex, i);
			storage->Autorelease();
			
			Camera *tempcam = new Camera(Vector2(_shadowParameter.resolution), storage, Camera::Flags::UpdateAspect | Camera::Flags::UpdateStorageFrame | Camera::Flags::Orthogonal | Camera::Flags::NoFlush, 1.0f);
			tempcam->SetMaterial(depthMaterial->Autorelease());
			tempcam->SetLODCamera(_shadowTarget);
			tempcam->SetLightManager(nullptr);
			tempcam->SetPriority(kRNShadowCameraPriority);
			tempcam->SetClipNear(1.0f);
			tempcam->SceneNode::SetFlags(tempcam->SceneNode::GetFlags() | SceneNode::Flags::HideInEditor | SceneNode::Flags::NoSave);
			tempcam->Autorelease();

			_shadowDepthCameras.AddObject(tempcam);
			AddDependency(tempcam);
			
			try
			{
				OpenGLQueue::GetSharedInstance()->SubmitCommand([&] {
					storage->BindAndUpdateBuffer();
				}, true);
			}
			catch(Exception e)
			{
				RemoveShadowCameras();
				return false;
			}
		}
		
		return true;
	}
	
	bool Light::ActivatePointShadows()
	{
		if(_shadowDepthCameras.GetCount() > 0)
			DeactivateShadows();
		
		RN_ASSERT(_shadowParameter.splits.size() == 1, "The shadow parameter for point lights needs exactly one split!");
		
		Texture::Parameter textureParameter;
		textureParameter.wrapMode = Texture::WrapMode::Repeat;
		textureParameter.filter = Texture::Filter::Nearest;
		textureParameter.format = Texture::Format::Depth24I;
		textureParameter.depthCompare = false;
		textureParameter.maxMipMaps = 0;
		
		Texture *depthtex = (new TextureCubeMap(textureParameter))->Autorelease();
		
		Shader   *depthShader = ResourceCoordinator::GetSharedInstance()->GetResourceWithName<Shader>(kRNResourceKeyPointShadowDepthShader, nullptr);
		Material *depthMaterial = (new Material(depthShader))->Autorelease();
		depthMaterial->SetPolygonOffset(true);
		depthMaterial->SetPolygonOffsetFactor(_shadowParameter.splits[0].biasFactor);
		depthMaterial->SetPolygonOffsetUnits(_shadowParameter.splits[0].biasUnits);
		depthMaterial->SetOverride(Material::Override::GroupDiscard | Material::Override::Culling);
		
		RenderStorage *storage = new RenderStorage(RenderStorage::BufferFormatDepth, 0, 1.0f);
		storage->SetDepthTarget(depthtex, -1);
		storage->Autorelease();
		
		RN::Camera *shadowcam = new CubemapCamera(Vector2(_shadowParameter.resolution), storage, Camera::Flags::UpdateAspect | Camera::Flags::UpdateStorageFrame | Camera::Flags::NoFlush, 1.0f);
		shadowcam->SetMaterial(depthMaterial);
		shadowcam->SetPriority(kRNShadowCameraPriority);
		shadowcam->SetClipNear(0.01f);
		shadowcam->SetClipFar(_range);
		shadowcam->SetFOV(90.0f);
		shadowcam->SetLightManager(nullptr);
		shadowcam->SetWorldRotation(Vector3(0.0f, 0.0f, 0.0f));
		shadowcam->SceneNode::SetFlags(shadowcam->SceneNode::GetFlags() | SceneNode::Flags::HideInEditor | SceneNode::Flags::NoSave);
		shadowcam->Autorelease();
		
		_shadowDepthCameras.AddObject(shadowcam);
		AddDependency(shadowcam);
		
		try
		{
			OpenGLQueue::GetSharedInstance()->SubmitCommand([&] {
				storage->BindAndUpdateBuffer();
			}, true);
		}
		catch(Exception e)
		{
			RemoveShadowCameras();
			return false;
		}
		
		return true;
	}
	
	bool Light::ActivateSpotShadows()
	{
		if(_shadowDepthCameras.GetCount() > 0)
			DeactivateShadows();
		
		RN_ASSERT(_shadowParameter.splits.size() == 1, "The shadow parameter for spot lights needs exactly one split!");
		
		Texture::Parameter textureParameter;
		textureParameter.wrapMode = Texture::WrapMode::Clamp;
		textureParameter.filter = Texture::Filter::Linear;
		textureParameter.format = Texture::Format::Depth24I;
		textureParameter.depthCompare = true;
		textureParameter.maxMipMaps = 0;
		
		Texture *depthtex = new Texture2D(textureParameter);
		depthtex->Autorelease();
		
		Shader   *depthShader = ResourceCoordinator::GetSharedInstance()->GetResourceWithName<Shader>(kRNResourceKeyDirectionalShadowDepthShader, nullptr);
		Material *depthMaterial = (new Material(depthShader))->Autorelease();
		depthMaterial->SetPolygonOffset(true);
		depthMaterial->SetPolygonOffsetFactor(_shadowParameter.splits[0].biasFactor);
		depthMaterial->SetPolygonOffsetUnits(_shadowParameter.splits[0].biasUnits);
		depthMaterial->SetOverride(Material::Override::GroupDiscard | Material::Override::Culling);
		
		RenderStorage *storage = new RenderStorage(RenderStorage::BufferFormatDepth, 0, 1.0f);
		storage->SetDepthTarget(depthtex, -1);
		storage->Autorelease();
		
		RN::Camera *shadowcam = new Camera(Vector2(_shadowParameter.resolution), storage, Camera::Flags::UpdateAspect | Camera::Flags::UpdateStorageFrame | Camera::Flags::NoFlush, 1.0f);
		shadowcam->SetMaterial(depthMaterial);
		shadowcam->SetPriority(kRNShadowCameraPriority);
		shadowcam->SetClipNear(0.01f);
		shadowcam->SetClipFar(_range);
		shadowcam->SetFOV(_angle * 2.0f);
		shadowcam->SetLightManager(nullptr);
		shadowcam->SetWorldRotation(Vector3(0.0f, 0.0f, 0.0f));
		shadowcam->SceneNode::SetFlags(shadowcam->SceneNode::GetFlags() | SceneNode::Flags::HideInEditor | SceneNode::Flags::NoSave);
		shadowcam->Autorelease();
		
		_shadowDepthCameras.AddObject(shadowcam);
		AddDependency(shadowcam);
		
		try
		{
			OpenGLQueue::GetSharedInstance()->SubmitCommand([&] {
				storage->BindAndUpdateBuffer();
			}, true);
		}
		catch(Exception e)
		{
			RemoveShadowCameras();
			return false;
		}
		
		return true;
	}
	
	
	void Light::Update(float delta)
	{
		SceneNode::Update(delta);
		if(_dirty)
		{
			SetRange(GetScale().GetMax());
			_dirty = false;
		}
		UpdateShadows();
	}
	
	void Light::UpdateEditMode(float delta)
	{
		SceneNode::UpdateEditMode(delta);
		if(_dirty)
		{
			SetRange(GetScale().GetMax());
			_dirty = false;
		}
		UpdateShadows();
	}
	
	void Light::UpdateShadows()
	{
		if(_suppressShadows || _shadowDepthCameras.GetCount() == 0)
			return;
		
		if(_lightType == Type::DirectionalLight)
		{
			if(_shadowTarget)
			{
				float near = _shadowTarget->GetClipNear();
				float far;
				
				for(uint32 i = 0; i < _shadowParameter.splits.size(); i++)
				{
					if(((GetLastFrame()+_shadowParameter.splits[i].updateOffset) % _shadowParameter.splits[i].updateInterval) == 0)
					{
						Camera *cam = _shadowDepthCameras.GetObjectAtIndex<Camera>(i);
						cam->SetFlags(cam->GetFlags()&(~Camera::Flags::NoRender));
						cam->SetClearMask(Camera::ClearMask::Depth);
					}
					else
					{
						Camera *cam = _shadowDepthCameras.GetObjectAtIndex<Camera>(i);
						cam->SetFlags(cam->GetFlags()|Camera::Flags::NoRender);
						cam->SetClearMask(0);
						continue;
					}
					
					float linear = _shadowTarget->GetClipNear() + (_shadowTarget->GetClipFar() - _shadowTarget->GetClipNear())*(i+1.0f) / float(_shadowParameter.splits.size());
					float log = _shadowTarget->GetClipNear() * powf(_shadowTarget->GetClipFar() / _shadowTarget->GetClipNear(), (i+1.0f) / float(_shadowParameter.splits.size()));
					far = linear*_shadowParameter.distanceBlendFactor+log*(1.0f-_shadowParameter.distanceBlendFactor);
					
					Camera *tempcam = _shadowDepthCameras.GetObjectAtIndex<Camera>(i);
					tempcam->SetWorldRotation(GetWorldRotation());
					
					_shadowCameraMatrices[i] = std::move(tempcam->MakeShadowSplit(_shadowTarget, this, near, far));
					
					near = far;
				}
			}
		}
		else if(_lightType == Type::SpotLight)
		{
			RN::Camera *shadowcam = static_cast<RN::Camera*>(_shadowDepthCameras.GetFirstObject());
			
			shadowcam->SetWorldPosition(GetWorldPosition());
			shadowcam->SetWorldRotation(GetWorldRotation());
			shadowcam->SetClipFar(_range);
			shadowcam->SetFOV(_angle * 2.0f);
			
			_shadowCameraMatrices.clear();
			_shadowCameraMatrices.emplace_back(shadowcam->GetProjectionMatrix() * shadowcam->GetViewMatrix());
		}
		else
		{
			RN::Camera *shadowcam = static_cast<RN::Camera*>(_shadowDepthCameras.GetFirstObject());
			shadowcam->SetWorldPosition(GetWorldPosition());
			shadowcam->SetClipFar(_range);
		}
	}

	void Light::ReCalculateColor()
	{
		_finalColor = Vector3(_color->r, _color->g, _color->b);
		_finalColor *= (float)_intensity;
	}
	
	void Light::DidUpdate(SceneNode::ChangeSet changeSet)
	{
		SceneNode::DidUpdate(changeSet);
		
		if(changeSet & ChangeSet::Position && Math::FastAbs(GetScale().GetMax()-GetRange()) > k::EpsilonFloat)
			_dirty = true;
	}
}
