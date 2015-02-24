// Global Light
// Brian Washburn
// 12-12-09

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform vec3 lightColor;
uniform vec3 lightSpecular;
uniform vec3 LightCoord;
uniform float lightIntensity;
uniform float lightRange;
varying vec3 theLightDir;
uniform mat4 lightMatrix;
uniform mat4 invViewMatrix;

void main (void)
{

	vec4 normal = texture2D(normalTexture, gl_TexCoord[0].st);
	
	vec3 N = normalize(normal.xyz);
	vec3 L = normalize(-theLightDir);
	
	float lambertTerm = dot(N.xyz,L);
	
	if(lambertTerm < 0.0) // front sides
	{
		discard;
	} 
	
	if (texture2D(diffuseTexture, gl_TexCoord[0].st).x == 0.0) {
		discard;
	}

	normal.xyz = (normal.xyz - 0.5) * 2.0;

	vec3 eyePos = (normal.a * gl_TexCoord[1].xyz); 
	
	vec4 worldPos = invViewMatrix * vec4(eyePos, 1.0);
	
	vec4 shadow = texture2DLod(shadowMap, (lightMatrix * worldPos).st / (lightMatrix * worldPos).w, 0.0);
	
	float dist = length(LightCoord.xyz - worldPos.xyz);

	vec4 final_color = vec4(lightColor.xyz , 0.0);
							
	vec3 E = normalize(-eyePos);
	vec3 R = reflect(-L, N);
	
	final_color.rgb += lightColor * lambertTerm;

	float lit_factor = (dist/200.0 <= shadow.r);
	float lit_factor2 = (dist > 180.0) ;
	
	//EXPONENTIAL SHADOWS
	
	float	depth_scale = 15.0f;
	float 	receiver = depth_scale * (dist / 200.0) - 0.001;
	float	shadowtemp = exp(3.0 * ( shadow.r * depth_scale - receiver )); 
	float occlusionFactor = smoothstep(0.1, 1.0, shadowtemp + 0.35);
	
	final_color.rgb *= vec3(max(occlusionFactor, lit_factor2));
	
	gl_FragColor = final_color;
			
}