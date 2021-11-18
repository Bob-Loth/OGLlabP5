CONTROLS
WASD: 	W,S moves camera forward/backward along w.
	 A,D moves camera left/right along u.
	Multiple keys can be held down to move in a diagonal motion.
	For default keyboard events from openGL, there is a a period of startup. I have since fixed it by only recording when 	WASD are pressed and released, and doing my own update per-frame, rather than whenever a keyboard event decides 	to register, which was not frequent enough to prevent really stuttery motion.
QE: Q moves light down -z, towards the goal. E moves light down +z, away from the goal.
M: changes the color of the goalie from shiny green to shiny yellow.
Z: wireframe
G: starts/stops the spline camera, detailed in SPLINE PATH.
-/=: the "plus" and "minus" keys, the ones not on the numpad, can be used to increase/decrease the mouse view sensitivity. 
C/V: decrease/increase the movement of the WASD keys. C slows the camera down, V speeds it up.

SPLINE PATH:
Start just above the edge of the pool opposite the main scene. Maintain a lookAt at the center of the goal's bounding box.
Spline 1 travels to the ball's first-rendered position.
Spline 2 travels from the ball's first-rendered position, to just to the right (+x) of the center of the goal's bounding box, making a small arc in the vertical (+y) direction on the way.

Pressing G for the first time will start the camera path, by teleporting the camera to the start point. You could make this smoother with an extra spline, but results were unsatisfying depending on distance from current eye position to the camera start position.

Pressing G during the camera path will stop the camera path, return motion and viewing control to the user.

Allowing the camera to complete its path without pressing G will return motion and viewing control to the user, at the position and orientation at the end of the path.

Pressing G after the camera path has completed will restart the camera path.


TEXTURED MESH
The water is grass.jpg, but color-shifted towards blue. I'm passing in alpha as a uniform variable, so that I can set it per-mesh.
I also experimented with UV-mapping in blender. I drew lines that resemble a water polo ball on a 3d ball mesh, and blender  generated a texture atlas from it, which is pretty cool.

COLORS
Shooter=shiny red
Goalie=shiny green
Goal=dark grey
Pool exterior+floor=light grey
Pool walls=blue (from lab5)
Ball=yellow+black(textured with ballTex.png)
Water(textured with water.jpg)
