#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 color;
uniform sampler2D texture_diffuse1;
uniform bool useTexture;

void main()
{
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * lightColor;

   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;

   // sample texture
   vec3 surfaceColor;
   if (useTexture) {
        surfaceColor = texture(texture_diffuse1, TexCoords).rgb;
   } else {
        surfaceColor = color;
   }

   // combine lighting and texture
   vec3 result = (ambient + diffuse) * surfaceColor;

   FragColor = vec4(result, 1.0);
};
