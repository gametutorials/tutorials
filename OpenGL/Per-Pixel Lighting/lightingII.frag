varying vec3 normal, lightDir, halfVector;
	
void main()
{
	vec4 diffuseLight, specularLight;	
	float shine = 64.0;
	vec3 n = normalize(normal);

	float lightIntensity = max(dot(n, normalize(lightDir)), 0.0);

	diffuseLight = lightIntensity * gl_LightSource[0].diffuse;
	specularLight = pow(max(dot(n, normalize(halfVector)), 0.0), shine) * gl_LightSource[0].specular;

	gl_FragColor = diffuseLight + specularLight + gl_LightSource[0].ambient;
}