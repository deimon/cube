uniform sampler2D texture;
varying vec4 vColor;
varying vec3 normal;

void main(void)
{
  vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
  
  if(texColor.a == 0.)
    discard;
    
  //vec3 L = normalize(vec3(0.5, 0.5, 0.5));
  //float nDotL = max(dot(normal, L), 0.3);
  //gl_FragColor = vColor * texColor * nDotL;
  gl_FragColor = vColor * texColor;
}