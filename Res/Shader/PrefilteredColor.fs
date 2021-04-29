#ifdef GL_ES
precision mediump float;
#endif
const float PI = 3.14159265359;
const float gamma=2.2;
uniform vec4 U_Setting;
uniform samplerCube U_Texture;
varying vec3 V_Texcoord;
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
vec3 GetColor(vec3 texcoord){
	vec3 color=textureCube(U_Texture, texcoord).rgb;
	color=pow(color,vec3(gamma));
	return color;
}
vec3 GammaCorrection(vec3 color){
	return pow(color,vec3(1.0/gamma));
}
float VanDerCorpus(uint n, uint base){
    float invBase = 1.0 / float(base);
    float denom   = 1.0;
    float result  = 0.0;
    for(uint i = 0u; i < 32u; ++i){
        if(n > 0u){
            denom   = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n       = uint(float(n) / 2.0);
        }
    }

    return result;
}
vec2 HammersleyNoBitOps(uint i, uint N){
    return vec2(float(i)/float(N), VanDerCorpus(i, 2u));
}
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness){
    float a = roughness*roughness;
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}
void main(){
	const uint SAMPLE_COUNT = 1024u;
	vec3 normal = GetNormal();//fixed normal to get the right uv
    vec3 R = normal;
    vec3 V = normal;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);
    float roughness=U_Setting.x;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i){
        vec2 Xi = HammersleyNoBitOps(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, normal, roughness);
        vec3 L  = normalize(reflect(-V,H));
        float NdotL = max(dot(normal, L), 0.0);
        if(NdotL > 0.0){
            prefilteredColor += GetColor(L).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;
	prefilteredColor = GammaCorrection(prefilteredColor);
    gl_FragColor = vec4(prefilteredColor, 1.0);
}