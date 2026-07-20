/*
 * Audio.h
 *
 * Audio subsystem for STM32H7A3.
 *
 * The audio handle references externally owned DAC and DMA handles.
 * Board.h / Board.c are responsible for defining and attaching those
 * peripheral handles.
 *
 * Audio does not expose a hardware interrupt handler. DMA interrupt
 * handlers call DMA_IRQHandler(), and the DMA driver notifies Audio
 * through its registered callback.
 */

#ifndef INC_AUDIO_H_
#define INC_AUDIO_H_

#include <stdint.h>
#include "STM32H7A3_DAC.h"
#include "STM32H7A3_DMA.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    AUDIO_STATUS_OK = 0U,
    AUDIO_STATUS_ERROR,
    AUDIO_STATUS_BUSY,
    AUDIO_STATUS_TIMEOUT,
    AUDIO_STATUS_INVALID_PARAMETER

} Audio_StatusTypeDef;

typedef enum
{
    AUDIO_STATE_RESET = 0U,
    AUDIO_STATE_READY,
    AUDIO_STATE_PLAYING,
    AUDIO_STATE_ERROR

} Audio_StateTypeDef;

typedef enum
{
    AUDIO_PLAY_ONCE = 0U,
    AUDIO_PLAY_LOOP

} Audio_PlayModeTypeDef;

struct Audio_Handle;

typedef void (*Audio_BufferCallbackTypeDef)(
    struct Audio_Handle *Handle,
    void *Buffer,
    uint32_t SampleCount
);

typedef void (*Audio_ErrorCallbackTypeDef)(
    struct Audio_Handle *Handle
);

typedef struct Audio_Handle
{
    /*
     * Externally owned peripheral handles.
     */
    DAC_HandleTypeDef *DAC;
    DMA_HandleTypeDef *DMA;

    /*
     * Basic timer used as the DAC sample trigger.
     *
     * TIM6 and TIM7 are supported.
     */
    TIM_TypeDef *TimerInstance;

    /*
     * Actual timer kernel clock supplied to TimerInstance.
     */
    uint32_t TimerKernelClockHz;

    /*
     * Desired DAC sample rate.
     */
    uint32_t SampleRateHz;

    Audio_PlayModeTypeDef PlayMode;

    /*
     * Active playback buffer.
     *
     * During circular playback, the DMA half-transfer and
     * transfer-complete events divide this buffer into two halves.
     */
    void *Buffer;
    uint32_t BufferSampleCount;

    /*
     * Called from the DMA driver's callback context.
     *
     * Keep these callbacks short because they execute during the
     * DMA interrupt unless the application only sets flags here.
     */
    Audio_BufferCallbackTypeDef HalfTransferCallback;
    Audio_BufferCallbackTypeDef TransferCompleteCallback;
    Audio_ErrorCallbackTypeDef ErrorCallback;

    volatile Audio_StateTypeDef State;

} Audio_HandleTypeDef;

/*
 * Initializes the attached DAC, DMA, and sample timer.
 *
 * Before calling:
 *
 * - Configure the DAC output GPIO as analog with no pull.
 * - Attach valid DAC and DMA handles.
 * - Set TimerInstance, TimerKernelClockHz, and SampleRateHz.
 *
 * Audio_Init registers its internal event callback with the DMA handle.
 */
Audio_StatusTypeDef Audio_Init(
    Audio_HandleTypeDef *Handle
);

/*
 * Starts one-shot or circular playback.
 *
 * SampleCount must be nonzero, even, and no greater than 65535.
 */
Audio_StatusTypeDef Audio_Start(
    Audio_HandleTypeDef *Handle,
    void *Buffer,
    uint32_t SampleCount,
    Audio_PlayModeTypeDef PlayMode
);

/*
 * Stops playback and writes the DAC midpoint.
 */
Audio_StatusTypeDef Audio_Stop(
    Audio_HandleTypeDef *Handle
);

/*
 * Writes the DAC midpoint while playback is stopped.
 */
Audio_StatusTypeDef Audio_WriteSilence(
    Audio_HandleTypeDef *Handle
);

/*
 * Changes the sample rate while playback is stopped.
 */
Audio_StatusTypeDef Audio_SetSampleRate(
    Audio_HandleTypeDef *Handle,
    uint32_t SampleRateHz
);

/*
 * Returns the current audio state.
 */
Audio_StateTypeDef Audio_GetState(
    const Audio_HandleTypeDef *Handle
);

/*
 * Cache helpers for DMA buffers.
 */
void Audio_CleanBufferCache(
    const void *Buffer,
    uint32_t ByteCount
);

void Audio_InvalidateBufferCache(
    const void *Buffer,
    uint32_t ByteCount
);

#ifdef __cplusplus
}
#endif

#endif /* INC_AUDIO_H_ */
