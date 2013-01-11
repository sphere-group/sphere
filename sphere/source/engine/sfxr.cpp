#include "sfxr.hpp"
#include <cstdlib>
#include <cmath>

/*
	Portions (huge portions, actually) of this SFXR library taken from 'sfxr', 
	a free sound effect generator by DrPetter, 2007-12-14
    developed for LD48#10
*/


#define rnd(n) (rand()%(n+1))
inline float frnd(float range)
{
	return (float)rnd(10000)/10000*range;
}


sSfxr::sSfxr()
        : master_vol(0.05f)
        , sound_vol(0.5f)
        , wav_bits(16)
        , wav_freq(44100)

        , filesample(0.0f)
		, fileacc(0)
		, playing_sample(false)
{
	ResetParams();
}

sSfxr::~sSfxr()
{
	// nothing for now, maybe later on we'll have to close a stream when we add .play()
}


void sSfxr::setWaveType(int v){ 
	if(v<0 || v >NUM_WAVETYPES)
		v = 0;
	wave_type = v; 
}

// bool playing_sample=false;


void sSfxr::ResetParams()
{
	wave_type = 0; // WaveType SQUAREWAVE

	p_base_freq=0.3f;
	p_freq_limit=0.0f;
	p_freq_ramp=0.0f;
	p_freq_dramp=0.0f;
	p_duty=0.0f;
	p_duty_ramp=0.0f;

	p_vib_strength=0.0f;
	p_vib_speed=0.0f;
	p_vib_delay=0.0f;

	p_env_attack=0.0f;
	p_env_sustain=0.3f;
	p_env_decay=0.4f;
	p_env_punch=0.0f;

	filter_on=false;
	p_lpf_resonance=0.0f;
	p_lpf_freq=1.0f;
	p_lpf_ramp=0.0f;
	p_hpf_freq=0.0f;
	p_hpf_ramp=0.0f;
	
	p_pha_offset=0.0f;
	p_pha_ramp=0.0f;

	p_repeat_speed=0.0f;

	p_arp_speed=0.0f;
	p_arp_mod=0.0f;
}


void sSfxr::ResetSample(bool restart)
{
	if(!restart)
		phase=0;
	fperiod=100.0/(p_base_freq*p_base_freq+0.001);
	period=(int)fperiod;
	fmaxperiod=100.0/(p_freq_limit*p_freq_limit+0.001);
	fslide=1.0-pow((double)p_freq_ramp, 3.0)*0.01;
	fdslide=-pow((double)p_freq_dramp, 3.0)*0.000001;
	square_duty=0.5f-p_duty*0.5f;
	square_slide=-p_duty_ramp*0.00005f;
	if(p_arp_mod>=0.0f)
		arp_mod=1.0-pow((double)p_arp_mod, 2.0)*0.9;
	else
		arp_mod=1.0+pow((double)p_arp_mod, 2.0)*10.0;
	arp_time=0;
	arp_limit=(int)(pow(1.0f-p_arp_speed, 2.0f)*20000+32);
	if(p_arp_speed==1.0f)
		arp_limit=0;
	if(!restart)
	{
		// reset filter
		fltp=0.0f;
		fltdp=0.0f;
		fltw=pow(p_lpf_freq, 3.0f)*0.1f;
		fltw_d=1.0f+p_lpf_ramp*0.0001f;
		fltdmp=5.0f/(1.0f+pow(p_lpf_resonance, 2.0f)*20.0f)*(0.01f+fltw);
		if(fltdmp>0.8f) fltdmp=0.8f;
		fltphp=0.0f;
		flthp=pow(p_hpf_freq, 2.0f)*0.1f;
		flthp_d=1.0f+p_hpf_ramp*0.0003f;
		// reset vibrato
		vib_phase=0.0f;
		vib_speed=pow(p_vib_speed, 2.0f)*0.01f;
		vib_amp=p_vib_strength*0.5f;
		// reset envelope
		env_vol=0.0f;
		env_stage=0;
		env_time=0;
		env_length[0]=(int)(p_env_attack*p_env_attack*100000.0f);
		env_length[1]=(int)(p_env_sustain*p_env_sustain*100000.0f);
		env_length[2]=(int)(p_env_decay*p_env_decay*100000.0f);

		fphase=pow(p_pha_offset, 2.0f)*1020.0f;
		if(p_pha_offset<0.0f) fphase=-fphase;
		fdphase=pow(p_pha_ramp, 2.0f)*1.0f;
		if(p_pha_ramp<0.0f) fdphase=-fdphase;
		iphase=abs((int)fphase);
		ipp=0;
		for(int i=1024-1;i>=0;--i)
			phaser_buffer[i]=0.0f;

		for(int i=0;i<32;i++)
			noise_buffer[i]=frnd(2.0f)-1.0f;

		rep_time = 0;
		rep_limit = (int)(pow(1.0f-p_repeat_speed, 2.0f)*20000+32);
		if(p_repeat_speed==0.0f)
			rep_limit = 0;
	}
}


void sSfxr::SynthSample(int length, float* buffer, FILE* file, char* filebuffer)
{
	for(int i=0;i<length;i++)
	{
		if(!playing_sample)
			break;

		rep_time++;
		if(rep_limit!=0 && rep_time>=rep_limit)
		{
			rep_time=0;
			ResetSample(true);
		}

		// frequency envelopes/arpeggios
		arp_time++;
		if(arp_limit!=0 && arp_time>=arp_limit)
		{
			arp_limit = 0;
			fperiod *= arp_mod;
		}
		fslide += fdslide;
		fperiod *= fslide;
		if(fperiod>fmaxperiod)
		{
			fperiod=fmaxperiod;
			if(p_freq_limit>0.0f)
				playing_sample = false;
		}
		float rfperiod = (float)fperiod;
		if(vib_amp>0.0f)
		{
			vib_phase += vib_speed;
			rfperiod = (float)fperiod*(1.0f+sin(vib_phase)*vib_amp);
		}
		period = (int)rfperiod;
		if(period<8) period = 8;
		square_duty += square_slide;
		if(square_duty<0.0f) square_duty = 0.0f;
		if(square_duty>0.5f) square_duty = 0.5f;		
		// volume envelope
		++env_time;
		if(env_time>env_length[env_stage])
		{
			env_time = 0;
			++env_stage;
			if(env_stage==3)
				playing_sample = false;
		}
		if(env_stage==0)
			env_vol = (float)env_time/env_length[0];
		if(env_stage==1)
			env_vol = 1.0f+pow(1.0f-(float)env_time/env_length[1], 1.0f)*2.0f*p_env_punch;
		if(env_stage==2)
			env_vol = 1.0f-(float)env_time/env_length[2];

		// phaser step
		fphase += fdphase;
		iphase = abs((int)fphase);
		if(iphase>1023) iphase=1023;

		if(flthp_d!=0.0f)
		{
			flthp *= flthp_d;
			if(flthp<0.00001f) flthp = 0.00001f;
			if(flthp>0.1f) flthp = 0.1f;
		}

		float ssample=0.0f;
		for(int si=0;si<8;si++) // 8x supersampling
		{
			float sample=0.0f;
			phase++;
			if(phase>=period)
			{
				//phase=0;
				phase%=period;
				if(wave_type==3)
					for(int i=0;i<32;i++)
						noise_buffer[i]=frnd(2.0f)-1.0f;
			}
			// base waveform
			float fp=(float)phase/period;
			switch(wave_type)
			{
			case 0: // square
				if(fp<square_duty)
					sample=0.5f;
				else
					sample=-0.5f;
				break;
			case 1: // sawtooth
				sample=1.0f-fp*2;
				break;
			case 2: // sine
				sample=(float)sin(fp*2*3.14159265f);
				break;
			case 3: // noise
				sample=noise_buffer[phase*32/period];
				break;
			}
			// lp filter
			float pp=fltp;
			fltw*=fltw_d;
			if(fltw<0.0f) fltw=0.0f;
			if(fltw>0.1f) fltw=0.1f;
			if(p_lpf_freq!=1.0f)
			{
				fltdp+=(sample-fltp)*fltw;
				fltdp-=fltdp*fltdmp;
			}
			else
			{
				fltp=sample;
				fltdp=0.0f;
			}
			fltp+=fltdp;
			// hp filter
			fltphp+=fltp-pp;
			fltphp-=fltphp*flthp;
			sample=fltphp;
			// phaser
			phaser_buffer[ipp&1023]=sample;
			sample+=phaser_buffer[(ipp-iphase+1024)&1023];
			ipp=(ipp+1)&1023;
			// final accumulation and envelope application
			ssample+=sample*env_vol;
		}
		ssample=ssample/8*master_vol;

		ssample*=2.0f*sound_vol;

		if(buffer!=NULL) // --------------------------------------------
		{
			if(ssample>1.0f) ssample=1.0f;
			if(ssample<-1.0f) ssample=-1.0f;
			*buffer++=ssample;
		}
		if(file!=NULL) // --------------------------------------------
		{
			// quantize depending on format
			// accumulate/count to accomodate variable sample rate?
			ssample*=4.0f; // arbitrary gain to get reasonable output volume...
			if(ssample>1.0f) ssample=1.0f;
			if(ssample<-1.0f) ssample=-1.0f;
			filesample+=ssample;
			fileacc++;
			if(wav_freq==44100 || fileacc==2)
			{
				filesample/=fileacc;
				fileacc=0;
				if(wav_bits==16)
				{
					short isample=(short)(filesample*32000);
					fwrite(&isample, 1, 2, file);
				}
				else
				{
					unsigned char isample=(unsigned char)(filesample*127+128);
					fwrite(&isample, 1, 1, file);
				}
				filesample=0.0f;
			}
			file_sampleswritten++;
		}
		if(filebuffer!=NULL) // --------------------------------------------
		{
			ssample*=4.0f; // arbitrary gain to get reasonable output volume...
			if(ssample>1.0f) ssample = 1.0f; // clamp
			if(ssample<-1.0f) ssample = -1.0f; // clamp
			filesample += ssample;
			fileacc++;
			if(wav_freq==44100 || fileacc==2)
			{
				filesample/=fileacc;
				fileacc=0;
				if(wav_bits==16)
				{
					short isample=(short)(filesample*32000);
					memcpy(filebuffer, &isample, sizeof(isample) );
					filebuffer += sizeof(isample);
				}
				else
				{
					unsigned char isample = (unsigned char)(filesample*127+128);
					memcpy(filebuffer, &isample, sizeof(isample) );
					filebuffer += sizeof(isample);
				}
				filesample=0.0f;
			}
			file_sampleswritten++;
		}
	}
}


// basically a copy of sSfxr::SynthSample to check how big the bytearray must be.
long sSfxr::CalcSampleSize(int length)
{
	long processedSamples = 0;

	for(int i=0;i<length;i++)
	{
		if(!playing_sample)
			break;

		rep_time++;
		if(rep_limit!=0 && rep_time>=rep_limit)
		{
			rep_time=0;
			ResetSample(true);
		}

		// frequency envelopes/arpeggios
		arp_time++;
		if(arp_limit!=0 && arp_time>=arp_limit)
		{
			arp_limit=0;
			fperiod*=arp_mod;
		}
		fslide+=fdslide;
		fperiod*=fslide;
		if(fperiod>fmaxperiod)
		{
			fperiod=fmaxperiod;
			if(p_freq_limit>0.0f)
				playing_sample=false;
		}
		float rfperiod=(float)fperiod;
		if(vib_amp>0.0f)
		{
			vib_phase+=vib_speed;
			rfperiod=(float)fperiod*(1.0f+sin(vib_phase)*vib_amp);
		}
		period=(int)rfperiod;
		if(period<8) period=8;
		square_duty+=square_slide;
		if(square_duty<0.0f) square_duty=0.0f;
		if(square_duty>0.5f) square_duty=0.5f;		
		// volume envelope
		env_time++;
		if(env_time>env_length[env_stage])
		{
			env_time=0;
			env_stage++;
			if(env_stage==3)
				playing_sample=false;
		}
		if(env_stage==0)
			env_vol=(float)env_time/env_length[0];
		if(env_stage==1)
			env_vol=1.0f+pow(1.0f-(float)env_time/env_length[1], 1.0f)*2.0f*p_env_punch;
		if(env_stage==2)
			env_vol=1.0f-(float)env_time/env_length[2];

		// phaser step
		fphase+=fdphase;
		iphase=abs((int)fphase);
		if(iphase>1023) iphase=1023;

		if(flthp_d!=0.0f)
		{
			flthp*=flthp_d;
			if(flthp<0.00001f) flthp=0.00001f;
			if(flthp>0.1f) flthp=0.1f;
		}

		++processedSamples;

	}
	return processedSamples;
}




audiere::SoundEffect* sSfxr::getSoundEffect(audiere::SoundEffectType type, audiere::File* memoryfile)
{
	long bytearraylength = 0;

	// Lets calculate how big the bytearray must be...
	ResetSample(false);
	playing_sample=true;

	while(playing_sample)
		bytearraylength += CalcSampleSize(256);

	if(!bytearraylength){
			// return NULL; // maybe try to return an empty wave...
		//bytearraylength = 1024;
	}

	if(wav_bits==16)
		bytearraylength = 44 + bytearraylength*2; // header + 2 bytes per sample
	else
		bytearraylength = 44 + bytearraylength; // header + 1 bytes per sample


	char* buffer = new char [bytearraylength];

	//DEBUG (clear buffer)
	//memset(buffer, 0, bytearraylength);

	int pos = 0;

	// write wav header
	unsigned int dword=0;
	unsigned short word=0;
	
	memcpy(&buffer[pos], "RIFF", 4); // RIFF;
	pos += 4;

	// remaining file size (we'll overwrite this value later)
	dword=0;
	memcpy(&buffer[pos], &dword, sizeof(dword));
	pos += sizeof(dword); // +=4

	memcpy(&buffer[pos], "WAVE", 4); // "WAVE"
	pos += 4;

	memcpy(&buffer[pos], "fmt ", 4); // "fmt "
	pos += 4;

	dword=16;	 // chunk size
	memcpy(&buffer[pos], &dword, sizeof(dword));
	pos += sizeof(dword); // +=4

	word=1; // no compression
	memcpy(&buffer[pos], &word, sizeof(word));
	pos += sizeof(word); // +=2

	word=1; // channels (mono)
	memcpy(&buffer[pos], &word, sizeof(word));
	pos += sizeof(word); // +=2

	dword = wav_freq; // sample rate
	memcpy(&buffer[pos], &dword, sizeof(dword));
	pos += sizeof(dword); // +=4

	dword=wav_freq*wav_bits/8; // bytes/sec
	memcpy(&buffer[pos], &dword, sizeof(dword));
	pos += sizeof(dword); // +=4

	word=wav_bits/8; // block align
	memcpy(&buffer[pos], &word, sizeof(word));
	pos += sizeof(word); // +=2

	word=wav_bits; // bits per sample
	memcpy(&buffer[pos], &word, sizeof(word));
	pos += sizeof(word); // +=2

	memcpy(&buffer[pos], "data", 4); // "data"
	pos += 4;

	int datasize = pos;

	dword=0; // chunk size, will be overwritten later
	memcpy(&buffer[pos], &dword, sizeof(dword));
	pos += sizeof(dword); // +=4

	// write sample data
	file_sampleswritten=0;
	filesample=0.0f;
	fileacc=0;

	ResetSample(false);
	playing_sample=true;

	while(playing_sample){
		SynthSample(256, NULL, NULL, &buffer[pos] );
		if(wav_bits==16)
			pos += 256*2;
		else
			pos += 256;

		
	}

	// seek back to header and set size info
	dword = datasize - 4 + file_sampleswritten*wav_bits/8;
	memcpy(&buffer[4], &dword, sizeof(dword));

	// seek back to header and set chunk size (data)
	dword=file_sampleswritten*wav_bits/8;
	memcpy(&buffer[datasize], &dword, sizeof(dword));

	//BEBUG NILTON
	//FILE* foutput=fopen("denneboom.wav", "wb");
	//fwrite(&buffer[0], 1, bytearraylength, foutput); // chunk size (data)
	//fclose(foutput);


	// audiere::File* memoryfile = audiere::CreateMemoryFile(buffer, bytearraylength);
	memoryfile = audiere::CreateMemoryFile(buffer, bytearraylength);
	//audiere::SoundEffect* sound = SA_OpenSoundEffect(adrfile.get(), type, memoryfile);
	audiere::SoundEffect* sound = SA_OpenSoundEffect(memoryfile, NULL, type);

	//DEBUG
/*
	if(!sound){
		SaveWav("kaboom.wav");
		FILE* foutput=fopen("bug3.txt", "wb"); 
		char  path[] = "DEBUG: no sound obtained!"; 
		fwrite(path, 1, strlen(path), foutput);	

		fwrite((char *) &bytearraylength, 1, sizeof(long), foutput);	
		fclose(foutput);
	}
*/
	delete [] buffer;
	
	return sound;
}





bool sSfxr::SaveWav(const char* filename)
{
	FILE* foutput=fopen(filename, "wb");
	if(!foutput)
		return false;
	// write wav header
	//char string[32];
	unsigned int dword=0;
	unsigned short word=0;
	fwrite("RIFF", 4, 1, foutput); // "RIFF"
	dword=0;
	fwrite(&dword, 1, 4, foutput); // remaining file size
	fwrite("WAVE", 4, 1, foutput); // "WAVE"

	fwrite("fmt ", 4, 1, foutput); // "fmt "
	dword=16;
	fwrite(&dword, 1, 4, foutput); // chunk size
	word=1;
	fwrite(&word, 1, 2, foutput); // compression code
	word=1;
	fwrite(&word, 1, 2, foutput); // channels
	dword=wav_freq;
	fwrite(&dword, 1, 4, foutput); // sample rate
	dword=wav_freq*wav_bits/8;
	fwrite(&dword, 1, 4, foutput); // bytes/sec
	word=wav_bits/8;
	fwrite(&word, 1, 2, foutput); // block align
	word=wav_bits;
	fwrite(&word, 1, 2, foutput); // bits per sample

	fwrite("data", 4, 1, foutput); // "data"
	dword=0;
	int foutstream_datasize=ftell(foutput);
	fwrite(&dword, 1, 4, foutput); // chunk size

	// write sample data
	file_sampleswritten=0;
	filesample=0.0f;
	fileacc=0;

	//PlaySample();
	ResetSample(false);
	playing_sample=true;

	while(playing_sample)
		SynthSample(256, NULL, foutput, NULL);

	// seek back to header and write size info
	fseek(foutput, 4, SEEK_SET);
	dword=0;
	dword=foutstream_datasize-4+file_sampleswritten*wav_bits/8;
	fwrite(&dword, 1, 4, foutput); // remaining file size
	fseek(foutput, foutstream_datasize, SEEK_SET);
	dword=file_sampleswritten*wav_bits/8;
	fwrite(&dword, 1, 4, foutput); // chunk size (data)
	fclose(foutput);
	return true;
}







