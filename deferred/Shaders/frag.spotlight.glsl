// Differed Shading
// Brian Washburn
// 10-20-9

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
uniform sampler2D currentScene;
uniform vec3 lightColor;
uniform vec3 lightSpecular;
uniform float lightIntensity;
uniform float lightRange;
varying vec3 theLightDir;
varying vec3 LightPos;

void main (void)
{
	if (texture2D(diffuseTexture, gl_TexCoord[0].st).x == 1.0) {
		gl_FragColor = texture2D(currentScene, gl_TexCoord[0].st);
		return;
	}

	vec4 normal = texture2D(normalTexture, gl_TexCoord[0].st);
	
	normal.xyz = (normal.xyz - 0.5) * 2.0;
	
	float realDepth = normal.a;
	int skip = 0;
	vec3 worldPos = (realDepth * gl_TexCoord[1].xyz); 

	float shininess = texture2D(specularTexture, gl_TexCoord[0].st).a * 128.0;
	vec3 specular = texture2D(specularTexture, gl_TexCoord[0].st);

	vec4 final_color = vec4(0.0, 0.0, 0.0, 0.0);
							
	vec3 N = normalize(normal.xyz);
	vec3 L = normalize(LightPos - worldPos.xyz);
	vec3 E = normalize(-worldPos);
	vec3 R = reflect(-L, N);
	
	float lambertTerm = dot(N.xyz,L);
	float neglambertTerm = dot(-N.xyz,L);
	
	float specular_index = pow( max(dot(R, E), 0.0), shininess);
	
	if(lambertTerm > 0.0 && dot(-L, theLightDir) > 0.8) // outer radius
	{
		final_color.rgb += lightColor * max(lambertTerm, neglambertTerm);
		
		final_color.rgb += specular * lightSpecular * specular_index;

	} else if(lambertTerm > 0.0 && dot(-L, theLightDir) > 0.7) { // inner radius
	
		final_color.rgb += lightColor * max(lambertTerm, neglambertTerm);
		
		final_color.rgb += specular * lightSpecular * specular_index;
		
		final_color.rgb *= ((dot(-L, theLightDir) - 0.7) / 0.1);

	} else {
		gl_FragColor = texture2D(currentScene, gl_TexCoord[0].st);
		return;
	}
	
	gl_FragColor = texture2D(currentScene, gl_TexCoord[0].st) * exp2(-(final_color)); // LUV light accumulation
				
}