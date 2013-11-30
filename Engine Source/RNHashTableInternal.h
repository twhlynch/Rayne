//
//  RNHashTableInternal.h
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_HASHTABLEINTERNAL_H__
#define __RAYNE_HASHTABLEINTERNAL_H__

#include "RNBase.h"

#define kRNHashTablePrimitiveCount 64

namespace RN
{
	static const size_t HashTableCapacity[kRNHashTablePrimitiveCount] =
	{
		3, 7, 13, 23, 41, 71, 127, 191, 251, 383, 631, 1087, 1723,
		2803, 4523, 7351, 11959, 19447, 31231, 50683, 81919, 132607,
		214519, 346607, 561109, 907759, 1468927, 2376191, 3845119,
		6221311, 10066421, 16287743, 26354171, 42641881, 68996069,
		111638519, 180634607, 292272623, 472907251,
#if RN_PLATFORM_64BIT
		765180413UL, 1238087663UL, 2003267557UL, 3241355263UL, 5244622819UL,
#endif
	};
	
	static const size_t HashTableMaxCount[kRNHashTablePrimitiveCount] =
	{
		3, 6, 11, 19, 32, 52, 85, 118, 155, 237, 390, 672, 1065,
		1732, 2795, 4543, 7391, 12019, 19302, 31324, 50629, 81956,
		132580, 214215, 346784, 561026, 907847, 1468567, 2376414,
		3844982, 6221390, 10066379, 16287773, 26354132, 42641916,
		68996399, 111638327, 180634415, 292272755,
#if RN_PLATFORM_64BIT
		472907503UL, 765180257UL, 1238087439UL, 2003267722UL, 3241355160UL,
#endif
	};
}

#endif /* __RAYNE_HASHTABLEINTERNAL_H__ */
