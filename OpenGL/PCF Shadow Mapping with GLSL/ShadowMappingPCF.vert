varying vec4 projCoord;

void main()
{
	vec4 realPos = gl_ModelViewMatrix * gl_Vertex;
  
	projCoord = gl_TextureMatrix[0] * realPos;
	gl_FrontColor = gl_Color;

	gl_Position = ftransform();
}