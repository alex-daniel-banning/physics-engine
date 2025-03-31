#version 460 core
out vec4 FragColor;

uniform vec4 objectColor;
void main()
{
   FragColor = vec4(objectColor);
   //FragColor = vec4(0.5, 0.0, 0.0, 1.0);
};
