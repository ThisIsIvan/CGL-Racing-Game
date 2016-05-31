$B_SHADER_VERSION
#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D fbo_texture;
uniform float speed;

varying vec4 texCoordVarying;
varying vec2 v_blurTexCoords[14];

void main()
{
    float factor[14];
    factor[0] = 0.0044299121055113265;
    factor[1] = 0.00895781211794;
    factor[2] = 0.0215963866053;
    factor[3] = 0.0443683338718;
    factor[4] = 0.0776744219933;
    factor[5] = 0.115876621105;
    factor[6] = 0.147308056121;
    factor[7] = 0.159576912161;
    factor[8] = 0.147308056121;
    factor[9] = 0.115876621105;
    factor[10] = 0.0443683338718;
    factor[11] = 0.0215963866053;
    factor[12] = 0.00895781211794;
    factor[13] = 0.0044299121055113265;
    
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    for(int i = 4; i < 10; i++){
        vec4 sampleColor = texture2D(fbo_texture, v_blurTexCoords[i])*factor[i];
        //if(sampleColor.w > 0.0){
            color += sampleColor;
        //}
    }
    color += texture2D(fbo_texture, texCoordVarying.st);
    gl_FragColor = vec4(2.5*color.x, 1.0*color.y, color.z, color.w);
}