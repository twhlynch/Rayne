//
//  RNUIServer.cpp
//  Rayne
//
//  Copyright 2014 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNBaseInternal.h"
#include "RNUIServer.h"
#include "RNKernel.h"
#include "RNWorld.h"
#include "RNWindow.h"
#include "RNUILabel.h"
#include "RNUIButton.h"

#if RN_PLATFORM_MAC_OS

static const char *__RNUIMenuItemWrapperKey = "__RNUIMenuItemWrapperKey";

@interface __RNUIMenuItemWrapper : NSObject
@property (nonatomic, assign) RN::UI::MenuItem *item;
+ (__RNUIMenuItemWrapper *)wrapperWithItem:(RN::UI::MenuItem *)item;
@end

@implementation __RNUIMenuItemWrapper
@synthesize item;

- (void)setItem:(RN::UI::MenuItem *)titem
{
	if(item)
	{
		item->RemoveAssociatedOject(__RNUIMenuItemWrapperKey);
		item->Release();
	}
	
	item = titem;
	
	if(item)
	{
		item->SetAssociatedObject(__RNUIMenuItemWrapperKey, (RN::Object *)self, RN::Object::MemoryPolicy::Assign);
		item->Retain();
	}
}

- (void)dealloc
{
	[self setItem:nil];
	[super dealloc];
}

+ (__RNUIMenuItemWrapper *)wrapperWithItem:(RN::UI::MenuItem *)item
{
	__RNUIMenuItemWrapper *wrapper = [[__RNUIMenuItemWrapper alloc] init];
	[wrapper setItem:item];
	return [wrapper autorelease];
}
@end

#endif /* RN_PLATFORM_MAC_OS */

namespace RN
{
	namespace UI
	{
		RNDefineSingleton(Server)
		
		Server::Server()
		{
			uint32 flags = Camera::Flags::Orthogonal | Camera::Flags::UpdateAspect | Camera::Flags::UpdateStorageFrame | Camera::Flags::NoSorting | Camera::Flags::NoDepthWrite | Camera::Flags::BlendedBlitting;
			_camera = new Camera(Vector2(0.0f), Texture::Format::RGBA8888, flags, RenderStorage::BufferFormatColor);
			_camera->SetClearColor(RN::Color(0.0f, 0.0f, 0.0f, 0.0f));
			_camera->SetFlags(_camera->GetFlags() | Camera::Flags::Orthogonal);
			_camera->SetClipNear(-500.0f);
			_camera->SetLightManager(nullptr);
			
			if(_camera->GetWorld())
				_camera->GetWorld()->RemoveSceneNode(_camera);
			
			_mainWidget = nullptr;
			_tracking = nullptr;
			_mode = Mode::SingleTracking;
			
			_drawDebugFrames = false;
			_menu = nullptr;
			
			TranslateMenuToPlatform();
		}
		
		Server::~Server()
		{
			_camera->Release();
			SafeRelease(_menu);
		}
		
		void Server::SetDrawDebugFrames(bool drawDebugFrames)
		{
			_drawDebugFrames = drawDebugFrames;
		}
		
		void Server::SetMainMenu(Menu *menu)
		{
			SafeRelease(_menu);
			_menu = SafeRetain(menu);
			
			TranslateMenuToPlatform();
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
			
			if(widget == _mainWidget)
			{
				_mainWidget = nullptr;
				
				if(_tracking && _tracking->_widget == widget)
					_tracking = nullptr;
			}
			
			_widgets.erase(std::remove(_widgets.begin(), _widgets.end(), widget), _widgets.end());
			widget->_server = nullptr;
			widget->Autorelease();
		}
		
		void Server::MoveWidgetToFront(Widget *widget)
		{
			RN_ASSERT(widget->_server == this, "");
			
			_widgets.erase(std::remove(_widgets.begin(), _widgets.end(), widget), _widgets.end());
			_widgets.push_front(widget);
		}
		
		
		bool Server::ConsumeEvent(Event *event)
		{
			if(event->IsMouse())
			{
				if(_tracking)
				{
					switch(event->GetType())
					{
						case Event::Type::MouseMoved:
							_tracking->MouseDragged(event);
							return true;
							
						case Event::Type::MouseDragged:
							_tracking->MouseDragged(event);
							return true;
							
						case Event::Type::MouseUp:
							_tracking->MouseUp(event);
							_tracking = nullptr;
							return true;
							
						default:
							break;
					}
				}
				
				const Vector2& position = event->GetMousePosition();
				
				Widget *hitWidget = nullptr;
				View *hit = nullptr;
				
				for(auto i = _widgets.rbegin(); i != _widgets.rend(); i ++)
				{
					Widget *widget = *i;
				
					hit = widget->PerformHitTest(position, event);
					if(hit)
					{
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
							_tracking   = hit;
							return true;
							
						case Event::Type::MouseDragged:
							hit->MouseDragged(event);
							_tracking   = hit;
							return true;
							
						case Event::Type::MouseUp:
							hit->MouseUp(event);
							
							_tracking = nullptr;
							return true;
							
						default:
							break;
					}
				}
				
				return false;
			}
			
			
			Responder *responder = _mainWidget ? _mainWidget->GetFirstResponder() : nullptr;
			
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
		
		void Server::UpdateSize()
		{
			Rect actualFrame = Window::GetSharedInstance()->GetFrame();
			if(_frame != actualFrame)
			{
				_frame = actualFrame;
				
				_camera->SetOrthogonalFrustum(_frame.GetBottom(), _frame.GetTop(), _frame.GetLeft(), _frame.GetRight());
				_camera->SetFrame(_frame);
				
				MessageCenter::GetSharedInstance()->PostMessage(kRNUIServerDidResizeMessage, nullptr, nullptr);
			}
		}
		
		void Server::Render(Renderer *renderer)
		{
			UpdateSize();
			
			// Draw all widgets into the camera
			Kernel::GetSharedInstance()->PushStatistics("ui.update");
			
			for(Widget *widget : _widgets)
			{
				widget->Update();
			}
			
			Kernel::GetSharedInstance()->PopStatistics();
			
			
			// Draw all widgets into the camera
			Kernel::GetSharedInstance()->PushStatistics("ui.render");
			
			renderer->SetMode(Renderer::Mode::ModeUI);
			renderer->BeginCamera(_camera);
			
			for(Widget *widget : _widgets)
			{
				widget->Render(renderer);
			}
			
			renderer->FinishCamera();
			
			Kernel::GetSharedInstance()->PopStatistics();
		}
		
		
#if RN_PLATFORM_MAC_OS
		
		NSMenu *TranslateRNUIMenuToNSMenu(Menu *menu)
		{
			NSMenu *temp = [[NSMenu alloc] init];
			[temp setAutoenablesItems:NO];
			
			if(!menu)
				return [temp autorelease];
			
			menu->GetItems()->Enumerate<MenuItem>([&](MenuItem *item, size_t index, bool &stop) {
				
				if(!item->IsSeparator())
				{
					NSString *title = [NSString stringWithUTF8String:item->GetTitle()->GetUTF8String()];
					NSString *key   = [NSString stringWithUTF8String:item->GetKeyEquivalent()->GetUTF8String()];
					
					NSMenuItem *titem = [[NSMenuItem alloc] initWithTitle:title action:@selector(performMenuBarAction:) keyEquivalent:key];
					[titem setEnabled:item->IsEnabled()];
					
					objc_setAssociatedObject(titem, __RNUIMenuItemWrapperKey, [__RNUIMenuItemWrapper wrapperWithItem:item], OBJC_ASSOCIATION_RETAIN);
					
					if(item->GetSubMenu())
					{
						NSMenu *subMenu = TranslateRNUIMenuToNSMenu(item->GetSubMenu());
						[titem setSubmenu:subMenu];
					}
					
					[temp addItem:[titem autorelease]];
				}
				else
				{
					[temp addItem:[NSMenuItem separatorItem]];
				}
			});
			
			return [temp autorelease];
		}
		
		NSMenuItem *NSQuitMenuItem()
		{
			NSString *quitTitle = [@"Quit " stringByAppendingString:[[NSProcessInfo processInfo] processName]];
			NSMenuItem *quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"];
			
			return [quitMenuItem autorelease];
		}
		
		void Server::TranslateMenuToPlatform()
		{
			@autoreleasepool
			{
				NSMenu *menu = [[NSMenu alloc] init];
				[menu setAutoenablesItems:NO];
				
				NSMenu *quitMenu = [[NSMenu alloc] init];
				[quitMenu addItem:NSQuitMenuItem()];
				
				NSMenuItem *appMenu = [[NSMenuItem alloc] init];
				[appMenu setSubmenu:[quitMenu autorelease]];
				
				[menu addItem:[appMenu autorelease]];
				
				if(_menu)
				{
					_menu->GetItems()->Enumerate<MenuItem>([&](MenuItem *item, size_t index, bool &stop) {
						
						NSMenu *tMenu = TranslateRNUIMenuToNSMenu(item->GetSubMenu());
						[tMenu setTitle:[NSString stringWithUTF8String:item->GetTitle()->GetUTF8String()]];
						
						NSMenuItem *menuItem = [[NSMenuItem alloc] init];
						[menuItem setSubmenu:tMenu];
						
						[menu addItem:[menuItem autorelease]];
						
					});
				}
				
				[NSApp setMainMenu:[menu autorelease]];
			}
		}
		
		void Server::PerformMenuBarAction(void *titem)
		{
			NSMenuItem *nsitem = (NSMenuItem *)titem;
			MenuItem *item = [(__RNUIMenuItemWrapper *)objc_getAssociatedObject(nsitem, __RNUIMenuItemWrapperKey) item];
			
			if(item && item->GetCallback())
			{
				item->GetCallback()(item);
			}
		}
		
#endif /* RN_PLATFORM_MAC_OS */

#if RN_PLATFORM_WINDOWS
		void Server::TranslateMenuToPlatform()
		{
		}
#endif
	}
}
