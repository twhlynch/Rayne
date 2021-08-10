//
//  RNOpenXRInternals.h
//  Rayne-OpenXR
//
//  Copyright 2021 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __RAYNE_OpenXRINTERNALS_H_
#define __RAYNE_OpenXRINTERNALS_H_

#include "RNOpenXR.h"

#include "RNOpenXRVulkanSwapChain.h"

#include "openxr/openxr_platform_defines.h"
#include "openxr/openxr_platform.h"
#include "openxr/openxr.h"

namespace RN
{
	struct OpenXRWindowInternals
	{
		XrInstance instance;
		XrSystemId systemID;
		XrSystemProperties systemProperties;
		XrSession session;

		XrSpace trackingSpace;
        XrTime predictedDisplayTime;

		XrView *views;

		PFN_xrGetVulkanInstanceExtensionsKHR GetVulkanInstanceExtensionsKHR;
		PFN_xrGetVulkanDeviceExtensionsKHR GetVulkanDeviceExtensionsKHR;
		PFN_xrGetVulkanGraphicsDeviceKHR GetVulkanGraphicsDeviceKHR;
		PFN_xrGetVulkanGraphicsRequirementsKHR GetVulkanGraphicsRequirementsKHR;
	};

    struct OpenXRSwapchainInternals
    {
        XrSwapchain swapchain;
    };
}


#endif /* __RAYNE_OpenXRINTERNALS_H_ */
