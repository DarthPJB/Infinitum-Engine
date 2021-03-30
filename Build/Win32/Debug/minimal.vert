// Vertex Shader – file "minimal.vert"
 
#version 140
 
in  vec3 vPosition;
in  vec3 vColor;
in  vec3 vNormal;
in  vec2 vTexCoord;

uniform  mat4 vProject;
uniform  mat4 vModelview;
uniform  mat4 vTranspose;

out vec2 TexCoord;
out vec4 ex_Color;
out vec2 DemoCoord;
 
void main(void)
{

	mat4 Matrix = vProject * vModelview;
	gl_Position = Matrix * vec4(vPosition, 1.0);
	vec3 EyeVec = -(vec3(Matrix * vec4(vPosition, 1.0))/12);

	//retarded math to calulate the depth of the point (ex_eyevec.z) minus the distance from the camera (-5)
	// normalised to 0.0->1.0 (/12) converted to inverse normal (1-(-value)) 1.0 -> 0.0
	float depth = 1-(EyeVec.z-5);

	DemoCoord = normalize(EyeVec).xy;
	TexCoord = clamp(vTexCoord,0,1);
	ex_Color = vec4(vColor,1.0);
}