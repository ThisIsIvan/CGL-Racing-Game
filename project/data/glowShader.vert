$B_SHADER_VERSION
#ifdef GL_ES
precision highp float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;

varying vec4 texCoordVarying;
varying vec4 calcPos;
//varying float hasParticle;
varying vec2 uv;
uniform float isVertical;

varying vec2 v_blurTexCoords[14];

void main()
{
    texCoordVarying = TexCoord;
    calcPos = ProjectionMatrix * ModelViewMatrix * Position;
    gl_Position = calcPos;
    uv = (vec2( gl_Position.x, gl_Position.y ) + vec2(1.0) ) / vec2(2.0);
}
