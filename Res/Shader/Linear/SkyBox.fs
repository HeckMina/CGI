#ifdef GL_ES
precision mediump float;
#endif
varying vec3 V_Texcoord;
uniform samplerCube U_Texture;
const float gamma=2.2;
vec3 GammaDecode(vec3 color){
	return pow(color,vec3(gamma));
}
void main(){
	vec3 color=textureCube(U_Texture,normalize(V_Texcoord)).rgb;
	color=GammaDecode(color);
	gl_FragColor=vec4(color,1.0);
}