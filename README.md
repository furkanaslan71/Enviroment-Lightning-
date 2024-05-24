# Computer Graphics II - Homework II
## Skybox Generation
This was a task that I've never done before so naturally my first move was to search through the internet to implement the skybox. Both slides and the sources I found on the internet were an implementation that does not apply to my case with their current state. All of them were generating the skybox from the six image files of an already prepared cube map. However, first I had to map the HDR equirectangular image to a cube map, and then generate the skybox from the cube map, so I learned to math behind it and then got to work.

My approach was putting the camera to the center of the world, making it look at the face of a cube (that is also at the center of the world, aligned with the axes and has an edge length of 1) and performing an off-screen rendering to capture the corresponding face's texture and repeat the process for the rest of 5 faces of the cube. The resulting texture was my texture for the skybox. Here is the fragment shader that maps the equirectangular image to the skybox faces' : 

```glsl
#version 460 core

out vec4 FragColor;

in vec3 fragWorldPos;

uniform sampler2D equirectangularMap;

const float PI = 3.14159265359;

void main()
{       
    vec3 direction = normalize(fragWorldPos); 
    float u = 0.5 + atan(direction.z, direction.x) / (2.0 * PI);    
    float v = 0.5 - asin(direction.y) / PI;
    vec3 color = texture(equirectangularMap, vec2(u, v)).rgb;
    FragColor = vec4(color, 1.0);
}
```

