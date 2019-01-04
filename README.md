# The_hunter
Course project, which demonstrates ASCII -type turn based game 
About this program.
	Game "Hunter" proposes the player to take role of hares’ hunter, but it's not so easy. 
Hares are strong and aggressive. Good news - you can kill them, bad news - they can kill you as 
well. Very bad news - there are plenty of these evil creatures.
Rules:
	You play as a hunter, who has a rifle and traps. Hunter has 2 action points, which he 
can use on turning, moving, shooting, setting trap or use binocular. ('z'- shoot, 'x' - set trap, 
'c' - binocular, arrow keys - turn/move, Esc - in game menu).  Each action takes 1 action point, 
but moving always ends turn. You can shoot - but only directly right or up. The chance to hit - 
95% on 1-4 cells distance, 45% - 5-10 cells, 5% - more than 10 cells. Dead hare transforms into 
grave, which is barrier for hunter and hares. Bullets can't fly thru grave too. You can set a trap
 on cell you stand, but - if hunter remains on that cell to next move - he dies.
	 Also you can choose difficulty level: easy, medium or hard. On easy level you see entire 
field, on medium - sector before hunter of 2 cells right and up, and on hard level only one cell 
before hunter in direction he looks. You can use binocular, which shows cells in direction you 
looking, but only to closest hare or grave and only to next move.
There are 80 of hares in the field 30*60 cells. 15 closest of them move to hunter on each move. If 
hare stands on trap - it dies, and new grave appears, like after shooting. If hare stands on hunter 
cell - hunter dies.
After the game end you can record your result to table of records, which is sorted in descending
 order.
Creating the game:
	Creating the game, I used structures (Struct ) for different objects , in which I wrote 
properties of each game object. Characteristics are set as structure fields. Similar objects were 
united in dynamic arrays of structures, this allowed me to change their quantity correctly. Objects 
are linked with cells on playing field, with feedback (changes in object structure reassign to field,
 described as double-dimensioned array).  For working with screen (output of game objects and actions)
 I used WinAPI console functions, console descriptor etc. For better clearance of code, I separated 
different elements and actions in discrete functions. Interaction between functions is realized by 
transmitting structure arrays and parameters as functions arguments. I also used “fstream” for writing 
to file and reading from file (for save and load game, table of records). 
Difficulties:
	The most difficult thing in creating this game was not the complicated algorithm or abstruse 
logic, but accuracy and correct sequence of actions and setting the proper feedback. I had to transfer
 only necessary and sufficient data for each function. It was available with dynamic memory using.
