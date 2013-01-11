#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include "ssfxr.hpp"
#include "../common/minmax.hpp"

////////////////////////////////////////////////////////////////////////////////

SSFXR::SSFXR()
{
    m_Sfxr.ResetParams();
}

////////////////////////////////////////////////////////////////////////////////

SSFXR::~SSFXR()
{
    Destroy();
}

////////////////////////////////////////////////////////////////////////////////

//bool
//SSFXR::Define(const char* filename, IFileSystem& fs)
//{
//    return Initialize();
//}

////////////////////////////////////////////////////////////////////////////////

void
SSFXR::Destroy()
{
	//delete m_Sfxr;
	//m_Sfxr = NULL;
	m_Sfxr.~sSfxr();
}

////////////////////////////////////////////////////////////////////////////////

SSFXR*
SSFXR::Clone()
{
    SSFXR* sfxr_new = new SSFXR;
    if (sfxr_new)
    {
		sfxr_new->setArpeggio(this->getArpeggio());
		sfxr_new->setArpeggioSpeed(this->getArpeggioSpeed());
		sfxr_new->setAttack(this->getAttack());
		sfxr_new->setBaseFrequency(this->getBaseFrequency());
		sfxr_new->setBitrate(this->getBitrate());
		sfxr_new->setDecay(this->getDecay());
		sfxr_new->setFilter(this->getFilter());
		sfxr_new->setFilterResonance(this->getFilterResonance());
		sfxr_new->setFrequencySlide(this->getFrequencySlide());
		sfxr_new->setFrequencySlideDelta(this->getFrequencySlideDelta());
		sfxr_new->setHighPassFilterCutoffSweep(this->getHighPassFilterCutoffSweep());
		sfxr_new->setHighPassFilterCutoff(this->getHighPassFilterCutoff());
		sfxr_new->setLowPassFilterCutoff(this->getLowPassFilterCutoff());
		sfxr_new->setLowPassFilterCutoffSweep(this->getLowPassFilterCutoffSweep());
		sfxr_new->setMasterVolume((float)this->getMasterVolume());
		sfxr_new->setMinFrequency(this->getMinFrequency());
		sfxr_new->setPhaserOffset(this->getPhaserOffset());
		sfxr_new->setPhaserOffsetSweep(this->getPhaserOffsetSweep());
		sfxr_new->setRelease(this->getRelease());
		sfxr_new->setRepeatSpeed(this->getRepeatSpeed());
		sfxr_new->setSampleRate(this->getSampleRate());
		sfxr_new->setSoundVolume((float)this->getSoundVolume());
		sfxr_new->setSquareDuty(this->getSquareDuty());
		sfxr_new->setSquareDutySweep(this->getSquareDutySweep());
		sfxr_new->setSustain(this->getSustain());
		sfxr_new->setVibratoDelay(this->getVibratoDelay());
		sfxr_new->setVibratoDepth(this->getVibratoDepth());
		sfxr_new->setVibratoSpeed(this->getVibratoSpeed());
		sfxr_new->setWaveType(this->getWaveType());
    }
    return sfxr_new;
}

////////////////////////////////////////////////////////////////////////////////

bool
SSFXR::Save(const char* filename)
{
    return m_Sfxr.SaveWav(filename);
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

bool
SSFXR::Reset()
{
	m_Sfxr.ResetParams();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
