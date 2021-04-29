#ifdef GL_ES
precision mediump float;
#endif
const float PI = 3.14159265359;
const float gamma=2.2;
uniform samplerCube U_Texture;
varying vec3 V_Texcoord;
vec3 GetColor(vec3 texcoord){
	vec3 color=textureCube(U_Texture, texcoord).rgb;
	color=pow(color,vec3(gamma));
	return color;
}
vec3 GammaCorrection(vec3 color){
	return pow(color,vec3(1.0/gamma));
}
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
	vec3 normal = GetNormal();
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, normal);
	up = cross(normal, right);
	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	vec3 irradiance = vec3(0.0);
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta){
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta){
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
			irradiance += GetColor(sampleVec) * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));
	gl_FragColor=vec4(irradiance,1.0);
}