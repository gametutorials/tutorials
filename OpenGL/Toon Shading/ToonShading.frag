varying vec3 lightDir, normal;

void main()
{
	vec4 pixel;
	
	float intensity = dot(lightDir, normalize(normal));
	
	if (intensity > 0.85)
		pixel = gl_LightSource[0].specular;
	else if (intensity > 0.50)
		pixel = gl_LightSource[0].diffuse;
	else if (intensity > 0.25)
		pixel = gl_LightSource[0].diffuse * 2.0/3.0;
	else
		pixel = gl_LightSource[0].ambient;
	
	gl_FragColor = pixel;
} 
