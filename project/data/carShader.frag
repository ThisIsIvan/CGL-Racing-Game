$B_SHADER_VERSION
#ifdef GL_ES
precision lowp float;
#endif

uniform sampler2D fbo_texture;
uniform float speed;

varying vec4 texCoordVarying;

void main()
{
    
    vec4 color = texture2D(fbo_texture,texCoordVarying.xy);
    vec4 color2 = texture2D(fbo_texture, vec2(0.0, 0.0));

    gl_FragColor = color;
}
