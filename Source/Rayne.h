//
//  Rayne.h
//  Rayne
//
//  Copyright 2015 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_RAYNE_H__
#define __RAYNE_RAYNE_H__

#include "Base/RNArgumentParser.h"
#include "Base/RNApplication.h"
#include "Base/RNBase.h"
#include "Base/RNKernel.h"
#include "Base/RNSettings.h"
#include "Base/RNUnicode.h"
#include "Base/RNMemoryPool.h"

#include "Assets/RNAsset.h"
#include "Assets/RNAssetCoordinator.h"
#include "Assets/RNAssetLoader.h"

#include "Data/RNAny.h"
#include "Data/RNIntrusiveList.h"
#include "Data/RNRingBuffer.h"
#include "Data/RNRRef.h"

#include "Debug/RNLogger.h"
#include "Debug/RNLoggingEngine.h"

#include "Math/RNAlgorithm.h"
#include "Math/RNColor.h"
#include "Math/RNConstants.h"
#include "Math/RNMath.h"
#include "Math/RNMatrixQuaternion.h"
#include "Math/RNMatrix.h"
#include "Math/RNQuaternion.h"
#include "Math/RNSIMD.h"
#include "Math/RNVector.h"

#include "Objects/RNAutoreleasePool.h"
#include "Objects/RNObject.h"
#include "Objects/RNCatalogue.h"
#include "Objects/RNKVO.h"
#include "Objects/RNSerialization.h"
#include "Objects/RNArray.h"
#include "Objects/RNSet.h"
#include "Objects/RNCountedSet.h"
#include "Objects/RNDictionary.h"
#include "Objects/RNJSONSerialization.h"
#include "Objects/RNNull.h"
#include "Objects/RNNumber.h"
#include "Objects/RNValue.h"
#include "Objects/RNCharacterSet.h"
#include "Objects/RNString.h"

#include "Rendering/RNFramebuffer.h"
#include "Rendering/RNGPUBuffer.h"
#include "Rendering/RNGPUResource.h"
#include "Rendering/RNMaterial.h"
#include "Rendering/RNMesh.h"
#include "Rendering/RNRenderer.h"
#include "Rendering/RNRenderingDescriptor.h"
#include "Rendering/RNShader.h"
#include "Rendering/RNShaderLibrary.h"
#include "Rendering/RNWindow.h"

#include "Scene/RNCamera.h"
#include "Scene/RNScene.h"
#include "Scene/RNSceneCoordinator.h"
#include "Scene/RNSceneNode.h"

#include "System/RNFileCoordinator.h"
#include "System/RNScreen.h"

#include "Threads/RNThread.h"
#include "Threads/RNRunLoop.h"
#include "Threads/RNSpinLock.h"
#include "Threads/RNThreadLocalStorage.h"
#include "Threads/RNWorkGroup.h"
#include "Threads/RNWorkQueue.h"

#endif /* _RAYNE_RAYNE_H__ */
