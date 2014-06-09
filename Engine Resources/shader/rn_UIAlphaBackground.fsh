//
//  rn_UIAlphaBackground.fsh
//  Rayne
//
//  Copyright 2014 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#version 150
precision highp float;

in vec4 vertColor;
out vec4 fragColor0;

void main()
{
	fragColor0 = vec4(pow(vertColor.rgb, vec3(2.2)), 1.0);
}
