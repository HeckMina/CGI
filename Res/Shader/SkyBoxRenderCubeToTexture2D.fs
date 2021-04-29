#ifdef GL_ES
precision mediump float;
#endif
precision highp float;
varying vec3 V_Texcoord;
uniform samplerCube U_Texture;
void main(){
	vec3 normal=V_Texcoord;//normalize(V_Texcoord);
	float abs_x=abs(normal.x);
	float abs_y=abs(normal.y);
	float abs_z=abs(normal.z);
	float axis=max(max(abs_x,abs_z),abs_y);
	if(axis==abs_x){
	}else if(axis==abs_y){
	}else if(axis==abs_z){
		normal.x=-normal.x;
		normal.z=-normal.z;
	}
	normal=normalize(normal);
	vec3 color=textureCube(U_Texture,normal);
	gl_FragColor=vec4(color,1.0);
}