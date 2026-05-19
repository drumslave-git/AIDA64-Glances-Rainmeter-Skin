# AGENT.md — Rainmeter Panel Skin

This file describes the Rainmeter skin project, its structure, data sources, style conventions, and rules for future edits.

## Project goal

Maintain a modular Rainmeter dashboard skin named `Panel`.

The skin displays local Windows hardware stats from AIDA64 and remote server stats from Glances.

The current design direction is:

- Strict, minimal, **black & white**. No accent hues.
- **Panel background is 90% opaque** (`ColorBg` alpha `230`) — a small amount of see-through so the desktop barely shows through. Every other color (text, borders, divider, bars, graph lines, graph background) stays fully opaque (`alpha 255`) so foreground elements remain crisp.
- **No rounded corners** — `PanelRadius` and `GraphRadius` are both `0`.
- **No colored borders** — frames around panels and graphs are a quiet neutral gray; the header divider is a brighter neutral gray. No orange / red / colored strokes.
- Large readable typography (white text on black).
- Percentage values represented with progress bars and/or line graphs, not plain percentage text.
- Layout controlled through shared variables instead of hardcoded pixel values.
- Every `.ini` skin file lives inside its own folder.

Do NOT reintroduce: rounded corners, semi-transparent backgrounds, orange / red accent colors, decorative gradients, or per-panel custom palettes. If a new visual element is needed, define a single shared variable in `Variables.inc` and reuse it.

## Required Rainmeter folder structure

The skin root must be:

```text
Panel/
  @Resources/
    Variables.inc
    Styles.inc
    README.txt
    Scripts/
      GlancesProcesses.lua

  AIDA64/
    CPU/
      CPU.ini
    GPU/
      GPU.ini
    FPS/
      FPS.ini
    MEM/
      MEM.ini
    NETWORK/
      NETWORK.ini

  Glances/
    CPU/
      CPU.ini
    GPU/
      GPU.ini
    MEM/
      MEM.ini
    NETWORK/
      NETWORK.ini
    PROCESSES_CPU/
      PROCESSES_CPU.ini
    PROCESSES_MEM/
      PROCESSES_MEM.ini

  PROCESSES_CPU/
    PROCESSES_CPU.ini
  PROCESSES_MEM/
    PROCESSES_MEM.ini
  PROCESSES_GPU/
    PROCESSES_GPU.ini
```

Do not flatten this structure.
Do not put `CPU.ini`, `GPU.ini`, `FPS.ini`, `MEM.ini`, `NETWORK.ini`, or `PROCESSES_*.ini` directly under `AIDA64/` or `Glances/`.

The top-level `PROCESSES_CPU/`, `PROCESSES_MEM/`, `PROCESSES_GPU/` panels are the local AIDA64/UsageMonitor-based panels. The Glances-sourced equivalents live under `Glances/PROCESSES_CPU/` and `Glances/PROCESSES_MEM/`. There is no `Glances/PROCESSES_GPU/` — see "Glances data source" below for why.

## Shared resources

All shared layout, colors, fonts, API URLs, registry keys, and graph sizing belong in:

```text
Panel/@Resources/Variables.inc
```

All shared meter styles belong in:

```text
Panel/@Resources/Styles.inc
```

Each panel `.ini` should include both:

```ini
@include=#@#Variables.inc
@include2=#@#Styles.inc
```

## Current style baseline

Preserve this visual style unless explicitly asked to redesign:

```ini
FontName=Trebuchet MS
FontSize=22
TitleSize=30
Padding=10

ColorText=255,255,255,255
ColorMuted=170,170,170,255
ColorTitle=255,255,255,255
ColorBg=0,0,0,230
ColorBorder=255,255,255,255
ColorDivider=255,255,255,255
ColorBarBg=50,50,50,255
ColorBar=255,255,255,255
ColorGraphGrid=70,70,70,255

ColorGraphBg=20,20,20,255
ColorGraph1=255,255,255,255
ColorGraph2=170,170,170,255
ColorGraph3=255,255,255,255
```

Do not change these to colored / accent values. The skin is intentionally monochrome. When a graph has multiple series (e.g. NETWORK Down vs Up), differentiate them by **shade** (`ColorGraph1` white vs `ColorGraph2` mid-gray), never by hue.

The only color allowed to use alpha `< 255` is `ColorBg` (currently `230`, i.e. 90% opaque). Foreground colors — text, borders, divider, bars, graph lines, graph background, graph grid — must stay at alpha `255` so they render crisp on top of the slightly translucent panel.

`ColorBorder` is reserved for **frame strokes** (panel border via `[StyleBg]`, graph rectangle border via per-panel graph background shapes) and is pure white.

`ColorGraphGrid` is reserved for the dark **horizontal grid lines drawn inside the graph rectangle** by `Meter=Line` (`HorizontalLines=3`, `HorizontalLineColor=#ColorGraphGrid#`). Do NOT point graph grid lines at `ColorBorder` — that would make them solid white and overwhelm the graph.

## Tile grid sizing

Every panel's outer dimensions come from a uniform tile grid declared in
`@Resources/Variables.inc`:

```ini
CellW=400          ; pixel width  of one tile column
RowH=280           ; pixel height of one tile row
CELLS_COUNT=1      ; default tile width  in cells
ROWS_COUNT=1       ; default tile height in rows
PanelWidth=(#CELLS_COUNT#*#CellW#)
PanelHeight=(#ROWS_COUNT#*#RowH#)
Width=#PanelWidth#
```

Each `.ini` overrides `CELLS_COUNT` and/or `ROWS_COUNT` in its own
`[Variables]` section when it is not a plain 1x1 tile. `PanelWidth`,
`PanelHeight`, and `Width` are then derived automatically — do not redeclare
them per panel.

Current tile sizes by panel:

| Panel                              | CELLS x ROWS | Pixels  |
|------------------------------------|--------------|---------|
| `AIDA64/CPU`                       | 1 x 1        | 400x280 |
| `AIDA64/MEM`                       | 1 x 1        | 400x280 |
| `AIDA64/GPU`                       | 1 x 1        | 400x280 |
| `AIDA64/FPS`                       | 1 x 1        | 400x280 |
| `AIDA64/NETWORK`                   | 1 x 1        | 400x280 |
| `Glances/CPU`                      | 1 x 1        | 400x280 |
| `Glances/MEM`                      | 1 x 1        | 400x280 |
| `Glances/GPU`                      | 1 x 1        | 400x280 |
| `Glances/NETWORK`                  | 1 x 1        | 400x280 |
| `Glances/PROCESSES_CPU`            | 2 x 1        | 800x280 |
| `Glances/PROCESSES_MEM`            | 2 x 1        | 800x280 |
| `PROCESSES_CPU` (local)            | 1 x 1        | 400x280 |
| `PROCESSES_MEM` (local)            | 1 x 1        | 400x280 |
| `PROCESSES_GPU` (local)            | 1 x 1        | 400x280 |

Most panels are 1x1. Two-cell-wide tiles are reserved for the Glances
`PROCESSES_*` panels (they need horizontal room for the cmdline column).
The AIDA64 FPS panel is its own 1x1 tile (split out from the GPU panel) so
the large RTSS FPS readout and its autoscaled history get a dedicated tile
instead of being squeezed into the bottom of the GPU panel.

To add a new panel:

1. Decide its tile footprint (cells wide x rows tall).
2. In the new `.ini`'s `[Variables]` block, override `CELLS_COUNT` and
   `ROWS_COUNT` only if either differs from `1`.
3. Use `#PanelWidth#`, `#PanelHeight#`, and `#Width#` from `Variables.inc`
   for outer dimensions — do not hardcode pixel sizes.

## Layout rules

Avoid hardcoded pixel positioning inside individual panel files whenever
possible. The inner content layout uses a single vertical unit `LineH`
(text-row line height, currently 40 px), independent of the outer tile row
height `RowH`.

Shared inner-layout constants live in `Variables.inc`:

```ini
TitleX=#Padding#
TitleY=10
DividerX=#Padding#
DividerY=60
DividerW=(#PanelWidth#-(#Padding#*2))

LineH=40
BarOffsetY=32
BarH=6

LabelX=#Padding#
ValueX=(#PanelWidth#-#Padding#)

BarX=#Padding#
BarW=(#PanelWidth#-(#Padding#*2))
```

There are NO `Row0`..`RowN` or `GraphY_*` variables — neither in
`Variables.inc` nor in panel `[Variables]` sections. Each meter inlines its
own Y position as a `LineH` multiple. The title takes the first two line
slots (Y=0..2*LineH), so content starts at `(#LineH#*2)`:

```ini
[LabelClock]
Meter=String
MeterStyle=StyleLabel
Y=(#LineH#*2)
Text=Clock

[LabelUtil]
Meter=String
MeterStyle=StyleLabel
Y=(#LineH#*3)
Text=Utilization

[ValueUtil]
Meter=String
MeterStyle=StyleValue
MeasureName=MeasureCPUUtil
Y=(#LineH#*3)
Text=%1%

[BarUtilBg]
Meter=Image
MeterStyle=StyleBarBg
Y=((#LineH#*3)+#BarOffsetY#)

[GraphUtil]
Meter=Line
MeterStyle=StyleGraphLine
Y=(#LineH#*5)
```

Panel background + 1 px frame is provided by the shared `[StyleBg]`
defined in `@Resources/Styles.inc`. Every panel's `[MeterBg]` must use it
verbatim — do NOT inline a custom `Shape=...` line:

```ini
[MeterBg]
Meter=Shape
MeterStyle=StyleBg
```

`[StyleBg]` defines the rectangle with a half-pixel inset
(`Rectangle 0.5,0.5,(#Width#-1),(#PanelHeight#-1),#PanelRadius#`) so the
1 px centered stroke renders crisply on pixels `0` and `Width-1` /
`PanelHeight-1`, fully inside the panel bounds. This guarantees:

- The border is visible (a centered stroke on a `0,0,W,H` path would
  render half outside the meter and either be clipped or — under
  `DynamicWindowSize=1` — push the window 1 px wider/taller than
  `#PanelWidth#` x `#PanelHeight#`).
- The panel's outer extent stays exactly `#PanelWidth#` x `#PanelHeight#`,
  so tile-grid alignment is preserved.

If you ever need a custom panel background (don't, unless absolutely
necessary), preserve the half-pixel inset and reuse `#ColorBg#` /
`#ColorBorder#` / `#PanelRadius#` from `Variables.inc`.

## Shared meter styles

Use existing styles rather than duplicating font/alignment settings:

```ini
MeterStyle=StyleBg
MeterStyle=StyleTitle
MeterStyle=StyleLabel
MeterStyle=StyleValue
MeterStyle=StyleDivider
MeterStyle=StyleBarBg
MeterStyle=StyleBar
MeterStyle=StyleGraphLine
MeterStyle=StyleGraphLineAuto
```

Use `StyleGraphLineAuto` for network and FPS graphs where scale changes a lot.
Use `StyleGraphLine` for fixed 0–100 percentage graphs.

## Progress bar rules

Percentage values should be shown as horizontal progress bars AND a numeric
percent on the same row as the label.

Every Utilization section must follow this structure:

1. A `[LabelUtil]` label on the left (`StyleLabel`, `Text=Utilization`).
2. A `[ValueUtil]` numeric percent on the right of the same row
   (`StyleValue`, `Text=%1%` or `Text=[Measure:0]%`).
3. A `[BarUtilBg]` background track on the next sub-row
   (`StyleBarBg`, `Meter=Image`).
4. A `[BarUtil]` filled bar on top of that track
   (`StyleBar`, `Meter=Bar`, with the same backing measure as `ValueUtil`).

Use this pattern (`N` here is the inline `LineH` multiple for the row, e.g.
`3` for the second content row):

```ini
[LabelUtil]
Meter=String
MeterStyle=StyleLabel
Y=(#LineH#*3)
Text=Utilization

[ValueUtil]
Meter=String
MeterStyle=StyleValue
MeasureName=MeasureCPUUtil
Y=(#LineH#*3)
Text=%1%

[BarUtilBg]
Meter=Image
MeterStyle=StyleBarBg
Y=((#LineH#*3)+#BarOffsetY#)

[BarUtil]
Meter=Bar
MeasureName=MeasureCPUUtil
MeterStyle=StyleBar
Y=((#LineH#*3)+#BarOffsetY#)
```

For Glances panels (where the backing WebParser measure is a float), use
`DynamicVariables=1` + `Text=[Measure:0]%` instead of `MeasureName=` +
`Text=%1%`, so decimals are stripped from values like `0.0`:

```ini
[ValueUtil]
Meter=String
MeterStyle=StyleValue
Y=(#LineH#*2)
DynamicVariables=1
Text=[CPUTotal:0]%
```

Measures backing progress bars must have:

```ini
MinValue=0
MaxValue=100
```

The bar BACKGROUND (`StyleBarBg`) is a decorative track, not a real bar — it
has no measure. Render it as `Meter=Image`, NOT `Meter=Bar`. A `Bar` meter
without `MeasureName=` triggers `MeasureName= is not valid` errors in the
Rainmeter log.

The bar FILL color must be set via `BarColor=` on `[StyleBar]`, NOT
`SolidColor=`. On a `Meter=Bar` meter, `SolidColor` only paints the meter's
background rectangle; without an explicit `BarColor` Rainmeter defaults the
fill to bright green (`0,255,0,255`), which is wrong for this monochrome
skin. The track behind the bar is provided separately by `[StyleBarBg]`.

## Graph rules

Rainmeter `Line` meters are used for live graphs.

Graphs are live in-memory history only. They reset on skin reload.

The graph Y position is **always** the shared `#GraphY#` variable defined in
`Variables.inc`:

```ini
GraphY=(#PanelHeight#-#GraphH#-#PanelBottomPadding#)
```

This anchors every graph to the bottom of its panel so graph baselines
align horizontally across all panels in the dashboard regardless of how
many text rows sit above the graph. Do NOT inline `(#LineH#*5)`,
`(#LineH#*4)`, or the long `(#PanelHeight#-#GraphH#-#PanelBottomPadding#)`
expression — always reference `#GraphY#`.

Graph background pattern:

```ini
[GraphSomethingBg]
Meter=Shape
Shape=Rectangle #GraphX#,#GraphY#,#GraphW#,#GraphH#,#GraphRadius# | Fill Color #ColorGraphBg# | StrokeWidth 1 | Stroke Color #ColorBorder#
```

Fixed percentage graph pattern:

```ini
[GraphUtil]
Meter=Line
MeasureName=MeasureCPUUtil
MeterStyle=StyleGraphLine
Y=#GraphY#
LineCount=1
LineColor=#ColorGraph1#
```

Autoscaled graph pattern:

```ini
[GraphNetwork]
Meter=Line
MeasureName=MeasureDownMbit
MeasureName2=MeasureUpMbit
MeterStyle=StyleGraphLineAuto
Y=#GraphY#
LineCount=2
LineColor=#ColorGraph1#
LineColor2=#ColorGraph2#
AutoScale=1
```

Use `AutoScale=1` for network traffic and FPS because values vary widely.
Do not use autoscale for utilization percentages unless explicitly requested.

## Vertical layout convention

Each 1x1 panel is `PanelHeight=280` and uses a fixed vertical structure:

| Y range   | Content                                        |
|-----------|------------------------------------------------|
| 0–40      | Title (`StyleTitle`, `TitleY=10`)              |
| 60        | Header divider (`StyleDivider`, `DividerY=60`) |
| 80–200    | Content rows (`LineH=40`, slots `*2..*4`)      |
| 200–270   | Graph (`#GraphY#`, `GraphH=70`)                |
| 270–280   | Bottom padding (`PanelBottomPadding=10`)       |

Content text rows live at `Y=(#LineH#*N)` for `N` in `2..4`. A row that
includes a progress bar reserves the bottom of its `LineH` strip for the
bar at `Y=((#LineH#*N)+#BarOffsetY#)` (`BarOffsetY=32`, `BarH=6`), so the
next row at `Y=(#LineH#*(N+1))` begins ~2 px below the bar.

Pack content rows from the top (`*2`, `*3`, `*4`) without intentional gaps.
If a panel only has one or two text rows, leave the empty slot(s) ABOVE
the graph rather than between content rows — related metrics should sit
together. Never leave a hole in the middle of the content area.

`PROCESSES_*` panels override the convention by occupying rows `*2..*6`
(no graph), with the optional Glances diagnostic footer at
`(#PanelHeight#-16)`.

## AIDA64 data source

AIDA64 exports Windows/local sensor data through Registry.

Registry base key:

```ini
AIDAKey=Software\FinalWire\AIDA64\SensorValues
```

AIDA64 Registry measures must use:

```ini
Measure=Registry
RegHKey=HKEY_CURRENT_USER
RegKey=#AIDAKey#
RegValue=#SomeVariable#
```

Important: AIDA64 exported values use `Value.*` keys, not bare sensor names.

Current AIDA64 variables:

```ini
; CPU
AIDA_CPUClock=Value.SCPUCLK
AIDA_CPUUtil=Value.SCPUUTI
AIDA_CPUTemp=Value.TCPU

; GPU
AIDA_GPUClock=Value.SGPU1CLK
AIDA_GPUUtil=Value.SGPU1UTI
AIDA_GPUTemp=Value.TGPU1
AIDA_GPUVRAMPercent=Value.SVMEMUSAGE
AIDA_GPUVRAMUsed=Value.SUSEDVMEM
AIDA_GPUVRAMFree=Value.SFREEVMEM
AIDA_FPS=Value.SRTSSFPS

; MEM
AIDA_MEMClock=Value.SMEMCLK
AIDA_MEMUtil=Value.SMEMUTI
AIDA_MEMUsed=Value.SUSEDMEM
AIDA_MEMFree=Value.SFREEMEM
```

AIDA64 must be running and configured to write sensor values to Registry:

```text
AIDA64 → Preferences → Hardware Monitoring → External Applications → Enable writing sensor values to Registry
```

Useful check command:

```powershell
reg query "HKCU\Software\FinalWire\AIDA64\SensorValues"
```

## Glances data source

Glances provides remote Ubuntu/TCloud stats through HTTP JSON APIs.

Current Glances URLs should use local HTTP, not public HTTPS, to avoid Rainmeter TLS/certificate revocation errors:

```ini
GlancesCpuUrl=http://192.168.88.219:61208/api/4/cpu
GlancesMemUrl=http://192.168.88.219:61208/api/4/mem
GlancesGpuUrl=http://192.168.88.219:61208/api/4/gpu/gpu_id/value/nvidia0
GlancesNetworkUrl=http://192.168.88.219:61208/api/4/network/interface_name/value/enp5s0
GlancesUpdateRate=3
GlancesVRAMTotalMB=24576
```

Do not use `https://glances.tcloud.monster` in Rainmeter unless certificate validation is known to work. Rainmeter previously failed with:

```text
(Fetch error) The supplied certificate has been revoked (ErrorCode=12170)
```

### Known Glances JSON shapes

CPU endpoint:

```json
{"total": 0.0, "cpucore": 12}
```

Useful fields:

```text
total
cpucore
```

MEM endpoint:

```json
{"total": 132517171200, "available": 126478454784, "percent": 4.6, "used": 6038716416}
```

Useful fields:

```text
total
used
percent
```

GPU endpoint:

```json
{"nvidia0": [{"name": "NVIDIA GeForce RTX 3090", "mem": 1.8541971842447917, "proc": 0, "temperature": 27, "fan_speed": 34}]}
```

Useful fields:

```text
nvidia0[0].name
nvidia0[0].mem        ; VRAM percentage
nvidia0[0].proc       ; GPU utilization percentage
nvidia0[0].temperature
nvidia0[0].fan_speed
```

Glances GPU endpoint does not provide FPS or GPU clock in the known response.
Do not invent FPS or clock in Glances GPU panels unless a new API endpoint is provided.

Network endpoint:

```json
{"enp5s0": [{"speed": 1048576000, "bytes_recv_rate_per_sec": 682720.0, "bytes_sent_rate_per_sec": 324178.0}]}
```

Useful fields:

```text
speed
bytes_recv_rate_per_sec
bytes_sent_rate_per_sec
```

Convert network bytes/sec to Mbps:

```ini
Formula=MeasureDownBytes * 8 / 1000000
Formula=MeasureUpBytes * 8 / 1000000
```

Processlist endpoint
(`#GlancesProcessListUrl#` = `#GlancesBaseUrl#/processlist/top/#GlancesProcessListTopN#`,
with `GlancesProcessListTopN=50` by default):

```json
[
  {"memory_info": {"rss": 119394304, ...}, "pid": 26204,
   "memory_percent": 0.09, "cpu_times": {...}, "gids": {...},
   "num_threads": 26, "cpu_percent": 2.8, "name": "python3.12",
   "status": "S", "nice": 0, "io_counters": [...], "key": "pid",
   "time_since_update": 2.89, "cmdline": [...], "username": "root"},
  ...
]
```

IMPORTANT: the field order from this server is NOT alphabetical and NOT what
the published Glances API doc example shows — it is Python `dict` insertion
order, which is implementation-defined and can vary across Glances versions.
Therefore Lua patterns that anchor on "first field" / "last field" markers
WILL silently fail. The parser must extract balanced top-level `{...}` objects
with a brace-counting scanner (skipping braces inside JSON strings) and then
pattern-match individual field names inside each extracted object. This is
what `@Resources/Scripts/GlancesProcesses.lua` does in `extractObjects()`.

Also: full `/processlist` is ~256 KB / 451 processes on this server, which is
expensive for Rainmeter's regex engine to capture in one group. Fetch
`/processlist/top/#GlancesProcessListTopN#` instead (default 50, ~19 KB) and
let Lua sort and trim to TopCount. 50 is wide enough that the top 5 by CPU
and the top 5 by RSS both reliably fall inside the working set.

Useful per-process fields:

```text
name
cpu_percent
memory_percent
memory_info.rss   ; bytes of resident memory
pid
username
```

Glances `/processlist` returns rows in the server's currently-active sort order,
which is a single global setting. There is no `?sort=` query parameter on this
endpoint. To get distinct "top by CPU" vs "top by MEM" rankings from a single
Glances instance, sorting must happen client-side. This skin does that in
`@Resources/Scripts/GlancesProcesses.lua`.

Glances does NOT expose per-process GPU usage. The `/gpu` endpoint is
aggregate-only (`name`, `mem`, `proc`, `temperature`, `fan_speed`) with no
`pid`. Do not invent a `Glances/PROCESSES_GPU/` panel. The local
`PROCESSES_GPU/PROCESSES_GPU.ini` keeps using AIDA64/UsageMonitor for that.

### Glances processlist Lua helper

`@Resources/Scripts/GlancesProcesses.lua` is called from a `Measure=Script`
measure. It reads the JSON file that a sibling `Measure=WebParser` with
`Download=1` writes to disk, extracts each process record with a Lua pattern
anchored on `"cmdline" .- "username"`, sorts client-side by the chosen field,
and exposes the top N rows via inline calls in meters.

The wiring uses TWO WebParser measures + one Script measure, no file I/O:

```ini
; Parent WebParser captures the entire /processlist JSON body in one greedy group.
[GlancesProcessListApi]
Measure=WebParser
URL=#GlancesPROCESSESUrl#
UpdateRate=#GlancesUpdateRate#
RegExp=(?s)^(.+)$
SSLVerify=#GlancesSSLVerify#

; Child WebParser exposes that captured group as its StringValue.
[GlancesProcessListJson]
Measure=WebParser
URL=[GlancesProcessListApi]
StringIndex=1

; Lua script reads the JSON straight from the child measure's StringValue.
[ScriptProc]
Measure=Script
ScriptFile=#@#Scripts\GlancesProcesses.lua
SourceMeasure=GlancesProcessListJson   ; child WebParser measure name
SortField=cpu_percent                  ; cpu_percent | memory_percent | rss
TopCount=5
Blacklist=#GlancesProcBlacklist#       ; pipe-separated, case-insensitive
Debug=0                                ; 1 = emit !Log diagnostics
UpdateDivider=1
```

Meter inline calls (require `DynamicVariables=1`):

```ini
Text=[&ScriptProc:GetName(1)]
Text=[&ScriptProc:GetCmdline(1)]
Text=[&ScriptProc:GetCpuPercent(1)]%
Text=[&ScriptProc:GetMemoryPercent(1)]%
Text=[&ScriptProc:GetRss(1)]
Text=[&ScriptProc:GetRssHuman(1)]B
Text=[&ScriptProc:GetStatus()]
```

`GetCmdline()` returns the space-joined argv from the JSON `cmdline` array
(e.g. `"/venv/bin/python3.12 -m glances -w"`). Kernel threads with an empty
`cmdline` return an empty string. The cmdline column should use
`ClipString=2` and a smaller font (`StyleProcCmdline`, `FontSize=11`) so
long argv lists are gracefully truncated.

`GetRssHuman` returns a value already scaled to B / K / M / G with one decimal.
Do not add an extra `B` suffix beyond what the meter `Text=` already concatenates.

Do NOT use `Download=1` + `DownloadFile=...` + Lua `io.open(...)` for this —
that round-trip is async and the Script measure can race the file write.
Reading the JSON directly from the WebParser string value is synchronous from
the Script's point of view (the child measure has the parsed group ready as
soon as the parent finishes its fetch).

## WebParser rules

Rainmeter `WebParser` does not truly parse JSON. It fetches text and extracts values with regex.

Keep regexes narrow enough to match current JSON, but not overcomplicated.

Examples:

```ini
[MeasureCPUApi]
Measure=WebParser
URL=#GlancesCPUUrl#
UpdateRate=#GlancesUpdateRate#
RegExp=(?siU).*"total"\s*:\s*([0-9.]+).*"cpucore"\s*:\s*([0-9.]+).*
```

Then child measures read `StringIndex`:

```ini
[MeasureCPUUtil]
Measure=WebParser
URL=[MeasureCPUApi]
StringIndex=1
MinValue=0
MaxValue=100
```

## Current panel content rules

### `Panel/AIDA64/CPU/CPU.ini`

Display:

1. Clock
2. Utilization progress bar
3. Temperature
4. CPU utilization graph

### `Panel/AIDA64/GPU/GPU.ini`

Required order:

1. Clock
2. Utilization
3. VRAM
4. VRAM graph

Use fixed 0–100 graph for VRAM percentage.

FPS is intentionally NOT part of this panel. RTSS FPS lives in
`Panel/AIDA64/FPS/FPS.ini` so the GPU panel stays a standard 1x1 tile.

### `Panel/AIDA64/FPS/FPS.ini`

Display:

1. Large centered FPS readout
2. Autoscaled FPS history graph anchored to the bottom of the panel

Use AIDA64 `Value.SRTSSFPS` for FPS (exposed as `#AIDA_FPS#`).
Use `StyleGraphLineAuto` (or `AutoScale=1`) for the FPS graph because the
range varies a lot between idle (0) and in-game (100+).

The big number is positioned by inlining the midpoint between the divider
and the graph; do not hardcode a row index for it:

```ini
Y=((#DividerY#+(#PanelHeight#-#GraphH#-#PanelBottomPadding#))/2)
```

### `Panel/AIDA64/MEM/MEM.ini`

Display:

1. Clock
2. Memory used / total
3. Memory utilization progress bar
4. Memory utilization graph

Total memory is calculated:

```ini
Formula=MeasureMEMUsed + MeasureMEMFree
```

### `Panel/AIDA64/NETWORK/NETWORK.ini`

Use Rainmeter native network measures, not AIDA64 registry.

Display:

1. Download Mbps
2. Upload Mbps
3. Autoscaled network graph with download and upload lines

Measures:

```ini
Measure=NetIn
Measure=NetOut
```

Convert bytes/sec to Mbps:

```ini
Formula=MeasureNetIn * 8 / 1000000
Formula=MeasureNetOut * 8 / 1000000
```

### `Panel/Glances/CPU/CPU.ini`

Display:

1. CPU utilization progress bar
2. CPU cores
3. CPU utilization graph

### `Panel/Glances/GPU/GPU.ini`

Display only fields present in known Glances response:

1. Device name
2. GPU utilization progress bar
3. VRAM used / total and progress bar
4. VRAM graph

VRAM total is not provided by Glances GPU response, so use:

```ini
GlancesVRAMTotalMB=24576
```

VRAM used is calculated:

```ini
Formula=MeasureVRAMPercent * #GlancesVRAMTotalMB# / 100
```

Do not add FPS to Glances GPU unless an FPS source is provided.

### `Panel/Glances/MEM/MEM.ini`

Display:

1. Memory used / total
2. Memory utilization progress bar
3. Memory utilization graph

Convert bytes to GB for display where needed:

```ini
Formula=MeasureMEMUsedBytes / 1073741824
Formula=MeasureMEMTotalBytes / 1073741824
```

### `Panel/Glances/NETWORK/NETWORK.ini`

Display:

1. Link speed Mbps
2. Download Mbps
3. Upload Mbps
4. Autoscaled network graph with download and upload lines

Use `AutoScale=1`.

### `Panel/Glances/PROCESSES_CPU/PROCESSES_CPU.ini`

Mirror of `Panel/PROCESSES_CPU/PROCESSES_CPU.ini` but sourced from Glances
`/processlist/top/N` instead of the local `UsageMonitor` plugin.

Display top 5 rows across THREE columns:

1. Process `name` (left, muted, `StyleProcName`, fixed width `#ProcNameW#`,
   `ClipString=2`).
2. Joined `cmdline` argv (middle, muted, smaller `StyleProcCmdline`,
   flexible width `#ProcCmdW#`, `ClipString=2`).
3. `cpu_percent` with a `%` suffix (right, `StyleValue`, `StringAlign=Right`).

Data wiring:

1. `GlancesProcessListApi` (`Measure=WebParser`, `RegExp=(?s)^(.+)$`).
2. `GlancesProcessListJson` (`Measure=WebParser`, `URL=[GlancesProcessListApi]`,
   `StringIndex=1`).
3. `ScriptProc` (`Measure=Script`) with `SortField=cpu_percent` and
   `SourceMeasure=GlancesProcessListJson`.
4. Meters call `[&ScriptProc:GetName(N)]`,
   `[&ScriptProc:GetCmdline(N)]`, and `[&ScriptProc:GetCpuPercent(N)]`.

Use `#GlancesProcBlacklist#` from `Variables.inc` for the `Blacklist=` option,
not a panel-local blacklist.

The panel is a 2x1 tile (twice as wide as a standard panel) because the
cmdline column needs horizontal room. This is set by `CELLS_COUNT=2` in the
panel's `[Variables]` section; `PanelWidth`, `PanelHeight`, and `Width` are
all derived from that — do not hardcode `PanelWidth=` here.

A small diagnostic footer (`[StatusLine]`, `StyleStatus`) renders
`[&ScriptProc:GetStatus()]` at the bottom of the panel. To hide it: either
delete the meter or set `ScriptProc Debug=0` (the meter stays but the log
spam stops; the meter itself always renders whatever `GetStatus()` returned
last). The status string looks like `OK 5/49 procs, json=19184B, top1=firefox`
when healthy.

### `Panel/Glances/PROCESSES_MEM/PROCESSES_MEM.ini`

Same structure as `Glances/PROCESSES_CPU` (also a 2x1 tile via
`CELLS_COUNT=2`) but with `SortField=rss` and the value column showing
memory via `[&ScriptProc:GetRssHuman(N)]B`. The value column reservation is
slightly wider (`ProcValReservedW=160` vs `140` for CPU) because RSS values
like `12.3 G` are wider than `99.9%`.

Do NOT use `AutoScale=1` here — `GetRssHuman` already scales bytes to B/K/M/G.

There is intentionally no `Glances/PROCESSES_GPU/` panel; Glances does not
expose per-process GPU usage.

## Validation checklist after edits

1. Confirm the folder structure is still exactly nested under `Panel/`.
2. Confirm every `.ini` has:
   ```ini
   @include=#@#Variables.inc
   @include2=#@#Styles.inc
   ```
3. Confirm no styles were lost from `Styles.inc`.
4. Confirm the strict black & white visual style remains intact: only `ColorBg` may use alpha `< 255` (currently `230` for 90% opacity), all other colors use alpha `255`, `PanelRadius` and `GraphRadius` are `0`, no orange / red / accent hues are reintroduced, and no per-panel custom palette overrides shared variables.
5. Confirm percentage values use bars and have `MinValue=0` / `MaxValue=100`.
   Every Utilization section must include a `[ValueUtil]` numeric percent on
   the same row as the `[LabelUtil]` label, with the bar pair below it.
6. Confirm network and FPS graphs use `AutoScale=1`.
7. Confirm AIDA64 registry keys use `Value.*` names.
8. Confirm Glances URLs use local HTTP unless explicitly changed by the user.
9. Confirm no unsupported Glances fields are invented.
10. Confirm Glances `PROCESSES_*` panels use the shared
    `@Resources/Scripts/GlancesProcesses.lua` script and the shared
    `#GlancesProcBlacklist#` variable, not bespoke per-panel copies.
11. Confirm there is no `Glances/PROCESSES_GPU/` panel.
12. Confirm every `*Bg` background-bar meter uses `Meter=Image`, not
    `Meter=Bar` (a `Bar` meter without a `MeasureName=` errors on load).
13. Confirm the Glances `PROCESSES_*` panels feed JSON to Lua via a
    parent/child WebParser pair (`RegExp=(?s)^(.+)$` → child with
    `StringIndex=1`), NOT via `Download=1` + file I/O.
14. Confirm Glances `PROCESSES_*` panels fetch `/processlist/top/N`
    (small payload), not raw `/processlist` (~256 KB).
15. Confirm the Lua process parser uses brace-counting object extraction,
    not field-order anchoring. Glances field order is Python dict insertion
    order — implementation-defined and version-dependent.
16. Confirm every panel's outer size comes from the tile grid (`CELLS_COUNT`,
    `ROWS_COUNT`, `CellW`, `RowH`). The `[MeterBg]` Shape must use
    `#PanelHeight#` (NOT a legacy `#PanelH_*#` variable, and NOT a
    hardcoded pixel value). Panels override `CELLS_COUNT` / `ROWS_COUNT`
    only when not `1`, and never redeclare `PanelWidth`, `PanelHeight`, or
    `Width` themselves.


## User preferences for this project

- Keep structure clean and predictable.
- Preserve user-edited styles unless explicitly asked to redesign.
- Do not hallucinate sensor keys or API fields. If a value is not present in AIDA64 registry or Glances JSON, state that it is unavailable.

## DUMP of AIDA64 registry

HKEY_CURRENT_USER\Software\FinalWire\AIDA64\SensorValues
    Label.TMOBO    REG_SZ    Motherboard
    Value.TMOBO    REG_SZ    40
    Label.TCPU    REG_SZ    CPU
    Value.TCPU    REG_SZ    45
    Label.TCPUPKG    REG_SZ    CPU Package
    Value.TCPUPKG    REG_SZ    54
    Label.TCPUIAC    REG_SZ    CPU IA Cores
    Value.TCPUIAC    REG_SZ    56
    Label.TCC-1-1    REG_SZ    CPU Core #1
    Value.TCC-1-1    REG_SZ    56
    Label.TCC-1-2    REG_SZ    CPU Core #2
    Value.TCC-1-2    REG_SZ    54
    Label.TCC-1-3    REG_SZ    CPU Core #3
    Value.TCC-1-3    REG_SZ    48
    Label.TCC-1-4    REG_SZ    CPU Core #4
    Value.TCC-1-4    REG_SZ    47
    Label.TCC-1-5    REG_SZ    CPU Core #5
    Value.TCC-1-5    REG_SZ    51
    Label.TCC-1-6    REG_SZ    CPU Core #6
    Value.TCC-1-6    REG_SZ    44
    Label.TCC-1-7    REG_SZ    CPU Core #7
    Value.TCC-1-7    REG_SZ    49
    Label.TCC-1-8    REG_SZ    CPU Core #8
    Value.TCC-1-8    REG_SZ    48
    Label.TCC-1-9    REG_SZ    CPU Core #9
    Value.TCC-1-9    REG_SZ    44
    Label.TCC-1-10    REG_SZ    CPU Core #10
    Value.TCC-1-10    REG_SZ    44
    Label.TCC-1-11    REG_SZ    CPU Core #11
    Value.TCC-1-11    REG_SZ    44
    Label.TCC-1-12    REG_SZ    CPU Core #12
    Value.TCC-1-12    REG_SZ    44
    Label.TCC-1-13    REG_SZ    CPU Core #13
    Value.TCC-1-13    REG_SZ    46
    Label.TCC-1-14    REG_SZ    CPU Core #14
    Value.TCC-1-14    REG_SZ    46
    Label.TCC-1-15    REG_SZ    CPU Core #15
    Value.TCC-1-15    REG_SZ    46
    Label.TCC-1-16    REG_SZ    CPU Core #16
    Value.TCC-1-16    REG_SZ    46
    Label.TPCHDIO    REG_SZ    PCH Diode
    Value.TPCHDIO    REG_SZ    76
    Label.TVRM    REG_SZ    VRM
    Value.TVRM    REG_SZ    61
    Label.TGPU1    REG_SZ    GPU
    Value.TGPU1    REG_SZ    54
    Label.TGPU1MEM    REG_SZ    GPU Memory
    Value.TGPU1MEM    REG_SZ    66
    Label.TDIMMTS2    REG_SZ    DIMM2
    Value.TDIMMTS2    REG_SZ    52
    Label.TDIMMTS4    REG_SZ    DIMM4
    Value.TDIMMTS4    REG_SZ    52
    Label.THDD1    REG_SZ    Samsung SSD 990 PRO 4TB
    Value.THDD1    REG_SZ    47
    Label.THDD1TS2    REG_SZ    Samsung SSD 990 PRO 4TB #2
    Value.THDD1TS2    REG_SZ    51
    Label.THDD2    REG_SZ    CT1000P3PSSD8
    Value.THDD2    REG_SZ    54
    Label.THDD2TS2    REG_SZ    CT1000P3PSSD8 #2
    Value.THDD2TS2    REG_SZ    63
    Label.THDD3    REG_SZ    CT1000P2SSD8
    Value.THDD3    REG_SZ    45
    Label.THDD4    REG_SZ    Samsung Portable SSD T7
    Value.THDD4    REG_SZ    34
    Label.THDD4TS2    REG_SZ    Samsung Portable SSD T7 #2
    Value.THDD4TS2    REG_SZ    34
    Label.FCPU    REG_SZ    CPU
    Value.FCPU    REG_SZ    800
    Label.FCPUOPT    REG_SZ    CPU OPT
    Value.FCPUOPT    REG_SZ    824
    Label.FAIOPUMP    REG_SZ    AIO Pump
    Value.FAIOPUMP    REG_SZ    2246
    Label.FGPU1    REG_SZ    GPU
    Value.FGPU1    REG_SZ    0
    Label.FGPU1GPU2    REG_SZ    GPU2
    Value.FGPU1GPU2    REG_SZ    0
    Label.DGPU1    REG_SZ    GPU
    Value.DGPU1    REG_SZ    0
    Label.DGPU1GPU2    REG_SZ    GPU2
    Value.DGPU1GPU2    REG_SZ    0
    Label.VCPU    REG_SZ    CPU Core
    Value.VCPU    REG_SZ    1.439
    Label.VCPUVID    REG_SZ    CPU VID
    Value.VCPUVID    REG_SZ    1.512
    Label.V33V    REG_SZ    +3.3 V
    Value.V33V    REG_SZ    3.392
    Label.VP5V    REG_SZ    +5 V
    Value.VP5V    REG_SZ    5.120
    Label.VP12V    REG_SZ    +12 V
    Value.VP12V    REG_SZ    12.000
    Label.V3VSB    REG_SZ    +3.3 V Standby
    Value.V3VSB    REG_SZ    3.392
    Label.VBAT    REG_SZ    VBAT Battery
    Value.VBAT    REG_SZ    3.184
    Label.VVDD    REG_SZ    VDD
    Value.VVDD    REG_SZ    1.350
    Label.VCPUL2    REG_SZ    CPU L2
    Value.VCPUL2    REG_SZ    0.472
    Label.VVCCINAUX    REG_SZ    VCCIN Aux
    Value.VVCCINAUX    REG_SZ    1.840
    Label.VVCCSA    REG_SZ    VCCSA
    Value.VVCCSA    REG_SZ    1.232
    Label.VGPU1    REG_SZ    GPU Core
    Value.VGPU1    REG_SZ    0.975
    Label.VGPU1PCIE    REG_SZ    GPU PCIe
    Value.VGPU1PCIE    REG_SZ    11.917
    Label.VGPU112VHPWR    REG_SZ    GPU 12VHPWR
    Value.VGPU112VHPWR    REG_SZ    11.907
    Label.CGPU1PCIE    REG_SZ    GPU PCIe
    Value.CGPU1PCIE    REG_SZ    0.77
    Label.CGPU112VHPWR    REG_SZ    GPU 12VHPWR
    Value.CGPU112VHPWR    REG_SZ    3.44
    Label.PCPUPKG    REG_SZ    CPU Package
    Value.PCPUPKG    REG_SZ    57.98
    Label.PCPUIAC    REG_SZ    CPU IA Cores
    Value.PCPUIAC    REG_SZ    51.70
    Label.PCPUGTC    REG_SZ    CPU GT Cores
    Value.PCPUGTC    REG_SZ    6.29
    Label.PGPU1    REG_SZ    GPU
    Value.PGPU1    REG_SZ    50.10
    Label.PGPU1TDPP    REG_SZ    GPU TDP%
    Value.PGPU1TDPP    REG_SZ    14
    Label.PGPU1PCIE    REG_SZ    GPU PCIe
    Value.PGPU1PCIE    REG_SZ    9.20
    Label.PGPU112VHPWR    REG_SZ    GPU 12VHPWR
    Value.PGPU112VHPWR    REG_SZ    40.90
    Label.SDATE    REG_SZ    Date
    Value.SDATE    REG_SZ    15-May-26
    Label.SYEAR    REG_SZ    Year
    Value.SYEAR    REG_SZ    2026
    Label.SMONTH    REG_SZ    Month
    Value.SMONTH    REG_SZ    5
    Label.SMONTHNAME    REG_SZ    Month Name
    Value.SMONTHNAME    REG_SZ    May
    Label.SDAYOFMONTH    REG_SZ    Day of Month
    Value.SDAYOFMONTH    REG_SZ    15
    Label.SDOW    REG_SZ    Day of Week
    Value.SDOW    REG_SZ    5
    Label.SDOWNAME    REG_SZ    Day of Week Name
    Value.SDOWNAME    REG_SZ    Friday
    Label.SWEEKOFYEAR    REG_SZ    Week of Year
    Value.SWEEKOFYEAR    REG_SZ    20
    Label.STIME    REG_SZ    Time
    Value.STIME    REG_SZ    16:14:55
    Label.STIMENS    REG_SZ    Time (HH:MM)
    Value.STIMENS    REG_SZ    16:14
    Label.SHOUR12    REG_SZ    Hour (1-12)
    Value.SHOUR12    REG_SZ    4
    Label.SHOUR24    REG_SZ    Hour (0-23)
    Value.SHOUR24    REG_SZ    16
    Label.SMIN    REG_SZ    Minute
    Value.SMIN    REG_SZ    14
    Label.SSEC    REG_SZ    Second
    Value.SSEC    REG_SZ    55
    Label.SUPTIME    REG_SZ    UpTime
    Value.SUPTIME    REG_SZ    3d 01:51:52
    Label.SUPTIMENS    REG_SZ    UpTime (HH:MM)
    Value.SUPTIMENS    REG_SZ    3d 01:51
    Label.SCPUCLK    REG_SZ    CPU Clock
    Value.SCPUCLK    REG_SZ    5700
    Label.SCC-1-1    REG_SZ    CPU Core #1 Clock
    Value.SCC-1-1    REG_SZ    5700
    Label.SCC-1-2    REG_SZ    CPU Core #2 Clock
    Value.SCC-1-2    REG_SZ    5200
    Label.SCC-1-3    REG_SZ    CPU Core #3 Clock
    Value.SCC-1-3    REG_SZ    5100
    Label.SCC-1-4    REG_SZ    CPU Core #4 Clock
    Value.SCC-1-4    REG_SZ    800
    Label.SCC-1-5    REG_SZ    CPU Core #5 Clock
    Value.SCC-1-5    REG_SZ    5700
    Label.SCC-1-6    REG_SZ    CPU Core #6 Clock
    Value.SCC-1-6    REG_SZ    800
    Label.SCC-1-7    REG_SZ    CPU Core #7 Clock
    Value.SCC-1-7    REG_SZ    800
    Label.SCC-1-8    REG_SZ    CPU Core #8 Clock
    Value.SCC-1-8    REG_SZ    800
    Label.SCC-1-9    REG_SZ    CPU Core #9 Clock
    Value.SCC-1-9    REG_SZ    800
    Label.SCC-1-10    REG_SZ    CPU Core #10 Clock
    Value.SCC-1-10    REG_SZ    4100
    Label.SCC-1-11    REG_SZ    CPU Core #11 Clock
    Value.SCC-1-11    REG_SZ    800
    Label.SCC-1-12    REG_SZ    CPU Core #12 Clock
    Value.SCC-1-12    REG_SZ    4100
    Label.SCC-1-13    REG_SZ    CPU Core #13 Clock
    Value.SCC-1-13    REG_SZ    800
    Label.SCC-1-14    REG_SZ    CPU Core #14 Clock
    Value.SCC-1-14    REG_SZ    800
    Label.SCC-1-15    REG_SZ    CPU Core #15 Clock
    Value.SCC-1-15    REG_SZ    4100
    Label.SCC-1-16    REG_SZ    CPU Core #16 Clock
    Value.SCC-1-16    REG_SZ    4100
    Label.SCPUMUL    REG_SZ    CPU Multiplier
    Value.SCPUMUL    REG_SZ    57
    Label.SCPUFSB    REG_SZ    CPU FSB
    Value.SCPUFSB    REG_SZ    100
    Label.SNBMUL    REG_SZ    North Bridge Multiplier
    Value.SNBMUL    REG_SZ    45
    Label.SNBCLK    REG_SZ    North Bridge Clock
    Value.SNBCLK    REG_SZ    4500
    Label.SMEMCLK    REG_SZ    Memory Clock
    Value.SMEMCLK    REG_SZ    3000
    Label.SMEMSPEED    REG_SZ    Memory Speed
    Value.SMEMSPEED    REG_SZ    DDR5-6000
    Label.SDRAMFSB    REG_SZ    DRAM:FSB Ratio
    Value.SDRAMFSB    REG_SZ    30:1
    Label.SMEMTIM    REG_SZ    Memory Timings
    Value.SMEMTIM    REG_SZ    32-38-38-96 CR2
    Label.SMOBONAME    REG_SZ    Motherboard Name
    Value.SMOBONAME    REG_SZ    Asus Prime Z690-A
    Label.SBIOSVER    REG_SZ    BIOS Version
    Value.SBIOSVER    REG_SZ    4101
    Label.SCPUUTI    REG_SZ    CPU Utilization
    Value.SCPUUTI    REG_SZ    8
    Label.SCPU1UTI    REG_SZ    CPU1 Utilization
    Value.SCPU1UTI    REG_SZ    0
    Label.SCPU2UTI    REG_SZ    CPU2 Utilization
    Value.SCPU2UTI    REG_SZ    11
    Label.SCPU3UTI    REG_SZ    CPU3 Utilization
    Value.SCPU3UTI    REG_SZ    0
    Label.SCPU4UTI    REG_SZ    CPU4 Utilization
    Value.SCPU4UTI    REG_SZ    0
    Label.SCPU5UTI    REG_SZ    CPU5 Utilization
    Value.SCPU5UTI    REG_SZ    0
    Label.SCPU6UTI    REG_SZ    CPU6 Utilization
    Value.SCPU6UTI    REG_SZ    0
    Label.SCPU7UTI    REG_SZ    CPU7 Utilization
    Value.SCPU7UTI    REG_SZ    0
    Label.SCPU8UTI    REG_SZ    CPU8 Utilization
    Value.SCPU8UTI    REG_SZ    0
    Label.SCPU9UTI    REG_SZ    CPU9 Utilization
    Value.SCPU9UTI    REG_SZ    0
    Label.SCPU10UTI    REG_SZ    CPU10 Utilization
    Value.SCPU10UTI    REG_SZ    0
    Label.SCPU11UTI    REG_SZ    CPU11 Utilization
    Value.SCPU11UTI    REG_SZ    0
    Label.SCPU12UTI    REG_SZ    CPU12 Utilization
    Value.SCPU12UTI    REG_SZ    0
    Label.SCPU13UTI    REG_SZ    CPU13 Utilization
    Value.SCPU13UTI    REG_SZ    0
    Label.SCPU14UTI    REG_SZ    CPU14 Utilization
    Value.SCPU14UTI    REG_SZ    0
    Label.SCPU15UTI    REG_SZ    CPU15 Utilization
    Value.SCPU15UTI    REG_SZ    0
    Label.SCPU16UTI    REG_SZ    CPU16 Utilization
    Value.SCPU16UTI    REG_SZ    0
    Label.SCPU17UTI    REG_SZ    CPU17 Utilization
    Value.SCPU17UTI    REG_SZ    0
    Label.SCPU18UTI    REG_SZ    CPU18 Utilization
    Value.SCPU18UTI    REG_SZ    0
    Label.SCPU19UTI    REG_SZ    CPU19 Utilization
    Value.SCPU19UTI    REG_SZ    0
    Label.SCPU20UTI    REG_SZ    CPU20 Utilization
    Value.SCPU20UTI    REG_SZ    0
    Label.SCPU21UTI    REG_SZ    CPU21 Utilization
    Value.SCPU21UTI    REG_SZ    0
    Label.SCPU22UTI    REG_SZ    CPU22 Utilization
    Value.SCPU22UTI    REG_SZ    0
    Label.SCPU23UTI    REG_SZ    CPU23 Utilization
    Value.SCPU23UTI    REG_SZ    0
    Label.SCPU24UTI    REG_SZ    CPU24 Utilization
    Value.SCPU24UTI    REG_SZ    0
    Label.SMEMUTI    REG_SZ    Memory Utilization
    Value.SMEMUTI    REG_SZ    63
    Label.SUSEDMEM    REG_SZ    Used Memory
    Value.SUSEDMEM    REG_SZ    20533
    Label.SFREEMEM    REG_SZ    Free Memory
    Value.SFREEMEM    REG_SZ    12048
    Label.SVIRTMEMUTI    REG_SZ    Virtual Memory Utilization
    Value.SVIRTMEMUTI    REG_SZ    19
    Label.SUSEDVIRTMEM    REG_SZ    Used Virtual Memory
    Value.SUSEDVIRTMEM    REG_SZ    35266
    Label.SFREEVIRTMEM    REG_SZ    Free Virtual Memory
    Value.SFREEVIRTMEM    REG_SZ    148515
    Label.SPROCESSES    REG_SZ    Processes
    Value.SPROCESSES    REG_SZ    401
    Label.SUSERS    REG_SZ    Users
    Value.SUSERS    REG_SZ    1
    Label.SDRVCUTI    REG_SZ    Drive C: Utilization
    Value.SDRVCUTI    REG_SZ    72
    Label.SDRVCUSEDSPC    REG_SZ    Drive C: Used Space
    Value.SDRVCUSEDSPC    REG_SZ    666
    Label.SDRVCFREESPC    REG_SZ    Drive C: Free Space
    Value.SDRVCFREESPC    REG_SZ    263
    Label.SDRVDUTI    REG_SZ    Drive D: Utilization
    Value.SDRVDUTI    REG_SZ    59
    Label.SDRVDUSEDSPC    REG_SZ    Drive D: Used Space
    Value.SDRVDUSEDSPC    REG_SZ    545
    Label.SDRVDFREESPC    REG_SZ    Drive D: Free Space
    Value.SDRVDFREESPC    REG_SZ    386
    Label.SDRVEUTI    REG_SZ    Drive E: Utilization
    Value.SDRVEUTI    REG_SZ    24
    Label.SDRVEUSEDSPC    REG_SZ    Drive E: Used Space
    Value.SDRVEUSEDSPC    REG_SZ    129
    Label.SDRVEFREESPC    REG_SZ    Drive E: Free Space
    Value.SDRVEFREESPC    REG_SZ    412
    Label.SDRVFUTI    REG_SZ    Drive F: Utilization
    Value.SDRVFUTI    REG_SZ    78
    Label.SDRVFUSEDSPC    REG_SZ    Drive F: Used Space
    Value.SDRVFUSEDSPC    REG_SZ    2897
    Label.SDRVFFREESPC    REG_SZ    Drive F: Free Space
    Value.SDRVFFREESPC    REG_SZ    829
    Label.SDRVGUTI    REG_SZ    Drive G: Utilization
    Value.SDRVGUTI    REG_SZ    0
    Label.SDRVGUSEDSPC    REG_SZ    Drive G: Used Space
    Value.SDRVGUSEDSPC    REG_SZ    0.00
    Label.SDRVGFREESPC    REG_SZ    Drive G: Free Space
    Value.SDRVGFREESPC    REG_SZ    0.29
    Label.SSMASTA    REG_SZ    SMART Status
    Value.SSMASTA    REG_SZ    OK
    Label.SDSK1ACT    REG_SZ    Disk 1 Activity
    Value.SDSK1ACT    REG_SZ    0
    Label.SDSK1READSPD    REG_SZ    Disk 1 Read Speed
    Value.SDSK1READSPD    REG_SZ    0.0
    Label.SDSK1WRITESPD    REG_SZ    Disk 1 Write Speed
    Value.SDSK1WRITESPD    REG_SZ    0.0
    Label.SDSK2ACT    REG_SZ    Disk 2 Activity
    Value.SDSK2ACT    REG_SZ    0
    Label.SDSK2READSPD    REG_SZ    Disk 2 Read Speed
    Value.SDSK2READSPD    REG_SZ    0.0
    Label.SDSK2WRITESPD    REG_SZ    Disk 2 Write Speed
    Value.SDSK2WRITESPD    REG_SZ    0.0
    Label.SDSK3ACT    REG_SZ    Disk 3 Activity
    Value.SDSK3ACT    REG_SZ    0
    Label.SDSK3READSPD    REG_SZ    Disk 3 Read Speed
    Value.SDSK3READSPD    REG_SZ    0.0
    Label.SDSK3WRITESPD    REG_SZ    Disk 3 Write Speed
    Value.SDSK3WRITESPD    REG_SZ    0.0
    Label.SDSK4ACT    REG_SZ    Disk 4 Activity
    Value.SDSK4ACT    REG_SZ    0
    Label.SDSK4READSPD    REG_SZ    Disk 4 Read Speed
    Value.SDSK4READSPD    REG_SZ    0.0
    Label.SDSK4WRITESPD    REG_SZ    Disk 4 Write Speed
    Value.SDSK4WRITESPD    REG_SZ    0.0
    Label.SGPU1CLK    REG_SZ    GPU Clock
    Value.SGPU1CLK    REG_SZ    2700
    Label.SGPU1MEMCLK    REG_SZ    GPU Memory Clock
    Value.SGPU1MEMCLK    REG_SZ    15001
    Label.SGPU1UTI    REG_SZ    GPU Utilization
    Value.SGPU1UTI    REG_SZ    0
    Label.SGPU1MCUTI    REG_SZ    GPU MC Utilization
    Value.SGPU1MCUTI    REG_SZ    1
    Label.SGPU1VEUTI    REG_SZ    GPU VE Utilization
    Value.SGPU1VEUTI    REG_SZ    0
    Label.SGPU1BIUTI    REG_SZ    GPU BI Utilization
    Value.SGPU1BIUTI    REG_SZ    1
    Label.SGPU1USEDDEMEM    REG_SZ    GPU Used Dedicated Memory
    Value.SGPU1USEDDEMEM    REG_SZ    2614
    Label.SGPU1USEDDYMEM    REG_SZ    GPU Used Dynamic Memory
    Value.SGPU1USEDDYMEM    REG_SZ    239
    Label.SGPU1BUSTYP    REG_SZ    GPU Bus Type
    Value.SGPU1BUSTYP    REG_SZ    PCI-E 5.0 x16 @ 5.0 x16
    Label.SGPU1PERFCAP    REG_SZ    GPU PerfCap Reason
    Value.SGPU1PERFCAP    REG_SZ    Reliability Voltage
    Label.SVMEMUSAGE    REG_SZ    Video Memory Utilization
    Value.SVMEMUSAGE    REG_SZ    18
    Label.SUSEDVMEM    REG_SZ    Used Video Memory
    Value.SUSEDVMEM    REG_SZ    2920
    Label.SFREEVMEM    REG_SZ    Free Video Memory
    Value.SFREEVMEM    REG_SZ    13383
    Label.SSLISTA    REG_SZ    SLI Status
    Value.SSLISTA    REG_SZ    Disabled
    Label.SPRIIPADDR    REG_SZ    Primary IP Address
    Value.SPRIIPADDR    REG_SZ    192.168.88.13
    Label.SEXTIPADDR    REG_SZ    External IP Address
    Value.SEXTIPADDR    REG_SZ    188.163.109.135
    Label.SNIC1DLRATE    REG_SZ    NIC1 Download Rate
    Value.SNIC1DLRATE    REG_SZ    0.0
    Label.SNIC1ULRATE    REG_SZ    NIC1 Upload Rate
    Value.SNIC1ULRATE    REG_SZ    0.0
    Label.SNIC1TOTDL    REG_SZ    NIC1 Total Download
    Value.SNIC1TOTDL    REG_SZ    0.0
    Label.SNIC1TOTUL    REG_SZ    NIC1 Total Upload
    Value.SNIC1TOTUL    REG_SZ    0.0
    Label.SNIC1CONNSPD    REG_SZ    NIC1 Connection Speed
    Value.SNIC1CONNSPD    REG_SZ    3
    Label.SNIC2DLRATE    REG_SZ    NIC2 Download Rate
    Value.SNIC2DLRATE    REG_SZ    4.1
    Label.SNIC2ULRATE    REG_SZ    NIC2 Upload Rate
    Value.SNIC2ULRATE    REG_SZ    30.8
    Label.SNIC2TOTDL    REG_SZ    NIC2 Total Download
    Value.SNIC2TOTDL    REG_SZ    60077.0
    Label.SNIC2TOTUL    REG_SZ    NIC2 Total Upload
    Value.SNIC2TOTUL    REG_SZ    3265.7
    Label.SNIC2CONNSPD    REG_SZ    NIC2 Connection Speed
    Value.SNIC2CONNSPD    REG_SZ    2500
    Label.SNIC3DLRATE    REG_SZ    NIC3 Download Rate
    Value.SNIC3DLRATE    REG_SZ    0.0
    Label.SNIC3ULRATE    REG_SZ    NIC3 Upload Rate
    Value.SNIC3ULRATE    REG_SZ    0.0
    Label.SNIC3TOTDL    REG_SZ    NIC3 Total Download
    Value.SNIC3TOTDL    REG_SZ    0.0
    Label.SNIC3TOTUL    REG_SZ    NIC3 Total Upload
    Value.SNIC3TOTUL    REG_SZ    0.0
    Label.SNIC4DLRATE    REG_SZ    NIC4 Download Rate
    Value.SNIC4DLRATE    REG_SZ    0.0
    Label.SNIC4ULRATE    REG_SZ    NIC4 Upload Rate
    Value.SNIC4ULRATE    REG_SZ    0.0
    Label.SNIC4TOTDL    REG_SZ    NIC4 Total Download
    Value.SNIC4TOTDL    REG_SZ    0.0
    Label.SNIC4TOTUL    REG_SZ    NIC4 Total Upload
    Value.SNIC4TOTUL    REG_SZ    0.0
    Label.SNIC5DLRATE    REG_SZ    NIC5 Download Rate
    Value.SNIC5DLRATE    REG_SZ    0.0
    Label.SNIC5ULRATE    REG_SZ    NIC5 Upload Rate
    Value.SNIC5ULRATE    REG_SZ    0.0
    Label.SNIC5TOTDL    REG_SZ    NIC5 Total Download
    Value.SNIC5TOTDL    REG_SZ    0.0
    Label.SNIC5TOTUL    REG_SZ    NIC5 Total Upload
    Value.SNIC5TOTUL    REG_SZ    0.0
    Label.SDESKRES    REG_SZ    Desktop Resolution
    Value.SDESKRES    REG_SZ    3440 x 1440
    Label.SVREFRATE    REG_SZ    Vertical Refresh Rate
    Value.SVREFRATE    REG_SZ    175
    Label.SMASTVOL    REG_SZ    Master Volume
    Value.SMASTVOL    REG_SZ    20
    Label.SBATT    REG_SZ    Battery
    Value.SBATT    REG_SZ    No Battery
    Label.SAIDAFPS    REG_SZ    AIDA FPS
    Value.SAIDAFPS    REG_SZ    0
    Label.SRTSSFPS    REG_SZ    RTSS FPS
    Value.SRTSSFPS    REG_SZ    0