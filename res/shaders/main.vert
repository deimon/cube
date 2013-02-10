varying vec4 vColor;
varying vec3 normal;
uniform float sun;

attribute float light;
attribute float locLight;

void main(void)
{
  gl_Position = ftransform();
  vColor = gl_Color * clamp(light * sun + locLight, 0.0, 1.0);
  vColor.a = gl_Color.a;
  gl_TexCoord[0] = gl_MultiTexCoord0 * gl_TextureMatrix[0];
  normal = normalize(gl_Normal);
}