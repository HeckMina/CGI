#ifdef GL_ES
precision highp float;
#endif
uniform sampler2D U_HDRTexture;
uniform float U_Exposure;
varying vec4 V_Texcoord;
void main(){
	const float gamma = 2.2;
    vec3 hdr_color = texture2D(U_HDRTexture, V_Texcoord.xy).rgb;
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdr_color * U_Exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
	gl_FragColor=vec4(mapped, 1.0);
}