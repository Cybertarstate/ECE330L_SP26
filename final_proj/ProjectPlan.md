
Step1: Display "BattleShip"

Step2: Place Boats. 3 single segment boats, 2 double segment boats 
	2b.Player 1 - 5 boats
	2c.Player 2 - 5 boats

Game control:

Screen: Switches back and forth
		2 dimensional arrays


Potentiometer: [Cursor] - vertical and horizontal via switch,
	cursor needs to blink.


Switch:

Piezo Buzzer:

Light Sensor:

PC10/PC111 Buttons:





Gameplay/Rules:
boats: 3 single segment, 2 double segment

map: 

The map consists of two separate
bitmaps, one for the vertical segments (vertically aligned boats) which is an array
of 2 by 16 elements (32 total), and the other for horizontal segments (horizontally
aligned boats) which is an array of 3 by 8 elements (24 total).


```
Note that while you could choose to use an actual array variable for each of these
maps, the most memory efficient method is to use two unsigned integers. The
vertical segments can be allocated to the upper 16 bits of a 32 bit integer for the top
row of vertical segments, and the lower 16 bits of a 32 bit integer for the bottom
row of vertical segments. The three rows of horizontal segments can be represented
by three 8 bit bytes withing a 32 bit integer. 1’s would represent a boat and 0’s
empty space.
Player 1 places, player 2 places
Thus, if two unsigned integers are used to store the boat maps,
there will be two such integers for player one’s maps and two for player two’s
maps.

```

Cursor:


Damage: shown by blinking, and or duty cycle LED dimming for dead ship
lab(10)


Miss:solid red dot

Sound: setup,
player a, 
player b, 
launch, 
miss, 
final, 
hit.

Cursor Placement: Fast Blinky




