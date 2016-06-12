uniform highp mat4 ViewMatrix;
uniform highp mat4 ModelMatrix;
uniform highp mat4 ProjectionMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;

varying highp vec4 texCoordVarying;
varying highp vec4 posVarying;

void main()
{
    posVarying = ModelMatrix * Position;
    texCoordVarying = TexCoord;
    
    gl_Position = ProjectionMatrix * ViewMatrix * posVarying;
}