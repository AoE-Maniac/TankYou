#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D tex;
uniform vec4 tint;

varying vec2 texCoord;
varying vec3 normal;
varying vec3 lightDirection;
varying vec3 eyeCoord;

void kore() {
	
	const float amb = 0.4;
	const float diff = 0.5;
	const float spec = 0.5;
	const float n = 0.5;

	vec4 ambient = vec4(amb, amb, amb, 1.0);

	vec3 nor = normalize(normal);
	vec3 lightDir = normalize(lightDirection);
	vec4 diffuse = max(dot(lightDir, nor), 0.0) * vec4(diff, diff, diff, 1.0);
	
	vec3 halfVector = normalize(lightDir - normalize(eyeCoord));
	vec4 specular = pow(max(0.0, dot(halfVector, nor)), n) * vec4(spec, spec, spec, 1.0);

	vec4 light = ambient + diffuse + specular;

	gl_FragColor = light * texture2D(tex, texCoord) * tint;
}
