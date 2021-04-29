#ifdef GL_ES
precision highp float;
#endif
const vec4 U_PBRSetting=vec4(1.0,1.0,1.0,0.0);//surface_color,roughness,metallic,ao
const float PI = 3.14159265358979323846;
const vec3 Linear_Water_IOR=vec3(0.02);
const vec3 Linear_Glass_Low_IOR=vec3(0.03);
const vec3 Linear_Plastic_Low_IOR=vec3(0.03);
const vec3 Linear_Plastic_High_IOR=vec3(0.05);
const vec3 Linear_Glass_High_IOR=vec3(0.08);
const vec3 Linear_Ruby_IOR=vec3(0.08);
const vec3 Linear_Diamond_IOR=vec3(0.17);
const vec3 Linear_Iron_IOR=vec3(0.56,0.57,0.58);
const vec3 Linear_Copper_IOR=vec3(0.95,0.64,0.54);
const vec3 Linear_Gold_IOR=vec3(1.0,0.71,0.29);
const vec3 Linear_Aluminium_IOR=vec3(0.91,0.92,0.92);
const vec3 Linear_Silver_IOR=vec3(0.95,0.93,0.88);
const vec3 sRGB_Water_IOR=vec3(0.15);
const vec3 sRGB_Glass_Low_IOR=vec3(0.21);
const vec3 sRGB_Plastic_Low_IOR=vec3(0.21);
const vec3 sRGB_Plastic_High_IOR=vec3(0.24);
const vec3 sRGB_Glass_High_IOR=vec3(0.31);
const vec3 sRGB_Ruby_IOR=vec3(0.31);
const vec3 sRGB_Diamond_IOR=vec3(0.45);
const vec3 sRGB_Iron_IOR=vec3(0.77,0.78,0.78);
const vec3 sRGB_Copper_IOR=vec3(0.98,0.82,0.76);
const vec3 sRGB_Gold_IOR=vec3(1.0,0.86,0.57);
const vec3 sRGB_Aluminium_IOR=vec3(0.96,0.96,0.97);
const vec3 sRGB_Silver_IOR=vec3(0.98,0.97,0.95);
uniform vec4 U_LightPos;
uniform vec4 U_LightAmbient;
uniform vec4 U_LightDiffuse;
uniform vec4 U_LightSpecular;
uniform vec4 U_AmbientMaterial;
uniform vec4 U_SurfaceColor;
uniform vec4 U_Material;
uniform sampler2D U_DiffuseTexture;
uniform sampler2D U_RoughnessTexture;
uniform sampler2D U_MetallicTexture;
uniform sampler2D U_AmbientOcclusionTexture;
uniform sampler2D U_NormalTexture;
uniform vec4 U_CameraPos;
uniform vec4 U_LightOpt;
varying vec4 V_Normal;
varying vec4 V_WorldPos;
varying vec4 V_Texcoord;
varying mat3 V_TBN;
vec3 GetSurfaceColor(){
	if(U_PBRSetting.x==1.0){
		return texture2D(U_DiffuseTexture,V_Texcoord.xy).rgb;
	}
	return vec3(1.0,0.0,0.0);//Linear_Plastic_Low_IOR;
}
float GetSurfaceRoughness(){
	if(U_PBRSetting.y==1.0){
		return texture2D(U_RoughnessTexture,V_Texcoord.xy).r;
	}
	return U_Material.x;
}
float GetSurfaceMetallic(){
	if(U_PBRSetting.z==1.0){
		return texture2D(U_MetallicTexture,V_Texcoord.xy).r;
	}
	return U_Material.y;
}
vec3 GetAOColor(vec3 color){
	if(U_PBRSetting.w==1.0){
		float ao=texture2D(U_AmbientOcclusionTexture,V_Texcoord.xy).r;
		return vec3(0.2)*color*ao;
	}
	return vec3(0.0);
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
vec3 GetDiffuseReflactence(vec3 surface_color,vec3 specular_ratio,float metallic){
	vec3 diffuse_ratio=vec3(1.0)-specular_ratio;
	vec3 diffuse_material=diffuse_ratio*(1.0-metallic);
	return diffuse_material*(surface_color/PI);
}
vec3 BRDFDirectional(vec3 L,vec3 N,vec3 H,vec3 V,vec3 surface_color){
	float roughness=GetSurfaceRoughness();
	float metallic=GetSurfaceMetallic();
	float NdotL=max(dot(N,L),0.0);
	float D=TrowbridgeReitzGGXNDF(N,H,roughness);
	vec3  F0 = sRGB_Water_IOR;
	F0 =  mix(F0, surface_color, metallic);
	vec3  F=FresnelSchlick(F0,max(dot(H, V), 0.0));
	float G=SmithGF(N,V,L,roughness);
	vec3 specular_reflectance=(D*F*G)/max(4.0*max(dot(N,V),0.0)*NdotL,0.001);
	vec3 diffuse_reflectance=GetDiffuseReflactence(surface_color,F,metallic);
	return (diffuse_reflectance+specular_reflectance)*NdotL;
}
vec3 GetNormal(){
	if(U_PBRSetting.y==1.0){
		vec3 normal=texture2D(U_NormalTexture, V_Texcoord.xy).rgb;
		normal = normalize( normal * 2.0 - 1.0);
		vec3 surface_normal_dir = normalize(V_TBN * normal);
		return surface_normal_dir;return texture2D(U_RoughnessTexture,V_Texcoord.xy).r;
	}
	return normalize(V_Normal.xyz);
}
void main(){
	vec3 surface_normal_dir=GetNormal();
	vec3 surface_to_eye_dir=normalize(U_CameraPos.xyz-V_WorldPos.xyz);
	vec3 BRDFOutPutColor=vec3(0.0);
	vec3 surface_color=GetSurfaceColor();
	vec3 ambientColor=GetAOColor(surface_color);
	float light_intensity[4]={4.0,4.0,4.0,4.0};
	vec4 light_pos[4]={
		vec4(-1.0,1.0,1.0,0.0),
		vec4(1.0,1.0,1.0,0.0),
		vec4(-1.0,-1.0,1.0,0.0),
		vec4(1.0,-1.0,1.0,0.0)
	};
	vec4 light_color[4]={
		vec4(1.0),
		vec4(1.0),
		vec4(1.0),
		vec4(1.0)
	};
	for(int i=0;i<4;++i){
		vec3 surface_to_light_source_dir=normalize(light_pos[i]);
		vec3 half_v_dir=normalize(surface_to_eye_dir+surface_to_light_source_dir);
		BRDFOutPutColor+=BRDFDirectional(surface_to_light_source_dir,surface_normal_dir,half_v_dir,surface_to_eye_dir,surface_color)*light_color[i].rgb*light_intensity[i];
	}
	gl_FragColor=vec4(BRDFOutPutColor,1.0);
}