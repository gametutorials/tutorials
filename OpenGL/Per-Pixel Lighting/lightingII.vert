varying vec3 normal, lightDir, halfVector;
	
void main()
{	
	vec4 worldPos;

	normal = normalize(gl_NormalMatrix * gl_Normal);
		
	worldPos = gl_ModelViewMatrix * gl_Vertex;
	lightDir = normalize(vec3(gl_LightSource[0].position - worldPos));
	
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);
	
	gl_Position = ftransform();
} 

