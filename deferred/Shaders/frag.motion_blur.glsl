// Motion Blur
// Brian Washburn
// 12-12-09

uniform sampler2D currentScene;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
uniform sampler2D depthTex;
uniform mat4 prevInvViewMatrix;
uniform mat4 invViewMatrix;
uniform float sizeX;
uniform float sizeY;

void main (void)
{
	vec4 depth = texture2D(depthTex, gl_TexCoord[0].st);
	
	vec4 currentColor = texture2D(currentScene, gl_TexCoord[0].st);
	
	vec3 eyePos = (max(1.0-depth.a, 0.01) * gl_TexCoord[1].xyz); 
	
	vec4 thisWorldPos = invViewMatrix * vec4(eyePos, 1.0);
	thisWorldPos /= thisWorldPos.w;
	vec4 prevWorldPos = prevInvViewMatrix * vec4(eyePos, 1.0);
	prevWorldPos /= prevWorldPos.w;
	vec2 velocity = (thisWorldPos - prevWorldPos)/2.f;
	
	if (velocity == 0.0) {
		gl_FragColor = currentColor;
		return;
	}
	
	vec2 texCoord = gl_TexCoord[0].st;
		
	 for(int i = 1; i < 6; i++)  
	 {  
		currentColor += texture2D(currentScene, gl_TexCoord[0].st + velocity * vec2(sizeX,sizeY) * float(i) * 0.4);
     }  

	gl_FragColor = currentColor / 6.0;
			
}