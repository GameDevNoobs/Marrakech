attribute vec3 a_posL;
attribute vec2 a_uv;
void main()
{
	vec4 posL = vec4(a_posL, 1.0);
	gl_Position = posL;
}