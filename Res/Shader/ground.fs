#ifdef GL_ES
precision mediump float;
#endif
varying vec4 V_Color;
void main()
{
	gl_FragColor=V_Color;
}