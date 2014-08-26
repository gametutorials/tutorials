uniform sampler2D texture1;
uniform vec3 ColorKey;

void main()
{
	vec4 pixel = texture2D(texture1, gl_TexCoord[0].st);

	if(pixel[0] == ColorKey[0] && pixel[1] == ColorKey[1] && pixel[2] == ColorKey[2])
	{
		discard;
	}

	gl_FragColor = pixel;
}