#version 330 core
in vec3 ourColor;
in vec3 Normal;  
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform bool lightToggle;

vec3 result;

void main()
{   
    if (lightToggle) {
        // ambient
        float ambientStrength = 0.25;
        vec3 ambient = ambientStrength * lightColor;
  	
        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // specular
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor; 
            
        result = (ambient + diffuse + specular) * ourColor;
    } else {
        result = ourColor;
    }
    FragColor = vec4(result, 1.0);
}