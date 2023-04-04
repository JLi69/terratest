#version 330 core

out vec4 color;
uniform vec4 uColor;
uniform vec2 uDimensions = vec2(1.0, 1.0);
uniform float uOutline = 2.0;

in vec2 tc;

void main()
{
	color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	if(tc.x < uOutline / uDimensions.x || tc.x > 1.0 - uOutline / uDimensions.x ||
	   tc.y < uOutline / uDimensions.y || tc.y > 1.0 - uOutline / uDimensions.y)
	   color = uColor;
}
