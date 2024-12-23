#version 330 core
in vec4 ParticleColor;
out vec4 FragColor;

void main()
{
    float dist = length(gl_PointCoord - vec2(0.5));
    if (dist > 0.5)
        discard;
    
    FragColor = ParticleColor;
}