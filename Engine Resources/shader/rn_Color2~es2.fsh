//
//  rn_Color2.fsh
//  Rayne
//
//  Copyright 2013 by Felix Pohl, Nils Daumann and Sidney Just. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

precision highp float;

varying vec4 outColor0;
varying vec4 outColor1;

void main()
{
	gl_FragColor = outColor0 * outColor1;
}
