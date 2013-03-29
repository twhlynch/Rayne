//
//  rn_Color1.vsh
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

precision highp float;

uniform mat4 matProjViewModel;

attribute vec3 vertPosition;
attribute vec4 vertColor0;

varying vec4 outColor;

void main()
{
	outColor = vertColor0;

	gl_Position = matProjViewModel * vec4(vertPosition, 1.0);
}
