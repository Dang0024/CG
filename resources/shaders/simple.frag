#version 150

in vec3 pass_Normal;
in vec3 pass_FragmentPos;	// _ ass3
in vec3 pass_VertexInViewSpace;	// _ ass3

out vec4 out_Color;

// _ ass3
uniform vec3 light;

void main() {

  vec3 ambientColor = vec3(0.6, 0.0, 0.0);
  vec3 diffuseColor = ambientColor;
  vec3 specularColor = vec3(1.0, 1.0, 1.0); // white
  float b = 24.0;

  vec3 lightAngle = normalize(light - pass_FragmentPos);
  vec3 halfway = normalize(pass_VertexInViewSpace + lightAngle);
  
  vec3 ambient = ambientColor;
  vec3 diffuse = diffuseColor * max(0, dot(lightAngle, pass_Normal));
  vec3 specular = specularColor * pow(max(0, dot(pass_Normal, halfway)), b);

  out_Color = vec4(ambient + diffuse + specular, 1.0);
}
