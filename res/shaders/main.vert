varying vec4 vColor;
varying vec3 normal;

void main(void)
{
  gl_Position = ftransform();
  vColor = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0 * gl_TextureMatrix[0];
  normal = normalize(gl_Normal);
}