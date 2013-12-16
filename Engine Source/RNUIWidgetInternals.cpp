//
//  RNUIWidgetInternals.cpp
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNUIWidgetInternals.h"
#include "RNUIStyle.h"

namespace RN
{
	namespace UI
	{
		RNDeclareMeta(WidgetBackgroundView)
		
		WidgetBackgroundView::WidgetBackgroundView(Widget *widget, Widget::Style tstyle, Dictionary *style) :
			_container(widget),
			_style(tstyle),
			_closeButton(nullptr),
			_minimizeButton(nullptr),
			_maximizeButton(nullptr)
		{
			Style *styleSheet = Style::GetSharedInstance();
			
			Dictionary *selected   = style->GetObjectForKey<Dictionary>(RNCSTR("selected"));
			Dictionary *deselected = style->GetObjectForKey<Dictionary>(RNCSTR("deselected"));
			Dictionary *border     = style->GetObjectForKey<Dictionary>(RNCSTR("border"));
			
			if(selected)
				ParseStyle(selected, Control::Selected);
			
			if(deselected)
				ParseStyle(deselected, Control::Normal);
			
			if(border)
				_border = Style::ParseEdgeInsets(border);
			
			_backdrop = new ImageView();
			_backdrop->SetAutoresizingMask(AutoresizingFlexibleHeight | AutoresizingFlexibleWidth);
			
			_shadow = new ImageView();
			_shadow->SetAutoresizingMask(AutoresizingFlexibleHeight | AutoresizingFlexibleWidth);
			
			_title = new Label();
			_title->SetAlignment(TextAlignment::Center);
			_title->SetTextColor(styleSheet->GetColor(Style::ColorStyle::TitleColor));
			_title->SetFont(styleSheet->GetFont(Style::FontStyle::DefaultFontBold));
			
			AddSubview(_shadow);
			AddSubview(_backdrop);
			AddSubview(_title);
			
			if(!_style & Widget::StyleTitled)
				_title->SetHidden(true);
			
			SetState(Control::Selected);
			CreateButton(Widget::TitleControl::Close);
			CreateButton(Widget::TitleControl::Minimize);
			CreateButton(Widget::TitleControl::Maximize);
		}
		
		WidgetBackgroundView::~WidgetBackgroundView()
		{
			_title->Release();
			_backdrop->Release();
			_shadow->Release();
			
			SafeRelease(_closeButton);
			SafeRelease(_minimizeButton);
			SafeRelease(_maximizeButton);
		}
		
		void WidgetBackgroundView::LayoutSubviews()
		{
			View::LayoutSubviews();
			
			Rect bounds = GetBounds();
			Rect backdropFrame = bounds;
			
			
			{
				backdropFrame.y      -= _border.top;
				backdropFrame.height += _border.top + _border.bottom;
				
				backdropFrame.x      -= _border.left;
				backdropFrame.width  += _border.left + _border.right;
				
				_backdrop->SetFrame(backdropFrame);
			}
			
			if(_shadowExtents.GetValueForState(_state))
			{
				Rect frame = backdropFrame;
				EdgeInsets insets = Style::ParseEdgeInsets(_shadowExtents.GetValueForState(_state));
				
				frame.y      -= insets.top;
				frame.height += insets.top + insets.bottom;
				
				frame.x      -= insets.left;
				frame.width  += insets.left + insets.right;
				
				_shadow->SetFrame(frame);
			}
			
			// Size the controls
			float offsetX = 5.0f;
			
			_controlButtons.Enumerate<Button>([&](Button *button, size_t index, bool *stop) {
				
				Rect frame = button->GetFrame();
				
				frame.x = offsetX;
				frame.y = - roundf((_border.top * 0.5) + (frame.height * 0.5f));
				
				offsetX += frame.width + 2.0f;
				button->SetFrame(frame);
			});
			
			// Size the title
			{
				float spaceLeft = bounds.width - offsetX - 5.0f;
				Vector2 size   = _title->GetSizeThatFits();
				
				bool fitsCentered = (size.x >= spaceLeft);
				float top = roundf((_border.top * 0.6) + (size.y * 0.5f));
				
				Rect rect = _title->GetFrame();
				
				if(!fitsCentered)
				{
					rect.x = offsetX;
					rect.width = bounds.width - rect.x - 5.0f;
					
					_title->SetAlignment(TextAlignment::Left);
				}
				else
				{
					rect.x = 0.0f;
					rect.width = bounds.width;
					
					_title->SetAlignment(TextAlignment::Center);
				}
				
				rect.y = -top;
				rect.height = size.y;
				
				_title->SetFrame(rect);
			}
		}
		
		void WidgetBackgroundView::CreateButton(Widget::TitleControl buttonStyle)
		{
			Style *styleSheet = Style::GetSharedInstance();
			Dictionary *style = nullptr;
			bool enabled = false;
			
			switch(buttonStyle)
			{
				case Widget::TitleControl::Close:
					style = styleSheet->GetWindowControlStyle(RNCSTR("close"));
					enabled = (_style & Widget::StyleClosable);
					break;
				case Widget::TitleControl::Maximize:
					style = styleSheet->GetWindowControlStyle(RNCSTR("maximize"));
					enabled = (_style & Widget::StyleMaximizable);
					break;
				case Widget::TitleControl::Minimize:
					style = styleSheet->GetWindowControlStyle(RNCSTR("minimize"));
					enabled = (_style & Widget::StyleMinimizable);
					break;
			}
			
			if(!style)
				return;
			
			Button *button = new Button(style);
			button->SetImagePosition(ImagePosition::ImageOnly);
			button->SizeToFit();
			button->SetEnabled(enabled);
			
			AddSubview(button);
			_controlButtons.AddObject(button);
			
			switch(buttonStyle)
			{
				case Widget::TitleControl::Close:
					_closeButton = button;
					_closeButton->AddListener(Control::EventType::MouseUpInside, [&]( Control *control, Control::EventType event) {
						_container->Close();
					}, nullptr);
					break;
				case Widget::TitleControl::Maximize:
					_maximizeButton = button;
					break;
				case Widget::TitleControl::Minimize:
					_minimizeButton = button;
					break;
			}
		}
		
		void WidgetBackgroundView::SetState(Control::State state)
		{
			_backdrop->SetImage(_backdropImages.GetValueForState(state));
			_shadow->SetImage(_shadowImages.GetValueForState(state));
			
			_state = state;
			SetNeedsLayoutUpdate();
		}
		
		void WidgetBackgroundView::SetTitle(String *title)
		{
			_title->SetText(title);
			SetNeedsLayoutUpdate();
		}
		
		
		void WidgetBackgroundView::ParseStyle(Dictionary *style, Control::State state)
		{
			Style *styleSheet = Style::GetSharedInstance();
			Texture *texture = styleSheet->GetTextureWithName(style->GetObjectForKey<String>(RNCSTR("texture")));
			
			Dictionary *atlas  = style->GetObjectForKey<Dictionary>(RNCSTR("atlas"));
			Dictionary *insets = style->GetObjectForKey<Dictionary>(RNCSTR("insets"));
			Dictionary *shadow = style->GetObjectForKey<Dictionary>(RNCSTR("shadow"));
			
			Image *image = new Image(texture);
			image->SetAtlas(Style::ParseAtlas(atlas), false);
			image->SetEdgeInsets(Style::ParseEdgeInsets(insets));
			
			_backdropImages.SetValueForState(image, state);
			image->Release();
			
			if(shadow)
			{
				atlas  = shadow->GetObjectForKey<Dictionary>(RNCSTR("atlas"));
				insets = shadow->GetObjectForKey<Dictionary>(RNCSTR("insets"));
				
				Image *image = new Image(texture);
				image->SetAtlas(Style::ParseAtlas(atlas), false);
				image->SetEdgeInsets(Style::ParseEdgeInsets(insets));
				
				_shadowImages.SetValueForState(image, state);
				image->Release();
				
				Dictionary *extents = shadow->GetObjectForKey<Dictionary>(RNCSTR("extents"));
				if(extents)
					_shadowExtents.SetValueForState(extents, state);
			}
		}
	}
}
