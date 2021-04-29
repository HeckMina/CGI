#ifdef GL_ES
precision highp float;
#endif
uniform sampler2D U_Texture;
varying vec4 V_Texcoord;
const float gamma=2.2;
vec3 GammaCorrection(vec3 color){
	return pow(color,vec3(1.0/gamma));
}
void main(){
    vec3 hdr_color = texture2D(U_Texture, V_Texcoord.xy).rgb;
    // reinhard tone mapping
    vec3 mapped = hdr_color / (hdr_color + vec3(1.0));
	gl_FragColor=vec4(GammaCorrection(mapped), 1.0);
}