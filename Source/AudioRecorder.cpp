/*
  ==============================================================================

    AudioRecorder.cpp
    Created: 27 Sep 2017 1:30:17pm
    Authors: Michael Castanieto
             Jonas Holfelt
             Gergely Csapo

  ==============================================================================
*/

#include "AudioRecorder.h"
#include "Envelope.h"
#include <numeric>

AudioRecorder::AudioRecorder (float bufferLengthInSeconds)
    : writeIndex (0), activeWriter (false)
{
    this->bufferLengthInSeconds = bufferLengthInSeconds;
    numChannels = 1;
}

AudioRecorder::~AudioRecorder()
{
    stop();
    for (int i = 0; i < numChannels; i++)
    {
        delete recBuff[i];
    }
    delete recBuff;
}

void AudioRecorder::startRecording()
{
    writeIndex = 0;

    if (sampleRate > 0)
    {
        const ScopedLock sl (writerLock);
        activeWriter = true;
    }
}

void AudioRecorder::stop()
{
    // First, clear this pointer to stop the audio callback from using our writer object..
    {
        const ScopedLock sl (writerLock);
        activeWriter = false;
        // clear the part of the buffer that isn't being written to
        for (int ch = 0; ch < numChannels; ch++)
        {
            for (int sample = writeIndex; sample < getBufferLengthInSamples (); sample++)
            {
                recBuff[ch][sample] = 0.0f;
            }
        }
        writeIndex = 0;

        truncate(recBuff, 0.08f);
        sampBuff.setDataToReferTo(recBuff, numChannels, sampStart, sampLength); //set the AudioBuffer pointer to the truncated segment
        specBuff = new float[sampLength];

        int j = 0;
        for (int i = sampStart; i < sampStart+sampLength; ++i)
        {
            specBuff[j] = recBuff[0][i]; // copy the truncated recording to the spectrum buffer
            j++; 
        }

        centroid = spectralCentroid(specBuff);
        std::cout << "centroid: " << centroid << std::endl;
    }
}

bool AudioRecorder::isRecording() const
{
    return activeWriter != false;
}

void AudioRecorder::audioDeviceAboutToStart (AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();

    bufferLengthInSamples = ceil (sampleRate * this->bufferLengthInSeconds); 
    
    // recBuff stores the recorded audio
    recBuff = new float*[numChannels];
    for(int ch = 0; ch < numChannels; ch++)
    {
        recBuff[ch] = new float[bufferLengthInSamples];
    }
}

void AudioRecorder::audioDeviceStopped() 
{
    sampleRate = 0;
}

void AudioRecorder::audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                           float** outputChannelData, int numOutputChannels,
                                           int numSamples) 
{
    if (activeWriter != false && writeIndex < bufferLengthInSamples-numSamples)
    {
        int destStartSample = writeIndex; 
        const ScopedLock sl (writerLock);
        
        // store the recorded audio into recBuff
        for (int ch = 0; ch < numChannels; ch++)
        {
            for (int sample = 0; sample < numSamples; sample++)
            {
                recBuff[ch][sample+destStartSample] = inputChannelData[ch][sample];
            }
        }
        writeIndex += numSamples;
    }
}

int AudioRecorder::getNumChannels()
{
    return numChannels;
}

float** AudioRecorder::getRecBuff() 
{
    return recBuff;
}

AudioBuffer<float> AudioRecorder::getSampBuff() 
{
    return sampBuff;
}

int AudioRecorder::getBufferLengthInSamples()
{
    return bufferLengthInSamples;
}

int AudioRecorder::getWriteIndex()
{
    return writeIndex;
}

void AudioRecorder::truncate (float** recording, float threshold)
{
    int j = this->bufferLengthInSamples; //reversed iterator
    this->sampStart = 0;
    this->sampLength = 0;

    for (int i = 0; i < this->bufferLengthInSamples; i++)
    {
        if(fabs(recording[0][i]) > threshold && this->sampStart == 0)
        {
            this->sampStart = i;
        }
        if(fabs(recording[0][j]) > threshold && this->sampLength == 0)
        {
            this->sampLength = this->bufferLengthInSamples - (sampStart + this->bufferLengthInSamples - j);
        }
        j--;
    }

    int rollOffLength = 5000;
    if(rollOffLength > sampLength)
        rollOffLength = sampLength;

    Envelope::generateRamp(1.0f, 0.001f, rollOffLength, "exp");
    
    j = 0;
    for (int i = sampStart+sampLength-rollOffLength+1; i <= sampStart+sampLength; i++)
    {
        recording[0][i] *= Envelope::ramp[j];
        j++;
    }  
}

float AudioRecorder::spectralCentroid(float* buff)
{
    dsp::FFT f(floor(log2(sampLength)));
    int FFTsize = pow(2, floor(log2(sampLength)));
    
    float* window = new float[FFTsize]; // windowing the truncated segment

    for (int i = 0; i < FFTsize; ++i)
    {
        window[i] = buff[i];
    }

    float binHz = sampleRate/FFTsize; // bins in Hz

    f.performFrequencyOnlyForwardTransform(window);
    
    // spectral centroid calculation
    float sc = 0;
    for (int i = 0; i < FFTsize/2; ++i)
    {
        sc += window[i]*(i*binHz);
    }

    sc /= std::accumulate(window, window+FFTsize, 0);
    
    return sc;
}
