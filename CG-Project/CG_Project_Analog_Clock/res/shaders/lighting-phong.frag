#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texSampler1;


out vec4 fragColor;

void main()
{


//ambient light
float ambientFactor = 0.2f;
vec3 ambient = lightColor * ambientFactor;

//diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float NdotL = max(dot(normal, lightDir),0.0f);
	vec3 diffuse = lightColor * NdotL;

//specular
float specularFactor = 0.8f;
float shininess = 40.0f;
vec3 viewDir = normalize(viewPos - FragPos);
vec3 reflectDir = reflect(lightDir, normal);
float RdotV = max(dot(reflectDir, viewDir),0.0f);
vec3 specular = lightColor * specularFactor * pow(RdotV,shininess);


	vec4 texel = texture(texSampler1, TexCoord);
	fragColor = vec4(ambient + diffuse + specular, 1.0f)*texel;
	//fragColor =  texture(texture_map, TexCoord);;

}
