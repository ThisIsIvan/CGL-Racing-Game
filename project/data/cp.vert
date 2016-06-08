
uniform highp mat4 ViewMatrix;
uniform highp mat4 ModelMatrix;
uniform highp mat4 ProjectionMatrix;
attribute vec4 Position;
varying highp vec4 posVarying;

void main()
{
    posVarying = ModelMatrix * Position;    
    gl_Position = ProjectionMatrix * ViewMatrix * posVarying;
}