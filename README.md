# unnamed_platformer

A platformer project I "borrowed" from @Zer0xCzk\
(To be fair, it wasn't even a prototype when I did)

Updates will probably be very infrequent and rare, but that mostly depends on my mood

## Level design:
Design is done via .txt files inside a designated levels/ folder\
(Currently you have to hardcode the name into the LevelGeneration() call)
### Example:
LevelGeneration("test") is translated as "levels/test.txt"
\
\
\
The .txt file is read by the code and the symbols are then translated to elements\
(Currently only the "N" for wall works, everything else is air)
### Example:
NNNNNN\
N....N\
NO.NFN\
NN.NNN\
(Elemets and their symbols are described in ElementCreation() function)
