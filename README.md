# Computer Graphics II - Homework II
## Skybox Generation
<!-- ![Equirectangular HDR Image][]  -->
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
## Environment Lightning

The algorithm was very straightforward: The surrounding image of the scene's centroid must be found. The centroid is the point where the region is divided into two parts that have the same energy level both horizontally and vertically. Then a light is generated at the position of the centroid, and its color value must be the total color value of the region (not average, the sum). That was just the 0th level of the algorithm which means 0 division and a `2^0` number of lights. If the level increases, instead of generating the light from the centroid, we must divide the region from the centroid and from the longest dimension, and then apply the same process for the resulting regions and then generate `2` lights from their centroids and so on. Due to the performance limitations, the maximum number of lights for us to implement was `2^7 = 128 `. 
