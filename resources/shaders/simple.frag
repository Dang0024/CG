#version 150

in vec3 pass_Normal;
in vec3 pass_FragPos;	// fragment position _ ass3
in vec3 pass_eyePos;	// eye (camera) position _ ass3

out vec4 out_Color;

// uniform values for Blinn-Phong reflection _ ass3
uniform vec3 lightSrc;
uniform vec3 diffuseColor;  // r, g, b color value

void main() {

  // values for shading
  vec3 ambientColor = diffuseColor;
  vec3 specularColor = vec3(1.0, 1.0, 1.0); // white
  float shininess = 24.0;

  // Adjusting and normalizing values before calculating angles
  vec3 normal = normalize(pass_Normal);
  vec3 light = normalize(lightSrc - pass_FragPos);
  vec3 eye = normalize(pass_eyePos - pass_FragPos); 
  vec3 halfway = normalize(eye + light);

  // Cosine values of each angle
  float diffuseAngle = max(0.0, dot(light, normal));
  float specularAngle = pow(max(0.0, dot(normal, halfway)), shininess);

  // final terms
  vec3 ambient = ambientColor;
  vec3 diffuse = diffuseColor * diffuseAngle;
  vec3 specular = specularColor * specularAngle;

  // final result
  out_Color = vec4(ambient + diffuse + specular, 1.0);
}
