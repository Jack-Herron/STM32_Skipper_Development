/*
 * Audio.c
 *
 * Audio subsystem for STM32H7A3.
 *
 * The audio handle references externally owned DAC and DMA handles.
 */

#include <STDLIB.h>
#include "Audio.h"

#define AUDIO_CACHE_LINE_SIZE       32UL
#define AUDIO_TIMER_MAX_PERIOD      65536UL
#define AUDIO_DAC_12BIT_MIDPOINT    2048U
#define AUDIO_DAC_8BIT_MIDPOINT     128U

static uint8_t Audio_IsHandleValid(
    const Audio_HandleTypeDef *Handle
);

static Audio_StatusTypeDef Audio_ConfigureTimer(
    Audio_HandleTypeDef *Handle
);

static Audio_StatusTypeDef Audio_ConfigureDMA(
    Audio_HandleTypeDef *Handle
);

static void Audio_DMAEventCallback(
    DMA_HandleTypeDef *DMAHandle,
    DMA_EventTypeDef Event
);

static Audio_HandleTypeDef *Audio_FindOwner(
    DMA_HandleTypeDef *DMAHandle
);

static uint32_t Audio_GetSampleByteCount(
    const Audio_HandleTypeDef *Handle,
    uint32_t SampleCount
);

static void Audio_StartTimer(
    Audio_HandleTypeDef *Handle
);

static void Audio_StopTimer(
    Audio_HandleTypeDef *Handle
);

/*
 * The generic DMA callback only receives a DMA handle.
 *
 * Since the DMA handle is externally owned rather than embedded in the
 * audio handle, keep the currently attached audio handle here.
 *
 * This implementation supports one active audio output instance.
 */
static Audio_HandleTypeDef *Audio_ActiveHandle = NULL;

Audio_StatusTypeDef Audio_Init(
    Audio_HandleTypeDef *Handle)
{
    Audio_StatusTypeDef status;

    if(Audio_IsHandleValid(Handle) == 0U)
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    Handle->State = AUDIO_STATE_RESET;
    Handle->Buffer = NULL;
    Handle->BufferSampleCount = 0U;

    status = Audio_ConfigureDMA(Handle);

    if(status != AUDIO_STATUS_OK)
    {
        Handle->State = AUDIO_STATE_ERROR;
        return status;
    }

    if(DAC_Init(Handle->DAC) != DAC_STATUS_OK)
    {
        Handle->State = AUDIO_STATE_ERROR;
        return AUDIO_STATUS_ERROR;
    }

    if(DMA_Init(Handle->DMA) != DMA_STATUS_OK)
    {
        Handle->State = AUDIO_STATE_ERROR;
        return AUDIO_STATUS_ERROR;
    }

    status = Audio_ConfigureTimer(Handle);

    if(status != AUDIO_STATUS_OK)
    {
        Handle->State = AUDIO_STATE_ERROR;
        return status;
    }

    if(DAC_Enable(Handle->DAC) != DAC_STATUS_OK)
    {
        Handle->State = AUDIO_STATE_ERROR;
        return AUDIO_STATUS_ERROR;
    }

    Handle->State = AUDIO_STATE_READY;

    if(Audio_WriteSilence(Handle) != AUDIO_STATUS_OK)
    {
        Handle->State = AUDIO_STATE_ERROR;
        return AUDIO_STATUS_ERROR;
    }

    return AUDIO_STATUS_OK;
}

Audio_StatusTypeDef Audio_Start(
    Audio_HandleTypeDef *Handle,
    void *Buffer,
    uint32_t SampleCount,
    Audio_PlayModeTypeDef PlayMode)
{
    DMA_StatusTypeDef dma_status;
    uintptr_t dac_address;
    uint32_t byte_count;

    if(Audio_IsHandleValid(Handle) == 0U)
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    if((PlayMode != AUDIO_PLAY_ONCE) &&
       (PlayMode != AUDIO_PLAY_LOOP))
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    if((Buffer == NULL) ||
       (SampleCount == 0U) ||
       ((SampleCount & 1UL) != 0U) ||
       (SampleCount > 65535UL))
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    if(Handle->State == AUDIO_STATE_PLAYING)
    {
        return AUDIO_STATUS_BUSY;
    }

    if(Handle->State != AUDIO_STATE_READY)
    {
        return AUDIO_STATUS_ERROR;
    }

    if((Audio_ActiveHandle != NULL) &&
       (Audio_ActiveHandle != Handle) &&
       (Audio_ActiveHandle->State == AUDIO_STATE_PLAYING))
    {
        return AUDIO_STATUS_BUSY;
    }

    dac_address = DAC_GetDataRegisterAddress(Handle->DAC);

    if(dac_address == (uintptr_t)0U)
    {
        return AUDIO_STATUS_ERROR;
    }

    byte_count = Audio_GetSampleByteCount(
        Handle,
        SampleCount);

    if(byte_count == 0U)
    {
        return AUDIO_STATUS_ERROR;
    }

    /*
     * Select normal DMA for one-shot playback and circular DMA
     * for continuous looping.
     */
    if(PlayMode == AUDIO_PLAY_LOOP)
    {
        Handle->DMA->Config.Mode = DMA_MODE_CIRCULAR;
    }
    else
    {
        Handle->DMA->Config.Mode = DMA_MODE_NORMAL;
    }

    /*
     * DMA_Init() must be called again because the mode field is written
     * into the stream control register during initialization.
     */
    if(DMA_Init(Handle->DMA) != DMA_STATUS_OK)
    {
        return AUDIO_STATUS_ERROR;
    }

    Handle->PlayMode = PlayMode;
    Handle->Buffer = Buffer;
    Handle->BufferSampleCount = SampleCount;
    Audio_ActiveHandle = Handle;

    Audio_CleanBufferCache(
        Buffer,
        byte_count);

    if(DAC_ClearDMAUnderrun(Handle->DAC) != DAC_STATUS_OK)
    {
        Handle->Buffer = NULL;
        Handle->BufferSampleCount = 0U;
        Audio_ActiveHandle = NULL;

        return AUDIO_STATUS_ERROR;
    }

    if(DAC_EnableDMARequest(Handle->DAC) != DAC_STATUS_OK)
    {
        Handle->Buffer = NULL;
        Handle->BufferSampleCount = 0U;
        Audio_ActiveHandle = NULL;

        return AUDIO_STATUS_ERROR;
    }

    if(DAC_EnableDMAUnderrunInterrupt(Handle->DAC) != DAC_STATUS_OK)
    {
        (void)DAC_DisableDMARequest(Handle->DAC);

        Handle->Buffer = NULL;
        Handle->BufferSampleCount = 0U;
        Audio_ActiveHandle = NULL;

        return AUDIO_STATUS_ERROR;
    }

    dma_status = DMA_Start(
        Handle->DMA,
        dac_address,
        (uintptr_t)Buffer,
        SampleCount);

    if(dma_status != DMA_STATUS_OK)
    {
        (void)DAC_DisableDMAUnderrunInterrupt(Handle->DAC);
        (void)DAC_DisableDMARequest(Handle->DAC);
        (void)DAC_ClearDMAUnderrun(Handle->DAC);

        Handle->Buffer = NULL;
        Handle->BufferSampleCount = 0U;
        Audio_ActiveHandle = NULL;

        if(dma_status == DMA_STATUS_BUSY)
        {
            return AUDIO_STATUS_BUSY;
        }

        if(dma_status == DMA_STATUS_TIMEOUT)
        {
            return AUDIO_STATUS_TIMEOUT;
        }

        return AUDIO_STATUS_ERROR;
    }

    Handle->State = AUDIO_STATE_PLAYING;

    /*
     * Start TIM6/TIM7 only after DMA and DAC requests are armed.
     */
    Audio_StartTimer(Handle);

    return AUDIO_STATUS_OK;
}

Audio_StatusTypeDef Audio_Stop(
    Audio_HandleTypeDef *Handle)
{
    DMA_StatusTypeDef dma_status;
    Audio_StatusTypeDef silence_status;

    if(Audio_IsHandleValid(Handle) == 0U)
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    Audio_StopTimer(Handle);

    dma_status = DMA_Stop(Handle->DMA);

    (void)DAC_DisableDMAUnderrunInterrupt(Handle->DAC);
    (void)DAC_DisableDMARequest(Handle->DAC);
    (void)DAC_ClearDMAUnderrun(Handle->DAC);

    Handle->Buffer = NULL;
    Handle->BufferSampleCount = 0U;

    if(Audio_ActiveHandle == Handle)
    {
        Audio_ActiveHandle = NULL;
    }

    if(dma_status == DMA_STATUS_TIMEOUT)
    {
        Handle->State = AUDIO_STATE_ERROR;
        return AUDIO_STATUS_TIMEOUT;
    }

    if(dma_status != DMA_STATUS_OK)
    {
        Handle->State = AUDIO_STATE_ERROR;
        return AUDIO_STATUS_ERROR;
    }

    Handle->State = AUDIO_STATE_READY;

    silence_status = Audio_WriteSilence(Handle);

    if(silence_status != AUDIO_STATUS_OK)
    {
        Handle->State = AUDIO_STATE_ERROR;
    }

    return silence_status;
}

Audio_StatusTypeDef Audio_WriteSilence(
    Audio_HandleTypeDef *Handle)
{
    uint16_t midpoint;

    if(Audio_IsHandleValid(Handle) == 0U)
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    if(Handle->State == AUDIO_STATE_PLAYING)
    {
        return AUDIO_STATUS_BUSY;
    }

    if(Handle->DAC->Alignment == DAC_ALIGNMENT_8BIT_RIGHT)
    {
        midpoint = AUDIO_DAC_8BIT_MIDPOINT;
    }
    else if(Handle->DAC->Alignment == DAC_ALIGNMENT_12BIT_LEFT)
    {
        midpoint = AUDIO_DAC_12BIT_MIDPOINT << 4U;
    }
    else
    {
        midpoint = AUDIO_DAC_12BIT_MIDPOINT;
    }

    if(DAC_Write(Handle->DAC, midpoint) != DAC_STATUS_OK)
    {
        return AUDIO_STATUS_ERROR;
    }

    return AUDIO_STATUS_OK;
}

Audio_StatusTypeDef Audio_SetSampleRate(
    Audio_HandleTypeDef *Handle,
    uint32_t SampleRateHz)
{
    uint32_t previous_rate;
    Audio_StatusTypeDef status;

    if(Audio_IsHandleValid(Handle) == 0U)
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    if(SampleRateHz == 0U)
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    if(Handle->State == AUDIO_STATE_PLAYING)
    {
        return AUDIO_STATUS_BUSY;
    }

    previous_rate = Handle->SampleRateHz;
    Handle->SampleRateHz = SampleRateHz;

    status = Audio_ConfigureTimer(Handle);

    if(status != AUDIO_STATUS_OK)
    {
        Handle->SampleRateHz = previous_rate;
        (void)Audio_ConfigureTimer(Handle);
    }

    return status;
}

Audio_StateTypeDef Audio_GetState(
    const Audio_HandleTypeDef *Handle)
{
    if(Handle == NULL)
    {
        return AUDIO_STATE_ERROR;
    }

    return Handle->State;
}

void Audio_DAC_IRQHandler(
    Audio_HandleTypeDef *Handle)
{
    if(Audio_IsHandleValid(Handle) == 0U)
    {
        return;
    }

    if(DAC_IsDMAUnderrun(Handle->DAC) == 0U)
    {
        return;
    }

    /*
     * A DAC underrun means the DAC requested another sample but DMA did
     * not provide one in time. Stop the complete audio path before
     * clearing the underrun condition.
     */
    Audio_StopTimer(Handle);

    (void)DMA_Stop(Handle->DMA);
    (void)DAC_DisableDMAUnderrunInterrupt(Handle->DAC);
    (void)DAC_DisableDMARequest(Handle->DAC);
    (void)DAC_ClearDMAUnderrun(Handle->DAC);

    Handle->Buffer = NULL;
    Handle->BufferSampleCount = 0U;
    Handle->State = AUDIO_STATE_ERROR;

    if(Audio_ActiveHandle == Handle)
    {
        Audio_ActiveHandle = NULL;
    }

    if(Handle->ErrorCallback != NULL)
    {
        Handle->ErrorCallback(Handle);
    }
}

void Audio_CleanBufferCache(
    const void *Buffer,
    uint32_t ByteCount)
{
#if (__DCACHE_PRESENT == 1U)

    uintptr_t start;
    uintptr_t end;

    if((Buffer == NULL) || (ByteCount == 0U))
    {
        return;
    }

    start =
        ((uintptr_t)Buffer) &
        ~((uintptr_t)AUDIO_CACHE_LINE_SIZE - 1UL);

    end =
        (((uintptr_t)Buffer +
          ByteCount +
          AUDIO_CACHE_LINE_SIZE - 1UL) &
         ~((uintptr_t)AUDIO_CACHE_LINE_SIZE - 1UL));

    SCB_CleanDCache_by_Addr(
        (uint32_t *)start,
        (int32_t)(end - start));

#else

    (void)Buffer;
    (void)ByteCount;

#endif
}

void Audio_InvalidateBufferCache(
    const void *Buffer,
    uint32_t ByteCount)
{
#if (__DCACHE_PRESENT == 1U)

    uintptr_t start;
    uintptr_t end;

    if((Buffer == NULL) || (ByteCount == 0U))
    {
        return;
    }

    start =
        ((uintptr_t)Buffer) &
        ~((uintptr_t)AUDIO_CACHE_LINE_SIZE - 1UL);

    end =
        (((uintptr_t)Buffer +
          ByteCount +
          AUDIO_CACHE_LINE_SIZE - 1UL) &
         ~((uintptr_t)AUDIO_CACHE_LINE_SIZE - 1UL));

    SCB_InvalidateDCache_by_Addr(
        (uint32_t *)start,
        (int32_t)(end - start));

#else

    (void)Buffer;
    (void)ByteCount;

#endif
}

static uint8_t Audio_IsHandleValid(
    const Audio_HandleTypeDef *Handle)
{
    if(Handle == NULL)
    {
        return 0U;
    }

    if((Handle->DAC == NULL) ||
       (Handle->DMA == NULL) ||
       (Handle->TimerInstance == NULL))
    {
        return 0U;
    }

    if((Handle->DAC->Instance == NULL) ||
       (Handle->DMA->Instance == NULL))
    {
        return 0U;
    }

    if((Handle->TimerKernelClockHz == 0U) ||
       (Handle->SampleRateHz == 0U))
    {
        return 0U;
    }

    if((Handle->TimerInstance != TIM6) &&
       (Handle->TimerInstance != TIM7))
    {
        return 0U;
    }

    return 1U;
}

static Audio_StatusTypeDef Audio_ConfigureTimer(
    Audio_HandleTypeDef *Handle)
{
    uint32_t prescaler;
    uint32_t period_ticks;
    uint64_t denominator;

    prescaler = 0U;

    while(prescaler <= 65535UL)
    {
        denominator =
            (uint64_t)(prescaler + 1UL) *
            (uint64_t)Handle->SampleRateHz;

        period_ticks =
            (uint32_t)(
                ((uint64_t)Handle->TimerKernelClockHz +
                 (denominator / 2ULL)) /
                denominator);

        if((period_ticks >= 1UL) &&
           (period_ticks <= AUDIO_TIMER_MAX_PERIOD))
        {
            break;
        }

        prescaler++;
    }

    if(prescaler > 65535UL)
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    if(Handle->TimerInstance == TIM6)
    {
#if defined(RCC_APB1LENR_TIM6EN)
        RCC->APB1LENR |= RCC_APB1LENR_TIM6EN;
#else
        return AUDIO_STATUS_ERROR;
#endif
    }
    else
    {
#if defined(RCC_APB1LENR_TIM7EN)
        RCC->APB1LENR |= RCC_APB1LENR_TIM7EN;
#else
        return AUDIO_STATUS_ERROR;
#endif
    }

    (void)RCC->APB1LENR;

    Handle->TimerInstance->CR1 = 0U;
    Handle->TimerInstance->CR2 = 0U;
    Handle->TimerInstance->DIER = 0U;
    Handle->TimerInstance->SR = 0U;

    Handle->TimerInstance->PSC = prescaler;
    Handle->TimerInstance->ARR = period_ticks - 1UL;

    /*
     * MMS = 010: update event becomes TRGO.
     */
    Handle->TimerInstance->CR2 = TIM_CR2_MMS_1;

    Handle->TimerInstance->EGR = TIM_EGR_UG;
    Handle->TimerInstance->SR = 0U;

    return AUDIO_STATUS_OK;
}

static Audio_StatusTypeDef Audio_ConfigureDMA(
    Audio_HandleTypeDef *Handle)
{
    uint32_t width_bytes;

    width_bytes = DAC_GetDataWidthBytes(Handle->DAC);

    if(width_bytes == 1U)
    {
        Handle->DMA->Config.PeripheralDataWidth =
            DMA_DATA_WIDTH_BYTE;

        Handle->DMA->Config.MemoryDataWidth =
            DMA_DATA_WIDTH_BYTE;
    }
    else if(width_bytes == 2U)
    {
        Handle->DMA->Config.PeripheralDataWidth =
            DMA_DATA_WIDTH_HALFWORD;

        Handle->DMA->Config.MemoryDataWidth =
            DMA_DATA_WIDTH_HALFWORD;
    }
    else
    {
        return AUDIO_STATUS_INVALID_PARAMETER;
    }

    Handle->DMA->Config.Direction =
        DMA_DIRECTION_MEMORY_TO_PERIPHERAL;

    Handle->DMA->Config.Mode =
        DMA_MODE_CIRCULAR;

    Handle->DMA->Config.PeripheralIncrement = 0U;
    Handle->DMA->Config.MemoryIncrement = 1U;

    Handle->DMA->Config.EnableTransferCompleteInterrupt = 1U;
    Handle->DMA->Config.EnableHalfTransferInterrupt = 1U;
    Handle->DMA->Config.EnableTransferErrorInterrupt = 1U;
    Handle->DMA->Config.EnableDirectModeErrorInterrupt = 1U;

    Handle->DMA->Callback = Audio_DMAEventCallback;

    return AUDIO_STATUS_OK;
}

static void Audio_DMAEventCallback(
    DMA_HandleTypeDef *DMAHandle,
    DMA_EventTypeDef Event)
{
    Audio_HandleTypeDef *audio;
    uint32_t half_samples;
    uint32_t half_bytes;
    uint8_t *buffer_bytes;

    audio = Audio_FindOwner(DMAHandle);

    if((audio == NULL) ||
       (audio->Buffer == NULL) ||
       (audio->BufferSampleCount == 0U))
    {
        return;
    }

    if((Event & (DMA_EVENT_FIFO_ERROR |
                 DMA_EVENT_DIRECT_MODE_ERROR |
                 DMA_EVENT_TRANSFER_ERROR)) != 0U)
    {
        Audio_StopTimer(audio);

        (void)DMA_Stop(audio->DMA);
        (void)DAC_DisableDMAUnderrunInterrupt(audio->DAC);
        (void)DAC_DisableDMARequest(audio->DAC);

        audio->State = AUDIO_STATE_ERROR;

        if(Audio_ActiveHandle == audio)
        {
            Audio_ActiveHandle = NULL;
        }

        if(audio->ErrorCallback != NULL)
        {
            audio->ErrorCallback(audio);
        }

        return;
    }

    half_samples = audio->BufferSampleCount / 2UL;
    half_bytes = Audio_GetSampleByteCount(
        audio,
        half_samples);

    buffer_bytes = (uint8_t *)audio->Buffer;

    if((Event & DMA_EVENT_HALF_TRANSFER) != 0U)
    {
        if(audio->HalfTransferCallback != NULL)
        {
            audio->HalfTransferCallback(
                audio,
                audio->Buffer,
                half_samples);

            Audio_CleanBufferCache( audio->Buffer, half_bytes);
        }
    }

    if((Event & DMA_EVENT_TRANSFER_COMPLETE) != 0U)
    {
        if(audio->TransferCompleteCallback != NULL)
        {
            audio->TransferCompleteCallback(
                audio,
                &buffer_bytes[half_bytes],
                half_samples);

            Audio_CleanBufferCache(&buffer_bytes[half_bytes], half_bytes);
        }
    }
}

static Audio_HandleTypeDef *Audio_FindOwner(
    DMA_HandleTypeDef *DMAHandle)
{
    if((Audio_ActiveHandle != NULL) &&
       (Audio_ActiveHandle->DMA == DMAHandle))
    {
        return Audio_ActiveHandle;
    }

    return NULL;
}

static uint32_t Audio_GetSampleByteCount(
    const Audio_HandleTypeDef *Handle,
    uint32_t SampleCount)
{
    uint32_t width;

    width = DAC_GetDataWidthBytes(Handle->DAC);

    if((width != 1U) && (width != 2U))
    {
        return 0U;
    }

    return SampleCount * width;
}

static void Audio_StartTimer(
    Audio_HandleTypeDef *Handle)
{
    Handle->TimerInstance->SR = 0U;
    Handle->TimerInstance->CR1 |= TIM_CR1_CEN;
}

static void Audio_StopTimer(
    Audio_HandleTypeDef *Handle)
{
    Handle->TimerInstance->CR1 &= ~TIM_CR1_CEN;
    Handle->TimerInstance->SR = 0U;
}
