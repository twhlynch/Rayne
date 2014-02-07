//
//  TGSponzaWorld.h
//  Test Game
//
//  Copyright 2014 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __Test_Game__TGSponzaWorld__
#define __Test_Game__TGSponzaWorld__

#include <Rayne.h>
#include "TGWorld.h"

namespace TG
{
	class SponzaWorld : public World
	{
	public:
		SponzaWorld();
		
		void LoadOnThread(RN::Thread *thread) override;
	};
}

#endif /* defined(__Test_Game__TGSponzaWorld__) */
