# Vox Humana - Polymoog 280a preset - Emulation notes

## Video by _nonedescriptivep_.

[_Polymoog "Vox Humana" preset | Showcase & Tutorial_](https://www.youtube.com/watch?v=W1mi1L1AuSI)

Osc 1
* Saw
* Detune: -1 Hz
* Level: -26 dB

Osc 2
* Square
* Pulse width ~50% (53% used)
* Level: -15 dB

LFO 1
* Target: Osc 1 (Saw) pitch
* Rate: 5 Hz
* Amount: 40 cent
* Shape: Triangle or Sine

LFO 2
* Target: Osc 2 (Square) pitch
* Rate: 3.1 Hz
* Amount: 24 cent
* Shape: Triangle or Sine

LFO 3
* Target: Osc 2 (Square) pulse width
* Rate: 0.2 Hz
* Depth ~25% (28 used)
* Shape: Triangle or Sine

Filter
* Lowpass, 12 dB/oct
* Cutoff: 2637 Hz
* Resonance: -7.0 dB
* Key tracking: 144 cent/key

Envelope
* Attack: 0.2 s
* Decay: 0
* Sustain: 100%
* Release: 4 s

Effects
* Phaser
* Reverb

Sound samples in the video:
* 6:42: modulated saw wave only (A)
* 6:53: modulated saw wave only (C)
* 7:37: modulated square wave only (Bflat)
* 9:04: pulse wave without PWM
* 10:12: both waves together
* 11:56: sequences of sounds
* 12:49: added reverb
* 13:13: added phaser


## _Keyboard_ magazine article

[_Steal This Sound, Gary Numan's “Cars”_](https://web.archive.org/web/20150928161839/http://www.keyboardmag.com/create-sounds/1256/steal-this-sound-gary-numans-cars/28419)

Preset for [Arturia Jupiter-8V](https://downloads.arturia.com/products/jupiter8-v/manual/Jupiter-8V_Manual_2_5_EN.pdf) 
(emulation of [Roland Jupiter 8](https://cdn.roland.com/assets/media/pdf/JP-8_OM.pdf)).

VCO1
* Shape: saw

VCO2
* Shape: pulse
* small detune

Mixer
* equal mix of both VCOs

LFO1
* Target: VCO2 pulse width
* Rate: 5.72 Hz
* Amount: 0.625 (62.5% ?)

LFO2
* Target: VCO1 (saw) pitch
* Rate: 5.04 Hz
* Amount: 0.084 (50.4 c)

LFO3
* Target: VCO2 (pulse) pitch
* Rate: 3.5 Hz
* Amount: 0.052 (31.2 c) 

VCF:
* Cutoff 1.986 kHz
* Resonance: 0
* Key follow: 0.75

Envelope: 
* Attack: 453 ms
* Decay: 0
* Sustain: 100%
* Release: 4.942 ms

Effects
* reverb
* and other effects if needed

Conversion of LFO amount to cents: (value * 600)
* amount scale: 0 - 10
* amount = 10: CV = 5V
* CV = 1V: depth = 1200 c (1V / oct)
* depth = (0.5 * amount) * 1200 = 600 * amount


## Gearspace

[How to reproduce the Polymoog "Vox Humana" sound](https://gearspace.com/board/electronic-music-instruments-and-electronic-music-production/1078110-how-reproduce-polymoog-quot-vox-humana-quot-sound.html)
