#version 410 core

uniform sampler1D tex;
uniform vec2 center;
uniform float scale;
uniform int iter;

in vec2 fCoord;
out vec4 color;

void main()
{
    vec2 z, c;
    c.x = 1.3333 * (fCoord.x - 0.5) * scale - center.x;
    c.y = (fCoord.y - 0.5) * scale - center.y;
    int i;
    z = c;
    for(i=0; i<iter; i++) {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;
        if((x * x + y * y) > 4.0) break;
        z.x = x;
        z.y = y;
    }
    vec4 tcolor;

    if (i == iter)
    {
       tcolor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
       //tcolor = vec4(float(i) / iter, 0.0f, 0.0f, 1.0f);
       tcolor = texture(tex, float(i) / iter).rgba;
    }
    color = tcolor;
}