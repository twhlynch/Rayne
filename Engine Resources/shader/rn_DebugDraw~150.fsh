//
//  rn_DebugDraw.fsh
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#version 150
precision highp float;

in vec4 vertColor;

out vec4 fragColor0;

void main()
{
	fragColor0 = vertColor;
}
