#ifdef GL_ES
precision highp float;
#endif
varying vec4 V_Texcoord;
const float PI = 3.14159265359;
float SchlickGGX(float dot_N_X,float k){
	k=(k*k)/2.0;
    return dot_N_X / (dot_N_X * (1.0 - k) + k);
}
float SmithGF(vec3 normal, vec3 surface_to_eye, vec3 surface_to_light, float k){
    float dot_N_V = max(dot(normal, surface_to_eye), 0.0);
    float dot_N_L = max(dot(normal, surface_to_light), 0.0);
    return SchlickGGX(dot_N_V, k) * SchlickGGX(dot_N_L, k);
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
vec2 IntegrateBRDF(float NdotV, float roughness){
    vec3 ViewDir=vec3(sqrt(1.0 - NdotV*NdotV),0.0,NdotV);
    vec3 Normal = vec3(0.0, 0.0, 1.0);
    const uint SAMPLE_COUNT = 1024u;
    float F0_Scale = 0.0;
    float F0_Offset = 0.0;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i){
        vec2 Xi = HammersleyNoBitOps(i, SAMPLE_COUNT);
        vec3 RandomH  = ImportanceSampleGGX(Xi, Normal, roughness);
        vec3 L  = normalize(reflect(-ViewDir,RandomH));
        float NdotL = max(L.z, 0.0);
        float NdotH = max(RandomH.z, 0.0);
        float VdotH = max(dot(ViewDir, RandomH), 0.0);
        if(NdotL > 0.0){
            float G = SmithGF(Normal, ViewDir, L, roughness);
            float Fr = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);
            F0_Scale += (1.0 - Fc) * Fr;
            F0_Offset += Fc * Fr;
        }
    }
    F0_Scale /= float(SAMPLE_COUNT);
    F0_Offset /= float(SAMPLE_COUNT);
    return vec2(F0_Scale, F0_Offset);
}
void main(){
	vec2 color=IntegrateBRDF(V_Texcoord.x, V_Texcoord.y);
	gl_FragColor=vec4(color,0.0,0.0);
}