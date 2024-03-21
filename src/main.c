/*
 * Custom oscillator for Korg *logue synthesizers.
 * Simulates Vox Humana preset from Polymoog Keyboard 280a.
 * Author: Grzegorz Szwoch (GregVuki)
 */

#include "userosc.h"

const float pitch_scale = 0.00390625f; // 1/256
const float step_scale = 2.0833333333333333e-05f; // 1/48000
const float int32_scale = 2147483646.f; // 2**31-2, scale (-1..1) to int32_t range
const float shape_scale = 0.0009765625f; // 1/1024, scale the shape parameter
extern const float midi_to_hz_lut_f[152]; // defined in the firmware

// Global parameters
// current pitch
float cur_pitch = 69.f;
// saw wave
float saw_last = 0.f;
float saw_phase = 0.5f;
float saw_detune = -0.04f; // -4 cents
float saw_mix = 0.45f;
// square wave
float squ_last = 1.f;
float squ_phase = 0.f;
float squ_pwidth = 0.5f;
float squ_mix = 0.45f;
// LFO - saw wave pitch modulation
float lfo_saw_phase = 0.75f;
float lfo_saw_step = 5.0f * step_scale; // 5.0 Hz
float lfo_saw_depth = 0.50f; // 50 cents
// LFO - square wave pitch modulation
float lfo_squ_phase = 0.75f;
float lfo_squ_step = 3.5f * step_scale; // 3.5 Hz
float lfo_squ_depth = 0.31f; // 31 cents
// LFO - square wave pulse width modulation
float lfo_pwm_phase = 0.75f;
float lfo_pwm_step = 2.0f * step_scale; // 2.0 Hz
float lfo_pwm_depth = 0.31f; // 31%
// parameter initialization
int8_t init_done = 0;
// End of global parameters

// Triangle wave LFO - update phase and value
__fast_inline float update_lfo(float* phase, float step)
{
    // update and wrap phase
    *phase += step;
    *phase -= (int)(*phase);
    // calculate triangle wave
    float y = 2.f * (*phase) - 1.f;
    uint32_t* py = (uint32_t*)(&y);
    *py &= 0x7fffffff; // abs(y) - clear sign bit
    return (2.f * y - 1.f);
}

// Convert pitch to phase step
__fast_inline float calc_step(float pitch)
{
    const uint8_t semi = (uint8_t)pitch;
    const float frac = pitch - semi;
    const float f1 = midi_to_hz_lut_f[semi];
    // use quadratic approximation
    const float fscale = (frac > 1e-3) ? (0.00171723f * frac * frac + 0.05774266f * frac + 1.0000016f) : 1.f;
    const float freq = f1 * fscale;
    return step_scale * freq;
}

// Called once, when the oscillator is loaded.
void OSC_INIT(uint32_t platform, uint32_t api)
{
    (void)platform;
    (void)api;
    init_done = 0;
}

// Generate 'nframes' samples, write them to 'framebuf'.
void OSC_CYCLE(const user_osc_param_t* const params, int32_t* framebuf, const uint32_t nframes)
{
    // glide the pitch if it has changed
    const float new_pitch = pitch_scale * params->pitch;
    const float pitch_delta = (nframes == 64) ? 0.015625f * (new_pitch - cur_pitch) : (new_pitch - cur_pitch) / nframes;

    // generation loop
    uint32_t n;
    for (n = 0; n < nframes; ++n) {
        // calculate LFOs
        const float mod_saw = update_lfo(&lfo_saw_phase, lfo_saw_step);
        const float mod_squ = update_lfo(&lfo_squ_phase, lfo_squ_step);
        const float mod_pwm = update_lfo(&lfo_pwm_phase, lfo_pwm_step);
        // calculate pitch and pulse width
        cur_pitch += pitch_delta;
        const float saw_pitch = cur_pitch + saw_detune + lfo_saw_depth * mod_saw;
        const float squ_pitch = cur_pitch + lfo_squ_depth * mod_squ;
        const float squ_cur_pwidth = squ_pwidth + lfo_pwm_depth * mod_pwm;
        // (assume that squ_cur_pwidth newer goes outside (0, 1))
        // convert pitch to phase step
        const float saw_step = calc_step(saw_pitch);
        const float squ_step = calc_step(squ_pitch);
        // update phase
        saw_phase += saw_step;
        saw_phase -= (int)saw_phase;
        squ_phase += squ_step;
        squ_phase -= (int)squ_phase;

        // calculate saw wave
        float saw_val = 2.f * saw_phase - 1.f;
        if (saw_phase < saw_step) {
            // phase wrap occurred - calculate PolyBLEP
            const float d1 = saw_phase / saw_step;
            saw_last -= d1 * d1;
            saw_val += (d1 - 1.f) * (d1 - 1.f);
        }

        // calculate square wave
        float squ_val = (squ_phase < squ_cur_pwidth) ? 1.f : -1.f;
        if (squ_phase < squ_step) {
            // phase wrap occurred - calculate PolyBLEP (pulse up)
            const float d2 = squ_phase / squ_step;
            squ_last += d2 * d2;
            squ_val -= (d2 - 1.f) * (d2 - 1.f);
        }
        if (squ_cur_pwidth <= squ_phase && squ_phase < squ_cur_pwidth + squ_step) {
            // pulse state changed - calculate PolyBLEP (pulse down)
            const float d3 = (squ_phase - squ_cur_pwidth) / squ_step;
            squ_last -= d3 * d3;
            squ_val += (d3 - 1.f) * (d3 - 1.f);
        }

        // mix waves and save values
        const float out = saw_mix * saw_last + squ_mix * squ_last;
        saw_last = saw_val;
        squ_last = squ_val;

        // convert to integer and write to the buffer
        framebuf[n] = (int32_t)(int32_scale * out + 0.5f);
    }

    cur_pitch = new_pitch;
}

// Called on Note On.
void OSC_NOTEON(const user_osc_param_t* const params)
{
    cur_pitch = pitch_scale * params->pitch;
}

// Called on Note Off
void OSC_NOTEOFF(const user_osc_param_t* const params)
{
    (void)params;
}

// Called when a parameter value is changed.
void OSC_PARAM(uint16_t index, uint16_t value)
{
    // When the unit is loaded, this function is called for all parameters, with value = 0.
    // Detect this and avoid overwriting the default values.
    // Change the parameter only after the value is nonzaro for the first time.
    if (!init_done) {
        if (value > 0) {
            init_done = 1;
        } else {
            return; // zero sent and initialization is not done yet
        }
    }

    switch (index) {
    case k_user_osc_param_id1:
        // Param 1: Saw wave pitch modulation rate, in 0.1 Hz units
        lfo_saw_step = 0.1f * (float)value * step_scale;
        break;

    case k_user_osc_param_id2:
        // Param2: Saw wave pitch modulation depth, in cents
        lfo_saw_depth = 0.01f * (float)value;
        break;

    case k_user_osc_param_id3:
        // Param3: Square wave pitch modulation rate, in 0.1 Hz units
        lfo_squ_step = 0.1f * (float)value * step_scale;
        break;

    case k_user_osc_param_id4:
        // Param4: Square wave pitch modulation depth, in cents
        lfo_squ_depth = 0.01f * (float)value;
        break;

    case k_user_osc_param_id5:
        // Param5: Square wave pulse width modulation rate, in 0.1 Hz units
        lfo_pwm_step = 0.1f * (float)value * step_scale;
        break;

    case k_user_osc_param_id6:
        // Param6: Square wave pulse width modulation depth, in units of 0.005%
        lfo_pwm_depth = 0.005f * (float)value;
        break;

    case k_user_osc_param_shape:
        // Shape (0..1023): percentage of the saw wave in the mix
        {
            const float saw_perc = shape_scale * value; // value/1024
            saw_mix = 0.9f * saw_perc;
            squ_mix = 0.9f * (1.f - saw_perc);
        }
        break;

    case k_user_osc_param_shiftshape:
        // Shift+Shape (0..1023): negative detune of the saw wave
        // 100% (1024) = 100 c
        saw_detune = -shape_scale * (float)value; // -value/1024
        break;

    default:
        break;
    }
}
