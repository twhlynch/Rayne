//
//  RNUIServer.h
//  Rayne
//
//  Copyright 2016 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_UISERVER_H_
#define __RAYNE_UISERVER_H_

#include "RNUIConfig.h"
#include <deque>

namespace RN
{
	namespace UI
	{
		class Window;

		class Server : public Object
		{
		public:
			UIAPI Server(Camera *camera);
			UIAPI ~Server();

			UIAPI static Server *GetDefaultServer();
			UIAPI void MakeDefaultServer();

			UIAPI void AddWindow(UI::Window *window);
			UIAPI void RemoveWindow(UI::Window *window);
			
			UIAPI void AddToScene(Scene *scene);

			float GetHeight() const { return _camera->GetRenderPass()->GetFrame().height; }
			float GetWidth() const { return _camera->GetRenderPass()->GetFrame().width; }

			Camera *GetCamera() const { return _camera; }
		private:

			Camera *_camera;
			SceneNode *_windowContainer;

			RNDeclareMetaAPI(Server, UIAPI)
		};
	}
}


#endif /* __RAYNE_UISERVER_H_ */
