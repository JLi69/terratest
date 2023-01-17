#version 430

layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 texCoords;
//Window dimensions
uniform vec2 uWindowDimensions;
//Dimensions in pixels
uniform vec2 uDimensions = vec2(1.0, 1.0);
//Position in pixels
uniform vec2 uPixPos = vec2(0.0, 0.0);
uniform bool uFlip = false;

uniform float uRotation = 0.0f;

out vec2 tc;

void main()
{
	mat4 win;
	win[0] = vec4(1.0 / uWindowDimensions.x, 0.0, 0.0, 0.0);
	win[1] = vec4(0.0, 1.0 / uWindowDimensions.y, 0.0, 0.0); 
	win[2] = vec4(0.0, 0.0, 1.0, 0.0);
	win[3] = vec4(0.0, 0.0, 0.0, 1.0);

	mat4 scale;
	scale[0] = vec4(uDimensions.x, 0.0, 0.0, 0.0);
	scale[1] = vec4(0.0, uDimensions.y, 0.0, 0.0); 
	scale[2] = vec4(0.0, 0.0, 1.0, 0.0);
	scale[3] = vec4(0.0, 0.0, 0.0, 1.0);

	mat4 translate;
	translate[0] = vec4(1.0, 0.0, 0.0, 0.0);
	translate[1] = vec4(0.0, 1.0, 0.0, 0.0); 
	translate[2] = vec4(0.0, 0.0, 1.0, 0.0);
	translate[3] = vec4(uPixPos.x / uWindowDimensions.x * 2.0f, uPixPos.y / uWindowDimensions.y * 2.0f, 0.0, 1.0);

	float rotation = uRotation * ((uFlip) ? (-1.0) : (1.0));

	mat4 rotate;
	rotate[0] = vec4(cos(rotation), -sin(rotation), 0.0, 0.0);
	rotate[1] = vec4(sin(rotation), cos(rotation), 0.0, 0.0); 
	rotate[2] = vec4(0.0, 0.0, 1.0, 0.0);
	rotate[3] = vec4(0.0, 0.0, 0.0, 1.0);

	gl_Position = translate * win * rotate * scale * pos;

	tc = texCoords;
}
