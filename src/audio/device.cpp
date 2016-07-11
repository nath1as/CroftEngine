#include "device.h"

#include "alext.h"
#include "utils.h"

#include <boost/log/trivial.hpp>

namespace audio
{

Device::~Device()
{
    if(m_context)  // T4Larson <t4larson@gmail.com>: fixed
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_context);
    }

    if(m_device)
    {
        alcCloseDevice(m_device);
    }
}

Device::Device()
{
    BOOST_LOG_TRIVIAL(info) << "Probing OpenAL devices...";

    const char *devlist = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

    if(!devlist)
    {
        BOOST_LOG_TRIVIAL(warning) << BOOST_CURRENT_FUNCTION << ": No audio devices";
        return;
    }

    while(*devlist)
    {
        BOOST_LOG_TRIVIAL(info) << "Probing device " << devlist;
        ALCdevice* dev = alcOpenDevice(devlist);

        if(alcIsExtensionPresent(dev, ALC_EXT_EFX_NAME) == ALC_TRUE)
        {
            BOOST_LOG_TRIVIAL(info) << "Device supports EFX";
            m_device = dev;
            const ALCint paramList[] = {
                ALC_STEREO_SOURCES,  16,
                ALC_FREQUENCY,       44100,
                0 };
            m_context = alcCreateContext(m_device, paramList);
            // fails e.g. with Rapture3D, where EFX is supported
            if(m_context)
            {
                break;
            }
        }
        alcCloseDevice(dev);
        devlist += std::strlen(devlist) + 1;
    }

    if(!m_context)
    {
        BOOST_LOG_TRIVIAL(warning) << BOOST_CURRENT_FUNCTION << ": Failed to create OpenAL context.";
        alcCloseDevice(m_device);
        m_device = nullptr;
        BOOST_THROW_EXCEPTION(std::runtime_error("Failed to create OpenAL device and context"));
    }

    alcMakeContextCurrent(m_context);

    loadALExtFunctions(m_device);

    BOOST_LOG_TRIVIAL(info) << "Using OpenAL device " << alcGetString(m_device, ALC_DEVICE_SPECIFIER);

    alSpeedOfSound(330.0 * 512.0);
    alDopplerVelocity(330.0 * 510.0);
    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    m_underwaterFilter = std::make_shared<FilterHandle>();

    alFilteri(m_underwaterFilter->get(), AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    DEBUG_CHECK_AL_ERROR();
    alFilterf(m_underwaterFilter->get(), AL_LOWPASS_GAIN, 0.7f);      // Low frequencies gain.
    DEBUG_CHECK_AL_ERROR();
    alFilterf(m_underwaterFilter->get(), AL_LOWPASS_GAINHF, 0.0f);    // High frequencies gain.
    DEBUG_CHECK_AL_ERROR();
}
}
