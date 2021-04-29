#ifdef GL_ES
precision mediump float;
#endif
attribute vec4 position;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
varying vec3 V_Texcoord;
void main(){
	V_Texcoord=position.xyz;
	vec4 Position=ProjectionMatrix*ViewMatrix*position;
	gl_Position=Position.xyzw;
}