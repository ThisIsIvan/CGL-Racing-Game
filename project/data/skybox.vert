uniform lowp mat4 ViewMatrix;
uniform lowp mat4 ModelMatrix;
uniform lowp mat4 ProjectionMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;

varying lowp vec4 texCoordVarying;
varying lowp vec4 posVarying;

void main()
{
    posVarying = ModelMatrix * Position;
    texCoordVarying = TexCoord;
    
    gl_Position = ProjectionMatrix * ViewMatrix * posVarying;
}