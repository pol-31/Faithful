Due to this https://www.glfw.org/docs/3.3/input_guide.html :
"""
GLFW provides many kinds of input. While some can only be polled, like time,
or only received via callbacks, like scrolling, many provide both callbacks and
polling. Callbacks are more work to use than polling but is less CPU intensive
and guarantees that you do not miss state changes.
"""
Decision:
- For actions which requires some smooth mechanic (moving, camera rotation and
    other things, possibly based on acceleration) we use pooling;
- For the rest of actions - only callbacks.

font: https://youtu.be/2X7RR9SaN_c?si=cnz9gLreMwK2nf0y
general appearance: Decapitated - Veins


** some useful English vacabulary: подолати перешкоду, ніхто не заважає тобі ʼщось робитьʼ, заважати комусь

There are two (by now) mode:
battle_mode: attack, block, deflect;
peacefull_mode: speaking with people, mb some other peacefull actions

to switch __battle_mode__ to __peacefull_mode__ press "F"


THERE IS NO JUMP, but stil exist some obstackes which can be подолані using key "E"

Both modes supports evading:
__EVADE__________________________________________________
| mouse position [0;360] affects which direction will be evading
| to evade simply press __alt__ (direction is already set by mouse position)
|________________________________________________________

_________________BATTLE MODE_____________________________
| in battle mode appears disk around the player which
|      shows vector of needed action (block, deflect, attack)
| mouse position [0;360] affects vector of that action direction (its also represented by that disk)
| ------------------------------------------------------------
| BLOCK:
| Space - block, 1 fast press (like click) allows to "deflect" attack, holding - default
|       blocking (time of holding is limited and depends on stamina),
| while blocking (while holding space bar) you can press 1-3 (1-n) modes of blocking
|       for more precise blocking which affects enemy's stamina (колящий/рубящий/режущий)
| by default mode is 1 and it allow to block any __weak__ enemy
|
| some weapons not allow you to block, then deflection/evading is the key
| some weapons cannot be blocked by some weapons (or too hard and requires high skill)
|
| mouse position [0;360] affects which direction will be blocking
| ------------------------------------------------------------
| ATTACK:
|
| to attack user has to press 1-9 keyboard buttons (which attack is binded to specific key and user can rebind for comfortness)
| attacks should be performed in timing if you wanna combo (each next attack is harder)
| but nobody is taking you out of just spamming: press whatever you want, but this takes a way more stamina
|
| mouse position [0;360] affects which direction will be attacking
|
| holding right mouse button slow down time around and allow you to choose skill (like throw sand in the eyes OR shariken/knife or some _special_ skills (may be based on class))
| holding left mouse button slow down time around and allow you to choose consumable item like some potion (user may rebind for confortness)
|   for this both mechanic like this: disk with n (assume n = 9) sections appears and depends on cursor move vector it will take appropriate item:
|      for example if we moving cursor strictly up, then vector = [0; 1] and we taking item #1; if we moving in direction [0.2; 0.7] -> item 2 and clockwise so on
|
|
| to use throwable weapon you need firstly choose direction by mouse
|______________________________________________________________________________


There is no Map_scene, BUT there is wide spectr of LOD (Ctrl + +/-)
smooth transitions, but there is fixed amount of LOD-levels
(affects tesselation, model/effect detalization)
Rotations: free 360-degree rotations

