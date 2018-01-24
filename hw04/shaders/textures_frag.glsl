#version 330 core

precision highp float;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture1;
uniform int mode;

float f(vec2 p)
{
	return texture(ourTexture1,p).x - 0.5;
}

vec2 grad(vec2 p)
{
    vec2 h = vec2( 4./128.0, 0.0 );
    return vec2( f(p+h.xy) - f(p-h.xy),
                 f(p+h.yx) - f(p-h.yx) )/(128.0*h.x);
}

void main()
{
    if (mode == 0) {
//        vec3 c = texture(ourTexture1,TexCoord).xxx;
//        vec3 res = smoothstep(.5, .5, c);
//        color = vec4(res,1.0);
        float v = f(TexCoord);
        vec2  g = grad(TexCoord);
        float c = (v)/length(g);
        float res = c * 300.;
        color = vec4(res,res,res, 1.0);
    }
    else if (mode == 1) {
        vec3 c = texture(ourTexture1,TexCoord).xxx;
        vec3 c1 = (c-.45) * 40.;
        vec3 c2 = 1.-(c-.5) * 40.;
        vec3 res = mix(c1, c2, (c-.5)*40.);
        color = vec4(res,1.0);
    }
    else if (mode == 2) {
        vec3 c = texture(ourTexture1,TexCoord).xxx;
        vec3 c1 = clamp((c-.5)*40.,0.,1.);
        vec3 c2 = clamp(1.-(c-.5)/5., 0., 1.);
        vec3 res = 1.-mix(c1, c2, (c-.5)*40.);
        color = vec4(res,1.0);
    }
    else if (mode == 3) {
        vec3 c = texture(ourTexture1,TexCoord).xxx;
        vec3 gc = texture(ourTexture1,TexCoord + vec2(-0.02,0.02)).xxx;
        vec3 c1 = clamp((c-.5)*40.,0.,1.);
        vec3 c2 = clamp(1.-(gc-.5)/2., 0., 1.);
        vec3 res = 1.-mix(c1, c2, (c-.5)*40.);
        color = vec4(res,1.0);
    }
}