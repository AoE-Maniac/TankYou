uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 N;
uniform vec3 lightPos;

attribute vec3 pos;
attribute vec2 tex;
attribute vec3 nor;
varying vec2 texCoord;
varying vec3 normal;
varying vec3 lightDirection;
varying vec3 eyeCoord;

void kore() {
	eyeCoord = (V * M * vec4(pos, 1.0)).xyz;
	vec3 transformedLightPos = (V * M * vec4(lightPos, 1.0)).xyz;
	lightDirection = transformedLightPos - eyeCoord;
	
	gl_Position = P * vec4(eyeCoord.x, eyeCoord.y, eyeCoord.z, 1.0);
	texCoord = tex;
	normal = (N * vec4(nor, 0.0)).xyz;
}