$B_SHADER_VERSION
#ifdef GL_ES
precision highp float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;

varying vec4 texCoordVarying;
varying vec2 v_blurTexCoords[14];

uniform float isVertical;

void main()
{
    texCoordVarying = TexCoord;
    vec4 calcPos = ProjectionMatrix * ModelViewMatrix * Position;
    v_blurTexCoords[ 0] = texCoordVarying.st + vec2(-0.028, 0.0);
    v_blurTexCoords[ 1] = texCoordVarying.st + vec2(-0.024, 0.0);
    v_blurTexCoords[ 2] = texCoordVarying.st + vec2(-0.020, 0.0);
    v_blurTexCoords[ 3] = texCoordVarying.st + vec2(-0.016, 0.0);
    v_blurTexCoords[ 4] = texCoordVarying.st + vec2(-0.006, 0.0);
    v_blurTexCoords[ 5] = texCoordVarying.st + vec2(-0.004, 0.0);
    v_blurTexCoords[ 6] = texCoordVarying.st + vec2(-0.002, 0.0);
    v_blurTexCoords[ 7] = texCoordVarying.st + vec2( 0.002, 0.0);
    v_blurTexCoords[ 8] = texCoordVarying.st + vec2( 0.004, 0.0);
    v_blurTexCoords[ 9] = texCoordVarying.st + vec2( 0.006, 0.0);
    v_blurTexCoords[10] = texCoordVarying.st + vec2( 0.016, 0.0);
    v_blurTexCoords[11] = texCoordVarying.st + vec2( 0.020, 0.0);
    v_blurTexCoords[12] = texCoordVarying.st + vec2( 0.024, 0.0);
    v_blurTexCoords[13] = texCoordVarying.st + vec2( 0.028, 0.0);
    
    gl_Position = calcPos;
}