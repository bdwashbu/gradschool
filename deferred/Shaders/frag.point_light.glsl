// Point Light Shading 
// Brian Washburn
// 12-12-09

uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
//uniform sampler2D currentScene;
uniform vec3 lightColor;
uniform vec3 lightSpecular;
uniform float lightIntensity;
uniform float lightRange;
varying vec3 theLightDir;
varying vec3 LightPos;

void main (void)
{
	vec4 normal = texture2D(normalTexture, gl_TexCoord[0].st);
	normal.xyz = (normal.xyz - 0.5) * 2.0;
	
	vec3 eyePos = (normal.a * gl_TexCoord[1].xyz); 
    float dist = length(LightPos - eyePos.xyz);
    
    if (dist > lightRange) {
		discard;
	}
    
	vec4 final_color = vec4(0.0);
							
	vec3 N = normalize(normal.xyz);
	vec3 L = normalize(LightPos - eyePos.xyz);	
	vec3 E = normalize(-eyePos);
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0) // front sides
	{
		vec3 R = reflect(-L, N);
		float shininess = texture2D(specularTexture, gl_TexCoord[0].st).a * 128.0;
		vec3 specular = texture2D(specularTexture, gl_TexCoord[0].st);
		final_color.rgb += lightColor * lambertTerm;
		float specular_index = pow( max(dot(R, E), 0.0), shininess);
		final_color.rgb += specular * lightSpecular * specular_index;
		final_color *= (1.0 - pow((dist / lightRange), lightIntensity)); // falloff

	} else {
		discard;
	}
	
	gl_FragColor = final_color;
			
}