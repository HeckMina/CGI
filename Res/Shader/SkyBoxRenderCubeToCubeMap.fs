#ifdef GL_ES
precision mediump float;
#endif
precision highp float;
varying vec3 V_Texcoord;
uniform samplerCube U_Texture;
vec3 GetNormal(){
	vec3 normal=V_Texcoord;
	float abs_x=abs(normal.x);
	float abs_y=abs(normal.y);
	float abs_z=abs(normal.z);
	float axis=max(max(abs_x,abs_z),abs_y);
	if(axis==abs_x){
		if(normal.x>0){//+x
			normal.y=-normal.y;
		}else{//-x
			normal.y=-normal.y;
		}
	}else if(axis==abs_y){
		if(normal.y>0){//+y
			normal.z=-normal.z;
		}else{//-y
			normal.z=-normal.z;
		}
	}else if(axis==abs_z){
		if(normal.z>0){//+z
			normal.y=-normal.y;
		}else{//-z
			normal.y=-normal.y;
		}
		normal.z=-normal.z;
		normal.x=-normal.x;
	}
	return normalize(normal);
}
void main(){
	vec3 normal=GetNormal();
	vec3 color=textureCube(U_Texture,normal);
	gl_FragColor=vec4(color,1.0);
}