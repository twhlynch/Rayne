//
//  RNUIImageView.cpp
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNUIImageView.h"
#include "RNResourcePool.h"

#define kRNImageViewShaderResourceName "kRNImageViewShaderResourceName"

namespace RN
{
	namespace UI
	{
		RNDeclareMeta(ImageView)
		
		ImageView::ImageView()
		{
			Initialize();
		}
		
		ImageView::ImageView(const Rect& frame) :
			View(frame)
		{
			Initialize();
		}
		
		ImageView::ImageView(Image *image)
		{
			Initialize();
			_isDirty = true;
			
			if(image)
			{
				Rect frame = Frame();
				frame.width  = image->Width();
				frame.height = image->Height();
				
				SetFrame(frame);
				SetImage(image);
			}
		}
		
		ImageView::~ImageView()
		{
			if(_mesh)
				_mesh->Release();
			
			if(_image)
				_image->Release();
		}
		
		
		void ImageView::Initialize()
		{
			Shader *shader = ResourcePool::SharedInstance()->ResourceWithName<Shader>(kRNResourceKeyUIImageShader);
			DrawMaterial()->SetShader(shader);
			
			_mesh  = nullptr;
			_image = 0;
		}

		
		void ImageView::SetFrame(const Rect& frame)
		{
			View::SetFrame(frame);
			_isDirty = true;
		}
		
		void ImageView::SetImage(Image *image)
		{
			DrawMaterial()->RemoveTextures();
			
			if(_image)
			{
				_image->Release();
				_image = nullptr;
			}

			if(image)
			{
				DrawMaterial()->AddTexture(image->Texture());
				_image = image->Retain();
			}
			
			_isDirty = true;
		}
		
		void ImageView::Update()
		{
			View::Update();
			
			if(_isDirty)
			{
				if(_mesh)
				{
					_mesh->Release();
					_mesh = nullptr;
				}
				
				if(_image)
				{
					DrawMaterial()->AddTexture(_image->Texture());
					_mesh = _image->FittingMesh(Frame().Size())->Retain();
				}
				
				_isDirty = false;
			}
		}
		
		bool ImageView::Render(RenderingObject& object)
		{
			object.mesh = _mesh;
			return (_image != 0);
		}
	}
}
