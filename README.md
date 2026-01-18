# Henon Map Visualizations

This repository provides two workflows for exploring a Henon-like iterated map.

Quick build commands (Linux)
```bash
g++ -std=c++17 -O2 blender_viz/henon_ply_creator.cpp -o henon_ply_creator
g++ -std=c++17 -O2 viz/single_point_henon.cpp -o single_point_henon -lglfw -lGL -lGLU -lglut -lpthread -ldl -lm
g++ -std=c++17 -O2 viz/flow_map.cpp -o flow_map -I inc -lglfw -lGL -lGLU -lglut -lpthread -ldl -lm
```

## Real-time visualizers

- The `viz/` directory contains immediate-mode OpenGL applications for interactive exploration and debugging (live, not for export):
  - `viz/single_point.cpp`: draws a single orbit/trajectory as a line strip from a seed point. Useful for studying individual orbits and quick parameter tweaks.
  - `viz/flow_map.cpp`: draws trajectories/streamlines, includes a `Camera` and `HenonField` abstraction, axis-locked movement, control-modified zoom/scale, and throttled parameter updates for smoother interaction.

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

<div>
  <figure style="display:inline-block;margin:8px;text-align:center">
    <img src="renders/flow_map_20260118_150920.png" alt="flow_map 150920" width="512"/>
    <figcaption>Each point in the field origin cube is iterated over allowing you to vizualize the attractor</figcaption>
  </figure>
  <figure style="display:inline-block;margin:8px;text-align:center">
    <img src="renders/flow_map_20260118_150957.png" alt="flow_map 150957" width="512"/>
    <figcaption>You can zoom in / out & move the camera around</figcaption>
  </figure>
  <figure style="display:inline-block;margin:8px;text-align:center">
    <img src="renders/flow_map_20260118_151007.png" alt="flow_map 151007" width="512"/>
    <figcaption>You can move & scale the field origin cube</figcaption>
  </figure>
  <figure style="display:inline-block;margin:8px;text-align:center">
    <img src="renders/flow_map_20260118_151011.png" alt="flow_map 151011" width="512"/>
    <figcaption>You can change grid resolution & number of iterations</figcaption>
  </figure>
  <figure style="display:inline-block;margin:8px;text-align:center">
    <img src="renders/flow_map_20260118_151020.png" alt="flow_map 151020" width="512"/>
    <figcaption>You can change Henon's a & b parameters</figcaption>
  </figure>
  <figure style="display:inline-block;margin:8px;text-align:center">
    <img src="renders/flow_map_20260118_151029.png" alt="flow_map 151029" width="512"/>
    <figcaption>Colors are based on each points rate of change</figcaption>
  </figure>
</div>


## PLY Exporter

- `blender_viz/henon_ply_creator.cpp` is the data exporter: it generates a plain ASCII PLY file (`henon_3d.ply`) that you can import into Blender or other 3D tools for offline rendering and post-processing.

Example exporter output (Blender preview):

![Blender export preview](renders/blender_viz.png)