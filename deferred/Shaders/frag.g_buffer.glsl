// G-buffer render
// Brian Washburn
// 12-12-09

varying vec3 normal;

void main (void)
{						
	vec4 outputSpecular = pow(gl_FrontMaterial.specular, 2.2);
	outputSpecular.a = gl_FrontMaterial.shininess * 0.05555555;
	float depth = gl_FragCoord.z / gl_FragCoord.w / 200.0;
	
	gl_FragData[0] = pow(gl_FrontMaterial.diffuse, 2.2);	
	vec3 scaledNorm = (normalize(normal.xyz) * 0.5 + 0.5);
	gl_FragData[1] = vec4(scaledNorm.xyz, depth);
	gl_FragData[2] = outputSpecular;
}