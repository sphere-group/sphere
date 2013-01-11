#ifndef SFXR_HPP
#define SFXR_HPP

// identifier too long
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <stack>
#include <string>
#include <cstdio>
#include <audiere.h>
#include "audio.hpp"

class sSfxr
{
public:
	enum WaveType {
        SQUAREWAVE = 0,
        SAWTOOTH,
        SINEWAVE,
        NOISE,
        NUM_WAVETYPES,
    };
    sSfxr();
    virtual ~sSfxr();

    //bool DefineParams(int );
	audiere::SoundEffect* getSoundEffect(audiere::SoundEffectType type, audiere::File* memoryfile);
	bool SaveWav(const char* filename);

	void ResetParams();
	long CalcSampleSize(int length);
	long GetSampleSize(){
		long bytearraylength = 0;
		// Lets calculate how big the bytearray must be...
		ResetSample(false);
		playing_sample=true;
		while(playing_sample)
			bytearraylength += CalcSampleSize(256);
		return bytearraylength;
	}

	//int getMaxWaveTypes() { return NUM_WAVETYPES; }


private:

	void ResetSample(bool restart);
	void SynthSample(int length, float* buffer, FILE* file, char* filebuffer);


public: // could be private...

	float master_vol;
	float sound_vol;
	int wav_bits; // 8 or 16
	int wav_freq; // 44100, 

	int wave_type; // 0..3

	float p_base_freq;
	float p_freq_limit;
	float p_freq_ramp;
	float p_freq_dramp;

	float p_duty;
	float p_duty_ramp;

	float p_vib_strength;
	float p_vib_speed;
	float p_vib_delay;

	float p_env_attack;
	float p_env_sustain;
	float p_env_decay;
	float p_env_punch;

	bool filter_on;
	
	float p_lpf_freq;
	float p_lpf_ramp;
	float p_lpf_resonance;
	float p_hpf_freq;
	float p_hpf_ramp;

	float p_pha_offset;
	float p_pha_ramp;

	
	float p_repeat_speed;

	float p_arp_speed;
	float p_arp_mod;


public:

	float getMasterVolume() const { return master_vol; }
	void setMasterVolume(float v) { master_vol = v; }
	float getSoundVolume() const { return sound_vol; }
	void setSoundVolume(float v) { sound_vol = v; }
	int getBitrate() const { return wav_bits; }
	void setBitrate(int v) { wav_bits = v; }
	int getSampleRate() const { return wav_freq; }
	void setSampleRate(int v) { wav_freq = v; }
	int getWaveType() const { return wave_type; }
	void setWaveType(int v);

	float getBaseFrequency() const { return p_base_freq; }
	void setBaseFrequency(float v) { p_base_freq = v; if(p_freq_limit>v) p_freq_limit = v;}
	float getMinFrequency() const { return p_freq_limit; }
	void setMinFrequency(float v) { p_freq_limit = v>p_base_freq ? p_base_freq : v; }
	float getFrequencySlide() const { return p_freq_ramp; }
	void setFrequencySlide(float v) { p_freq_ramp = v; }
	float getFrequencySlideDelta() const { return p_freq_dramp; }
	void setFrequencySlideDelta(float v) { p_freq_dramp = v; }

	float getSquareDuty() const { return p_duty; }
	void setSquareDuty(float v) { p_duty = v; }
	float getSquareDutySweep() const { return p_duty_ramp; }
	void setSquareDutySweep(float v) { p_duty_ramp = v; }

	float getVibratoDepth() const { return p_vib_strength; }
	void setVibratoDepth(float v) { p_vib_strength = v; }
	float getVibratoSpeed() const { return p_vib_speed; }
	void setVibratoSpeed(float v) { p_vib_speed = v; }
	float getVibratoDelay() const { return p_vib_delay; }
	void setVibratoDelay(float v) { p_vib_delay = v; }

	float getAttack() const { return p_env_attack; }
	void setAttack(float v) { p_env_attack = v; }
	float getSustain() const { return p_env_sustain; }
	void setSustain(float v) { p_env_sustain = v; }
	float getDecay() const { return p_env_decay; }
	void setDecay(float v) { p_env_decay = v; }
	float getRelease() const { return p_env_punch; } ; // GetPunch() ?
	void setRelease(float v) { p_env_punch = v; }

	bool getFilter() const { return filter_on; } // Seems to do nothing for now...
	void setFilter(bool v) { filter_on = v; }

	float getLowPassFilterCutoff() const { return p_lpf_freq; }
	void setLowPassFilterCutoff(float v) { p_lpf_freq = v; }
	float getLowPassFilterCutoffSweep() const { return p_lpf_ramp; }
	void setLowPassFilterCutoffSweep(float v) { p_lpf_ramp = v; }
	float getFilterResonance() const { return p_lpf_resonance; }
	void setFilterResonance(float v) { p_lpf_resonance = v; }
	float getHighPassFilterCutoff() const { return p_hpf_freq; }
	void setHighPassFilterCutoff(float v) { p_hpf_freq = v; }
	float getHighPassFilterCutoffSweep() const { return p_hpf_ramp; }
	void setHighPassFilterCutoffSweep(float v) { p_hpf_ramp = v; }

	float getPhaserOffset() const { return p_pha_offset; }
	void setPhaserOffset(float v) { p_pha_offset = v; }
	float getPhaserOffsetSweep() const { return p_pha_ramp; }
	void setPhaserOffsetSweep(float v) { p_pha_ramp = v; }

	float getRepeatSpeed() const { return p_repeat_speed; }
	void setRepeatSpeed(float v) { p_repeat_speed = v; }

	float getArpeggio() const { return p_arp_speed; } // 'change amount'
	void setArpeggio(float v) { p_arp_speed = v; }
	float getArpeggioSpeed() const { return p_arp_mod; }
	void setArpeggioSpeed(float v) { p_arp_mod = v; }


private:

	int file_sampleswritten; // no access
	float filesample; // no acces
	int fileacc; // no acces
	bool playing_sample; // no acces

	int phase; // internal for phasercalc

	double fperiod;
	double fmaxperiod;
	double fslide;
	double fdslide;
	int period;
	float square_duty;
	float square_slide;
	int env_stage;
	int env_time;
	int env_length[3];
	float env_vol;
	float fphase;
	float fdphase;
	int iphase;
	float phaser_buffer[1024];
	int ipp;
	float noise_buffer[32];
	float fltp;
	float fltdp;
	float fltw;
	float fltw_d;
	float fltdmp;
	float fltphp;
	float flthp;
	float flthp_d;
	float vib_phase;
	float vib_speed;
	float vib_amp;
	int rep_time;
	int rep_limit;
	int arp_time;
	int arp_limit;
	double arp_mod;

};
#endif
