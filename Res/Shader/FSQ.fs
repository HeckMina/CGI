#ifdef GL_ES
precision highp float;
#endif
uniform sampler2D U_Texture;
varying vec4 V_Texcoord;
void main(){
	vec4 color=texture2D(U_Texture, V_Texcoord.xy);
	gl_FragColor=color;
}