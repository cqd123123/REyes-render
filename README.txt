Some result images are under the folder reyes/output/ the animation sequence is zipped to animation.rar
The sample scene is included in the source file, the name is TestScene.h. You can run different test scenes by change the
function call in main() function under main.cpp. The test scene has been modified a little bit to make it compatible with
the system I implemented. (The default camera position, light parameter ... etc may be different). The output folder is under
REYES/output .  The default image name is "frame0.bmp"

The MersenneTwister.h is a random number generator I use. The code is from a open source website.
Noise.h and noise.cpp is perlin noise generator, The code is also from a open source website, I tested it in my system.
However the noise it generated contains artifacts, so the code is not used in my system. Instead, I read displacement map
from an image instead. The displacement map is from the book "Real time rendering"

The rest of the code is written by me.

Image_buffer contains the image buffer class and texture class.
TestScene.h contains testscene I use.
Mesh.h contains mesh class.
Patch.h contains patch struct.
reyes_render.h contains major rendering pipline. The rendering is per patch basis, the patch is tesellated if maximum
edge of its bounding box project onto the image plane is larger than some threshold. The threshold level is given in main.cpp
RI_TESELLATION_LEVEL. It's a float point number, this number will be multiplied by pixel size. For example, when this number is 
0.5, the patch will be tesellated into patches, that the largest projected bounding box edge is smaller than half pixel size.

Ri.h contains rendering state variables, and functions to implement Ri calls

Sampler.h contains sample generator for pixels.
shader.h contains shader functions.
shape.h contains all geometry definition, sphere, torus, cone and cylinder.
Transform.h contains class Transform to handle all kinds of transformations.
util.h contains some ultility function like min, max etc.

The project depends on two libraries, Eigen and cimg. Eigen is a linear algebra library and cimg is a image library. Only
basic function of these two libraries are used. The library files have been merged into project and requires no installing.