//
//  RNUIServer.cpp
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNUIServer.h"
#include "RNKernel.h"
#include "RNWorld.h"
#include "RNWindow.h"
#include "RNUILabel.h"
#include "RNUIButton.h"

namespace RN
{
	namespace UI
	{
		Server::Server()
		{
			uint32 flags = Camera::FlagOrthogonal | Camera::FlagUpdateAspect | Camera::FlagUpdateStorageFrame | Camera::FlagNoSorting;
			_camera = new Camera(Vector2(0.0f), TextureParameter::Format::RGBA8888, flags, RenderStorage::BufferFormatColor);
			_camera->SetClearColor(RN::Color(0.0f, 0.0f, 0.0f, 0.0f));
			_camera->SetAllowsDepthWrite(false);
			_camera->SetUseBlending(true);
			
			_camera->clipnear = -500.0f;
			
			if(_camera->GetWorld())
				_camera->GetWorld()->RemoveSceneNode(_camera);
			
			_mainWidget = nullptr;
			_mode = Mode::SingleTracking;
			
			_drawDebugFrames = false;
		}
		
		Server::~Server()
		{
			_camera->Release();
		}
		
		void Server::SetDrawDebugFrames(bool drawDebugFrames)
		{
			_drawDebugFrames = drawDebugFrames;
		}
		
		void Server::AddWidget(Widget *widget)
		{
			RN_ASSERT(widget->_server == nullptr, "");
			
			_widgets.push_front(widget);
			widget->_server = this;
			widget->Retain();
		}
		
		void Server::RemoveWidget(Widget *widget)
		{
			RN_ASSERT(widget->_server == this, "");
			
			_widgets.erase(std::remove(_widgets.begin(), _widgets.end(), widget), _widgets.end());
			widget->_server = nullptr;
			widget->Release();
		}
		
		void Server::MoveWidgetToFront(Widget *widget)
		{
			RN_ASSERT(widget->_server == this, "");
			
			_widgets.erase(std::remove(_widgets.begin(), _widgets.end(), widget), _widgets.end());
			_widgets.push_front(widget);
		}
		
		
		bool Server::ConsumeEvent(Event *event)
		{
			Responder *responder = _mainWidget ? _mainWidget->GetFirstResponder() : nullptr;
			
			if(event->IsMouse())
			{
				if(responder && event->GetType() == Event::Type::MouseMoved)
				{
					responder->MouseMoved(event);
					return true;
				}
				
				const Vector2& position = event->GetMousePosition();
				
				Widget *hitWidget = nullptr;
				View *hit = nullptr;
				
				for(auto i = _widgets.rbegin(); i != _widgets.rend(); i ++)
				{
					Widget *widget = *i;
					
					if(widget->GetFrame().ContainsPoint(position))
					{
						Vector2 transformed = position;
						transformed.x -= widget->_frame.x;
						transformed.y -= widget->_frame.y;
						
						hit = widget->GetContentView()->HitTest(transformed, event);
						hitWidget = widget;
						break;
					}
				}
				
				if(hit)
				{
					switch(event->GetType())
					{
						case Event::Type::MouseWheel:
							hit->ScrollWheel(event);
							return true;
							
						case Event::Type::MouseDown:
							hit->MouseDown(event);
							_mainWidget = hitWidget;
							return true;
							
						case Event::Type::MouseUp:
							hit->MouseUp(event);
							return true;
							
						default:
							break;
					}
				}
			}
			
			if(responder && event->IsKeyboard())
			{
				switch(event->GetType())
				{
					case Event::Type::KeyDown:
						responder->KeyDown(event);
						break;
						
					case Event::Type::KeyUp:
						responder->KeyUp(event);
						break;
						
					case Event::Type::KeyRepeat:
						responder->KeyRepeat(event);
						break;
						
					default:
						break;
				}
				
				return true;
			}
			
			return false;
		}
		
		void Server::Render(Renderer *renderer)
		{
			Rect actualFrame = Window::GetSharedInstance()->GetFrame();
			if(_frame != actualFrame)
			{
				_frame = actualFrame;
					
				_camera->ortholeft   = _frame.GetLeft();
				_camera->orthobottom = _frame.GetTop();
				_camera->orthoright  = _frame.GetRight();
				_camera->orthotop    = _frame.GetBottom();
				
				_camera->SetFrame(_frame);
				_camera->PostUpdate();
				
				MessageCenter::GetSharedInstance()->PostMessage(kRNUIServerDidResizeMessage, nullptr, nullptr);
			}
			
			// Draw all widgets into the camera
			renderer->SetMode(Renderer::Mode::ModeUI);
			renderer->BeginCamera(_camera);
			
			for(Widget *widget : _widgets)
			{
				widget->Render(renderer);
			}
			
			renderer->FinishCamera();
		}
	}
}
