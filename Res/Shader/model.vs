attribute vec4 position;
attribute vec4 texcoord;
attribute vec4 normal;
attribute vec4 tangent;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 U_NormalMatrix;
varying vec4 V_Normal;
varying vec4 V_WorldPos;
varying vec4 V_Texcoord;
varying mat3 V_TBN;
void main(){
	V_Normal=U_NormalMatrix*normal;
	V_WorldPos=ModelMatrix*position;
	V_Texcoord=texcoord;
    vec3 t=normalize(vec3(ModelMatrix*vec4(tangent.xyz,0.0)));
    vec3 n = normalize(vec3(U_NormalMatrix*normal));
    vec3 b=cross(n,t);
    V_TBN=mat3(t,b,n);
	gl_Position=ProjectionMatrix*ViewMatrix*ModelMatrix*position;
}