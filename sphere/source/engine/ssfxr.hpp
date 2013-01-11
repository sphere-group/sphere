#ifndef __SSFXR_H
#define __SSFXR_H

#include "sfxr.hpp"


class SSFXR
{
public:
    SSFXR();
    ~SSFXR();

	audiere::SoundEffect* getSoundEffect(audiere::SoundEffectType type, audiere::File* memoryfile) { return m_Sfxr.getSoundEffect(type, memoryfile); };
    bool Save(const char* filename);
	
    void Destroy();
    SSFXR* Clone();
	bool Reset();

	int getMaxWaveTypes() { return m_Sfxr.NUM_WAVETYPES; }
	int GetSampleSize() {return (int)m_Sfxr.GetSampleSize(); };

	// Clamp between 0 and 1
	inline float clampPu(double v){ return (float)(v<0.0? 0.0 : v>1.0? 1.0: v); }

	// Clamp between -1 and 1
	inline float clampNPu(double v){ return (float)(v<-1.0? -1.0 : v>1.0? 1.0: v); }

	inline double getMasterVolume() const { return (double)m_Sfxr.master_vol; }
	void setMasterVolume(float v) { m_Sfxr.setMasterVolume(clampPu(v)); }
	double getSoundVolume() const { return (double)m_Sfxr.sound_vol; }
	void setSoundVolume(float v) { m_Sfxr.sound_vol = clampPu(v); }
	int getBitrate() const { return m_Sfxr.wav_bits; }
	void setBitrate(int v) { m_Sfxr.wav_bits = v; }
	int getSampleRate() const { return m_Sfxr.wav_freq; }
	void setSampleRate(int v) { m_Sfxr.wav_freq = v; }
	int getWaveType() const { return m_Sfxr.wave_type; }
	void setWaveType(int v) { m_Sfxr.wave_type = v; }

	float getBaseFrequency() const { return m_Sfxr.p_base_freq; }
	void setBaseFrequency(float v) { m_Sfxr.setBaseFrequency( clampPu(v) ); }
	float getMinFrequency() const { return m_Sfxr.p_freq_limit; }
	void setMinFrequency(float v) { m_Sfxr.setMinFrequency( clampPu(v) ); }
	float getFrequencySlide() const { return m_Sfxr.p_freq_ramp; }
	void setFrequencySlide(float v) { m_Sfxr.p_freq_ramp = clampNPu(v); }
	float getFrequencySlideDelta() const { return m_Sfxr.p_freq_dramp; }
	void setFrequencySlideDelta(float v) { m_Sfxr.p_freq_dramp = clampNPu(v); }

	float getSquareDuty() const { return m_Sfxr.p_duty; }
	void setSquareDuty(float v) { m_Sfxr.p_duty = clampPu(v); }
	float getSquareDutySweep() const { return m_Sfxr.p_duty_ramp; }
	void setSquareDutySweep(float v) { m_Sfxr.p_duty_ramp = clampNPu(v); }

	float getVibratoDepth() const { return m_Sfxr.p_vib_strength; }
	void setVibratoDepth(float v) { m_Sfxr.p_vib_strength = clampPu(v); }
	float getVibratoSpeed() const { return m_Sfxr.p_vib_speed; }
	void setVibratoSpeed(float v) { m_Sfxr.p_vib_speed = clampPu(v); }
	float getVibratoDelay() const { return m_Sfxr.p_vib_delay; }
	void setVibratoDelay(float v) { m_Sfxr.p_vib_delay = clampPu(v); }

	float getAttack() const { return m_Sfxr.p_env_attack; }
	void setAttack(float v) { m_Sfxr.p_env_attack = clampPu(v); }
	float getSustain() const { return m_Sfxr.p_env_sustain; }
	void setSustain(float v) { m_Sfxr.p_env_sustain = clampPu(v); }
	float getDecay() const { return m_Sfxr.p_env_decay; }
	void setDecay(float v) { m_Sfxr.p_env_decay = clampPu(v); }
	float getRelease() const { return m_Sfxr.p_env_punch; } ; // GetPunch() ?
	void setRelease(float v) { m_Sfxr.p_env_punch = clampPu(v); }

	bool getFilter() const { return m_Sfxr.filter_on; } // Seems to do nothing for now...
	void setFilter(bool v) { m_Sfxr.filter_on = v; }

	float getLowPassFilterCutoff() const { return m_Sfxr.p_lpf_freq; }
	void setLowPassFilterCutoff(float v) { m_Sfxr.p_lpf_freq = clampPu(v); }
	float getLowPassFilterCutoffSweep() const { return m_Sfxr.p_lpf_ramp; }
	void setLowPassFilterCutoffSweep(float v) { m_Sfxr.p_lpf_ramp = clampNPu(v); }
	float getFilterResonance() const { return m_Sfxr.p_lpf_resonance; }
	void setFilterResonance(float v) { m_Sfxr.p_lpf_resonance = clampPu(v); }
	float getHighPassFilterCutoff() const { return m_Sfxr.p_hpf_freq; }
	void setHighPassFilterCutoff(float v) { m_Sfxr.p_hpf_freq = clampPu(v); }
	float getHighPassFilterCutoffSweep() const { return m_Sfxr.p_hpf_ramp; }
	void setHighPassFilterCutoffSweep(float v) { m_Sfxr.p_hpf_ramp = clampNPu(v); }

	float getPhaserOffset() const { return m_Sfxr.p_pha_offset; }
	void setPhaserOffset(float v) { m_Sfxr.p_pha_offset = clampNPu(v); }
	float getPhaserOffsetSweep() const { return m_Sfxr.p_pha_ramp; }
	void setPhaserOffsetSweep(float v) { m_Sfxr.p_pha_ramp = clampNPu(v); }

	float getRepeatSpeed() const { return m_Sfxr.p_repeat_speed; }
	void setRepeatSpeed(float v) { m_Sfxr.p_repeat_speed = clampPu(v); }

	float getArpeggio() const { return m_Sfxr.p_arp_speed; } // 'change amount'
	void setArpeggio(float v) { m_Sfxr.p_arp_speed = clampPu(v); }
	float getArpeggioSpeed() const { return m_Sfxr.p_arp_mod; }
	void setArpeggioSpeed(float v) { m_Sfxr.p_arp_mod = clampNPu(v); }



private:
    bool Initialize();

private:
    sSfxr  m_Sfxr;
};

#endif
