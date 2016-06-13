
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

uniform bool braking;

uniform sampler2D DiffuseMap;
uniform sampler2D SpecularMap;
uniform sampler2D NormalMap;

varying highp vec4 ambientVarying;
varying highp vec4 diffuseVarying;
varying highp vec4 specularVarying;
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
    highp vec3 diffuse = /*Kd */ clamp(intensity, 0.0, 1.0) * Id;
    highp vec4 diffuseResult = vec4(diffuse, 1.0);
    
    // If vertex is lit, calculate specular term in view space using the Blinn-Phong model
    highp vec4 specularResult = vec4(0.0);
    if (intensity > 0.0)
    {
        highp vec3 eyeVec = normalize(EyePos.xyz - pos.xyz);
        highp vec3 h = normalize(l + eyeVec);
        
        highp float specIntensity = pow(max(dot(h,n), 0.0), Ns);
        highp vec3 specular = Ks * clamp(specIntensity, 0.0, 1.0) * Is;
        specularResult = vec4(specular, 1.0);
    }
    
    highp vec4 color = texture2D(DiffuseMap, texCoordVarying.st);
    if(color.x > 0.4 && color.z < 0.4){
        ambientResult = vec4(0.5, 0.25, 0.0, 1.0);
        diffuseResult = vec4(0.5, 0.25, 0.0, 1.0);
        if(braking){
            ambientResult = vec4(0.6, 0.2, 0.0, 1.0);
            diffuseResult = vec4(0.6, 0.2, 0.0, 1.0);
        }
    }
    gl_FragColor = (ambientResult + diffuseResult+ specularResult) * color;
}
