//
//  RNVRCamera.h
//  Rayne-VR
//
//  Copyright 2017 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_VRCAMERA_H_
#define __RAYNE_VRCAMERA_H_

#include "RNVR.h"
#include "RNVRTrackingState.h"
#include "RNVRWindow.h"

namespace RN
{
	class VRWindow;
	class VRCamera : public SceneNode
	{
	public:
		RNVRAPI VRCamera(VRWindow *window, RenderPass *previewRenderPass = nullptr, uint8 msaaSampleCount = 8, Window *debugWindow = nullptr);
		RNVRAPI ~VRCamera();

		RNVRAPI void Update(float delta) override;

		RNVRAPI SceneNode *GetHead() const { return _head; }
		RNVRAPI Camera *GetEye(uint8 eye) const { return _eye[eye]; }

		RNVRAPI const VRHMDTrackingState &GetHMDTrackingState() const;
		RNVRAPI const VRControllerTrackingState &GetControllerTrackingState(uint8 index) const;
		RNVRAPI const VRControllerTrackingState &GetTrackerTrackingState(uint8 index) const;
		RNVRAPI void SubmitControllerHaptics(uint8 controllerID, const VRControllerHaptics &haptics) const;

	private:
		void CreatePostprocessingPipeline();

		VRWindow *_window;
		Window *_debugWindow;
		SceneNode *_head;
		Camera *_eye[2];
		Entity *_hiddenAreaEntity[2];
		RenderPass *_previewRenderPass;
		uint8 _msaaSampleCount;

		RNDeclareMetaAPI(VRCamera, RNVRAPI)
	};
}


#endif /* __RAYNE_VRCAMERA_H_ */
