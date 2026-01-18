# Henon Map Visualizations

This repository provides two workflows for exploring a Henon-like iterated map.

## Real-time visualizers

- The `viz/` directory contains immediate-mode OpenGL applications for interactive exploration and debugging (live, not for export):
  - `viz/src/single_point_henon.cpp`: draws a single orbit/trajectory as a line strip from a seed point. Useful for studying individual orbits and quick parameter tweaks.
  - `viz/src/flow_map.cpp`: draws trajectories/streamlines, includes a `Camera` and `HenonField` abstraction, axis-locked movement, control-modified zoom/scale, and throttled parameter updates for smoother interaction.
  - `viz/src/utils.cpp`: shared rendering utilities (Camera class, grid rendering, text rendering, screenshot saving)
  - `viz/inc/`: headers for utilities and dependencies (stb_image_write.h)

Controls (for `viz/flow_map.cpp`)

| Key(s) | Action | With `Ctrl` |
|---:|:---|:---|
| Arrow keys (← ↑ → ↓) | Rotate camera / orbit view | Axis-locked movement: move field origin cube along dominant axis |
| `PageUp` / `PageDown` | Zoom camera in / out | Adjust field grid `range` (scale) |
| `U` | Increase `field.resolution` (grid samples) | Decrease `field.resolution` |
| `I` | Increase `field.iterations` (per-sample trajectory length) | Decrease `field.iterations` |
| `O` | Increase parameter `a` | Decrease `a` |
| `P` | Increase parameter `b` | Decrease `b` |
| `Y` | Save a screenshot (PNG) to `renders/` | N/A |

Notes: parameter keys are throttled (changes apply at ~0.1s intervals) and HUD values are shown on-screen.

Flow map visual samples (captured with `flow_map`):

![flow_map 150920](renders/flow_map_20260118_150920.png)

Each point in the field origin cube is iterated over allowing you to vizualize the attractor.

![flow_map 150957](renders/flow_map_20260118_150957.png)

You can zoom in / out & move the camera around.

![flow_map 151007](renders/flow_map_20260118_151007.png)

You can move & scale the field origin cube.

![flow_map 151011](renders/flow_map_20260118_151011.png)

You can change grid resolution & number of iterations.

![flow_map 151020](renders/flow_map_20260118_151020.png)

You can change Henon's a & b parameters.

![flow_map 151029](renders/flow_map_20260118_151029.png)

Colors are based on each point's rate of change.


## PLY Exporter

- `blender_viz/henon_ply_creator.cpp` is the data exporter: it generates a plain ASCII PLY file (`henon_3d.ply`) that you can import into Blender or other 3D tools for offline rendering and post-processing.

Example exporter output (Blender preview):

![Blender export preview](renders/blender_viz.png)


## Building

### Requirements
- C++17 compiler
- CMake 3.10+
- GLFW3, OpenGL, GLUT (for visualizers)

### Build
```bash
cmake .
make        # Build all targets to build/bin/
make clean  # Remove build directory
```

Executables:
- `build/bin/flow_map` - Interactive 3D field visualizer
- `build/bin/single_point_henon` - Single trajectory tracer
- `build/bin/henon_ply_creator` - PLY export utility
