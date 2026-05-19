Panel Rainmeter skin

Percentage values are shown as progress bars instead of text:
- CPU/GPU/MEM utilization
- VRAM percentage used
- GPU fan percentage where available

Layout is controlled from @Resources\Variables.inc:
- Tile grid: CellW / RowH / CELLS_COUNT / ROWS_COUNT
- Derived outer size: PanelWidth / PanelHeight / Width
- Padding
- Inner content: RowStartY / LineH / BarOffsetY / BarH / Row0..Row6
- Override CELLS_COUNT / ROWS_COUNT in a panel's [Variables] section
  to make it 2x1, 1x2, etc.

Styles are controlled from @Resources\Styles.inc.


Graph update:
- CPU/MEM/GPU utilization panels now include Rainmeter Line meters.
- GPU graph shows two lines: GPU utilization and VRAM usage.
- NETWORK graph shows two lines: download and upload.
- Graphs are live in-memory history only; they reset when the skin reloads.


Graph notes:
- Network graphs use AutoScale=1, so low traffic should still be visible.
- AIDA64 FPS panel (separate tile) shows RTSS FPS from Value.SRTSSFPS plus an autoscaled FPS graph.
- Glances has no FPS source in the provided API responses, so FPS graph is AIDA64-only unless a Glances/RTSS endpoint is added.
