# 🏃 The Rescue

A 2D platformer built in **C++** with **SDL3**, developed as a final exam project for a game development course. The player must jump across platforms and reach the exit zone without falling into the void.

> 📸 *(Gameplay GIF/screenshot pending — coming soon)*

---

## 🎮 About the game

Guide the character across a hand-crafted level made of solid platforms at different heights, timing jumps carefully to avoid falling off the map. Reach the green exit tile to win.

- **Win condition:** touch the exit zone.
- **Lose condition:** fall off the map 3 times (3 lives total).

---

## 🕹️ Controls

| Key | Action |
|---|---|
| `A` / `←` | Move left |
| `D` / `→` | Move right |
| `SPACE` / `W` / `↑` | Jump (only while grounded) |
| `ESC` | Quit game |

---

## ✨ Features

- **Physics-based movement:** constant gravity (1000 px/s²) and a fixed jump impulse (−450 px/s), only triggered while grounded.
- **Tile-based level:** an 18×25 grid map (three tile types — air, solid, exit) rendered from a 16×16 tileset, scaled 2x to 32×32 on screen.
- **AABB collision:** axis-separated collision resolution against the tilemap (X and Y solved independently), preventing the player from tunneling through blocks; vertical velocity resets to zero on landing.
- **State-driven animation:** Idle, Run, Jump, and Fall states, each mapped to its own sprite strip, with per-frame timing (~0.1s) and horizontal flipping based on movement direction.
- **Lives & respawn system:** 3 lives; falling off the map costs a life and respawns the player at the start; reaching 0 lives ends the run.
- **Audio:** looping background music, a jump sound effect, and a death sound effect on losing a life.
- **HUD:** live lives counter and elapsed time via `SDL_RenderDebugTextFormat`, plus a large win/lose message at the end of the run.

---

## 🛠️ Tech stack

- **C++**
- **SDL3**
- **SDL3_image**
- **SDL3_mixer**

---

## 🧠 Technical highlights

- **Tilemap-driven level design:** level layout defined as a simple character-grid data structure, decoupled from rendering, allowing the level to be edited by changing a string array.
- **Axis-separated AABB collision:** resolves horizontal and vertical collisions independently per frame, checking ground contact via the tile directly beneath the player's center rather than a full rectangle sweep, avoiding the "landing wobble" that naive AABB implementations run into.
- **Runtime-computed animation frame counts:** frame counts are derived at load time from each spritesheet's actual texture width (`textureWidth / frameWidth`) instead of being hardcoded, making the animation system resilient to spritesheet changes.
- **Audio engine integration:** built on SDL3_mixer's Track/Mixer API, with a dedicated track for looping music and a separate track for one-shot sound effects (jump, death).
- **Delta-time-based simulation:** all movement, physics, and animation timing are frame-rate independent, driven by a clamped delta time to avoid instability on frame spikes.

---

## 🏗️ Project structure

This project follows a single-file architecture (`Lazaro_EF.cpp`), as required by the exam format. Despite the single-file constraint, the code is organized into clear responsibility areas:

| Section | Responsibility |
|---|---|
| `g_map[]` | Tilemap data (18×25 grid: air / solid / exit) |
| `Player` struct | Player state: position, velocity, grounded flag, facing direction, animation state |
| `Anim` struct / `LoadAnim()` | Spritesheet loading with runtime frame-count calculation |
| `isSolid()` / `isGoal()` / `rectHitsSolid()` / `rectHitsGoal()` | Tile-based collision queries (AABB vs. tilemap) |
| Main loop — input | Keyboard polling, jump trigger (edge-detected via event, grounded-gated) |
| Main loop — physics | Gravity integration, axis-separated movement and collision resolution |
| Main loop — game rules | Win/lose conditions, lives, respawn logic |
| Main loop — animation | State selection (Idle/Run/Jump/Fall) and frame timing |
| Main loop — render | Background tiling, tilemap rendering, character sprite, HUD, win/lose banner |

---

## 🎨 Asset credits

Character, terrain, and background sprites are from a pixel-art asset pack provided as part of the exam template. Sound effects and music were provided as part of the same template.

---

## ▶️ Play without building

You can download the pre-built executable (with all required DLLs) from the **[Releases](../../releases)** section of this repository, if available. Otherwise, see the build instructions below.

## 🚀 Building from source

This project relies on external native libraries that are not included in the repository (due to size). To build it:

1. Clone the repository.
2. Download the **VC** (not MinGW) development versions of:
   - [SDL3](https://github.com/libsdl-org/SDL/releases)
   - [SDL3_image](https://github.com/libsdl-org/SDL_image/releases)
   - [SDL3_mixer](https://github.com/libsdl-org/SDL_mixer/releases)
3. Place them on your system and configure the include/library paths in Visual Studio project properties (**C/C++ → General → Additional Include Directories**, **Linker → General → Additional Library Directories**, **Linker → Input → Additional Dependencies**), pointing to your SDL3 installation.
4. Build in `Debug` or `Release` mode for `x64`.
5. Run the generated `.exe` — make sure `SDL3.dll`, `SDL3_image.dll`, and `SDL3_mixer.dll` are next to it, along with the `assets/` folder.

---

## 👤 Author

**Luis Lazaro Machado** — [GitHub](https://github.com/LuisLazaroMachado)

---

## 📄 License

This project is distributed under the MIT License. See [LICENSE](LICENSE) for details.