# Snake Game in C

A classic Snake game written in C with the [raylib](https://www.raylib.com/) graphics library.

The snake's body is a **singly linked list** that I manage by hand (`malloc`/`free`): each move pushes a new head node and drops the tail, and eating an apple appends a new node.

## Features

- Grid-based movement at 18 FPS
- Random apple placement that never lands on the snake
- Collision detection against the walls and the snake's own body
- Score counter, pause screen with controls, and a clickable restart button
- Three game states: running, paused and game over

## Controls

| Key | Action |
| --- | --- |
| `W` `A` `S` `D` | Move up / left / down / right |
| `P` | Pause / resume |
| Mouse click on **RESTART** | Start a new game after a game over |

## Build and run (Linux)

Requirements: `gcc`, `make` and the X11 development libraries (raylib's static library and headers are already included in `lib/`).

```bash
sudo apt install build-essential libx11-dev libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev libgl1-mesa-dev
mkdir -p bin
make
./bin/snake
```

## Project structure

```
src/main.c     game logic, rendering and the linked-list snake
lib/           raylib static library and headers
makefile       build script
```

## License

MIT. See the header of `src/main.c`.
