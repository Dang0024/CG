#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;	// not needed? _ ass3

out vec3 pass_Normal;
out vec3 pass_FragmentPos;	// _ ass3
out vec3 pass_VertexInViewSpace;	// _ ass3

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	
	//pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
	pass_Normal = normalize(mat3(transpose(inverse(ModelMatrix))) * in_Normal);

	pass_FragmentPos = vec3(ModelMatrix * vec4(in_Position, 1.0));

	pass_VertexInViewSpace = normalize(inverse(ViewMatrix)[3].xyz - pass_FragmentPos);
}
