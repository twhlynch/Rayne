//
//  RNUIColorPicker.cpp
//  Rayne
//
//  Copyright 2014 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNUIColorPicker.h"

namespace RN
{
	namespace UI
	{
		RNDefineMeta(ColorPicker, Control)
		
		ColorPicker::ColorPicker() :
			ColorPicker(Rect())
		{}
		
		ColorPicker::ColorPicker(const Rect &frame) :
			Control(frame)
		{
			_colorWheel = new ColorWheel();
			
			_colorKnob = new View(Rect(0.0f, 0.0f, 4.0, 4.0));
			_colorKnob->SetBackgroundColor(RN::Color::Black());
			
			View *secondKnob = new View(Rect(1.0f, 1.0f, 2.0f, 2.0f));
			secondKnob->SetBackgroundColor(RN::Color::White());
			
			_colorKnob->AddSubview(secondKnob->Autorelease());
			
			AddSubview(_colorWheel);
			AddSubview(_colorKnob);
		}
		
		ColorPicker::~ColorPicker()
		{
			_colorWheel->Release();
			_colorKnob->Release();
		}
		
		
		
		void ColorPicker::LayoutSubviews()
		{
			Control::LayoutSubviews();
			
			Rect frame = GetBounds();
			Rect wheelRect = Rect(frame).Inset(5.0f, 5.0f);
			
			_colorWheel->SetFrame(wheelRect);
			
			
			_colorKnob->SetFrame(Rect(100.0f, 100.0f, 4.0f, 4.0f));
		}
		
		
		Vector3 ColorPicker::ColorFromHSV(float h, float s, float v)
		{
			float hi = h * 3.0 / k::Pi;
			float f  = hi - floorf(hi);
			
			Vector4 components(0.0, s, s * f, s * (1.0 - f));
			components = (Vector4(1.0 - components.x, 1.0 - components.y, 1.0 - components.z, 1.0 - components.w)) * v;
			
			if(hi < -2.0)
			{
				return Vector3(components.x, components.w, components.y);
			}
			else if(hi < -1.0)
			{
				return Vector3(components.z, components.x, components.y);
			}
			else if(hi < 0.0)
			{
				return Vector3(components.y, components.x, components.w);
			}
			else if(hi < 1.0)
			{
				return Vector3(components.y, components.z, components.x);
			}
			else if(hi < 2.0)
			{
				return Vector3(components.w, components.y, components.x);
			}
			else
			{
				return Vector3(components);
			}
		}
		
		Color *ColorPicker::ConvertColorFromWheel(const Vector2 &position, float brightness)
		{
			Vector2 coords = position * 2.0 - 1.0;
			
			float theta = atan2(coords.y, -coords.x);
			float r = coords.GetLength();
			
			Vector3 rgb = ColorFromHSV(theta, r, brightness);
			return Color::WithRNColor(RN::Color(rgb.x, rgb.y, rgb.z, 1.0));
		}
	}
}
