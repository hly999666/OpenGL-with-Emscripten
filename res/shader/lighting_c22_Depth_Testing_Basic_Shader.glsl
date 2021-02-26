//shader vertex
#version 300 es
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aNormal;
layout(location = 2) in vec2 aUV;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 uvScale;
out vec2 vUV;
void main()
{
	vUV = aUV* uvScale;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
//shader fragment
#version 300 es
precision highp float;
out vec4 FragColor;
uniform vec4 _lightColor;
uniform sampler2D diffuse_map;
uniform vec2 clipRange;
uniform int mode;


in vec2 vUV;
void main()
{
	 //FragColor =vec4(texture(diffuse_map, vUV).rgb,1.0);
	float ndc = gl_FragCoord.z * 2.0 - 1.0;
	float near = clipRange.x; float far = clipRange.y;
	float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near));
	linearDepth = (linearDepth-near) / (far-near);
	
	 
		FragColor = vec4(texture(diffuse_map, vUV).rgb, 1.0);
	  if (mode == 1) {
		  FragColor = vec4(vec3(linearDepth), 1.0);
	  }
	  else if (mode == 2) {
		  FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
	  }
	//FragColor = vec4(fract(vUV.x*2.0), fract(vUV.y * 2.0),0.0, 1.0);
}