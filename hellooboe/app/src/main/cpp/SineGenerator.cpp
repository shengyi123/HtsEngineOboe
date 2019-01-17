/*
 * Copyright 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <logging_macros.h>
#include "SineGenerator.h"
#include "HTS_engine.h"


constexpr int kDefaultFrameRate = 48000;
constexpr float kDefaultAmplitude = 0.01;
constexpr float kDefaultFrequency = 440.0;
constexpr double kTwoPi = M_PI * 2;
//constexpr int32_t offset = 0;
int32_t offset = 0;

SineGenerator::SineGenerator() {
    setup(kDefaultFrequency, kDefaultFrameRate, kDefaultAmplitude);
}

void SineGenerator::setup(double frequency, int32_t frameRate) {
    mFrameRate = frameRate;
    mPhaseIncrement = getPhaseIncremement(frequency);
}

void SineGenerator::setup(double frequency, int32_t frameRate, float amplitude) {
    setup(frequency, frameRate);
    mAmplitude = amplitude;
}

void SineGenerator::setSweep(double frequencyLow, double frequencyHigh, double seconds) {
    mPhaseIncrementLow = getPhaseIncremement(frequencyLow);
    mPhaseIncrementHigh = getPhaseIncremement(frequencyHigh);

    double numFrames = seconds * mFrameRate;
    mUpScaler = pow((frequencyHigh / frequencyLow), (1.0 / numFrames));
    mDownScaler = 1.0 / mUpScaler;
    mGoingUp = true;
    mSweeping = true;
}

void SineGenerator::render(int16_t *buffer, int32_t channelStride, int32_t numFrames) {
  /*  int sampleIndex = 0;
    for (int i = 0; i < numFrames; i++) {
        buffer[sampleIndex] = static_cast<int16_t>(INT16_MAX * sinf(mPhase) * mAmplitude);
        sampleIndex += channelStride;
        advancePhase();

    }*/
    int sampleIndex = 0;
    int i;
    // extern int16_t *HTS_wavebuffer ;
    // extern int32_t buffersize;
    LOGE("channelStride = %d",channelStride);
    LOGE("offset: %d buffersize:%d",offset,buffersize);
    for (i = 0; (i < numFrames) && ((offset+i) < buffersize); i++) {

        buffer[sampleIndex] = HTS_wavebuffer[ offset+i ];
        LOGE(" sampleIndex : %d", sampleIndex);
        sampleIndex += channelStride;
    }
    for(;i < numFrames;i++){
        buffer[sampleIndex] = 0;
        sampleIndex += channelStride;
    }


    offset += numFrames;

}

void SineGenerator::render(float *buffer, int32_t channelStride, int32_t numFrames) {
   /* for (int i = 0, sampleIndex = 0; i < numFrames; i++) {
        buffer[sampleIndex] = static_cast<float>(sinf(mPhase) * mAmplitude);
        sampleIndex += channelStride;
        advancePhase();
    }*/
    /*  int sampleIndex = 0;
     for (int i = 0; i < numFrames; i++) {
         buffer[sampleIndex] = static_cast<int16_t>(INT16_MAX * sinf(mPhase) * mAmplitude);
         sampleIndex += channelStride;
         advancePhase();

     }*/
    int sampleIndex = 0;
    int i;
    // extern int16_t *HTS_wavebuffer ;
    // extern int32_t buffersize;
    LOGE("channelStride = %d",channelStride);
    LOGE("offset: %d buffersize:%d",offset,buffersize);
    for (i = 0; (i < numFrames) && ((offset+i) < buffersize); i++) {

        buffer[sampleIndex] = ((float) (HTS_wavebuffer[ offset+i ]))/32767.0f ;
        LOGE(" sampleIndex : %d", sampleIndex);
        sampleIndex += channelStride;
    }
    for(;i < numFrames;i++){
        buffer[sampleIndex] = 0;
        sampleIndex += channelStride;
    }


    offset += numFrames;
}

void SineGenerator::advancePhase() {
    mPhase += mPhaseIncrement;
    while (mPhase >= kTwoPi) {
        mPhase -= kTwoPi;
    }
    if (mSweeping) {
        if (mGoingUp) {
            mPhaseIncrement *= mUpScaler;
            if (mPhaseIncrement > mPhaseIncrementHigh) {
                mGoingUp = false;
            }
        } else {
            mPhaseIncrement *= mDownScaler;
            if (mPhaseIncrement < mPhaseIncrementLow) {
                mGoingUp = true;
            }
        }
    }
}

double SineGenerator::getPhaseIncremement(double frequency) {
    return frequency * kTwoPi / mFrameRate;
}