#version 430

out vec4 color;
uniform vec4 uColor;

in vec2 tc;

void main()
{
	color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	if(tc.x < 1.0f / 16.0f || tc.x > 15.0f / 16.0f ||
	   tc.y < 1.0f / 16.0f || tc.y > 15.0f / 16.0f)
	   color = uColor;
}
