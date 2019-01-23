#version 420 core

uniform vec4 TINT;

layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec4 passColor; // NEW, passed color
in vec2 passUV; 

out vec4 outColor; 
  
void main() 
{
   vec4 diffuse = texture( gTexDiffuse, passUV ) * TINT;
   
   // multiply is component-wise
   // so this gets (diff.x * passColor.x, ..., diff.w * passColor.w)
   outColor = diffuse * passColor;  
}