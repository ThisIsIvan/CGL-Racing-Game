// Attributes
attribute float aTheta;

// Uniforms
uniform mat4 ProjectionMatrix;
uniform float uK;

void main(void)
{
    float x = 20.0; //cos(uK*aTheta)*sin(aTheta);
    float y = 20.0; //cos(uK*aTheta)*cos(aTheta);
    
    gl_Position = vec4(1.0, 1.0, 1.0, 1.0);
    gl_PointSize = 160.0;
}