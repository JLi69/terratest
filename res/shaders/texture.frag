#version 430

layout(location = 0) uniform sampler2D tex;

out vec4 color;
uniform vec4 uColor;
uniform bool uUseTexture = true;
uniform bool uFlip = false;
in vec2 tc;

uniform vec2 uTexFrac = vec2(1.0f, 1.0f);
uniform vec2 uTexOffset = vec2(0.0f, 0.0f);
uniform vec2 uTexSize = vec2(16.0f, 16.0f);
uniform float uTransparency = 1.0f;

void main()
{
	color = uColor;

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
			texCoord.y = (floor(uTexSize.y * (uTexOffset.y + uTexFrac.y)) + 0.5) / uTexSize.y - 0.01;
		if(texCoord.y < uTexOffset.y)
			texCoord.y = uTexOffset.y;

		color = texture(tex, texCoord);
		color.a *= uTransparency; 
	}
}
