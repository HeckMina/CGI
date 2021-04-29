#ifdef GL_ES
precision mediump float;
#endif
const float PI = 3.14159265358979323846;
varying vec3 V_Texcoord;
uniform samplerCube U_Texture;
void main(){
	gl_FragColor=textureCube(U_Texture,normalize(V_Texcoord));
}