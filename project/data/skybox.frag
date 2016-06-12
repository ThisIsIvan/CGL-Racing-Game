uniform sampler2D DiffuseMap;
varying highp vec4 texCoordVarying;

void main()
{
    highp vec4 color = texture2DProj(DiffuseMap, texCoordVarying);
    gl_FragColor = color;
}