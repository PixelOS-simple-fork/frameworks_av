/*
 * Copyright (C) 2008 The Android Open Source Project
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

//#define LOG_NDEBUG 0
#define LOG_TAG "ToneGenerator"

#include <inttypes.h>
#include <utility>

#include <math.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include "media/ToneGenerator.h"


namespace android {

using android::content::AttributionSourceState;

// Descriptors for all available tones (See ToneGenerator::ToneDescriptor class declaration for details)
const ToneGenerator::ToneDescriptor ToneGenerator::sToneDescriptors[] = {
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1336, 941, 0 }, 0, 0},
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_0
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1209, 697, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_1
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1336, 697, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_2
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1477, 697, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_3
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1209, 770, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_4
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1336, 770, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_5
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1477, 770, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_6
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1209, 852, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_7
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1336, 852, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_8
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1477, 852, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_9
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1209, 941, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_S
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1477, 941, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_P
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1633, 697, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_A
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1633, 770, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                             // TONE_DTMF_B
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1633, 852, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_C
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 1633, 941, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_DTMF_D
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_SUP_DIAL
        { .segments = { { .duration = 500 , .waveFreq = { 425, 0 }, 0, 0},
                        { .duration = 500, .waveFreq = { 0 }, 0, 0},
                           { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_SUP_BUSY
        { .segments = { { .duration = 200, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_SUP_CONGESTION
        { .segments = { { .duration = 200, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 0,
          .repeatSegment = 0 },                              // TONE_SUP_RADIO_ACK
        { .segments = { { .duration = 200, .waveFreq = { 425, 0 }, 0, 0},
                        { .duration = 200, .waveFreq = { 0 }, 0, 0},
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 2,
          .repeatSegment = 0 },                              // TONE_SUP_RADIO_NOTAVAIL
        { .segments = { { .duration = 330, .waveFreq = { 950, 1400, 1800, 0 }, 0, 0},
                        { .duration = 1000, .waveFreq = { 0 }, 0, 0},
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_SUP_ERROR
        { .segments = { { .duration = 200, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 600, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 3000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_SUP_CALL_WAITING
        { .segments = { { .duration = 1000, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 4000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_SUP_RINGTONE
        { .segments = { { .duration = 40, .waveFreq = { 400, 1200, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 0,
          .repeatSegment = 0 },                              // TONE_PROP_BEEP
        { .segments = { { .duration = 100, .waveFreq = { 1200, 0 }, 0, 0 },
                        { .duration = 100, .waveFreq = { 0 }, 0, 0  },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 1,
          .repeatSegment = 0 },                              // TONE_PROP_ACK
        { .segments = { { .duration = 400, .waveFreq = { 300, 400, 500, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 0,
          .repeatSegment = 0 },                              // TONE_PROP_NACK
        { .segments = { { .duration = 200, .waveFreq = { 400, 1200, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 0,
          .repeatSegment = 0 },                              // TONE_PROP_PROMPT
        { .segments = { { .duration = 40, .waveFreq = { 400, 1200, 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 40, .waveFreq = { 400, 1200, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 0,
          .repeatSegment = 0 },                             // TONE_PROP_BEEP2
        { .segments = { { .duration = 250, .waveFreq = { 440, 0 }, 0, 0 },
                        { .duration = 250, .waveFreq = { 620, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_SUP_INTERCEPT
        { .segments = { { .duration = 250, .waveFreq = { 440, 0 }, 0, 0 },
                        { .duration = 250, .waveFreq = { 620, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 7,
          .repeatSegment = 0 },                             // TONE_SUP_INTERCEPT_ABBREV
        { .segments = { { .duration = 250, .waveFreq = { 480, 620, 0 }, 0, 0 },
                        { .duration = 250, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 7,
          .repeatSegment = 0 },                             // TONE_SUP_CONGESTION_ABBREV
        { .segments = { { .duration = 100, .waveFreq = { 350, 440, 0 }, 0, 0 },
                        { .duration = 100, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 2,
          .repeatSegment = 0 },                             // TONE_SUP_CONFIRM
        { .segments = { { .duration = 100, .waveFreq = { 480, 0 }, 0, 0 },
                        { .duration = 100, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 3,
          .repeatSegment = 0 },                              // TONE_SUP_PIP
        { .segments = {{ .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 425, 0 }, 0, 0},
                       { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_CDMA_DIAL_TONE_LITE
        { .segments = { { .duration = 2000, .waveFreq = { 440, 480, 0 }, 0, 0 },
                        { .duration = 4000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_CDMA_NETWORK_USA_RINGBACK
        { .segments = { { .duration = 250, .waveFreq = { 440, 0 }, 0, 0 },
                        { .duration = 250, .waveFreq = { 620, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt =  ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                             // TONE_CDMA_INTERCEPT
        { .segments = { { .duration = 250, .waveFreq = { 440, 0 }, 0, 0 },
                        { .duration = 250, .waveFreq = { 620, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt =  0,
          .repeatSegment = 0 },                             // TONE_CDMA_ABBR_INTERCEPT
        { .segments = { { .duration = 250, .waveFreq = { 480, 620, 0 }, 0, 0 },
                        { .duration = 250, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_CDMA_REORDER
        { .segments = { { .duration = 250, .waveFreq = { 480, 620, 0 }, 0, 0 },
                        { .duration = 250, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 7,
          .repeatSegment = 0 },                              // TONE_CDMA_ABBR_REORDER
        { .segments = { { .duration = 500, .waveFreq = { 480, 620, 0 }, 0, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_CDMA_NETWORK_BUSY
        { .segments = { { .duration = 100, .waveFreq = { 350, 440, 0 }, 0, 0 },
                        { .duration = 100, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 2,
          .repeatSegment = 0 },                              // TONE_CDMA_CONFIRM
        { .segments = { { .duration = 500, .waveFreq = { 660, 1000, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 0,
          .repeatSegment = 0 },                              // TONE_CDMA_ANSWER
        { .segments = { { .duration = 300, .waveFreq = { 440, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 0,
          .repeatSegment = 0 },                              // TONE_CDMA_NETWORK_CALLWAITING
        { .segments = { { .duration = 100, .waveFreq = { 480, 0 }, 0, 0 },
                        { .duration = 100, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 3,
          .repeatSegment = 0 },                              // TONE_CDMA_PIP

        { .segments = { { .duration = 32, .waveFreq = { 2091, 0}, 0, 0 },
                        { .duration = 64, .waveFreq = { 2556, 0}, 19, 0},
                        { .duration = 32, .waveFreq = { 2091, 0}, 0, 0},
                        { .duration = 48, .waveFreq = { 2556, 0}, 0, 0},
                        { .duration = 4000, .waveFreq = { 0 }, 0, 0},
                        { .duration = 0,  .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 0,
          .repeatSegment = 0 },                             // TONE_CDMA_CALL_SIGNAL_ISDN_NORMAL
        { .segments = { { .duration = 32, .waveFreq = { 2091, 0}, 0, 0 },
                        { .duration = 64, .waveFreq = { 2556, 0}, 7, 0 },
                        { .duration = 32, .waveFreq = { 2091, 0}, 0, 0 },
                        { .duration = 400, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 32,  .waveFreq = { 2091, 0}, 0, 0 },
                        { .duration = 64,  .waveFreq = { 2556, 0}, 7, 4 },
                        { .duration = 32,  .waveFreq = { 2091, 0}, 0, 0 },
                        { .duration = 4000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0,    .waveFreq = { 0 }, 0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                              // TONE_CDMA_CALL_SIGNAL_ISDN_INTERGROUP
        { .segments = { { .duration = 32, .waveFreq = { 2091, 0}, 0, 0 },
                        { .duration = 64, .waveFreq = { 2556, 0}, 3, 0 },
                        { .duration = 16, .waveFreq = { 2091, 0}, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 },     0, 0 },
                        { .duration = 32, .waveFreq = { 2091, 0}, 0, 0 },
                        { .duration = 64, .waveFreq = { 2556, 0}, 3, 4 },
                        { .duration = 16, .waveFreq = { 2091, 0}, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 },     0, 0 },
                        { .duration = 0,   .waveFreq = { 0 },     0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                            // TONE_CDMA_CALL_SIGNAL_ISDN_SP_PRI
        { .segments = { { .duration = 0,  .waveFreq = { 0 }, 0, 0} },
          .repeatCnt = 0,
          .repeatSegment = 0 },                            // TONE_CDMA_CALL_SIGNAL_ISDN_PAT3
        { .segments = { { .duration = 32, .waveFreq = { 2091, 0 }, 0, 0 },
                        { .duration = 64, .waveFreq = { 2556, 0 }, 4, 0 },
                        { .duration = 20, .waveFreq = { 2091, 0 }, 0, 0 },
                        { .duration = 0,  .waveFreq = { 0 }      , 0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                             // TONE_CDMA_CALL_SIGNAL_ISDN_PING_RING
        { .segments = { { .duration = 0,  .waveFreq = { 0 }, 0, 0} },
          .repeatCnt = 0,
          .repeatSegment = 0 },                             // TONE_CDMA_CALL_SIGNAL_ISDN_PAT5
        { .segments = { { .duration = 0,  .waveFreq = { 0 }, 0, 0} },
          .repeatCnt = 0,
          .repeatSegment = 0 },                             // TONE_CDMA_CALL_SIGNAL_ISDN_PAT6
        { .segments = { { .duration = 0,  .waveFreq = { 0 }, 0, 0} },
          .repeatCnt = 0,
          .repeatSegment = 0 },                             // TONE_CDMA_CALL_SIGNAL_ISDN_PAT7

        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 39, 0 },
                        { .duration = 4000, .waveFreq = { 0 },     0, 0 },
                        { .duration = 0,    .waveFreq = { 0 },     0, 0 } },
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_L
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 39, 0 },
                        { .duration = 4000, .waveFreq = { 0 },     0, 0 },
                        { .duration = 0,    .waveFreq = { 0 },     0, 0 } },
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_L
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 39, 0 },
                        { .duration = 4000, .waveFreq = { 0 },     0, 0 },
                        { .duration = 0,    .waveFreq = { 0 },     0, 0 } },
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_L
        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 15, 0 },
                        { .duration = 400, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 } },
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_SS
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 15, 0 },
                        { .duration = 400, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_SS
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 15, 0 },
                        { .duration = 400, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_SS
        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 15, 6 },
                        { .duration = 4000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_SSL
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 15, 6 },
                        { .duration = 4000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_SSL
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 15, 6 },
                        { .duration = 4000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_SSL
        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                          { .duration = 25, .waveFreq = { 4000, 0 }, 19, 0 },
                        { .duration = 1000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 19, 3 },
                        { .duration = 3000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_SS_2
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 19, 0 },
                        { .duration = 1000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 19, 3 },
                        { .duration = 3000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_SS_2
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 19, 0 },
                        { .duration = 1000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 19, 3 },
                        { .duration = 3000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_SS_2
        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 9, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 19, 3 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 9, 6 },
                        { .duration = 3000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_SLS
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 9, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 19, 3 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 9, 6 },
                        { .duration = 3000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_SLS
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 9, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 19, 3 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 9, 6 },
                        { .duration = 3000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_SLS
        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 9, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 9, 3 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 9, 6 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 9, 9 },
                        { .duration = 2500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_S_X4
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 9, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 9, 3 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 9, 6 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 9, 9 },
                        { .duration = 2500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_S_X4
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 9, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 9, 3 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 9, 6 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 9, 9 },
                        { .duration = 2500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_S_X4
        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 19, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_PBX_L
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 19, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_PBX_L
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 19, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_PBX_L
        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 3 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_PBX_SS
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 3 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_PBX_SS
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 3 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_PBX_SS
        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 15, 6 },
                        { .duration = 1000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_PBX_SSL
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 15, 6 },
                        { .duration = 1000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_PBX_SSL
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 15, 6 },
                        { .duration = 1000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_PBX_SSL
        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 15, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 6 },
                        { .duration = 1000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_PBX_SLS
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 15, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 6 },
                        { .duration = 1000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_PBX_SLS
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 15, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 6 },
                        { .duration = 1000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
           .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_PBX_SLS
        { .segments = { { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 6 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 3700, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 4000, 0 }, 7, 9 },
                        { .duration = 800, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_HIGH_PBX_S_X4
        { .segments = { { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 6 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2600, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 2900, 0 }, 7, 9 },
                        { .duration = 800, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_MED_PBX_S_X4
        { .segments = { { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 3 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 6 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1300, 0 }, 0, 0 },
                        { .duration = 25, .waveFreq = { 1450, 0 }, 7, 9 },
                        { .duration = 800, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                           // TONE_CDMA_LOW_PBX_S_X4

        { .segments = { { .duration = 62, .waveFreq = { 1109, 0 }, 0, 0 },
                        { .duration = 62, .waveFreq = { 784, 0 },  0, 0 },
                        { .duration = 62, .waveFreq = { 740, 0 },  0, 0 },
                        { .duration = 62, .waveFreq = { 622, 0 },  0, 0 },
                        { .duration = 62, .waveFreq = { 1109, 0 }, 0, 0 },
                        { .duration = 0,  .waveFreq = { 0 },       0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                            // TONE_CDMA_ALERT_NETWORK_LITE
        { .segments = { { .duration = 62, .waveFreq = { 1245, 0 }, 0, 0 },
                        { .duration = 62, .waveFreq = { 659, 0 },  2, 0 },
                        { .duration = 62, .waveFreq = { 1245, 0 }, 0, 0 },
                        { .duration = 0,  .waveFreq = { 0 },       0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                            // TONE_CDMA_ALERT_AUTOREDIAL_LITE
        { .segments = { { .duration = 400, .waveFreq = { 1150, 770, 0 }, 0, 0 },
                        { .duration = 0,   .waveFreq = { 0 },            0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                            // TONE_CDMA_ONE_MIN_BEEP
        { .segments = { { .duration = 120, .waveFreq = { 941, 1477, 0 }, 0, 0 },
                        { .duration = 0,   .waveFreq = { 0 },            0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                            // TONE_CDMA_KEYPAD_VOLUME_KEY_LITE
        { .segments = { { .duration = 375, .waveFreq = { 587, 0 }, 0, 0 },
                        { .duration = 125, .waveFreq = { 1175, 0 }, 0, 0 },
                        { .duration = 0,   .waveFreq = { 0 },       0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                            // TONE_CDMA_PRESSHOLDKEY_LITE
        { .segments = { { .duration = 62, .waveFreq = { 587, 0 }, 0, 0 },
                        { .duration = 62, .waveFreq = { 784, 0 }, 0, 0 },
                        { .duration = 62, .waveFreq = { 831, 0 }, 0, 0 },
                        { .duration = 62, .waveFreq = { 784, 0 }, 0, 0 },
                        { .duration = 62, .waveFreq = { 1109, 0 }, 0, 0 },
                        { .duration = 62, .waveFreq = { 784, 0 }, 0, 0 },
                        { .duration = 62, .waveFreq = { 831, 0 }, 0, 0 },
                        { .duration = 62, .waveFreq = { 784, 0 }, 0, 0 },
                        { .duration = 0,  .waveFreq = { 0 },      0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                             // TONE_CDMA_ALERT_INCALL_LITE
        { .segments = { { .duration = 125, .waveFreq = { 941, 0 }, 0, 0 },
                        { .duration = 10,  .waveFreq = { 0 },      2, 0 },
                        { .duration = 4990, .waveFreq = { 0 },     0, 0 },
                        { .duration = 0,    .waveFreq = { 0 },     0, 0 } },
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                            // TONE_CDMA_EMERGENCY_RINGBACK
        { .segments = { { .duration = 125, .waveFreq = { 1319, 0 }, 0, 0 },
                        { .duration = 125, .waveFreq = { 0 },       0, 0 },
                        { .duration = 0,   .waveFreq = { 0 },       0, 0 } },
          .repeatCnt = 2,
          .repeatSegment = 0 },                            // TONE_CDMA_ALERT_CALL_GUARD
        { .segments = { { .duration = 125, .waveFreq = { 1047, 0 }, 0, 0 },
                        { .duration = 125, .waveFreq = { 370,  0 }, 0, 0 },
                        { .duration = 0,   .waveFreq = { 0 },       0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                            // TONE_CDMA_SOFT_ERROR_LITE
        { .segments = { { .duration = 125, .waveFreq = { 1480, 0 }, 0, 0 },
                        { .duration = 125, .waveFreq = { 1397, 0 }, 0, 0 },
                        { .duration = 125, .waveFreq = { 784, 0 },  0, 0 },
                        { .duration = 0,   .waveFreq = { 0 },       0, 0 } },
          .repeatCnt = 0,
          .repeatSegment = 0 },                            // TONE_CDMA_CALLDROP_LITE

        { .segments = { { .duration = 500, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = 0,
          .repeatSegment = 0 },                           // TONE_CDMA_NETWORK_BUSY_ONE_SHOT
        { .segments = { { .duration = 400, .waveFreq = { 1150, 770 }, 0, 0 },
                        { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = 0,
          .repeatSegment = 0 },                           // TONE_CDMA_ABBR_ALERT
          { .segments = { { .duration = 0, .waveFreq = { 0 }, 0, 0 }},
          .repeatCnt = 0,
          .repeatSegment = 0 },                            // TONE_CDMA_SIGNAL_OFF

        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 350, 440, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_ANSI_DIAL
        { .segments = { { .duration = 500, .waveFreq = { 480, 620, 0 }, 0, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_ANSI_BUSY
        { .segments = { { .duration = 250, .waveFreq = { 480, 620, 0 }, 0, 0 },
                        { .duration = 250, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_ANSI_CONGESTION
        { .segments = { { .duration = 300, .waveFreq = { 440, 0 }, 0, 0 },
                        { .duration = 9700, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 100, .waveFreq = { 440, 0 }, 0, 0 },
                        { .duration = 100, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 100, .waveFreq = { 440, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 1 },                              // TONE_ANSI_CALL_WAITING
        { .segments = { { .duration = 2000, .waveFreq = { 440, 480, 0 }, 0, 0 },
                        { .duration = 4000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_ANSI_RINGTONE
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_JAPAN_DIAL
        { .segments = { { .duration = 500, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_JAPAN_BUSY
        { .segments = { { .duration = 1000, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_JAPAN_RADIO_ACK
        { .segments = { { .duration = 1000, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_JAPAN_RINGTONE
        { .segments = { { .duration = 375, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 375, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_GB_BUSY
        { .segments = { { .duration = 400, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 350, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 225, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 525, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_GB_CONGESTION
        { .segments = { { .duration = 400, .waveFreq = { 400, 450, 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 400, .waveFreq = { 400, 450, 0 }, 0, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0},
                        { .duration = 0, .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_GB_RINGTONE
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 400, 425, 450, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_AUSTRALIA_DIAL
        { .segments = { { .duration = 400, .waveFreq = { 400, 450, 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 400, .waveFreq = { 400, 450, 0 }, 0, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0},
                        { .duration = 0, .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_AUSTRALIA_RINGTONE
        { .segments = { { .duration = 375, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 375, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_AUSTRALIA_BUSY
        { .segments = { { .duration = 200, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 4400, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_AUSTRALIA_CALL_WAITING
        { .segments = { { .duration = 375, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 375, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_AUSTRALIA_CONGESTION
        { .segments = { { .duration = 750, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 750, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_SG_BUSY
        { .segments = { { .duration = 400, .waveFreq = { 401, 425, 449, 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 400, .waveFreq = { 401, 425, 449, 0 }, 0, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_SG_RINGTONE
        { .segments = { { .duration = 500, .waveFreq = { 480, 620, 0 }, 0, 0 },
                        { .duration = 500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_HK_BUSY
        { .segments = { { .duration = 400, .waveFreq = { 440, 480, 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 400, .waveFreq = { 440, 480, 0 }, 0, 0 },
                        { .duration = 3000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_HK_RINGTONE
        { .segments = { { .duration = 400, .waveFreq = { 400, 450, 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 400, .waveFreq = { 400, 450, 0 }, 0, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0},
                        { .duration = 0, .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_IE_RINGTONE
        { .segments = { { .duration = 180, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 4500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_IE_CALL_WAITING
        { .segments = { { .duration = ToneGenerator::TONEGEN_INF, .waveFreq = { 375, 400, 425, 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_INDIA_DIAL
        { .segments = { { .duration = 750, .waveFreq = { 400, 0 }, 0, 0 },
                      { .duration = 750, .waveFreq = { 0 }, 0, 0 },
                      { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                               // TONE_INDIA_BUSY
        { .segments = { { .duration = 250, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 250, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_INDIA_CONGESTION
        { .segments = { { .duration = 200, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 100, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 7500, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                              // TONE_INDIA_CALL_WAITING
        { .segments = { { .duration = 400, .waveFreq = { 375, 400, 425, 0 }, 0, 0 },
                      { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                      { .duration = 400, .waveFreq = { 375, 400, 425, 0 }, 0, 0 },
                      { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                      { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                               // TONE_INDIA_RINGTONE
        { .segments = { { .duration = 1000, .waveFreq = { 440, 480, 0 }, 0, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                               // TONE_TW_RINGTONE
        { .segments = { { .duration = 200, .waveFreq = { 400, 0 }, 0, 0 },
                        { .duration = 3000, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = 3,
          .repeatSegment = 0 },                              // TONE_NZ_CALL_WAITING
        { .segments = { { .duration = 500, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 250, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 0 , .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 },                             // TONE_MY_CONGESTION
        { .segments = { { .duration = 400, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 200, .waveFreq = { 0 }, 0, 0 },
                        { .duration = 400, .waveFreq = { 425, 0 }, 0, 0 },
                        { .duration = 2000, .waveFreq = { 0 }, 0, 0},
                        { .duration = 0, .waveFreq = { 0 }, 0, 0}},
          .repeatCnt = ToneGenerator::TONEGEN_INF,
          .repeatSegment = 0 }                              // TONE_MY_RINGTONE
};

// Used by ToneGenerator::getToneForRegion() to convert user specified supervisory tone type
// to actual tone for current region.
const unsigned char /*tone_type*/ ToneGenerator::sToneMappingTable[NUM_REGIONS-1][NUM_SUP_TONES] = {
        {   // ANSI
            TONE_ANSI_DIAL,             // TONE_SUP_DIAL
            TONE_ANSI_BUSY,             // TONE_SUP_BUSY
            TONE_ANSI_CONGESTION,       // TONE_SUP_CONGESTION
            TONE_SUP_RADIO_ACK,         // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,    // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,             // TONE_SUP_ERROR
            TONE_ANSI_CALL_WAITING,     // TONE_SUP_CALL_WAITING
            TONE_ANSI_RINGTONE          // TONE_SUP_RINGTONE
        },
        {   // JAPAN
            TONE_JAPAN_DIAL,             // TONE_SUP_DIAL
            TONE_JAPAN_BUSY,             // TONE_SUP_BUSY
            TONE_SUP_CONGESTION,         // TONE_SUP_CONGESTION
            TONE_JAPAN_RADIO_ACK,        // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,     // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,              // TONE_SUP_ERROR
            TONE_SUP_CALL_WAITING,       // TONE_SUP_CALL_WAITING
            TONE_JAPAN_RINGTONE          // TONE_SUP_RINGTONE
        },
        {   // GB
            TONE_ANSI_DIAL,              // TONE_SUP_DIAL
            TONE_GB_BUSY,                // TONE_SUP_BUSY
            TONE_GB_CONGESTION,          // TONE_SUP_CONGESTION
            TONE_SUP_RADIO_ACK,          // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,     // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,              // TONE_SUP_ERROR
            TONE_SUP_CALL_WAITING,       // TONE_SUP_CALL_WAITING
            TONE_GB_RINGTONE             // TONE_SUP_RINGTONE
        },
        {   // AUSTRALIA
            TONE_AUSTRALIA_DIAL,        // TONE_SUP_DIAL
            TONE_AUSTRALIA_BUSY,        // TONE_SUP_BUSY
            TONE_AUSTRALIA_CONGESTION,  // TONE_SUP_CONGESTION
            TONE_SUP_RADIO_ACK,         // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,    // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,             // TONE_SUP_ERROR
            TONE_AUSTRALIA_CALL_WAITING,// TONE_SUP_CALL_WAITING
            TONE_AUSTRALIA_RINGTONE     // TONE_SUP_RINGTONE
        },
        {   // SINGAPORE
            TONE_SUP_DIAL,               // TONE_SUP_DIAL
            TONE_SG_BUSY,                // TONE_SUP_BUSY
            TONE_SUP_CONGESTION,         // TONE_SUP_CONGESTION
            TONE_SUP_RADIO_ACK,          // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,     // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,              // TONE_SUP_ERROR
            TONE_SUP_CALL_WAITING,       // TONE_SUP_CALL_WAITING
            TONE_SG_RINGTONE             // TONE_SUP_RINGTONE
        },
        {   // HONGKONG
            TONE_SUP_DIAL,               // TONE_SUP_DIAL
            TONE_HK_BUSY,                // TONE_SUP_BUSY
            TONE_SUP_CONGESTION,         // TONE_SUP_CONGESTION
            TONE_SUP_RADIO_ACK,          // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,     // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,              // TONE_SUP_ERROR
            TONE_SUP_CALL_WAITING,       // TONE_SUP_CALL_WAITING
            TONE_HK_RINGTONE             // TONE_SUP_RINGTONE
        },
        {   // IRELAND
            TONE_SUP_DIAL,               // TONE_SUP_DIAL
            TONE_SUP_BUSY,               // TONE_SUP_BUSY
            TONE_SUP_CONGESTION,         // TONE_SUP_CONGESTION
            TONE_SUP_RADIO_ACK,          // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,     // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,              // TONE_SUP_ERROR
            TONE_IE_CALL_WAITING,        // TONE_SUP_CALL_WAITING
            TONE_IE_RINGTONE             // TONE_SUP_RINGTONE
        },
        {   // INDIA
            TONE_INDIA_DIAL,             // TONE_SUP_DIAL
            TONE_INDIA_BUSY,             // TONE_SUP_BUSY
            TONE_INDIA_CONGESTION,       // TONE_SUP_CONGESTION
            TONE_SUP_RADIO_ACK,          // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,     // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,              // TONE_SUP_ERROR
            TONE_INDIA_CALL_WAITING,     // TONE_SUP_CALL_WAITING
            TONE_INDIA_RINGTONE          // TONE_SUP_RINGTONE
        },
        {   // TAIWAN
            TONE_SUP_DIAL,               // TONE_SUP_DIAL
            TONE_SUP_BUSY,               // TONE_SUP_BUSY
            TONE_SUP_CONGESTION,         // TONE_SUP_CONGESTION
            TONE_SUP_RADIO_ACK,          // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,     // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,              // TONE_SUP_ERROR
            TONE_SUP_CALL_WAITING,       // TONE_SUP_CALL_WAITING
            TONE_TW_RINGTONE             // TONE_SUP_RINGTONE
        },
        {   // NEW ZEALAND
            TONE_JAPAN_DIAL,              // TONE_SUP_DIAL
            TONE_JAPAN_BUSY,              // TONE_SUP_BUSY
            TONE_SUP_CONGESTION,          // TONE_SUP_CONGESTION
            TONE_SUP_RADIO_ACK,           // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,      // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,               // TONE_SUP_ERROR
            TONE_NZ_CALL_WAITING,         // TONE_SUP_CALL_WAITING
            TONE_GB_RINGTONE              // TONE_SUP_RINGTONE
        },
        {   // MALAYSIA
            TONE_SUP_DIAL,                // TONE_SUP_DIAL
            TONE_SUP_BUSY,                // TONE_SUP_BUSY
            TONE_MY_CONGESTION,           // TONE_SUP_CONGESTION
            TONE_SUP_RADIO_ACK,           // TONE_SUP_RADIO_ACK
            TONE_SUP_RADIO_NOTAVAIL,      // TONE_SUP_RADIO_NOTAVAIL
            TONE_SUP_ERROR,               // TONE_SUP_ERROR
            TONE_SUP_CALL_WAITING,        // TONE_SUP_CALL_WAITING
            TONE_MY_RINGTONE              // TONE_SUP_RINGTONE
        }
};


////////////////////////////////////////////////////////////////////////////////
//                           ToneGenerator class Implementation
////////////////////////////////////////////////////////////////////////////////


//---------------------------------- public methods ----------------------------


////////////////////////////////////////////////////////////////////////////////
//
//    Method:        ToneGenerator::ToneGenerator()
//
//    Description:    Constructor. Initializes the tone sequencer, intantiates required sine wave
//        generators, does not initialize output audio track.
//
//    Input:
//        streamType:        Type of stream used for tone playback
//        volume:            volume applied to tone (0.0 to 1.0)
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////
ToneGenerator::ToneGenerator(audio_stream_type_t streamType, float volume, bool threadCanCallJava,
        std::string opPackageName)
        : mOpPackageName(std::move(opPackageName)) {

    ALOGV("ToneGenerator constructor: streamType=%d, volume=%f", streamType, volume);

    mState = TONE_IDLE;

    if (AudioSystem::getOutputSamplingRate(&mSamplingRate, streamType) != NO_ERROR) {
        ALOGE("Unable to marshal AudioFlinger");
        return;
    }
    mThreadCanCallJava = threadCanCallJava;
    mStreamType = streamType;
    mVolume = volume;
    mpToneDesc = NULL;
    mpNewToneDesc = NULL;
    // Generate tone by chunks of 20 ms to keep cadencing precision
    mProcessSize = (mSamplingRate * 20) / 1000;

    char value[PROPERTY_VALUE_MAX];
    if (property_get("gsm.operator.iso-country", value, "") == 0) {
        property_get("gsm.sim.operator.iso-country", value, "");
    }
    // If dual sim device has two SIM cards inserted and is not registered to any network,
    // "," is set to "gsm.operator.iso-country" prop.
    // In this case, "gsm.sim.operator.iso-country" prop should be used.
    if (strlen(value) == 1 && strstr(value, ",") != NULL) {
        property_get("gsm.sim.operator.iso-country", value, "");
    }

    if (strstr(value, "us") != NULL ||
        strstr(value, "ca") != NULL) {
        mRegion = ANSI;
    } else if (strstr(value, "jp") != NULL) {
        mRegion = JAPAN;
    } else if (strstr(value, "gb") != NULL) {
        mRegion = GB;
    } else if (strstr(value, "au") != NULL) {
        mRegion = AUSTRALIA;
    } else if (strstr(value, "sg") != NULL) {
        mRegion = SINGAPORE;
    } else if (strstr(value, "hk") != NULL) {
        mRegion = HONGKONG;
    } else if (strstr(value, "ie") != NULL) {
        mRegion = IRELAND;
    } else if (strstr(value, "in") != NULL) {
        mRegion = INDIA;
    } else if (strstr(value, "tw") != NULL) {
        mRegion = TAIWAN;
    } else if (strstr(value, "nz") != NULL) {
        mRegion = NZ;
    } else if (strstr(value, "my") != NULL) {
        mRegion = MY;
    } else {
        mRegion = CEPT;
    }

}

////////////////////////////////////////////////////////////////////////////////
//
//    Method:        ToneGenerator::onFirstRef()
//
//    Description:  Called upon first RefBase reference. Initializes audio track
//                  with weak pointer to self as the registered callback.
//    Input:
//        none
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////

void ToneGenerator::onFirstRef() {
    if (initAudioTrack()) {
        ALOGV("ToneGenerator INIT OK, time: %d", (unsigned int)(systemTime()/1000000));
    } else {
        ALOGV("!!!ToneGenerator INIT FAILED!!!");
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//    Method:        ToneGenerator::~ToneGenerator()
//
//    Description:    Destructor. Stop sound playback and delete audio track if
//      needed and delete sine wave generators.
//
//    Input:
//        none
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////
ToneGenerator::~ToneGenerator() {
    ALOGV("ToneGenerator destructor");

    if (mpAudioTrack != 0) {
        stopTone();
        ALOGV("Delete Track: %p", mpAudioTrack.get());
        mpAudioTrack.clear();
    }
    clearWaveGens();
}

////////////////////////////////////////////////////////////////////////////////
//
//    Method:        ToneGenerator::startTone()
//
//    Description:    Starts tone playback.
//
//    Input:
//        toneType:        Type of tone generated (values in enum tone_type)
//        durationMs:      The tone duration in milliseconds. If the tone is limited in time by definition,
//              the actual duration will be the minimum of durationMs and the defined tone duration.
//              Ommiting or setting durationMs to -1 does not limit tone duration.
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////
bool ToneGenerator::startTone(tone_type toneType, int durationMs) {
    bool lResult = false;
    status_t lStatus;

    if (toneType >= NUM_TONES)
        return lResult;

    toneType = getToneForRegion(toneType);
    if (toneType == TONE_CDMA_SIGNAL_OFF) {
        return true;
    }

    if (mState == TONE_IDLE) {
        ALOGV("startTone: try to re-init AudioTrack");
        if (!initAudioTrack()) {
            return lResult;
        }
    }

    ALOGV("startTone toneType %d", toneType);

    mLock.lock();

    // Get descriptor for requested tone
    mpNewToneDesc = &sToneDescriptors[toneType];

    mDurationMs = durationMs;

    if (mState == TONE_STOPPED) {
        ALOGV("Start waiting for previous tone to stop");
        lStatus = mWaitCbkCond.waitRelative(mLock, seconds(3));
        if (lStatus != NO_ERROR) {
            ALOGE("--- start wait for stop timed out, status %d", lStatus);
            mState = TONE_IDLE;
            mLock.unlock();
            return lResult;
        }
    }

    if (mState == TONE_INIT) {
        if (prepareWave()) {
            ALOGV("Immediate start, time %d", (unsigned int)(systemTime()/1000000));
            lResult = true;
            mState = TONE_STARTING;
            if (clock_gettime(CLOCK_MONOTONIC, &mStartTime) != 0) {
                mStartTime.tv_sec = 0;
            }
            mLock.unlock();
            mpAudioTrack->start();
            mLock.lock();
            if (mState == TONE_STARTING) {
                ALOGV("Wait for start callback");
                lStatus = mWaitCbkCond.waitRelative(mLock, seconds(3));
                if (lStatus != NO_ERROR) {
                    ALOGE("--- Immediate start timed out, status %d", lStatus);
                    mState = TONE_IDLE;
                    lResult = false;
                }
            }
        } else {
            mState = TONE_IDLE;
        }
    } else {
        ALOGV("Delayed start");
        mState = TONE_RESTARTING;
        mStartTime.tv_sec = 0;
        lStatus = mWaitCbkCond.waitRelative(mLock, seconds(3));
        if (lStatus == NO_ERROR) {
            if (mState != TONE_IDLE) {
                lResult = true;
            }
            ALOGV("cond received");
        } else {
            ALOGE("--- Delayed start timed out, status %d", lStatus);
            mState = TONE_IDLE;
        }
    }
    mLock.unlock();

    ALOGV_IF(lResult, "Tone started, time %d", (unsigned int)(systemTime()/1000000));
    ALOGW_IF(!lResult, "Tone start failed!!!, time %d", (unsigned int)(systemTime()/1000000));

    return lResult;
}

////////////////////////////////////////////////////////////////////////////////
//
//    Method:        ToneGenerator::stopTone()
//
//    Description:    Stops tone playback.
//
//    Input:
//        none
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////
void ToneGenerator::stopTone() {
    ALOGV("stopTone");

    mLock.lock();
    if (mState != TONE_IDLE && mState != TONE_INIT) {
        if (mState == TONE_PLAYING || mState == TONE_STARTING || mState == TONE_RESTARTING) {
            struct timespec stopTime;
            // If the start time is valid, make sure that the number of audio samples produced
            // corresponds at least to the time between the start and stop commands.
            // This is needed in case of cold start of the output stream.
            if ((mStartTime.tv_sec != 0) && (clock_gettime(CLOCK_MONOTONIC, &stopTime) == 0)) {
                time_t sec = stopTime.tv_sec - mStartTime.tv_sec;
                auto nsec = stopTime.tv_nsec - mStartTime.tv_nsec;
                if (nsec < 0) {
                    --sec;
                    nsec += 1000000000;
                }

                if ((sec + 1) > ((time_t)(INT_MAX / mSamplingRate))) {
                    mMaxSmp = sec * mSamplingRate;
                } else {
                    // mSamplingRate is always > 1000
                    sec = sec * 1000 + nsec / 1000000; // duration in milliseconds
                    mMaxSmp = (unsigned int)(((int64_t)sec * mSamplingRate) / 1000);
                }
                ALOGV("stopTone() forcing mMaxSmp to %d, total for far %" PRIu64, mMaxSmp,
                      mTotalSmp);
            } else {
                mState = TONE_STOPPING;
            }
        }
        ALOGV("waiting cond");
        status_t lStatus = mWaitCbkCond.waitRelative(mLock, seconds(3));
        if (lStatus == NO_ERROR) {
            // If the tone was restarted exit now before calling clearWaveGens();
            if (mState != TONE_INIT) {
                mLock.unlock();
                return;
            }
            ALOGV("track stop complete, time %d", (unsigned int)(systemTime()/1000000));
        } else {
            ALOGE("--- Stop timed out");
            mState = TONE_IDLE;
            mpAudioTrack->stop();
        }
        clearWaveGens();
    }

    mLock.unlock();
}

//---------------------------------- private methods ---------------------------




////////////////////////////////////////////////////////////////////////////////
//
//    Method:        ToneGenerator::initAudioTrack()
//
//    Description:    Allocates and configures AudioTrack used for PCM output.
//
//    Input:
//        none
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////
bool ToneGenerator::initAudioTrack() {
    // Open audio track in mono, PCM 16bit, default sampling rate.
    // TODO b/182392769: use attribution source util
    AttributionSourceState attributionSource = AttributionSourceState();
    attributionSource.packageName = mOpPackageName;
    attributionSource.token = sp<BBinder>::make();
    mpAudioTrack = new AudioTrack(attributionSource);
    ALOGV("AudioTrack(%p) created", mpAudioTrack.get());

    audio_attributes_t attr;
    audio_stream_type_t streamType = mStreamType;
    if (mStreamType == AUDIO_STREAM_VOICE_CALL) {
        streamType = AUDIO_STREAM_DTMF;
    }
    attr = AudioSystem::streamTypeToAttributes(streamType);
    attr.flags = static_cast<audio_flags_mask_t>(attr.flags | AUDIO_FLAG_LOW_LATENCY);

    const size_t frameCount = mProcessSize;
    status_t status = mpAudioTrack->set(
            AUDIO_STREAM_DEFAULT,
            0,    // sampleRate
            AUDIO_FORMAT_PCM_16_BIT,
            AUDIO_CHANNEL_OUT_MONO,
            frameCount,
            AUDIO_OUTPUT_FLAG_NONE,
            wp<AudioTrack::IAudioTrackCallback>::fromExisting(this),
            0,    // notificationFrames
            0,    // sharedBuffer
            mThreadCanCallJava,
            AUDIO_SESSION_ALLOCATE,
            AudioTrack::TRANSFER_CALLBACK,
            nullptr,
            attributionSource,
            &attr);
    // Set caller name so it can be logged in destructor.
    // MediaMetricsConstants.h: AMEDIAMETRICS_PROP_CALLERNAME_VALUE_TONEGENERATOR
    mpAudioTrack->setCallerName("tonegenerator");
    if (status != NO_ERROR) {
        ALOGE("AudioTrack(%p) set failed with error %d", mpAudioTrack.get(), status);
        mpAudioTrack.clear();
        return false;
    }

    mpAudioTrack->setVolume(mVolume);
    mState = TONE_INIT;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
//    Method:        ToneGenerator::onMoreData()
//
//    Description:    AudioTrack callback implementation. Generates a block of
//        PCM samples
//        and manages tone generator sequencer: tones pulses, tone duration...
//
//    Input:
//        buffer  An buffer object containing a pointer which we will fill with
//                buffer.size() bytes.
//
//    Output:
//        The number of bytes we successfully wrote.
//
////////////////////////////////////////////////////////////////////////////////
size_t ToneGenerator::onMoreData(const AudioTrack::Buffer& buffer) {

    int16_t *lpOut = reinterpret_cast<int16_t*>(buffer.data());
    uint32_t lNumSmp = (buffer.size() / sizeof(int16_t) < UINT32_MAX) ?
            buffer.size() / sizeof(int16_t) : UINT32_MAX;
    if (buffer.size() == 0) return 0;
    // We will write to the entire buffer unless we are stopped, then we return
    // 0 at loop end
    size_t bytesWritten = lNumSmp * sizeof(int16_t);

    // Clear output buffer: WaveGenerator accumulates into lpOut buffer
    memset(lpOut, 0, buffer.size());

    while (lNumSmp) {
        unsigned int lReqSmp = lNumSmp < mProcessSize*2 ? lNumSmp : mProcessSize;
        unsigned int lGenSmp;
        unsigned int lWaveCmd = WaveGenerator::WAVEGEN_CONT;
        bool lSignal = false;

        mLock.lock();


        // Update pcm frame count and end time (current time at the end of this process)
        mTotalSmp += lReqSmp;

        // Update tone gen state machine and select wave gen command
        switch (mState) {
        case TONE_PLAYING:
            lWaveCmd = WaveGenerator::WAVEGEN_CONT;
            break;
        case TONE_STARTING:
            ALOGV("Starting Cbk");

            lWaveCmd = WaveGenerator::WAVEGEN_START;
            break;
        case TONE_STOPPING:
        case TONE_RESTARTING:
            ALOGV("Stop/restart Cbk");

            lWaveCmd = WaveGenerator::WAVEGEN_STOP;
            mNextSegSmp = TONEGEN_INF; // forced to skip state machine management below
            break;
        case TONE_STOPPED:
            ALOGV("Stopped Cbk");
            goto audioCallback_EndLoop;
        default:
            ALOGV("Extra Cbk");
            goto audioCallback_EndLoop;
        }

        // Exit if tone sequence is over
        if (mpToneDesc->segments[mCurSegment].duration == 0 ||
            mTotalSmp > mMaxSmp) {
            if (mState == TONE_PLAYING) {
                mState = TONE_STOPPING;
            }
            if (mpToneDesc->segments[mCurSegment].duration == 0) {
                goto audioCallback_EndLoop;
            }
            // fade out before stopping if maximum duration reached
            lWaveCmd = WaveGenerator::WAVEGEN_STOP;
            mNextSegSmp = TONEGEN_INF; // forced to skip state machine management below
        }

        if (mTotalSmp > mNextSegSmp && mNextSegSmp != TONEGEN_INF) {
            // Time to go to next sequence segment

            ALOGV("End Segment, time: %d", (unsigned int)(systemTime()/1000000));

            lGenSmp = lReqSmp;

            // If segment,  ON -> OFF transition : ramp volume down
            if (mpToneDesc->segments[mCurSegment].waveFreq[0] != 0) {
                lWaveCmd = WaveGenerator::WAVEGEN_STOP;
                unsigned int lFreqIdx = 0;
                uint16_t lFrequency = mpToneDesc->segments[mCurSegment].waveFreq[lFreqIdx];

                while (lFrequency != 0) {
                    WaveGenerator *lpWaveGen = mWaveGens.valueFor(lFrequency);
                    lpWaveGen->getSamples(lpOut, lGenSmp, lWaveCmd);
                    lFrequency = mpToneDesc->segments[mCurSegment].waveFreq[++lFreqIdx];
                }
                ALOGV("ON->OFF, lGenSmp: %d, lReqSmp: %d", lGenSmp, lReqSmp);
            }

            // check if we need to loop and loop for the reqd times
            if (mpToneDesc->segments[mCurSegment].loopCnt) {
                if (mLoopCounter < mpToneDesc->segments[mCurSegment].loopCnt) {
                    ALOGV ("in if loop loopCnt(%d) loopctr(%d), CurSeg(%d)",
                          mpToneDesc->segments[mCurSegment].loopCnt,
                          mLoopCounter,
                          mCurSegment);
                    mCurSegment = mpToneDesc->segments[mCurSegment].loopIndx;
                    ++mLoopCounter;
                } else {
                    // completed loop. go to next segment
                    mLoopCounter = 0;
                    mCurSegment++;
                    ALOGV ("in else loop loopCnt(%d) loopctr(%d), CurSeg(%d)",
                          mpToneDesc->segments[mCurSegment].loopCnt,
                          mLoopCounter,
                          mCurSegment);
                }
            } else {
                mCurSegment++;
                ALOGV ("Goto next seg loopCnt(%d) loopctr(%d), CurSeg(%d)",
                      mpToneDesc->segments[mCurSegment].loopCnt,
                      mLoopCounter,
                      mCurSegment);

            }

            // Handle loop if last segment reached
            if (mpToneDesc->segments[mCurSegment].duration == 0) {
                ALOGV("Last Seg: %d", mCurSegment);

                // Pre increment loop count and restart if total count not reached. Stop sequence otherwise
                if (++mCurCount <= mpToneDesc->repeatCnt) {
                    ALOGV("Repeating Count: %d", mCurCount);

                    mCurSegment = mpToneDesc->repeatSegment;
                    if (mpToneDesc->segments[mpToneDesc->repeatSegment].waveFreq[0] != 0) {
                        lWaveCmd = WaveGenerator::WAVEGEN_START;
                    }

                    ALOGV("New segment %d, Next Time: %lld", mCurSegment,
                            ((long long)(mNextSegSmp)*1000)/mSamplingRate);


                } else {
                    lGenSmp = 0;
                    ALOGV("End repeat, time: %d", (unsigned int)(systemTime()/1000000));
                }
            } else {
                ALOGV("New segment %d, Next Time: %lld", mCurSegment,
                        ((long long)(mNextSegSmp)*1000)/mSamplingRate);

                if (mpToneDesc->segments[mCurSegment].waveFreq[0] != 0) {
                    // If next segment is not silent,  OFF -> ON transition : reset wave generator
                    lWaveCmd = WaveGenerator::WAVEGEN_START;

                    ALOGV("OFF->ON, lGenSmp: %d, lReqSmp: %d", lGenSmp, lReqSmp);
                } else {
                    lGenSmp = 0;
                }
            }

            // Update next segment transition position. No harm to do it also for last segment as
            // mNextSegSmp won't be used any more.
            // Handle 32 bit wraparound gracefully.
            const uint64_t res = static_cast<uint64_t>(mNextSegSmp) +
                    (mpToneDesc->segments[mCurSegment].duration * mSamplingRate) / 1000;
            mNextSegSmp = static_cast<uint32_t>(std::min<uint64_t>(TONEGEN_INF, res));

        } else {
            // Inside a segment keep tone ON or OFF
            if (mpToneDesc->segments[mCurSegment].waveFreq[0] == 0) {
                lGenSmp = 0;  // If odd segment, tone is currently OFF
            } else {
                lGenSmp = lReqSmp;  // If event segment, tone is currently ON
            }
        }

        if (lGenSmp) {
            // If samples must be generated, call all active wave generators and acumulate waves in lpOut
            unsigned int lFreqIdx = 0;
            uint16_t lFrequency = mpToneDesc->segments[mCurSegment].waveFreq[lFreqIdx];

            while (lFrequency != 0) {
                WaveGenerator *lpWaveGen = mWaveGens.valueFor(lFrequency);
                lpWaveGen->getSamples(lpOut, lGenSmp, lWaveCmd);
                lFrequency = mpToneDesc->segments[mCurSegment].waveFreq[++lFreqIdx];
            }
        }

        lNumSmp -= lReqSmp;
        lpOut += lReqSmp;

audioCallback_EndLoop:
        switch (mState) {
        case TONE_RESTARTING:
            ALOGV("Cbk restarting track");
            if (prepareWave()) {
                mState = TONE_STARTING;
                if (clock_gettime(CLOCK_MONOTONIC, &mStartTime) != 0) {
                    mStartTime.tv_sec = 0;
                }
                // must reload mpToneDesc as prepareWave() may change mpToneDesc
            } else {
                ALOGW("Cbk restarting prepareWave() failed");
                mState = TONE_IDLE;
                mpAudioTrack->stop();
                // Force loop exit
                lNumSmp = 0;
            }
            lSignal = true;
            break;
        case TONE_STOPPING:
            ALOGV("Cbk Stopping");
            mState = TONE_STOPPED;
            // Force loop exit
            lNumSmp = 0;
            break;
        case TONE_STOPPED:
            mState = TONE_INIT;
            ALOGV("Cbk Stopped track");
            mpAudioTrack->stop();
            // Force loop exit
            lNumSmp = 0;
            bytesWritten = 0;
            lSignal = true;
            break;
        case TONE_STARTING:
            ALOGV("Cbk starting track");
            mState = TONE_PLAYING;
            lSignal = true;
            break;
        case TONE_PLAYING:
            break;
        default:
            // Force loop exit
            lNumSmp = 0;
            bytesWritten = 0;
            break;
        }

        if (lSignal)
            mWaitCbkCond.broadcast();
        mLock.unlock();
    }
    return bytesWritten;
}


////////////////////////////////////////////////////////////////////////////////
//
//    Method:        ToneGenerator::prepareWave()
//
//    Description:    Prepare wave generators and reset tone sequencer state machine.
//      mpNewToneDesc must have been initialized before calling this function.
//    Input:
//        none
//
//    Output:
//        returned value:   true if wave generators have been created, false otherwise
//
////////////////////////////////////////////////////////////////////////////////
bool ToneGenerator::prepareWave() {
    unsigned int segmentIdx = 0;

    if (mpNewToneDesc == NULL) {
        return false;
    }

    // Remove existing wave generators if any
    clearWaveGens();

    mpToneDesc = mpNewToneDesc;

    if (mDurationMs == -1) {
        mMaxSmp = TONEGEN_INF;
    } else {
        if (mDurationMs > (int)(TONEGEN_INF / mSamplingRate)) {
            mMaxSmp = (mDurationMs / 1000) * mSamplingRate;
        } else {
            mMaxSmp = (mDurationMs * mSamplingRate) / 1000;
        }
        ALOGV("prepareWave, duration limited to %d ms", mDurationMs);
    }

    while (mpToneDesc->segments[segmentIdx].duration) {
        // Get total number of sine waves: needed to adapt sine wave gain.
        unsigned int lNumWaves = numWaves(segmentIdx);
        unsigned int freqIdx = 0;
        unsigned int frequency = mpToneDesc->segments[segmentIdx].waveFreq[freqIdx];
        while (frequency) {
            // Instantiate a wave generator if  ot already done for this frequency
            if (mWaveGens.indexOfKey(frequency) == NAME_NOT_FOUND) {
                ToneGenerator::WaveGenerator *lpWaveGen =
                        new ToneGenerator::WaveGenerator(mSamplingRate,
                                frequency,
                                TONEGEN_GAIN/lNumWaves);
                mWaveGens.add(frequency, lpWaveGen);
            }
            frequency = mpNewToneDesc->segments[segmentIdx].waveFreq[++freqIdx];
        }
        segmentIdx++;
    }

    // Initialize tone sequencer
    mTotalSmp = 0;
    mCurSegment = 0;
    mCurCount = 0;
    mLoopCounter = 0;
    if (mpToneDesc->segments[0].duration == TONEGEN_INF) {
        mNextSegSmp = TONEGEN_INF;
    } else{
        mNextSegSmp = (mpToneDesc->segments[0].duration * mSamplingRate) / 1000;
    }

    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
//    Method:        ToneGenerator::numWaves()
//
//    Description:    Count number of sine waves needed to generate a tone segment (e.g 2 for DTMF).
//
//    Input:
//        segmentIdx        tone segment index
//
//    Output:
//        returned value:    nummber of sine waves
//
////////////////////////////////////////////////////////////////////////////////
unsigned int ToneGenerator::numWaves(unsigned int segmentIdx) {
    unsigned int lCnt = 0;

    if (mpToneDesc->segments[segmentIdx].duration) {
        while (mpToneDesc->segments[segmentIdx].waveFreq[lCnt]) {
            lCnt++;
        }
        lCnt++;
    }

    return lCnt;
}


////////////////////////////////////////////////////////////////////////////////
//
//    Method:        ToneGenerator::clearWaveGens()
//
//    Description:    Removes all wave generators.
//
//    Input:
//        none
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////
void ToneGenerator::clearWaveGens() {
    ALOGV("Clearing mWaveGens:");

    for (size_t lIdx = 0; lIdx < mWaveGens.size(); lIdx++) {
        delete mWaveGens.valueAt(lIdx);
    }
    mWaveGens.clear();
}

////////////////////////////////////////////////////////////////////////////////
//
//    Method:       ToneGenerator::getToneForRegion()
//
//    Description:  Get correct ringtone type according to current region.
//      The corrected ring tone type is the tone descriptor index in sToneDescriptors[].
//
//    Input:
//        none
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////
ToneGenerator::tone_type ToneGenerator::getToneForRegion(tone_type toneType) {
    tone_type regionTone;

    if (mRegion == CEPT || toneType < FIRST_SUP_TONE || toneType > LAST_SUP_TONE) {
        regionTone = toneType;
    } else {
        regionTone = (tone_type) sToneMappingTable[mRegion][toneType - FIRST_SUP_TONE];
    }

    ALOGV("getToneForRegion, tone %d, region %d, regionTone %d", toneType, mRegion, regionTone);

    return regionTone;
}


////////////////////////////////////////////////////////////////////////////////
//                WaveGenerator::WaveGenerator class    Implementation
////////////////////////////////////////////////////////////////////////////////

//---------------------------------- public methods ----------------------------

////////////////////////////////////////////////////////////////////////////////
//
//    Method:        WaveGenerator::WaveGenerator()
//
//    Description:    Constructor.
//
//    Input:
//        samplingRate:    Output sampling rate in Hz
//        frequency:       Frequency of the sine wave to generate in Hz
//        volume:          volume (0.0 to 1.0)
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////
ToneGenerator::WaveGenerator::WaveGenerator(uint32_t samplingRate,
        uint16_t frequency, float volume) {
    double d0;
    double F_div_Fs;  // frequency / samplingRate

    F_div_Fs = frequency / (double)samplingRate;
    d0 = - (float)GEN_AMP * sin(2 * M_PI * F_div_Fs);
    mS2_0 = (int16_t)d0;
    mS1 = 0;
    mS2 = mS2_0;

    mAmplitude_Q15 = (int16_t)(32767. * 32767. * volume / GEN_AMP);
    // take some margin for amplitude fluctuation
    if (mAmplitude_Q15 > 32500)
        mAmplitude_Q15 = 32500;

    d0 = 32768.0 * cos(2 * M_PI * F_div_Fs);  // Q14*2*cos()
    if (d0 > 32767)
        d0 = 32767;
    mA1_Q14 = (int16_t) d0;

    ALOGV("WaveGenerator init, mA1_Q14: %d, mS2_0: %d, mAmplitude_Q15: %d",
            mA1_Q14, mS2_0, mAmplitude_Q15);
}

////////////////////////////////////////////////////////////////////////////////
//
//    Method:        WaveGenerator::~WaveGenerator()
//
//    Description:    Destructor.
//
//    Input:
//        none
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////
ToneGenerator::WaveGenerator::~WaveGenerator() {
}

////////////////////////////////////////////////////////////////////////////////
//
//    Method:        WaveGenerator::getSamples()
//
//    Description:    Generates count samples of a sine wave and accumulates
//        result in outBuffer.
//
//    Input:
//        outBuffer:      Output buffer where to accumulate samples.
//        count:          number of samples to produce.
//        command:        special action requested (see enum gen_command).
//
//    Output:
//        none
//
////////////////////////////////////////////////////////////////////////////////
void ToneGenerator::WaveGenerator::getSamples(int16_t *outBuffer,
        unsigned int count, unsigned int command) {
    long lS1, lS2;
    long lA1, lAmplitude;
    long Sample;  // current sample

    // init local
    if (command == WAVEGEN_START) {
        lS1 = (long)0;
        lS2 = (long)mS2_0;
    } else {
        lS1 = mS1;
        lS2 = mS2;
    }
    lA1 = (long)mA1_Q14;
    lAmplitude = (long)mAmplitude_Q15;

    if (command == WAVEGEN_STOP) {
        lAmplitude <<= 16;
        if (count == 0) {
            return;
        }
        long dec = lAmplitude/count;
        // loop generation
        while (count) {
            count--;
            Sample = ((lA1 * lS1) >> S_Q14) - lS2;
            // shift delay
            lS2 = lS1;
            lS1 = Sample;
            Sample = ((lAmplitude>>16) * Sample) >> S_Q15;
            *(outBuffer++) += (int16_t)Sample;  // put result in buffer
            lAmplitude -= dec;
        }
    } else {
        // loop generation
        while (count) {
            count--;
            Sample = ((lA1 * lS1) >> S_Q14) - lS2;
            // shift delay
            lS2 = lS1;
            lS1 = Sample;
            Sample = (lAmplitude * Sample) >> S_Q15;
            *(outBuffer++) += (int16_t)Sample;  // put result in buffer
        }
    }

    // save status
    mS1 = lS1;
    mS2 = lS2;
}

}  // end namespace android
