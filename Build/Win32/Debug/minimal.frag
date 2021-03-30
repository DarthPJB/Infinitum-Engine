// Fragment Shader – file minimal.frag
 
#version 140
 
precision highp float;  //needed only for version 1.30

 
in  	vec2 DemoCoord;		//colour outputted by engine
in  	vec2 TexCoord;

uniform sampler2D vTexture;
uniform sampler2D vTexture2;

out vec4 gl_FragColor;		//the colour that should be renderificated to the screen.
 

void main(void)
{

	vec4 Color = texture2D(vTexture, TexCoord);  //texture
	//vec4 Color = texture2D(vTexture, DemoCoord);
	vec4 Color2 = texture2D(vTexture2, DemoCoord);	//background image

	gl_FragColor = (Color) + (Color2/3);
}