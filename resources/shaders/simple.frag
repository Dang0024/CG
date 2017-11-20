#version 150

in vec3 pass_Normal;
in vec3 pass_FragPos;	// _ ass3
in vec3 pass_VertexInViewSpace;	// _ ass3

out vec4 out_Color;

// _ ass3
uniform vec3 lightSrc;
uniform vec3 ambientColor;  // r, g, b color value

void main() {
  vec3 diffuseColor = ambientColor;
  vec3 specularColor = vec3(1.0, 1.0, 1.0); // white
  float shininess = 24.0;

  vec3 normal = normalize(pass_Normal);
  vec3 light = normalize(lightSrc - pass_FragPos);
  vec3 halfway = normalize(pass_VertexInViewSpace + light);
  
  float diffuseAngle = max(0.0, dot(light, normal));
  float specularAngle = pow(max(0.0, dot(normal, halfway)), shininess);

  vec3 ambient = ambientColor;
  vec3 diffuse = diffuseColor * diffuseAngle;
  vec3 specular = specularColor * specularAngle;

  out_Color = vec4(ambient + diffuse + specular, 1.0);
}
