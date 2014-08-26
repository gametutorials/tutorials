uniform sampler2DShadow shadowMap;
varying vec4 projCoord;

void main ()
{
	const float kTransparency = 0.3;
	vec4 color = gl_Color;

	float rValue = shadow2DProj(shadowMap, projCoord).r + kTransparency;
	rValue = clamp(rValue, 0.0, 1.0);

	vec3 coordPos  = projCoord.xyz / projCoord.w;

	if(coordPos.x >= 0.0 && coordPos.y >= 0.0 && coordPos.x <= 1.0 && coordPos.y <= 1.0 )
   	{
		gl_FragColor = color * rValue;
	}
	else
	{
		gl_FragColor = color;
	}
}

