#version 330 core

uniform sampler2D tex;

out vec4 color;
uniform vec4 uColor;
uniform bool uUseTexture = true;
uniform bool uFlip = false;
in vec2 tc;

uniform vec2 uTexFrac = vec2(1.0f, 1.0f);
uniform vec2 uTexOffset = vec2(0.0f, 0.0f);
uniform vec2 uTexSize = vec2(16.0f, 16.0f);
uniform float uTransparency = 1.0f;
uniform float uLevel = 2.0f;
uniform float uBrightness = 1.0f;

void main()
{
	if(tc.y > uLevel)
		discard;

	color = uColor * uBrightness;

	if(uUseTexture)
	{
		vec2 texCoord;
		if(!uFlip)
		{
			texCoord = vec2(
				(floor(uTexSize.x * (tc.x * uTexFrac.x + uTexOffset.x)) + 0.5) / uTexSize.x,		
				(floor(uTexSize.y * (tc.y * uTexFrac.y + uTexOffset.y)) + 0.5) / uTexSize.y
			);
		}	
		else if(uFlip)
		{
			texCoord = vec2(
				(floor(uTexSize.x * ((1.0f - tc.x) * uTexFrac.x + uTexOffset.x)) + 0.5) / uTexSize.x,
				(floor(uTexSize.y * ((tc.y) * uTexFrac.y + uTexOffset.y)) + 0.5) / uTexSize.y
			);
		}

		if(texCoord.y > uTexOffset.y + uTexFrac.y)
			texCoord.y = (floor(uTexSize.y * (uTexOffset.y + uTexFrac.y)) + 0.5) / uTexSize.y - 1.0 / uTexSize.y;

		color = texture(tex, texCoord);
		color.r *= uBrightness;
		color.g *= uBrightness;
		color.b *= uBrightness;
		color.a *= uTransparency; 
	}
}
