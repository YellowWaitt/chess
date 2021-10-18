# Chess

![](https://github.com/YellowWaitt/chess/blob/main/presentation.png)

A chess game I made to practice C++ programming and test some concept with the language.

The game contains several game mode :
- the classic chess
- [chess960](https://en.wikipedia.org/wiki/Fischer_random_chess) where the starting position is randomized
- king of the hill (KOTH), where you need to reach first the center with your king to win
- three cheks, you loose the game if your king is checked three times
- horde, the black need to eat all the white pawns to win

There is also a very basic AI using the [minmax](https://en.wikipedia.org/wiki/Minimax) algorithm (it may beat you in the horde mode if you are not carreful).

You can color squares using the right click button and draw arrows by keeping the right button press on a square and releasing it on another square.

## Building

You will need `gtkmm3` and the `glib-compile-resources` command to compile the game. On windows you can obtain them using https://www.msys2.org/, see the [documentation](https://gtkmm.org/en/download.html) for more details.

Once you have everything, download the repositorie and use the Makefile like so:
```
make resources
make
````
