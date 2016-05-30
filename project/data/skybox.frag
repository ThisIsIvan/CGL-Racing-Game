uniform sampler2D DiffuseMap;
varying lowp vec4 texCoordVarying;

void main()
{
    lowp vec4 color = texture2DProj(DiffuseMap, texCoordVarying);
    gl_FragColor = color;
}