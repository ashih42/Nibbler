# Nibbler
Classic game [Snake](https://en.wikipedia.org/wiki/Snake_(video_game_genre)), with online-multiplayer/first-person/3D/survival modes, in C++ using [SFML](https://www.sfml-dev.org/), [SDL2](https://www.libsdl.org/download-2.0.php), and OpenGL. (42 Silicon Valley)

<p float="left">
  <img src="https://github.com/ashih42/Nibbler/blob/master/Screenshots/sfml.png" width="280" />
  <img src="https://github.com/ashih42/Nibbler/blob/master/Screenshots/sdl.png" width="280" /> 
  <img src="https://github.com/ashih42/Nibbler/blob/master/Screenshots/opengl.png" width="285" />
</p>

To win each round, you must:
* Reach length of 10.
* (Multi-player) Survive longer than the opponent.

## Prerequisites

You are on macOS with `clang++`, `make`, and `brew` installed.

## Installing

```
./setup.sh
```

## Running

### 1-Player Mode

```
./Nibbler -l 1 boardWidth boardHeight
```

### Local 2-Player Mode

```
./Nibbler -l 2 boardWidth boardHeight
```

### Online 2-Player Mode

#### Server

```
./Nibbler -s port boardWidth boardHeight
```

#### Client

```
./Nibbler -c ipAddress port
```

## Controls

### Gameplay Controls
* **Player 1:** `Left Arrow`, `Right Arrow` Turn left or right.
* **Player 2:** `Numpad 4`, `Numpad 6` Turn left or right.
* `R` Restart new round.
* `Escape` Terminate the game.

### Graphics Controls
1. `1` Select **SFML** visualizer.
2. `2` Select **SDL2** visualizer.
    * `G` Toggle grid.
3. `3` Select **OpenGL** visualizer.
    * `W` Toggle wireframe mode.
    * `S` Toggle shading.
    * `V` Toggle first/third person POV.
