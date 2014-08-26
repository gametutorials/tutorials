uniform sampler2DShadow shadowMap;
varying vec4 projCoord;

void main ()
{
	const float kTransparency = 0.3;
	vec4 color = gl_Color;

	vec3 shadowUV = projCoord.xyz / projCoord.q;
	float mapScale = 1.0 / 512.0;

	vec4 shadowColor = shadow2D(shadowMap, shadowUV);

	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3( mapScale,  mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3( mapScale, -mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3( mapScale,  	  0, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(-mapScale,  mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(-mapScale, -mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(-mapScale,  	  0, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(        0,  mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(        0, -mapScale, 0));

	shadowColor = shadowColor / 9.0;

	shadowColor += kTransparency;
	shadowColor = clamp(shadowColor, 0.0, 1.0);

	if(shadowUV.x >= 0.0 && shadowUV.y >= 0.0 && shadowUV.x <= 1.0 && shadowUV.y <= 1.0 )
	{
		gl_FragColor = color * shadowColor;
	}
	else
	{
		gl_FragColor = color;
	}
}