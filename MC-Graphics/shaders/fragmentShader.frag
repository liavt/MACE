#version 330 core


in vec2 textureCoord;

out vec4 color;

uniform vec4 augmentColor;
uniform float augmentColorStrength;

uniform sampler2D tex;

void main (void)  
{     
   color= mix(augmentColor,texture(tex,textureCoord),augmentColorStrength);
}       