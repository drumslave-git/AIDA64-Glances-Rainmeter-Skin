[StyleBackground]
W=#PanelWidth#
SolidColor=#ColorBg#

; Shared panel background + 1px border. Used by every panel's [MeterBg]
; (Meter=Shape). The path is inset by 0.5 px and shrunk by 1 px so the
; centered 1 px stroke lands exactly on pixels 0 and (Width-1) inside the
; panel bounds — the border is fully visible AND the meter's outer extent
; stays exactly #Width# x #PanelHeight#, so DynamicWindowSize never grows
; the window to accommodate the stroke.
[StyleBg]
Shape=Rectangle 0.5,0.5,(#Width#-1),(#PanelHeight#-1),#PanelRadius# | Fill Color #ColorBg# | StrokeWidth 1 | Stroke Color #ColorBorder#

[StyleTitle]
X=#TitleX#
Y=#TitleY#
FontFace=#FontName#
FontSize=#TitleSize#
FontColor=#ColorTitle#
StringStyle=Bold
AntiAlias=1
StringCase=Upper

[StyleLabel]
X=#LabelX#
H=#LineH#
FontFace=#FontName#
FontSize=#FontSize#
FontColor=#ColorText#
AntiAlias=1
StringAlign=Left

[StyleValue]
X=#ValueX#
H=#LineH#
FontFace=#FontName#
FontSize=#FontSize#
FontColor=#ColorText#
AntiAlias=1
StringAlign=Right

[StyleDivider]
SolidColor=#ColorDivider#
H=#DividerH#
W=#DividerW#

[StyleBarBg]
X=#BarX#
W=#BarW#
H=#BarH#
SolidColor=#ColorBarBg#

[StyleBar]
X=#BarX#
W=#BarW#
H=#BarH#
; Meter=Bar uses BarColor for the fill. SolidColor on a Bar meter is just
; the meter background; without BarColor Rainmeter defaults to bright green.
; The track behind the bar is handled separately by [StyleBarBg].
BarColor=#ColorBar#
BarOrientation=Horizontal


[StyleGraphBg]
X=#GraphX#
W=#GraphW#
H=#GraphH#

[StyleGraphLine]
X=#GraphX#
W=#GraphW#
H=#GraphH#
LineColor=#ColorGraph1#
LineWidth=2
AntiAlias=1
AutoScale=0
HorizontalLines=3
HorizontalLineColor=#ColorGraphGrid#

[StyleGraphLineAuto]
X=#GraphX#
W=#GraphW#
H=#GraphH#
LineColor=#ColorGraph1#
LineWidth=2
AntiAlias=1
AutoScale=1
HorizontalLines=3
HorizontalLineColor=#ColorGraphGrid#
