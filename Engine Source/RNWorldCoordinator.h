//
//  RNWorldCoordinator.h
//  Rayne
//
//  Copyright 2014 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_WORLDCOORDINATOR_H__
#define __RAYNE_WORLDCOORDINATOR_H__

#include "RNBase.h"
#include "RNMutex.h"
#include "RNWorld.h"
#include "RNProgress.h"

#define kRNWorldCoordinatorDidFinishLoading RNCSTR("kRNWorldCoordinatorDidFinishLoading")
#define kRNWorldCoordinatorWillBeginLoading RNCSTR("kRNWorldCoordinatorWillBeginLoading")
#define kRNWorldCoordinatorDidStepWorld     RNCSTR("kRNWorldCoordinatorDidStepWorld")

namespace RN
{
	class Kernel;
	class WorldCoordinator : public ISingleton<WorldCoordinator>
	{
	public:
		friend class Kernel;
		
		RNAPI WorldCoordinator();
		RNAPI ~WorldCoordinator();
		
		RNAPI Progress *LoadWorld(World *world);
		
		RNAPI bool IsLoading() const { return _loading.load(); }
		RNAPI World *GetWorld() const { return _world; }
		
	private:
		void StepWorld(FrameID frame, float delta);
		void RenderWorld(Renderer *renderer);
		
		bool AwaitFinishLoading();
		bool BeginLoading();
		void FinishLoading(bool state);
		
		Thread *_loadThread;
		World *_world;
		Mutex _lock;
		
		std::atomic<bool> _loading;
		std::future<bool> _loadFuture;
		Progress *_loadingProgress;
		
		RNDeclareSingleton(WorldCoordinator)
	};
}

#endif /* __RAYNE_WORLDCOORDINATOR_H__ */
