void main()
{
	vec3 normal, lightDir;
	vec4 diffuseColor;
	float intensity;
	
	normal = normalize(gl_Normal);

	lightDir = normalize(vec3(gl_LightSource[0].position));
	intensity = max(dot(normal, lightDir), 0.0);

	diffuseColor = intensity * gl_LightSource[0].diffuse;
	gl_FrontColor =  diffuseColor + gl_LightSource[0].ambient;

	gl_Position = ftransform();
}
