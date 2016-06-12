
uniform highp mat4 ViewMatrix;
uniform highp mat4 ModelMatrix;
uniform highp mat4 ProjectionMatrix;

attribute vec4 Position;

void main()
{
    highp vec4 pos = ModelMatrix * Position;
    
    gl_Position = ProjectionMatrix * ViewMatrix * pos;
}
