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

uniform float uRays = 16.0;
uniform float uPhase = 0.5; //Moon phase

void main()
{
	float dist = (tc.x - 0.5) * (tc.x - 0.5) + (tc.y - 0.5) * (tc.y - 0.5),
		  maxDist = pow(0.35 + sin(uRays * atan((tc.y - 0.5) / (tc.x - 0.5))) * 0.05, 2);

	//Moon phase appearance
	vec3 sunPos = vec3(-32.0 * cos(uPhase * 2.0 * 3.14159), -32.0 * cos(uPhase * 2.0 * 3.14159), 32.0 * sin(uPhase * 2.0 * 3.14159));
	vec3 moonVec = normalize(vec3(tc.x - 0.5, tc.y - 0.5, sqrt(0.5 * 0.5 - 2.0 * pow(tc.y - 0.5, 2.0) - 2.0 * pow(tc.x - 0.5, 2.0))));
	float phaseDotProd = dot(sunPos, normalize(moonVec));

	if(tc.y > uLevel || dist >= maxDist)
	{
		color = vec4(0.0, 0.0, 0.0, 0.0);
		return;
	}

	if(uRays == 0.0) //is the moon, do moon phases
		color = uColor * uBrightness * phaseDotProd;
	else //Probably not the moon, do not do moon phases
		color = uColor * uBrightness;
	color.a *= (1.0 - dist / maxDist);

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
