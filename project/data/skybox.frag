uniform samplerCube CubeTexture;
varying highp vec4 texCoordVarying;

void main()
{
    gl_FragColor = texture2D(CubeTexture, texCoordVarying).rgba;
}