dependencies:
- imagemagick (only during build)
- x11
- xext
- xfixes


Adding a new animal:

- add its name to the list in `Makefile`

- copy `animals/Template.c` to `animals/<name>.c`
 - replace <name> in the file with the name of your animal
 - modify the parameters

- copy `animals/Template.png` to `animals/<name>.png`
 - create sprites
