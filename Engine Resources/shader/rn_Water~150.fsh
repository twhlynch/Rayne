//
//  rn_Water.fsh
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#version 150
precision highp float;

uniform sampler2D mTexture0;
uniform sampler2D mTexture1;
in vec3 vertProjPos;
in vec2 vertTexcoord;

out vec4 fragColor0;

/*float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	
    f = f*f*(3.0-2.0*f);
	
    float n = p.x + p.y*57.0 + 113.0*p.z;
	
    float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                        mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                        mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    return res;
}


float noise(vec3 p) //Thx to Las^Mercury
{
	vec3 i = floor(p);
	vec4 a = dot(i, vec3(1., 57., 21.)) + vec4(0., 57., 21., 78.);
	vec3 f = cos((p-i)*acos(-1.))*(-.5)+.5;
	a = mix(sin(cos(a)*a),sin(cos(1.+a)*(1.+a)), f.x);
	a.xy = mix(a.xz, a.yw, f.y);
	return mix(a.x, a.y, f.z);
}*/


void main()
{
	vec3 normals = normalize(texture(mTexture1, vertTexcoord).xyz*2.0f-1.0f);
	
	vec2 coords = vertProjPos.xy/vertProjPos.z*0.5+0.5;
	coords.y = 1.0-coords.y;
	vec4 color0 = texture(mTexture0, coords*0.5+normals.xy*0.02);
	fragColor0 = color0;
}
