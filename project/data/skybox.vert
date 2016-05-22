uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mediump mat4 ProjectionMatrix;

varying highp vec4 texCoordVarying;
varying highp vec4 posVarying;

attribute vec4 TexCoord;
attribute vec4 Position;

void main()
{
    posVarying = ModelMatrix * Position;
    texCoordVarying = TexCoord;
    
    gl_Position = ProjectionMatrix * ViewMatrix * posVarying;
}