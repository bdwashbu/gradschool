// Apply Light - Applies the accumulated light buffer to the scene.
// Brian Washburn
// 12-12-09

uniform sampler2D lightMap;
uniform sampler2D diffuseTexture;
uniform sampler2D depthTexture;

void main (void)
{
	vec4 light = texture2D(lightMap, gl_TexCoord[0].st);
	vec4 diffuse = texture2D(diffuseTexture, gl_TexCoord[0].st);
	vec4 depth = texture2D(depthTexture, gl_TexCoord[0].st);
	float fog = pow(depth.a, 3.0);
	
	vec3 convertedLight = -log2(light) - 1.0;
	
	//gl_FragColor = pow(diffuse * vec4(convertedLight.xyz, 1.0), 1.0 / 2.2);
	
	gl_FragColor = (light * diffuse) + vec4(fog);

			
}