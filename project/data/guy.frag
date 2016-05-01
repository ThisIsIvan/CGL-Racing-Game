
uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mediump mat4 ProjectionMatrix;

uniform mediump mat3 NormalMatrix;

uniform mediump vec4 LightPos;
uniform mediump vec4 EyePos;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform lowp vec3 Ia;   // ambient light intensity
uniform lowp vec3 Id;   // diffuse light intensity
uniform lowp vec3 Is;   // specular light intensity

uniform sampler2D DiffuseMap;
uniform sampler2D SpecularMap;
uniform sampler2D NormalMap;

varying lowp vec4 ambientVarying;
varying lowp vec4 diffuseVarying;
varying lowp vec4 specularVarying;
varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
varying mediump vec3 normalVarying;     // normal in world space
varying mediump vec3 tangentVarying;    // tangent in world space

void main()
{
    mediump vec4 pos = posVarying;
    mediump vec3 normal = normalVarying;
    
    lowp vec4 ambientVarying = vec4(Ka * Ia, 1.0);
    
    // TODO: calculate diffuse lighting
    lowp vec4 diffuseVarying = vec4(0.0);
    mediump vec3 n = normal;
    mediump vec3 l = normalize(vec3(LightPos - pos));
    
    lowp float intensity = dot(n, l);
    lowp vec3 diffuse = Kd * clamp(intensity, 0.0, 1.0) * Id;
    diffuseVarying = vec4(clamp(diffuse, 0.0, 1.0), 1.0);
    
    // TODO: If vertex is lit, calculate specular term in view space using the Blinn-Phong model
    lowp vec4 specularVarying = vec4(0.0);
    if (intensity > 0.0)
    {
        mediump vec3 eyeVec = normalize(vec3(EyePos - pos));
        mediump vec3 h = normalize((eyeVec + l) / length(eyeVec + l));
        
        mediump vec3 specular = Ks * pow(max(0.0, dot(n, h)), Ns) * Is;
        specularVarying = vec4(clamp(specular, 0.0, 1.0), 1.0);
    }
    
    lowp vec4 color = vec4(1.0);
    
    gl_FragColor = color * (ambientVarying + diffuseVarying + specularVarying);
}
