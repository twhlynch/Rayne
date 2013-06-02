//
//  rn_Shadow.fsh
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef RN_SHADOW_FSH
#define RN_SHADOW_FSH

#ifdef RN_LIGHTING
uniform sampler2DArrayShadow lightDirectionalDepth;

uniform vec4 frameSize;
in vec4 vertDirLightProj[4];

//a textureOffset lookup for a 2DArrayShader sampler
float rn_textureOffset(sampler2DArrayShadow map, vec4 loc, vec2 offset)
{
	return texture(map, vec4(offset*frameSize.xy+loc.xy, loc.wz));
}

//basic 4x4 blur, with hardware bilinear filtering if enabled
float rn_ShadowPCF2x2(sampler2DArrayShadow map, vec4 projected)
{
	float shadow = rn_textureOffset(map, projected, vec2(0.0, 0.0));
	shadow += rn_textureOffset(map, projected, vec2(1.0, 0.0));
	shadow += rn_textureOffset(map, projected, vec2(0.0, 1.0));
	shadow += rn_textureOffset(map, projected, vec2(1.0, 1.0));
	shadow *= 0.25;
	return shadow;
}

//basic 4x4 blur, with hardware bilinear filtering if enabled
float rn_ShadowPCF4x4(sampler2DArrayShadow map, vec4 projected)
{
	float shadow = rn_textureOffset(map, projected, vec2(-2.0, -2.0));
	shadow += rn_textureOffset(map, projected, vec2(-1.0, -2.0));
	shadow += rn_textureOffset(map, projected, vec2(0.0, -2.0));
	shadow += rn_textureOffset(map, projected, vec2(1.0, -2.0));
	
	shadow += rn_textureOffset(map, projected, vec2(-2.0, -1.0));
	shadow += rn_textureOffset(map, projected, vec2(-1.0, -1.0));
	shadow += rn_textureOffset(map, projected, vec2(0.0, -1.0));
	shadow += rn_textureOffset(map, projected, vec2(1.0, -1.0));
	
	shadow += rn_textureOffset(map, projected, vec2(-2.0, 0.0));
	shadow += rn_textureOffset(map, projected, vec2(-1.0, 0.0));
	shadow += rn_textureOffset(map, projected, vec2(0.0, 0.0));
	shadow += rn_textureOffset(map, projected, vec2(1.0, 0.0));
	
	shadow += rn_textureOffset(map, projected, vec2(-2.0, 1.0));
	shadow += rn_textureOffset(map, projected, vec2(-1.0, 1.0));
	shadow += rn_textureOffset(map, projected, vec2(0.0, 1.0));
	shadow += rn_textureOffset(map, projected, vec2(1.0, 1.0));
	
	shadow *= 0.0625;
	return shadow;
}

//returns the shadow factor for the first directional light
float rn_ShadowDir1()
{
	vec3 proj[4];
	proj[0] = vertDirLightProj[0].xyz/vertDirLightProj[0].w;
	proj[1] = vertDirLightProj[1].xyz/vertDirLightProj[1].w;
	proj[2] = vertDirLightProj[2].xyz/vertDirLightProj[2].w;
	proj[3] = vertDirLightProj[3].xyz/vertDirLightProj[3].w;
	
//	vec4 dist = vec4(dot(proj[0], proj[0]), dot(proj[1], proj[1]), dot(proj[2], proj[2]), dot(proj[3], proj[3]));
//	vec4 zGreater = vec4(lessThan(dist, vec4(1.0)));
	
	float mapToUse = 3;
	
	bvec3 inMap[3];
	inMap[2] = lessThan(proj[2]*proj[2], vec3(1.0));
	inMap[1] = lessThan(proj[1]*proj[1], vec3(1.0));
	inMap[0] = lessThan(proj[0]*proj[0], vec3(1.0));
	
	if(inMap[2].x && inMap[2].y && inMap[2].z)
		mapToUse = 2;
	
	if(inMap[1].x && inMap[1].y && inMap[1].z)
		mapToUse = 1;
	
	if(inMap[0].x && inMap[0].y && inMap[0].z)
		mapToUse = 0;
	
/*	mapToUse = (zGreater.z > 0.5)? 2:mapToUse;
	mapToUse = (zGreater.y > 0.5)? 1:mapToUse;
	mapToUse = (zGreater.x > 0.5)? 0:mapToUse;*/
	
	vec4 projected = vec4(proj[int(mapToUse)]*0.5+0.5, mapToUse);
//	return rn_textureOffset(lightDirectionalDepth, projected, vec2(0.0));
	return rn_ShadowPCF2x2(lightDirectionalDepth, projected);
//	return rn_ShadowPCF4x4(lightDirectionalDepth, projected);
}

#else
#define rn_ShadowDir1() (1.0)
#endif

#endif
