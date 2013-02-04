
#version 150
precision highp float;

uniform sampler2D mTexture0; // SSAO
uniform sampler2D targetmap; // Scene

in vec2 texcoord;
out vec4 fragColor0;

void main()
{
	vec4 scene = texture(targetmap, texcoord);
	vec4 ssao = texture(mTexture0, texcoord);

	fragColor0 = scene * ssao;
}
