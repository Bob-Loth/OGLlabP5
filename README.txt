The player can swim around a small pool, and throw an infinite amount of water polo balls around, and observe the pretty particles and wave physics. They can also make some slight adjustments to the wave physics, and try shooting the ball into the goal.

Compiling/Running: Basic mkdir build && cd build && cmake .. && make && ./P3Base works fine.
The particle effects can get a little taxing, so if performance becomes an issue, reducing them by a factor of 5-10 should resolve it.

Most of the work for the final project involved playing around with hierarchical modeling, collision detection, forces on the ball, particle generation and mesh deformation. There's a generic third-person camera centered behind the player's view vector, which moves and rotates with the player. The player rotates in place, not the camera, so rotating the camera doesn't affect the world position of the player.


TOPICS:
Hierarchical Modeling:
Not much has changed on this part. The goalie and shooter still have their basic time-based animations. I added a kicking motion to the shooter.

Collision Detection:
The player and ball both have collision detection enabled with different bounding boxes, each around the area of the pool's water. The player is bounded to the pool's edges, with a 6m-ish restricted area around the goal. The ball is also bounded to the pool's edges, without the goal restriction. There's currently no collision detection for the goal, as most of the technology for the project is in the particle effects and the waveTex shader.

Forces:
The ball bounces off of the invisible pool walls, losing some speed each time it does. When it is below the water, it experiences a buoyancy force that is based on its depth, with the force of gravity and the buoyancy force being roughly equivalent at the water's surface (due to the ball being a floating object), and the buoyancy force increases with depth. The ball also loses a considerable amount of velocity, especially in the X-Z direction, due to the drag of the water.

Particle Generation:
Generates sparkling blue/white particles based on the position of the splash, and the velocity of the ball upon impact. The splash data can store up to 5 splashes, before the particles get re-used.

Mesh Deformation:
The new mesh is much denser, and uses waveTex.glsl to determine where each vertex should be, based on the position and time of the last 5 splashes of the ball. Currently, the height of the deformation is run-time adjustable with the +/- keys, but there are a lot of other numbers in waveTex.glsl that are good to play around with, notably wavelength, propagationAdjust, distance, what factors of splashForce are used for the magnitude of offset, and remaining.


PROBLEMS/WEIRDNESS:
Most of this physical rendering I did without factoring in frametime, which can make the addition of 5 particle systems, each keeping track of 300 particles, affect some things. The most notable one is the time it takes for the ball to reset. It resets to the hand position after 10 seconds by default, which can be frustrating if, say, a Raspberry Pi can't process the particles fast enough for the ball to hit the water after a lobbed throw. This is slightly remedied by adjusting the particles on a per-system basis, which improves frametime overall, but the actual fix would be to do the reset after a particular number of frames elapsed, or some other way to take into account the different rendering speeds on different GPUs and display refresh rates.

Another trouble is that each particle system does its own in-system sorting, but that sorting doesn't extend to the other particle systems. This isn't really apparent visually until you do things like throw the ball at the water to splash in one direction, then quickly swim to the other side of the pool and splash the water in the other direction. Then you will start to see that the particles of the two different particle systems are not being rendered in the correct order. You can fix that by including all the particles into one particle system, then sectioning off their behaviors based on what "splash" you want to attach a particular particle to.

Currently the collision detection of the water is pretty basic, and it's not based on the current vertex position (during a wave), as that can get expensive and complex very fast.

CONTROLS
WASD: 	W,S moves camera+player forward/backward along w.
	 A,D moves camera+player left/right along u.
	Multiple keys can be held down to move in a diagonal motion.
	For default keyboard events from openGL, there is a a period of startup. I have since fixed it by only recording when 	WASD are pressed and released, and doing my own update per-frame, rather than whenever a keyboard event decides 	to register, which was not frequent enough to prevent really stuttery motion.
QE: Q moves light down -z, towards the goal. E moves light down +z, away from the goal.
M: changes the color of the goalie from shiny green to shiny yellow.
Z: wireframe
G: starts/stops the spline camera, detailed in SPLINE PATH.
-/=: the "plus" and "minus" keys, the ones not on the numpad, can be used to increase/decrease the waveSize parameter, which affects the height of the offsetY's added to the vertex data.
C/V: decrease/increase the movement of the WASD keys. C slows the camera and the player down, V speeds it up.



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
Water(textured with water.jpg, ~1000x1000 vertex mesh generated by blender)
