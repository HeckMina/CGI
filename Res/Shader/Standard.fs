#ifdef GL_ES
precision highp float;
#endif
const float PI = 3.14159265358979323846;
const float gamma=2.2;
const float MAX_REFLECTION_LOD=4.0;
uniform sampler2D U_DiffuseTexture;
uniform sampler2D U_RoughnessTexture;
uniform sampler2D U_MetallicTexture;
uniform sampler2D U_NormalTexture;
uniform sampler2D U_LUTTexture;
uniform sampler2D U_AmbientOcclusionTexture;
uniform samplerCube U_DiffuseIrradianceMap;
uniform samplerCube U_PrefilteredEnvironmentMap;
uniform vec4 U_CameraPos;
uniform vec4 U_IOR;
uniform vec4 U_PBRSetting;//ao map,sRGB color,light count
uniform vec4 U_LightPos[4];
uniform vec4 U_LightColor[4];
uniform vec4 U_LightIntensity;
in vec4 V_Normal;
in vec4 V_WorldPos;
in vec4 V_Texcoord;
in mat3 V_TBN;
vec3 GammaEncode(vec3 color){
	return pow(color,vec3(1.0/gamma));
}
vec3 GammaDecode(vec3 color){
	return pow(color,vec3(gamma));
}
vec3 GetSurfaceColor(){
	vec3 color=texture2D(U_DiffuseTexture,V_Texcoord.xy).rgb;
	if(U_PBRSetting.y==1.0){
		return color;
	}
	return GammaDecode(color);
}
float GetSurfaceRoughness(){
	return texture2D(U_RoughnessTexture,V_Texcoord.xy).r;
}
float GetSurfaceMetallic(){
	return texture2D(U_MetallicTexture,V_Texcoord.xy).g;
}
vec3 GetAOColor(vec3 color){
	if(U_PBRSetting.x==1.0){
		float ao=texture2D(U_AmbientOcclusionTexture,V_Texcoord.xy).r;
		return vec3(0.05)*color*ao;
	}
	return vec3(0.01);
}
vec3 GetNormal(){
	vec3 normal=texture2D(U_NormalTexture, V_Texcoord.xy).rgb;
	normal = normalize( normal * 2.0 - 1.0);
	vec3 surface_normal_dir = normalize(V_TBN * normal);
	return surface_normal_dir;
}
float TrowbridgeReitzGGXNDF(vec3 normal,vec3 half,float roughness){
	float sqrt_roughness = roughness*roughness;
    float dot_NH  = max(dot(normal, half), 0.0);
    float sqrt_dot_NH  = dot_NH*dot_NH;
    float temp  = (sqrt_dot_NH * (sqrt_roughness - 1.0) + 1.0);
    return sqrt_roughness / (PI * temp * temp);
}
float SchlickGGX(float dot_N_X,float k){
    return dot_N_X / (dot_N_X * (1.0 - k) + k);
}
float SmithGF(vec3 normal, vec3 surface_to_eye, vec3 surface_to_light, float k){
    float dot_N_V = max(dot(normal, surface_to_eye), 0.0);
    float dot_N_L = max(dot(normal, surface_to_light), 0.0);
    return SchlickGGX(dot_N_V, k) * SchlickGGX(dot_N_L, k);
}
vec3 FresnelSchlick(vec3 precomputed_ior,float dot_H_V){
    return precomputed_ior + (1.0 - precomputed_ior) * pow(1.0 - dot_H_V, 5.0);
}
vec3 FresnelSchlickRoughness(vec3 precomputed_ior,float dot_H_V, float roughness){
    return precomputed_ior + (max(vec3(1.0 - roughness),precomputed_ior)-precomputed_ior) * pow(1.0 - dot_H_V, 5.0);
}
vec3 GetDiffuseMaterial(vec3 specular_ratio,float metallic){
	vec3 diffuse_ratio=vec3(1.0)-specular_ratio;
	vec3 diffuse_material=diffuse_ratio*(1.0-metallic);
	return diffuse_material;
}
vec3 GetSpecularRatio(float HDotV,vec3 surface_color,float metallic){
	vec3  F0 = U_IOR.xyz;
	F0 =  mix(F0, surface_color, metallic);
	return FresnelSchlick(F0,HDotV);
}
vec3 GetSpecularRatioRoughness(float HDotV,vec3 surface_color,float metallic, float roughness){
	vec3  F0 = U_IOR.rgb;
	F0 =  mix(F0, surface_color, metallic);
	return FresnelSchlickRoughness(F0,HDotV,roughness);
}
vec3 GetDiffuseReflactence(vec3 surface_color,vec3 specular_ratio,float metallic){
	vec3 diffuse_ratio=vec3(1.0)-specular_ratio;
	vec3 diffuse_material=diffuse_ratio*(1.0-metallic);
	return diffuse_material*(surface_color/PI);
}
vec3 GetAmbientColor(vec3 surface_color,vec3 N,vec3 V,float roughness,float metallic){
	float NdotV=max(dot(N, V), 0.0);
	vec3 specular_reflectance_ratio=GetSpecularRatioRoughness(NdotV,surface_color,metallic,roughness);
	vec3 diffuse_material=GetDiffuseMaterial(specular_reflectance_ratio,metallic);
	vec3 irradiance=textureCube(U_DiffuseIrradianceMap,N).rgb;
	vec3 R=normalize(2.0 * dot(V, N) * N - V);
	vec3 prefilteredColor = textureLod(U_PrefilteredEnvironmentMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
	float NdotL=max(dot(N,V),0.0);
	vec2 F0_Factor  = texture2D(U_LUTTexture, vec2(NdotL, roughness)).rg;
	vec3 specular_reflectance=prefilteredColor*(specular_reflectance_ratio*F0_Factor.x+vec3(F0_Factor.y));
	vec3 ambient_color=diffuse_material*irradiance*surface_color+specular_reflectance;
	return GetAOColor(ambient_color);
}
void main(){
	vec3 surface_normal_dir=GetNormal();
	vec3 surface_to_eye_dir=normalize(U_CameraPos.xyz-V_WorldPos.xyz);
	vec3 surface_color=GetSurfaceColor();
	float roughness=GetSurfaceRoughness();
	float metallic=GetSurfaceMetallic();
	vec3 BRDFOutPutColor=vec3(0.0);
	vec3 ambientColor=GetAmbientColor(surface_color,surface_normal_dir,surface_to_eye_dir,roughness,metallic);
	int light_count=int(U_PBRSetting.z);
	for(int i=0;i<light_count;++i){
		vec3 surface_to_light_source_dir=normalize(U_LightPos[i]);
        float distance = length(U_LightPos[i].xyz - V_WorldPos.xyz);
        float attenuation = 1.0 / (distance * distance);
		if(U_LightPos[i].w==0.0){
			attenuation=1.0;
		}
		vec3 half_v_dir=normalize(surface_to_eye_dir+surface_to_light_source_dir);
		float HdotV=max(dot(half_v_dir, surface_to_eye_dir), 0.0);
		float NdotL=max(dot(surface_normal_dir,surface_to_light_source_dir),0.0);
		vec3 specular_ratio=GetSpecularRatio(HdotV,surface_color,metallic);
		vec3 diffuse_material=GetDiffuseMaterial(specular_ratio,metallic);
		vec3 diffuse_reflectance=diffuse_material*(surface_color/PI);
		float D=TrowbridgeReitzGGXNDF(surface_normal_dir,half_v_dir,roughness);
		float G=SmithGF(surface_normal_dir,surface_to_eye_dir,surface_to_light_source_dir,roughness);
		vec3 specular_reflectance=(D*specular_ratio*G)/max(4.0*max(dot(surface_normal_dir,surface_to_eye_dir),0.0)*NdotL,0.001);
		vec3 current_light_color=(diffuse_reflectance+specular_reflectance)*NdotL*U_LightColor[i].rgb*U_LightIntensity[i]*attenuation;
		BRDFOutPutColor+=current_light_color;
	}
	gl_FragColor=vec4(ambientColor+BRDFOutPutColor,1.0);
}