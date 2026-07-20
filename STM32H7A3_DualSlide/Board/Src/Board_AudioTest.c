/*
 * Board_AudioTest.c
 *
 * Continuous dual-buffer audio test for the STM32H7A3 audio subsystem.
 *
 * Generates an eight-second repeating synthesizer sequence with a lead
 * melody, bass line, arpeggio, note envelopes, and rhythmic accents.
 *
 * DMA callbacks only set refill flags. Board_AudioTestUpdate() must be
 * called regularly from the main loop or operating-system task.
 */

#include <stddef.h>
#include <stdint.h>
#include "Board_AudioTest.h"

#define BOARD_AUDIO_FLAG_FILL_FIRST_HALF      (1UL << 0U)
#define BOARD_AUDIO_FLAG_FILL_SECOND_HALF     (1UL << 1U)

#define BOARD_AUDIO_DAC_MIDPOINT               2048L

#define BOARD_AUDIO_TEMPO_BPM                  100UL
#define BOARD_AUDIO_STEPS_PER_BEAT             2UL
#define BOARD_AUDIO_LEAD_GLIDE_PERCENT          20UL


#define AUDIO_LEVEL_GLOBAL                    100L    /* Percent */

#define BOARD_AUDIO_LEAD_LEVEL               (125L * AUDIO_LEVEL_GLOBAL / 100L)
#define BOARD_AUDIO_HARMONIC_LEVEL           (25L  * AUDIO_LEVEL_GLOBAL / 100L)
#define BOARD_AUDIO_BASS_LEVEL               (40L  * AUDIO_LEVEL_GLOBAL / 100L)
#define BOARD_AUDIO_ARPEGGIO_LEVEL           (25L  * AUDIO_LEVEL_GLOBAL / 100L)
#define BOARD_AUDIO_ACCENT_LEVEL             (35L  * AUDIO_LEVEL_GLOBAL / 100L)

#define C2  65UL
#define D2  73UL
#define E2  82UL
#define F2  87UL
#define G2  98UL
#define A2 110UL
#define B2 123UL

#define C3      131UL
#define D3      147UL
#define E3      165UL
#define F3      175UL
#define G3      196UL
#define A3      220UL
#define B3      247UL

#define C4      262UL
#define D4      294UL
#define E4      330UL
#define F4      349UL
#define G4      392UL
#define A4      440UL
#define B4      494UL

#define C5      523UL
#define D5      587UL
#define E5      659UL
#define F5      698UL
#define G5      784UL
#define A5      880UL
#define B5      988UL

#define C6      1047UL
#define D6      1175UL
#define E6      1319UL
#define F6      1397UL
#define G6      1568UL
#define A6      1760UL
#define B6      1976UL

#define REST    0UL

static uint16_t Board_AudioTestBuffer[
    BOARD_AUDIO_TEST_BUFFER_SAMPLES
] __attribute__((aligned(32)));

static Audio_HandleTypeDef *Board_AudioTestHandle = NULL;

static volatile uint32_t Board_AudioTestFlags = 0U;
static volatile uint8_t Board_AudioTestOverrun = 0U;

static uint32_t Board_AudioLeadPhase = 0U;
static uint32_t Board_AudioLeadHarmonicPhase = 0U;
static uint32_t Board_AudioBassPhase = 0U;
static uint32_t Board_AudioArpeggioPhase = 0U;
static uint32_t Board_AudioAccentPhase = 0U;

static uint32_t Board_AudioSequenceStep = 0U;
static uint32_t Board_AudioSamplesIntoStep = 0U;
static uint32_t Board_AudioSamplesPerStep = 1U;

static uint32_t Board_AudioLeadPhaseStepCurrent = 0U;
static uint32_t Board_AudioLeadPhaseStepTarget = 0U;
static uint32_t Board_AudioLeadGlideSamplesRemaining = 0U;
static uint8_t Board_AudioLeadGate = 0U;
static uint32_t Board_AudioBassPhaseStep = 0U;
static uint32_t Board_AudioArpeggioPhaseStep = 0U;
static uint32_t Board_AudioAccentPhaseStep = 0U;

#define BOARD_AUDIO_SEQUENCE_STEPS             128UL

static const uint16_t Board_AudioLeadSequence[
    BOARD_AUDIO_SEQUENCE_STEPS
] =
{
    D5, REST, F5, REST,
    A5, REST, F5, REST,

    E5, REST, D5, REST,
    B4, REST, D5, REST,

    G4, REST, B4, REST,
    D5, REST, B4, REST,

    A4, REST, C5, REST,
    E5, REST, C5, REST,

    F5, REST, E5, REST,
    D5, REST, C5, REST,

    B4, REST, D5, REST,
    F5, REST, D5, REST,

    G4, REST, A4, REST,
    B4, REST, A4, REST,

    D5, REST, A4, REST,
    F5, REST, D5, REST,

    D5, REST, F5, REST,
    A5, REST, F5, REST,

    E5, REST, D5, REST,
    B4, REST, D5, REST,

    G4, REST, B4, REST,
    D5, REST, B4, REST,

    A4, REST, C5, REST,
    E5, REST, C5, REST,

    F5, REST, E5, REST,
    D5, REST, C5, REST,

    B4, REST, D5, REST,
    F5, REST, D5, REST,

    G4, REST, A4, REST,
    B4, REST, A4, REST,

    D5, REST, F5, REST,
    D5, REST, REST, REST
};


static const uint16_t Board_AudioBassSequence[
    BOARD_AUDIO_SEQUENCE_STEPS
] =
{
    D3, REST, REST, REST,
    D3, REST, REST, REST,

    B2, REST, REST, REST,
    B2, REST, REST, REST,

    G2, REST, REST, REST,
    G2, REST, REST, REST,

    A2, REST, REST, REST,
    A2, REST, REST, REST,

    D3, REST, REST, REST,
    D3, REST, REST, REST,

    B2, REST, REST, REST,
    B2, REST, REST, REST,

    G2, REST, REST, REST,
    G2, REST, REST, REST,

    A2, REST, REST, REST,
    A2, REST, REST, REST,

    D3, REST, REST, REST,
    D3, REST, REST, REST,

    B2, REST, REST, REST,
    B2, REST, REST, REST,

    G2, REST, REST, REST,
    G2, REST, REST, REST,

    A2, REST, REST, REST,
    A2, REST, REST, REST,

    D3, REST, REST, REST,
    D3, REST, REST, REST,

    B2, REST, REST, REST,
    B2, REST, REST, REST,

    G2, REST, REST, REST,
    G2, REST, REST, REST,

    A2, REST, REST, REST,
    A2, REST, REST, REST
};


static const uint16_t Board_AudioArpeggioSequence[
    BOARD_AUDIO_SEQUENCE_STEPS
] =
{
    D4, A4, D5, A4,
    F5, A4, D5, A4,

    B3, F4, B4, F4,
    D5, F4, B4, F4,

    G3, D4, G4, D4,
    B4, D4, G4, D4,

    A3, E4, A4, E4,
    C5, E4, A4, E4,

    D4, A4, D5, A4,
    F5, A4, D5, A4,

    B3, F4, B4, F4,
    D5, F4, B4, F4,

    G3, D4, G4, D4,
    B4, D4, G4, D4,

    A3, E4, A4, E4,
    C5, E4, A4, E4,

    D4, A4, D5, A4,
    F5, A4, D5, A4,

    B3, F4, B4, F4,
    D5, F4, B4, F4,

    G3, D4, G4, D4,
    B4, D4, G4, D4,

    A3, E4, A4, E4,
    C5, E4, A4, E4,

    D4, A4, D5, A4,
    F5, A4, D5, A4,

    B3, F4, B4, F4,
    D5, F4, B4, F4,

    G3, D4, G4, D4,
    B4, D4, G4, D4,

    A3, E4, A4, E4,
    D5, A4, F5, D5
};


static void Board_AudioHalfTransferCallback(
    Audio_HandleTypeDef *Handle,
    void *Buffer,
    uint32_t SampleCount
);

static void Board_AudioTransferCompleteCallback(
    Audio_HandleTypeDef *Handle,
    void *Buffer,
    uint32_t SampleCount
);

static void Board_AudioErrorCallback(
    Audio_HandleTypeDef *Handle
);

static void Board_AudioFillSamples(
    uint16_t *Destination,
    uint32_t SampleCount
);

static uint16_t Board_AudioGenerateSample(void);

static int32_t Board_AudioTriangleWave(
    uint32_t *Phase,
    uint32_t PhaseStep
);

static int32_t Board_AudioPulseWave(
    uint32_t *Phase,
    uint32_t PhaseStep
);

static uint32_t Board_AudioFrequencyToPhaseStep(
    uint32_t FrequencyHz
);

static uint32_t Board_AudioGetEnvelope(void);

static uint32_t Board_AudioGetAccentEnvelope(void);

static void Board_AudioUpdateLeadGlide(void);

static void Board_AudioLoadSequenceStep(void);

static void Board_AudioAdvanceSequence(void);

static uint32_t Board_AudioTakeFlags(void);

Audio_StatusTypeDef Board_AudioTestStart(
    Audio_HandleTypeDef *AudioHandle)
{
    Audio_StatusTypeDef status;

    if(AudioHandle == NULL)
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    if(AudioHandle->SampleRateHz == 0U)
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    Board_AudioTestHandle = AudioHandle;

    Board_AudioTestFlags = 0U;
    Board_AudioTestOverrun = 0U;

    Board_AudioLeadPhase = 0U;
    Board_AudioLeadHarmonicPhase = 0U;
    Board_AudioBassPhase = 0U;
    Board_AudioArpeggioPhase = 0U;
    Board_AudioAccentPhase = 0U;

    Board_AudioSequenceStep = 0U;
    Board_AudioSamplesIntoStep = 0U;
    Board_AudioLeadPhaseStepCurrent = 0U;
    Board_AudioLeadPhaseStepTarget = 0U;
    Board_AudioLeadGlideSamplesRemaining = 0U;
    Board_AudioLeadGate = 0U;

    Board_AudioSamplesPerStep =
        (uint32_t)(
            ((uint64_t)AudioHandle->SampleRateHz * 60ULL) /
            ((uint64_t)BOARD_AUDIO_TEMPO_BPM *
             BOARD_AUDIO_STEPS_PER_BEAT));

    if(Board_AudioSamplesPerStep == 0U)
    {
        Board_AudioSamplesPerStep = 1U;
    }

    Board_AudioLoadSequenceStep();

    AudioHandle->HalfTransferCallback =
        Board_AudioHalfTransferCallback;

    AudioHandle->TransferCompleteCallback =
        Board_AudioTransferCompleteCallback;

    AudioHandle->ErrorCallback =
        Board_AudioErrorCallback;

    Board_AudioFillSamples(
        &Board_AudioTestBuffer[0],
        BOARD_AUDIO_TEST_HALF_SAMPLES);

    Board_AudioFillSamples(
        &Board_AudioTestBuffer[
            BOARD_AUDIO_TEST_HALF_SAMPLES
        ],
        BOARD_AUDIO_TEST_HALF_SAMPLES);

    Audio_CleanBufferCache(
        Board_AudioTestBuffer,
        sizeof(Board_AudioTestBuffer));

    status = Audio_Start(
        AudioHandle,
        Board_AudioTestBuffer,
        BOARD_AUDIO_TEST_BUFFER_SAMPLES,
        AUDIO_PLAY_LOOP);

    if(status != AUDIO_STATUS_OK)
    {
        AudioHandle->HalfTransferCallback = NULL;
        AudioHandle->TransferCompleteCallback = NULL;
        AudioHandle->ErrorCallback = NULL;

        Board_AudioTestHandle = NULL;
    }

    return status;
}

Audio_StatusTypeDef Board_AudioTestStop(void)
{
    Audio_StatusTypeDef status;

    if(Board_AudioTestHandle == NULL)
    {
        return AUDIO_STATUS_ERROR;
    }

    status = Audio_Stop(Board_AudioTestHandle);

    Board_AudioTestHandle->HalfTransferCallback = NULL;
    Board_AudioTestHandle->TransferCompleteCallback = NULL;
    Board_AudioTestHandle->ErrorCallback = NULL;

    Board_AudioTestFlags = 0U;
    Board_AudioTestHandle = NULL;

    return status;
}

void Board_AudioTestUpdate(void)
{
    uint32_t flags;

    if(Board_AudioTestHandle == NULL)
    {
        return;
    }

    flags = Board_AudioTakeFlags();

    if((flags &
        BOARD_AUDIO_FLAG_FILL_FIRST_HALF) != 0U)
    {
        Board_AudioFillSamples(
            &Board_AudioTestBuffer[0],
            BOARD_AUDIO_TEST_HALF_SAMPLES);

        Audio_CleanBufferCache(
            &Board_AudioTestBuffer[0],
            BOARD_AUDIO_TEST_HALF_SAMPLES *
            sizeof(Board_AudioTestBuffer[0]));
    }

    if((flags &
        BOARD_AUDIO_FLAG_FILL_SECOND_HALF) != 0U)
    {
        Board_AudioFillSamples(
            &Board_AudioTestBuffer[
                BOARD_AUDIO_TEST_HALF_SAMPLES
            ],
            BOARD_AUDIO_TEST_HALF_SAMPLES);

        Audio_CleanBufferCache(
            &Board_AudioTestBuffer[
                BOARD_AUDIO_TEST_HALF_SAMPLES
            ],
            BOARD_AUDIO_TEST_HALF_SAMPLES *
            sizeof(Board_AudioTestBuffer[0]));
    }
}

uint8_t Board_AudioTestHasOverrun(void)
{
    return Board_AudioTestOverrun;
}

static void Board_AudioHalfTransferCallback(
    Audio_HandleTypeDef *Handle,
    void *Buffer,
    uint32_t SampleCount)
{
    (void)Handle;
    (void)Buffer;
    (void)SampleCount;

    if((Board_AudioTestFlags &
        BOARD_AUDIO_FLAG_FILL_FIRST_HALF) != 0U)
    {
        Board_AudioTestOverrun = 1U;
    }

    Board_AudioTestFlags |=
        BOARD_AUDIO_FLAG_FILL_FIRST_HALF;
}

static void Board_AudioTransferCompleteCallback(
    Audio_HandleTypeDef *Handle,
    void *Buffer,
    uint32_t SampleCount)
{
    (void)Handle;
    (void)Buffer;
    (void)SampleCount;

    if((Board_AudioTestFlags &
        BOARD_AUDIO_FLAG_FILL_SECOND_HALF) != 0U)
    {
        Board_AudioTestOverrun = 1U;
    }

    Board_AudioTestFlags |=
        BOARD_AUDIO_FLAG_FILL_SECOND_HALF;
}

static void Board_AudioErrorCallback(
    Audio_HandleTypeDef *Handle)
{
    (void)Handle;

    Board_AudioTestOverrun = 1U;
}

static void Board_AudioFillSamples(
    uint16_t *Destination,
    uint32_t SampleCount)
{
    uint32_t index;

    if((Destination == NULL) ||
       (Board_AudioTestHandle == NULL))
    {
        return;
    }

    for(index = 0U; index < SampleCount; index++)
    {
        Destination[index] =
            Board_AudioGenerateSample();
    }
}

static uint16_t Board_AudioGenerateSample(void)
{
    int32_t lead;
    int32_t harmonic;
    int32_t bass;
    int32_t arpeggio;
    int32_t accent;

    int32_t mixed_sample;
    int32_t dac_sample;

    uint32_t note_envelope;
    uint32_t accent_envelope;

    Board_AudioUpdateLeadGlide();

    lead = Board_AudioTriangleWave(
        &Board_AudioLeadPhase,
        Board_AudioLeadPhaseStepCurrent);

    harmonic = Board_AudioTriangleWave(
        &Board_AudioLeadHarmonicPhase,
        Board_AudioLeadPhaseStepCurrent * 2U);

    bass = Board_AudioPulseWave(
        &Board_AudioBassPhase,
        Board_AudioBassPhaseStep);

    arpeggio = Board_AudioTriangleWave(
        &Board_AudioArpeggioPhase,
        Board_AudioArpeggioPhaseStep);

    accent = Board_AudioTriangleWave(
        &Board_AudioAccentPhase,
        Board_AudioAccentPhaseStep);

    if(Board_AudioLeadGate == 0U)
    {
        lead = 0L;
        harmonic = 0L;
    }

    note_envelope = Board_AudioGetEnvelope();
    accent_envelope = Board_AudioGetAccentEnvelope();

    lead =
        (int32_t)(
            ((int64_t)lead *
             BOARD_AUDIO_LEAD_LEVEL *
             note_envelope) /
            (4095LL * 4095LL));

    harmonic =
        (int32_t)(
            ((int64_t)harmonic *
             BOARD_AUDIO_HARMONIC_LEVEL *
             note_envelope) /
            (4095LL * 4095LL));

    bass =
        (int32_t)(
            ((int64_t)bass *
             BOARD_AUDIO_BASS_LEVEL) /
            4095LL);

    arpeggio =
        (int32_t)(
            ((int64_t)arpeggio *
             BOARD_AUDIO_ARPEGGIO_LEVEL *
             note_envelope) /
            (4095LL * 4095LL));

    accent =
        (int32_t)(
            ((int64_t)accent *
             BOARD_AUDIO_ACCENT_LEVEL *
             accent_envelope) /
            (4095LL * 4095LL));

    mixed_sample =
        lead +
        harmonic +
        bass +
        arpeggio +
        accent;

    dac_sample =
        BOARD_AUDIO_DAC_MIDPOINT +
        mixed_sample;

    if(dac_sample < 0L)
    {
        dac_sample = 0L;
    }
    else if(dac_sample > 4095L)
    {
        dac_sample = 4095L;
    }

    Board_AudioAdvanceSequence();

    return (uint16_t)dac_sample;
}

static int32_t Board_AudioTriangleWave(
    uint32_t *Phase,
    uint32_t PhaseStep)
{
    uint32_t phase_position;
    uint32_t ramp;
    int32_t value;

    if((Phase == NULL) ||
       (PhaseStep == 0U))
    {
        return 0L;
    }

    *Phase += PhaseStep;

    phase_position = *Phase >> 19U;

    if(phase_position < 4096U)
    {
        ramp = phase_position;

        value =
            -4095L +
            ((int32_t)ramp * 2L);
    }
    else
    {
        ramp =
            phase_position -
            4096U;

        value =
            4095L -
            ((int32_t)ramp * 2L);
    }

    return value;
}

static int32_t Board_AudioPulseWave(
    uint32_t *Phase,
    uint32_t PhaseStep)
{
    uint32_t pulse_position;

    if((Phase == NULL) ||
       (PhaseStep == 0U))
    {
        return 0L;
    }

    *Phase += PhaseStep;

    pulse_position =
        *Phase >> 28U;

    if(pulse_position < 5U)
    {
        return 4095L;
    }

    return -1365L;
}

static uint32_t Board_AudioFrequencyToPhaseStep(
    uint32_t FrequencyHz)
{
    if((Board_AudioTestHandle == NULL) ||
       (Board_AudioTestHandle->SampleRateHz == 0U) ||
       (FrequencyHz == REST))
    {
        return 0U;
    }

    return
        (uint32_t)(
            (((uint64_t)FrequencyHz << 32U) +
             (Board_AudioTestHandle->SampleRateHz / 2U)) /
            Board_AudioTestHandle->SampleRateHz);
}

static uint32_t Board_AudioGetEnvelope(void)
{
    uint32_t attack_samples;
    uint32_t release_start;
    uint32_t release_samples;
    uint32_t release_position;

    attack_samples =
        Board_AudioSamplesPerStep / 20U;

    if(attack_samples == 0U)
    {
        attack_samples = 1U;
    }

    release_start =
        (Board_AudioSamplesPerStep * 3U) /
        4U;

    if(Board_AudioSamplesIntoStep <
       attack_samples)
    {
        return
            (uint32_t)(
                ((uint64_t)Board_AudioSamplesIntoStep *
                 4095ULL) /
                attack_samples);
    }

    if(Board_AudioSamplesIntoStep >=
       release_start)
    {
        release_samples =
            Board_AudioSamplesPerStep -
            release_start;

        if(release_samples == 0U)
        {
            return 0U;
        }

        release_position =
            Board_AudioSamplesIntoStep -
            release_start;

        return
            4095U -
            (uint32_t)(
                ((uint64_t)release_position *
                 4095ULL) /
                release_samples);
    }

    return 4095U;
}

static uint32_t Board_AudioGetAccentEnvelope(void)
{
    uint32_t accent_samples;
    uint32_t remaining;

    if((Board_AudioSequenceStep & 0x3U) != 0U)
    {
        return 0U;
    }

    accent_samples =
        Board_AudioSamplesPerStep / 6U;

    if(accent_samples == 0U)
    {
        accent_samples = 1U;
    }

    if(Board_AudioSamplesIntoStep >=
       accent_samples)
    {
        return 0U;
    }

    remaining =
        accent_samples -
        Board_AudioSamplesIntoStep;

    return
        (uint32_t)(
            ((uint64_t)remaining *
             4095ULL) /
            accent_samples);
}

static void Board_AudioUpdateLeadGlide(void)
{
    uint32_t difference;
    uint32_t change;

    if(Board_AudioLeadGlideSamplesRemaining == 0U)
    {
        Board_AudioLeadPhaseStepCurrent =
            Board_AudioLeadPhaseStepTarget;

        return;
    }

    if(Board_AudioLeadPhaseStepCurrent <
       Board_AudioLeadPhaseStepTarget)
    {
        difference =
            Board_AudioLeadPhaseStepTarget -
            Board_AudioLeadPhaseStepCurrent;

        change =
            difference /
            Board_AudioLeadGlideSamplesRemaining;

        if(change == 0U)
        {
            change = 1U;
        }

        Board_AudioLeadPhaseStepCurrent += change;
    }
    else if(Board_AudioLeadPhaseStepCurrent >
            Board_AudioLeadPhaseStepTarget)
    {
        difference =
            Board_AudioLeadPhaseStepCurrent -
            Board_AudioLeadPhaseStepTarget;

        change =
            difference /
            Board_AudioLeadGlideSamplesRemaining;

        if(change == 0U)
        {
            change = 1U;
        }

        Board_AudioLeadPhaseStepCurrent -= change;
    }

    Board_AudioLeadGlideSamplesRemaining--;

    if(Board_AudioLeadGlideSamplesRemaining == 0U)
    {
        Board_AudioLeadPhaseStepCurrent =
            Board_AudioLeadPhaseStepTarget;
    }
}

static void Board_AudioLoadSequenceStep(void)
{
    uint32_t lead_frequency;
    uint32_t bass_frequency;
    uint32_t arpeggio_frequency;

    lead_frequency =
        Board_AudioLeadSequence[
            Board_AudioSequenceStep
        ];

    bass_frequency =
        Board_AudioBassSequence[
            Board_AudioSequenceStep
        ];

    arpeggio_frequency =
        Board_AudioArpeggioSequence[
            Board_AudioSequenceStep
        ];

    if(lead_frequency == REST)
    {
        /*
         * Silence the lead, but retain its pitch so the next sounding note
         * can glide from the previous note rather than from 0 Hz.
         */
        Board_AudioLeadGate = 0U;
        Board_AudioLeadGlideSamplesRemaining = 0U;
    }
    else
    {
        Board_AudioLeadGate = 1U;

        Board_AudioLeadPhaseStepTarget =
            Board_AudioFrequencyToPhaseStep(
                lead_frequency);

        if(Board_AudioLeadPhaseStepCurrent == 0U)
        {
            /*
             * The first note starts immediately because there is no previous
             * pitch from which to glide.
             */
            Board_AudioLeadPhaseStepCurrent =
                Board_AudioLeadPhaseStepTarget;

            Board_AudioLeadGlideSamplesRemaining = 0U;
        }
        else
        {
            Board_AudioLeadGlideSamplesRemaining =
                (Board_AudioSamplesPerStep *
                 BOARD_AUDIO_LEAD_GLIDE_PERCENT) /
                100U;

            if(Board_AudioLeadGlideSamplesRemaining == 0U)
            {
                Board_AudioLeadGlideSamplesRemaining = 1U;
            }
        }
    }

    Board_AudioBassPhaseStep =
        Board_AudioFrequencyToPhaseStep(
            bass_frequency);

    Board_AudioArpeggioPhaseStep =
        Board_AudioFrequencyToPhaseStep(
            arpeggio_frequency);

    Board_AudioAccentPhaseStep =
        Board_AudioFrequencyToPhaseStep(
            C3);
}

static void Board_AudioAdvanceSequence(void)
{
    Board_AudioSamplesIntoStep++;

    if(Board_AudioSamplesIntoStep >=
       Board_AudioSamplesPerStep)
    {
        Board_AudioSamplesIntoStep = 0U;

        Board_AudioSequenceStep++;

        if(Board_AudioSequenceStep >=
           BOARD_AUDIO_SEQUENCE_STEPS)
        {
            Board_AudioSequenceStep = 0U;
        }

        Board_AudioArpeggioPhase = 0U;
        Board_AudioAccentPhase = 0U;

        Board_AudioLoadSequenceStep();
    }
}

static uint32_t Board_AudioTakeFlags(void)
{
    uint32_t flags;
    uint32_t primask;

    primask = __get_PRIMASK();

    __disable_irq();

    flags = Board_AudioTestFlags;
    Board_AudioTestFlags = 0U;

    if(primask == 0U)
    {
        __enable_irq();
    }

    return flags;
}
