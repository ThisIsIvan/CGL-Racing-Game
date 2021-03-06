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

uniform sampler2D DiffuseMap;
uniform sampler2D SpecularMap;
uniform sampler2D NormalMap;

varying highp vec4 ambientVarying;
varying highp vec4 diffuseVarying;
varying highp vec4 texCoordVarying;

varying highp vec4 posVarying;        // pos in world space
varying highp vec3 normalVarying;     // normal in world space
varying highp vec3 tangentVarying;    // tangent in world space

void main()
{
    highp vec4 ambientResult = vec4(Ka * Ia, 1.0);
    
    highp vec4 pos = posVarying;
    highp vec3 n = normalize(normalVarying);
    highp vec3 l = normalize(LightPos - pos).xyz;
    
    highp float intensity = dot(n, l);
    highp vec3 diffuse = clamp(intensity, 0.0, 1.0) * Id;
    highp vec4 diffuseResult = vec4(clamp(diffuse, 0.0, 1.0), 1.0);
    
    highp vec4 color = texture2D(DiffuseMap, texCoordVarying.st);
    gl_FragColor = (ambientResult + diffuseResult) * color;
}