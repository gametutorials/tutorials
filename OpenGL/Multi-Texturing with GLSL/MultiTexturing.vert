uniform float wrap;

void main()
{		
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec4 textureCoords = gl_MultiTexCoord0;
	textureCoords.x -= wrap;

	gl_TexCoord[1] = textureCoords;
	
	gl_Position = ftransform();
} 

