[Variables]
; ---------- Shared ----------
UpdateMs=1000
FontName=Trebuchet MS
FontSize=22
TitleSize=30
Padding=10

; Strict, minimal, black & white. All alphas are 255 (no transparency).
ColorText=255,255,255,255
ColorMuted=170,170,170,255
ColorTitle=255,255,255,255
; Background is 90% opaque (alpha 230 = 0.9 * 255). Everything else stays
; fully opaque so borders, text, bars and graph lines remain crisp.
ColorBg=0,0,0,200
; Frame stroke around panels and graph rectangles. Pure white for a crisp,
; high-contrast border on the black background.
ColorBorder=255,255,255,255
ColorDivider=255,255,255,255
ColorBarBg=50,50,50,255
ColorBar=255,255,255,255
; Subtle grid lines drawn INSIDE the graph rectangle (HorizontalLines).
; Kept dark on purpose — making these white would overwhelm the graph.
ColorGraphGrid=70,70,70,255

; ---------- Tile grid sizing ----------
; A panel's outer size is computed from a tile grid:
;   PanelWidth  = CELLS_COUNT * CellW
;   PanelHeight = ROWS_COUNT  * RowH
; Each .ini overrides CELLS_COUNT / ROWS_COUNT in its own [Variables] section.
; Defaults give a standard 1x1 tile (400x280) sized to fit a typical
; panel with a graph.
CellW=480
RowH=308
CELLS_COUNT=1
ROWS_COUNT=1
PanelWidth=(#CELLS_COUNT#*#CellW#)
PanelHeight=(#ROWS_COUNT#*#RowH#)
Width=#PanelWidth#

; ---------- Inner content layout (rows of text inside a panel) ----------
; No rounded corners. Keep these names so existing Shape definitions still
; resolve, but the value is 0 to render strict square corners everywhere.
PanelRadius=0
PanelBottomPadding=10

TitleX=#Padding#
TitleY=10

DividerX=#Padding#
DividerY=60
DividerW=(#PanelWidth#-(#Padding#*2))
DividerH=1

LineH=40
BarOffsetY=32
BarH=6

LabelX=#Padding#
ValueX=(#PanelWidth#-#Padding#)

BarX=#Padding#
BarW=(#PanelWidth#-(#Padding#*2))

; ---------- AIDA64 Registry ----------
AIDAKey=Software\FinalWire\AIDA64\SensorValues

; AIDA64 CPU - original names
AIDA_CPU_Clock=Value.SCPUCLK
AIDA_CPU_Util=Value.SCPUUTI
AIDA_CPU_Temp=Value.TCPU
AIDA_CPU_Cores=Value.SCPUCLK

; AIDA64 CPU - aliases used by newer panels
AIDA_CPUClock=#AIDA_CPU_Clock#
AIDA_CPUUtil=#AIDA_CPU_Util#
AIDA_CPUTemp=#AIDA_CPU_Temp#

; AIDA64 GPU - original names
AIDA_GPU_Clock=Value.SGPU1CLK
AIDA_GPU_Util=Value.SGPU1UTI
AIDA_GPU_Temp=Value.TGPU1
AIDA_GPU_VRAMPercent=Value.SVMEMUSAGE
AIDA_GPU_VRAMUsed=Value.SUSEDVMEM
AIDA_GPU_VRAMFree=Value.SFREEVMEM
AIDA_GPU_FPS=Value.SRTSSFPS

; AIDA64 GPU - aliases used by newer panels
AIDA_GPUClock=#AIDA_GPU_Clock#
AIDA_GPUUtil=#AIDA_GPU_Util#
AIDA_GPUTemp=#AIDA_GPU_Temp#
AIDA_GPUVRAMPercent=#AIDA_GPU_VRAMPercent#
AIDA_GPUVRAMUsed=#AIDA_GPU_VRAMUsed#
AIDA_GPUVRAMFree=#AIDA_GPU_VRAMFree#
AIDA_FPS=#AIDA_GPU_FPS#

; AIDA64 MEM - original names
AIDA_MEM_Clock=Value.SMEMCLK
AIDA_MEM_Util=Value.SMEMUTI
AIDA_MEM_Used=Value.SUSEDMEM
AIDA_MEM_Free=Value.SFREEMEM

; AIDA64 MEM - aliases used by newer panels
AIDA_MEMClock=#AIDA_MEM_Clock#
AIDA_MEMUtil=#AIDA_MEM_Util#
AIDA_MEMUsed=#AIDA_MEM_Used#
AIDA_MEMFree=#AIDA_MEM_Free#

; ---------- Glances API (https://github.com/nicolargo/glances/blob/develop/docs/api/restful.rst) ----------
; Must match server scheme (http:// redirects to HTML, breaks WebParser)
GlancesBaseUrl=https://glances.tcloud.monster/api/4
GlancesUpdateRate=3
; Rainmeter WebParser: 0 = skip TLS verify (needed for some Glances hosts)
GlancesSSLVerify=0
; Per-item keys for /gpu/gpu_id/value/{id} and /network/interface_name/value/{name}
GlancesGpuId=nvidia0
GlancesNetworkInterface=enp5s0
GlancesCpuUrl=#GlancesBaseUrl#/cpu
GlancesMemUrl=#GlancesBaseUrl#/mem
GlancesGpuUrl=#GlancesBaseUrl#/gpu/gpu_id/value/#GlancesGpuId#
GlancesNetworkUrl=#GlancesBaseUrl#/network/interface_name/value/#GlancesNetworkInterface#
; Working set fetched from Glances; client-side Lua sorts and trims to TopCount.
; 50 is a good compromise: small payload (~30KB) yet wide enough to contain
; both the top processes by CPU and the top processes by RSS in practice.
GlancesProcessListTopN=50
GlancesProcessListUrl=#GlancesBaseUrl#/processlist/top/#GlancesProcessListTopN#

; Aliases used by panels
GlancesCPUUrl=#GlancesCpuUrl#
GlancesMEMUrl=#GlancesMemUrl#
GlancesGPUUrl=#GlancesGpuUrl#
GlancesNETWORKUrl=#GlancesNetworkUrl#
GlancesPROCESSESUrl=#GlancesProcessListUrl#

; Glances PROCESSES panels - names hidden from the top list (case-insensitive, pipe-separated)
GlancesProcBlacklist=systemd|kthreadd|kworker|ksoftirqd|migration|rcu_sched|rcu_preempt|swapper|init|System Idle Process


; ---------- Graph layout ----------
GraphX=#Padding#
GraphW=(#PanelWidth#-(#Padding#*2))
GraphH=70
GraphRadius=0
; Single shared anchor for the graph Y position. Anchored to the bottom of
; the panel so every graph baseline lines up across panels regardless of
; how many text rows sit above it. Resolves to 200 with the default tile
; (PanelHeight=280, GraphH=70, PanelBottomPadding=10). Always reference
; #GraphY# from per-panel graph meters; never inline (#LineH#*N).
GraphY=(#PanelHeight#-#GraphH#-#PanelBottomPadding#)

ColorGraphBg=20,20,20,255
ColorGraph1=255,255,255,255
ColorGraph2=170,170,170,255
ColorGraph3=255,255,255,255
