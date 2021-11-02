CONTROLS
WASD: W,S increases/decreases camera height, A,D rotates around center point
QE: Q moves light down -z, towards the goal. E moves light down +z, away from the goal.
M: changes the color of the goalie from shiny green to shiny yellow.
Z: wireframe
GH: Zoom in/out.

DESCRIPTION
I found a free pool model! However, I think there's an error in how the stairs are drawn, and the ordering/grouping of the shapes in the mesh seems nonsensical.
Free is free, I guess.
I added a shooter, with some basic hierarchical transforms for the upper body, along with a ball that's defined based on the position of the hand.
Right now, until I figure out a sane way to smoothly transition the ball from "dummy-space" to world space at the end of the throw, and determine the direction the ball should travel based on the position of the shooter, the shooter will be pump-faking.
I also think I will need to eventually graduate from sine-wave animation to make something a bit jerkier/asymmetric, especially for the legs. 

TEXTURED MESH
The water is grass.jpg, but color-shifted towards blue. I also enabled blending, and set the texture shader to render any textured mesh with an alpha value of 0.5, so the result color is 50% "water" and 50% of the surface behind it.

COLORS
Shooter=shiny red
Goalie=shiny green
Goal=dark grey
Pool exterior+floor=light grey
Pool walls=blue (from lab5)
Ball=yellow

NO NORMALS
the actual code is found in Shape.cpp, under computeNormals.
Demonstration of handling a mesh with no normals included in default execution, the 
mesh with no normals is the sphere from icoNoNormals.obj , located in the scene in the hand of the shooter (the ball).