//
//  rn_Color1.fsh
//  Rayne
//
//  Copyright 2013 by Felix Pohl, Nils Daumann and Sidney Just. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#version 150
precision highp float;

in vec4 outColor;

out vec4 fragColor0;

void main()
{
	fragColor0 = outColor;
}
