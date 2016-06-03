
uniform highp mat4 ViewMatrix;
uniform highp mat4 ModelMatrix;
uniform highp mat4 ProjectionMatrix;

uniform highp mat3 NormalMatrix;

uniform highp vec4 LightPos;
uniform highp vec4 EyePos;

uniform highp vec3 Ka;   // ambient material coefficient
uniform highp vec3 Kd;   // diffuse material coefficient
uniform highp vec3 Ks;   // specular material coefficient

uniform highp float Ns;   // specular material exponent (shininess)

uniform highp vec3 Ia;   // ambient light intensity
uniform highp vec3 Id;   // diffuse light intensity
uniform highp vec3 Is;   // specular light intensity

attribute vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec4 TexCoord;

varying highp vec4 ambientVarying;
varying highp vec4 diffuseVarying;
varying highp vec4 specularVarying;
varying highp vec4 texCoordVarying;

varying highp vec4 posVarying;        // pos in world space
varying highp vec3 normalVarying;     // normal in world space
varying highp vec3 tangentVarying;    // tangent in world space

void main()
{
    posVarying = ModelMatrix * Position;
    normalVarying = normalize(NormalMatrix * Normal);
    tangentVarying = normalize(NormalMatrix * Tangent);
    texCoordVarying = TexCoord;
    
    
    gl_Position = ProjectionMatrix * ViewMatrix * posVarying;
}
