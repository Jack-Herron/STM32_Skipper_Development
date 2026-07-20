/*
 * STM32H7A3_DMA.c
 *
 * Independent DMA1/DMA2 peripheral driver for STM32H7A3.
 */
#include <STDLIB.h>
#include "STM32H7A3_DMA.h"

#define DMA_DISABLE_TIMEOUT  1000000UL
#define DMA_MAX_TRANSFER_COUNT  65535UL

#define DMA_NORMALIZED_FEIF   (1UL << 0U)
#define DMA_NORMALIZED_DMEIF  (1UL << 2U)
#define DMA_NORMALIZED_TEIF   (1UL << 3U)
#define DMA_NORMALIZED_HTIF   (1UL << 4U)
#define DMA_NORMALIZED_TCIF   (1UL << 5U)

#define DMA_CONTROLLER_COUNT   2U
#define DMA_STREAM_COUNT       8U

static DMA_HandleTypeDef *DMA_HandleRegistry[DMA_CONTROLLER_COUNT][DMA_STREAM_COUNT];


static uint8_t DMA_IsHandleValid(
    const DMA_HandleTypeDef *Handle
);

static uint8_t DMA_IsConfigValid(
    const DMA_ConfigTypeDef *Config
);

static uint32_t DMA_GetDirectionBits(
    DMA_DirectionTypeDef Direction
);

static uint32_t DMA_GetPeripheralWidthBits(
    DMA_DataWidthTypeDef Width
);

static uint32_t DMA_GetMemoryWidthBits(
    DMA_DataWidthTypeDef Width
);

static uint32_t DMA_GetPriorityBits(
    DMA_PriorityTypeDef Priority
);

static uint32_t DMA_GetPeripheralBurstBits(
    DMA_BurstTypeDef Burst
);

static uint32_t DMA_GetMemoryBurstBits(
    DMA_BurstTypeDef Burst
);

static uint32_t DMA_GetFIFOThresholdBits(
    DMA_FIFOThresholdTypeDef Threshold
);

static uint32_t DMA_GetFlagShift(
    uint32_t StreamNumber
);

static uint32_t DMA_ReadNormalizedFlags(
    DMA_TypeDef *Controller,
    uint32_t StreamNumber
);

static void DMA_ClearNormalizedFlags(
    DMA_TypeDef *Controller,
    uint32_t StreamNumber,
    uint32_t NormalizedFlags
);

static DMA_StatusTypeDef DMA_DisableStream(
    DMA_HandleTypeDef *Handle
);

static void DMA_ConfigureDMAMUX(
    DMA_HandleTypeDef *Handle
);

static uint32_t DMA_GetControllerIndex(
    DMA_TypeDef *Controller
);

static IRQn_Type DMA_GetIRQNumber(
    DMA_TypeDef *Controller,
    uint32_t StreamNumber)
{
    if(Controller == DMA1)
    {
        switch(StreamNumber)
        {
            case 0U: return DMA1_Stream0_IRQn;
            case 1U: return DMA1_Stream1_IRQn;
            case 2U: return DMA1_Stream2_IRQn;
            case 3U: return DMA1_Stream3_IRQn;
            case 4U: return DMA1_Stream4_IRQn;
            case 5U: return DMA1_Stream5_IRQn;
            case 6U: return DMA1_Stream6_IRQn;
            case 7U: return DMA1_Stream7_IRQn;
            default: break;
        }
    }
    else if(Controller == DMA2)
    {
        switch(StreamNumber)
        {
            case 0U: return DMA2_Stream0_IRQn;
            case 1U: return DMA2_Stream1_IRQn;
            case 2U: return DMA2_Stream2_IRQn;
            case 3U: return DMA2_Stream3_IRQn;
            case 4U: return DMA2_Stream4_IRQn;
            case 5U: return DMA2_Stream5_IRQn;
            case 6U: return DMA2_Stream6_IRQn;
            case 7U: return DMA2_Stream7_IRQn;
            default: break;
        }
    }

    return NonMaskableInt_IRQn;
}

DMA_StatusTypeDef DMA_Init(
    DMA_HandleTypeDef *Handle)
{
    DMA_TypeDef *controller;
    DMA_HandleTypeDef *registered_handle;
    IRQn_Type irq_number;
    uint32_t controller_index;
    uint32_t stream_number;
    uint32_t control;
    uint32_t fifo_control;
    uint8_t interrupt_enabled;

    if(DMA_IsHandleValid(Handle) == 0U)
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    controller = DMA_GetController(Handle);
    stream_number = DMA_GetStreamNumber(Handle);
    controller_index = DMA_GetControllerIndex(controller);

    if((controller == NULL) ||
       (stream_number >= DMA_STREAM_COUNT) ||
       (controller_index >= DMA_CONTROLLER_COUNT))
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    registered_handle =
        DMA_HandleRegistry[controller_index][stream_number];

    if((registered_handle != NULL) &&
       (registered_handle != Handle))
    {
        return DMA_STATUS_BUSY;
    }

    irq_number = DMA_GetIRQNumber(
        controller,
        stream_number);

    if(irq_number == NonMaskableInt_IRQn)
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    /*
     * Determine whether this stream requires an NVIC interrupt.
     */
    interrupt_enabled = 0U;

    if((Handle->Config.EnableTransferCompleteInterrupt != 0U) ||
       (Handle->Config.EnableHalfTransferInterrupt != 0U) ||
       (Handle->Config.EnableTransferErrorInterrupt != 0U) ||
       (Handle->Config.EnableDirectModeErrorInterrupt != 0U) ||
       (Handle->Config.EnableFIFOErrorInterrupt != 0U))
    {
        interrupt_enabled = 1U;
    }

    /*
     * DMA1 and DMA2 are both on AHB1.
     */
    if(controller == DMA1)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    }
    else
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    }

#if defined(RCC_AHB1ENR_DMAMUX1EN)
    RCC->AHB1ENR |= RCC_AHB1ENR_DMAMUX1EN;
#endif

    /*
     * Ensure the peripheral clock writes have completed.
     */
    (void)RCC->AHB1ENR;

    if(DMA_DisableStream(Handle) != DMA_STATUS_OK)
    {
        return DMA_STATUS_TIMEOUT;
    }

    /*
     * Disable the NVIC interrupt while the stream is configured.
     */
    NVIC_DisableIRQ(irq_number);
    NVIC_ClearPendingIRQ(irq_number);

    control = 0U;

    control |= DMA_GetDirectionBits(
        Handle->Config.Direction);

    control |= DMA_GetPeripheralWidthBits(
        Handle->Config.PeripheralDataWidth);

    control |= DMA_GetMemoryWidthBits(
        Handle->Config.MemoryDataWidth);

    control |= DMA_GetPriorityBits(
        Handle->Config.Priority);

    control |= DMA_GetPeripheralBurstBits(
        Handle->Config.PeripheralBurst);

    control |= DMA_GetMemoryBurstBits(
        Handle->Config.MemoryBurst);

    if(Handle->Config.PeripheralIncrement != 0U)
    {
        control |= DMA_SxCR_PINC;
    }

    if(Handle->Config.MemoryIncrement != 0U)
    {
        control |= DMA_SxCR_MINC;
    }

    if(Handle->Config.Mode == DMA_MODE_CIRCULAR)
    {
        control |= DMA_SxCR_CIRC;
    }

    if(Handle->Config.EnableTransferCompleteInterrupt != 0U)
    {
        control |= DMA_SxCR_TCIE;
    }

    if(Handle->Config.EnableHalfTransferInterrupt != 0U)
    {
        control |= DMA_SxCR_HTIE;
    }

    if(Handle->Config.EnableTransferErrorInterrupt != 0U)
    {
        control |= DMA_SxCR_TEIE;
    }

    if(Handle->Config.EnableDirectModeErrorInterrupt != 0U)
    {
        control |= DMA_SxCR_DMEIE;
    }

    fifo_control = 0U;

    if(Handle->Config.FIFOMode == DMA_FIFO_ENABLED)
    {
        fifo_control |= DMA_SxFCR_DMDIS;

        fifo_control |= DMA_GetFIFOThresholdBits(
            Handle->Config.FIFOThreshold);
    }

    if(Handle->Config.EnableFIFOErrorInterrupt != 0U)
    {
        fifo_control |= DMA_SxFCR_FEIE;
    }

    Handle->Instance->CR = control;
    Handle->Instance->FCR = fifo_control;

    DMA_ConfigureDMAMUX(Handle);

    /*
     * Clear stale DMA flags before enabling the NVIC.
     */
    (void)DMA_ClearFlags(Handle);

    Handle->Busy = 0U;
    Handle->LastEvent = DMA_EVENT_NONE;

    /*
     * Register the handle before enabling the NVIC so that an interrupt
     * cannot occur before DMA_IRQHandler() can locate the handle.
     */
    DMA_HandleRegistry[controller_index][stream_number] = Handle;

    if(interrupt_enabled != 0U)
    {
        NVIC_SetPriority(
            irq_number,
            Handle->InterruptPriority);

        NVIC_ClearPendingIRQ(irq_number);
        NVIC_EnableIRQ(irq_number);
    }

    return DMA_STATUS_OK;
}

DMA_StatusTypeDef DMA_Start(
    DMA_HandleTypeDef *Handle,
    uintptr_t PeripheralAddress,
    uintptr_t MemoryAddress,
    uint32_t TransferCount)
{
    if(DMA_IsHandleValid(Handle) == 0U)
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    if((PeripheralAddress == (uintptr_t)0U) ||
       (MemoryAddress == (uintptr_t)0U) ||
       (TransferCount == 0U) ||
       (TransferCount > DMA_MAX_TRANSFER_COUNT))
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    if((Handle->Instance->CR & DMA_SxCR_EN) != 0U)
    {
        return DMA_STATUS_BUSY;
    }

    (void)DMA_ClearFlags(Handle);

    Handle->Instance->PAR = (uint32_t)PeripheralAddress;
    Handle->Instance->M0AR = (uint32_t)MemoryAddress;
    Handle->Instance->NDTR = TransferCount;

    Handle->LastEvent = DMA_EVENT_NONE;
    Handle->Busy = 1U;

    /*
     * Ensure all address and count writes complete before enabling DMA.
     */
    __DSB();

    Handle->Instance->CR |= DMA_SxCR_EN;

    return DMA_STATUS_OK;
}

DMA_StatusTypeDef DMA_Stop(
    DMA_HandleTypeDef *Handle)
{
    DMA_StatusTypeDef status;

    if(DMA_IsHandleValid(Handle) == 0U)
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    status = DMA_DisableStream(Handle);

    if(status != DMA_STATUS_OK)
    {
        return status;
    }

    (void)DMA_ClearFlags(Handle);

    Handle->Busy = 0U;
    Handle->LastEvent = DMA_EVENT_NONE;

    return DMA_STATUS_OK;
}

uint8_t DMA_IsBusy(
    const DMA_HandleTypeDef *Handle)
{
    if(DMA_IsHandleValid(Handle) == 0U)
    {
        return 0U;
    }

    return
        ((Handle->Instance->CR & DMA_SxCR_EN) != 0U)
        ? 1U
        : 0U;
}

uint32_t DMA_GetRemainingTransferCount(
    const DMA_HandleTypeDef *Handle)
{
    if(DMA_IsHandleValid(Handle) == 0U)
    {
        return 0U;
    }

    return Handle->Instance->NDTR;
}

DMA_StatusTypeDef DMA_SetMemoryAddress(
    DMA_HandleTypeDef *Handle,
    uintptr_t MemoryAddress)
{
    if(DMA_IsHandleValid(Handle) == 0U)
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    if(MemoryAddress == (uintptr_t)0U)
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    if((Handle->Instance->CR & DMA_SxCR_EN) != 0U)
    {
        return DMA_STATUS_BUSY;
    }

    Handle->Instance->M0AR = (uint32_t)MemoryAddress;

    return DMA_STATUS_OK;
}

DMA_StatusTypeDef DMA_SetPeripheralAddress(
    DMA_HandleTypeDef *Handle,
    uintptr_t PeripheralAddress)
{
    if(DMA_IsHandleValid(Handle) == 0U)
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    if(PeripheralAddress == (uintptr_t)0U)
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    if((Handle->Instance->CR & DMA_SxCR_EN) != 0U)
    {
        return DMA_STATUS_BUSY;
    }

    Handle->Instance->PAR = (uint32_t)PeripheralAddress;

    return DMA_STATUS_OK;
}

DMA_StatusTypeDef DMA_SetTransferCount(
    DMA_HandleTypeDef *Handle,
    uint32_t TransferCount)
{
    if(DMA_IsHandleValid(Handle) == 0U)
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    if((TransferCount == 0U) ||
       (TransferCount > DMA_MAX_TRANSFER_COUNT))
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    if((Handle->Instance->CR & DMA_SxCR_EN) != 0U)
    {
        return DMA_STATUS_BUSY;
    }

    Handle->Instance->NDTR = TransferCount;

    return DMA_STATUS_OK;
}

void DMA_IRQHandler(
    DMA_TypeDef *Controller,
    uint32_t StreamNumber)
{
    DMA_HandleTypeDef *Handle;
    uint32_t controller_index;
    uint32_t flags;
    DMA_EventTypeDef event;

    controller_index = DMA_GetControllerIndex(Controller);

    if((controller_index >= DMA_CONTROLLER_COUNT) ||
       (StreamNumber >= DMA_STREAM_COUNT))
    {
        return;
    }

    Handle = DMA_HandleRegistry[controller_index][StreamNumber];

    if(Handle == NULL)
    {
        /*
         * Clear any pending flags even when no handle is registered,
         * otherwise the interrupt can continuously retrigger.
         */
        flags = DMA_ReadNormalizedFlags(Controller, StreamNumber);

        if(flags != 0U)
        {
            DMA_ClearNormalizedFlags(
                Controller,
                StreamNumber,
                flags);
        }

        return;
    }

    flags = DMA_ReadNormalizedFlags(Controller, StreamNumber);
    event = DMA_EVENT_NONE;

    if((flags & DMA_NORMALIZED_FEIF) != 0U)
    {
        DMA_ClearNormalizedFlags(
            Controller,
            StreamNumber,
            DMA_NORMALIZED_FEIF);

        event = (DMA_EventTypeDef)(
            (uint32_t)event |
            (uint32_t)DMA_EVENT_FIFO_ERROR);
    }

    if((flags & DMA_NORMALIZED_DMEIF) != 0U)
    {
        DMA_ClearNormalizedFlags(
            Controller,
            StreamNumber,
            DMA_NORMALIZED_DMEIF);

        event = (DMA_EventTypeDef)(
            (uint32_t)event |
            (uint32_t)DMA_EVENT_DIRECT_MODE_ERROR);
    }

    if((flags & DMA_NORMALIZED_TEIF) != 0U)
    {
        DMA_ClearNormalizedFlags(
            Controller,
            StreamNumber,
            DMA_NORMALIZED_TEIF);

        event = (DMA_EventTypeDef)(
            (uint32_t)event |
            (uint32_t)DMA_EVENT_TRANSFER_ERROR);
    }

    if((flags & DMA_NORMALIZED_HTIF) != 0U)
    {
        DMA_ClearNormalizedFlags(
            Controller,
            StreamNumber,
            DMA_NORMALIZED_HTIF);

        event = (DMA_EventTypeDef)(
            (uint32_t)event |
            (uint32_t)DMA_EVENT_HALF_TRANSFER);
    }

    if((flags & DMA_NORMALIZED_TCIF) != 0U)
    {
        DMA_ClearNormalizedFlags(
            Controller,
            StreamNumber,
            DMA_NORMALIZED_TCIF);

        event = (DMA_EventTypeDef)(
            (uint32_t)event |
            (uint32_t)DMA_EVENT_TRANSFER_COMPLETE);

        if(Handle->Config.Mode == DMA_MODE_NORMAL)
        {
            Handle->Busy = 0U;
        }
    }

    if((event & (DMA_EVENT_FIFO_ERROR |
                 DMA_EVENT_DIRECT_MODE_ERROR |
                 DMA_EVENT_TRANSFER_ERROR)) != 0U)
    {
        Handle->Busy = 0U;
    }

    Handle->LastEvent = event;

    if((event != DMA_EVENT_NONE) &&
       (Handle->Callback != NULL))
    {
        Handle->Callback(Handle, event);
    }
}

DMA_StatusTypeDef DMA_ClearFlags(
    DMA_HandleTypeDef *Handle)
{
    DMA_TypeDef *controller;
    uint32_t stream_number;

    if(DMA_IsHandleValid(Handle) == 0U)
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    controller = DMA_GetController(Handle);
    stream_number = DMA_GetStreamNumber(Handle);

    if((controller == NULL) || (stream_number > 7U))
    {
        return DMA_STATUS_INVALID_PARAMETER;
    }

    DMA_ClearNormalizedFlags(
        controller,
        stream_number,
        DMA_NORMALIZED_FEIF |
        DMA_NORMALIZED_DMEIF |
        DMA_NORMALIZED_TEIF |
        DMA_NORMALIZED_HTIF |
        DMA_NORMALIZED_TCIF);

    return DMA_STATUS_OK;
}

uint32_t DMA_GetStreamNumber(
    const DMA_HandleTypeDef *Handle)
{
    DMA_Stream_TypeDef *stream;

    if((Handle == NULL) || (Handle->Instance == NULL))
    {
        return 0xFFFFFFFFUL;
    }

    stream = Handle->Instance;

    if((stream == DMA1_Stream0) || (stream == DMA2_Stream0))
    {
        return 0U;
    }

    if((stream == DMA1_Stream1) || (stream == DMA2_Stream1))
    {
        return 1U;
    }

    if((stream == DMA1_Stream2) || (stream == DMA2_Stream2))
    {
        return 2U;
    }

    if((stream == DMA1_Stream3) || (stream == DMA2_Stream3))
    {
        return 3U;
    }

    if((stream == DMA1_Stream4) || (stream == DMA2_Stream4))
    {
        return 4U;
    }

    if((stream == DMA1_Stream5) || (stream == DMA2_Stream5))
    {
        return 5U;
    }

    if((stream == DMA1_Stream6) || (stream == DMA2_Stream6))
    {
        return 6U;
    }

    if((stream == DMA1_Stream7) || (stream == DMA2_Stream7))
    {
        return 7U;
    }

    return 0xFFFFFFFFUL;
}

DMA_TypeDef *DMA_GetController(
    const DMA_HandleTypeDef *Handle)
{
    DMA_Stream_TypeDef *stream;

    if((Handle == NULL) || (Handle->Instance == NULL))
    {
        return NULL;
    }

    stream = Handle->Instance;

    if((stream == DMA1_Stream0) ||
       (stream == DMA1_Stream1) ||
       (stream == DMA1_Stream2) ||
       (stream == DMA1_Stream3) ||
       (stream == DMA1_Stream4) ||
       (stream == DMA1_Stream5) ||
       (stream == DMA1_Stream6) ||
       (stream == DMA1_Stream7))
    {
        return DMA1;
    }

    if((stream == DMA2_Stream0) ||
       (stream == DMA2_Stream1) ||
       (stream == DMA2_Stream2) ||
       (stream == DMA2_Stream3) ||
       (stream == DMA2_Stream4) ||
       (stream == DMA2_Stream5) ||
       (stream == DMA2_Stream6) ||
       (stream == DMA2_Stream7))
    {
        return DMA2;
    }

    return NULL;
}

static uint8_t DMA_IsHandleValid(
    const DMA_HandleTypeDef *Handle)
{
    if((Handle == NULL) || (Handle->Instance == NULL))
    {
        return 0U;
    }

    if(DMA_GetController(Handle) == NULL)
    {
        return 0U;
    }

    if(DMA_GetStreamNumber(Handle) > 7U)
    {
        return 0U;
    }

    if(DMA_IsConfigValid(&Handle->Config) == 0U)
    {
        return 0U;
    }

    return 1U;
}

static uint8_t DMA_IsConfigValid(
    const DMA_ConfigTypeDef *Config)
{
    if(Config == NULL)
    {
        return 0U;
    }

    if(Config->Request > DMAMUX_CxCR_DMAREQ_ID)
    {
        return 0U;
    }

    if(Config->Direction > DMA_DIRECTION_MEMORY_TO_MEMORY)
    {
        return 0U;
    }

    if(Config->PeripheralDataWidth > DMA_DATA_WIDTH_WORD)
    {
        return 0U;
    }

    if(Config->MemoryDataWidth > DMA_DATA_WIDTH_WORD)
    {
        return 0U;
    }

    if(Config->Mode > DMA_MODE_CIRCULAR)
    {
        return 0U;
    }

    if(Config->Priority > DMA_PRIORITY_VERY_HIGH)
    {
        return 0U;
    }

    if(Config->PeripheralBurst > DMA_BURST_INCR16)
    {
        return 0U;
    }

    if(Config->MemoryBurst > DMA_BURST_INCR16)
    {
        return 0U;
    }

    if(Config->FIFOMode > DMA_FIFO_ENABLED)
    {
        return 0U;
    }

    if(Config->FIFOThreshold > DMA_FIFO_THRESHOLD_FULL)
    {
        return 0U;
    }

    /*
     * Circular and memory-to-memory modes cannot be combined.
     */
    if((Config->Direction == DMA_DIRECTION_MEMORY_TO_MEMORY) &&
       (Config->Mode == DMA_MODE_CIRCULAR))
    {
        return 0U;
    }

    /*
     * Bursts require FIFO mode.
     */
    if((Config->FIFOMode == DMA_FIFO_DISABLED) &&
       ((Config->PeripheralBurst != DMA_BURST_SINGLE) ||
        (Config->MemoryBurst != DMA_BURST_SINGLE)))
    {
        return 0U;
    }

    return 1U;
}

static uint32_t DMA_GetDirectionBits(
    DMA_DirectionTypeDef Direction)
{
    switch(Direction)
    {
        case DMA_DIRECTION_MEMORY_TO_PERIPHERAL:
            return DMA_SxCR_DIR_0;

        case DMA_DIRECTION_MEMORY_TO_MEMORY:
            return DMA_SxCR_DIR_1;

        case DMA_DIRECTION_PERIPHERAL_TO_MEMORY:
        default:
            return 0U;
    }
}

static uint32_t DMA_GetPeripheralWidthBits(
    DMA_DataWidthTypeDef Width)
{
    switch(Width)
    {
        case DMA_DATA_WIDTH_HALFWORD:
            return DMA_SxCR_PSIZE_0;

        case DMA_DATA_WIDTH_WORD:
            return DMA_SxCR_PSIZE_1;

        case DMA_DATA_WIDTH_BYTE:
        default:
            return 0U;
    }
}

static uint32_t DMA_GetMemoryWidthBits(
    DMA_DataWidthTypeDef Width)
{
    switch(Width)
    {
        case DMA_DATA_WIDTH_HALFWORD:
            return DMA_SxCR_MSIZE_0;

        case DMA_DATA_WIDTH_WORD:
            return DMA_SxCR_MSIZE_1;

        case DMA_DATA_WIDTH_BYTE:
        default:
            return 0U;
    }
}

static uint32_t DMA_GetPriorityBits(
    DMA_PriorityTypeDef Priority)
{
    switch(Priority)
    {
        case DMA_PRIORITY_MEDIUM:
            return DMA_SxCR_PL_0;

        case DMA_PRIORITY_HIGH:
            return DMA_SxCR_PL_1;

        case DMA_PRIORITY_VERY_HIGH:
            return DMA_SxCR_PL;

        case DMA_PRIORITY_LOW:
        default:
            return 0U;
    }
}

static uint32_t DMA_GetPeripheralBurstBits(
    DMA_BurstTypeDef Burst)
{
    switch(Burst)
    {
        case DMA_BURST_INCR4:
            return DMA_SxCR_PBURST_0;

        case DMA_BURST_INCR8:
            return DMA_SxCR_PBURST_1;

        case DMA_BURST_INCR16:
            return DMA_SxCR_PBURST;

        case DMA_BURST_SINGLE:
        default:
            return 0U;
    }
}

static uint32_t DMA_GetMemoryBurstBits(
    DMA_BurstTypeDef Burst)
{
    switch(Burst)
    {
        case DMA_BURST_INCR4:
            return DMA_SxCR_MBURST_0;

        case DMA_BURST_INCR8:
            return DMA_SxCR_MBURST_1;

        case DMA_BURST_INCR16:
            return DMA_SxCR_MBURST;

        case DMA_BURST_SINGLE:
        default:
            return 0U;
    }
}

static uint32_t DMA_GetFIFOThresholdBits(
    DMA_FIFOThresholdTypeDef Threshold)
{
    switch(Threshold)
    {
        case DMA_FIFO_THRESHOLD_1_HALF:
            return DMA_SxFCR_FTH_0;

        case DMA_FIFO_THRESHOLD_3_QUARTERS:
            return DMA_SxFCR_FTH_1;

        case DMA_FIFO_THRESHOLD_FULL:
            return DMA_SxFCR_FTH;

        case DMA_FIFO_THRESHOLD_1_QUARTER:
        default:
            return 0U;
    }
}

static uint32_t DMA_GetFlagShift(
    uint32_t StreamNumber)
{
    static const uint8_t FlagShift[4] =
    {
        0U,
        6U,
        16U,
        22U
    };

    return FlagShift[StreamNumber & 0x3U];
}

static uint32_t DMA_ReadNormalizedFlags(
    DMA_TypeDef *Controller,
    uint32_t StreamNumber)
{
    uint32_t shift;
    uint32_t status;

    shift = DMA_GetFlagShift(StreamNumber);

    if(StreamNumber < 4U)
    {
        status = Controller->LISR;
    }
    else
    {
        status = Controller->HISR;
    }

    return
        (status >> shift) &
        (DMA_NORMALIZED_FEIF |
         DMA_NORMALIZED_DMEIF |
         DMA_NORMALIZED_TEIF |
         DMA_NORMALIZED_HTIF |
         DMA_NORMALIZED_TCIF);
}

static void DMA_ClearNormalizedFlags(
    DMA_TypeDef *Controller,
    uint32_t StreamNumber,
    uint32_t NormalizedFlags)
{
    uint32_t shift;
    uint32_t clear_mask;

    shift = DMA_GetFlagShift(StreamNumber);
    clear_mask = NormalizedFlags << shift;

    if(StreamNumber < 4U)
    {
        Controller->LIFCR = clear_mask;
    }
    else
    {
        Controller->HIFCR = clear_mask;
    }
}

static DMA_StatusTypeDef DMA_DisableStream(
    DMA_HandleTypeDef *Handle)
{
    uint32_t timeout;

    Handle->Instance->CR &= ~DMA_SxCR_EN;

    timeout = DMA_DISABLE_TIMEOUT;

    while((Handle->Instance->CR & DMA_SxCR_EN) != 0U)
    {
        if(timeout == 0U)
        {
            return DMA_STATUS_TIMEOUT;
        }

        timeout--;
    }

    return DMA_STATUS_OK;
}

static void DMA_ConfigureDMAMUX(
    DMA_HandleTypeDef *Handle)
{
    DMA_TypeDef *controller;
    uint32_t stream_number;
    uint32_t dmamux_channel;
    DMAMUX_Channel_TypeDef *channel_base;

    controller = DMA_GetController(Handle);
    stream_number = DMA_GetStreamNumber(Handle);

    /*
     * DMAMUX1 channels 0..7 feed DMA1 streams 0..7.
     * DMAMUX1 channels 8..15 feed DMA2 streams 0..7.
     */
    if(controller == DMA1)
    {
        dmamux_channel = stream_number;
    }
    else
    {
        dmamux_channel = stream_number + 8U;
    }

    channel_base = (DMAMUX_Channel_TypeDef *)DMAMUX1_Channel0;

    /*
     * No synchronization or event generation is enabled here.
     * Only the peripheral request ID is selected.
     */
    channel_base[dmamux_channel].CCR =
        Handle->Config.Request & DMAMUX_CxCR_DMAREQ_ID;
}

static uint32_t DMA_GetControllerIndex(
    DMA_TypeDef *Controller)
{
    if(Controller == DMA1)
    {
        return 0U;
    }

    if(Controller == DMA2)
    {
        return 1U;
    }

    return DMA_CONTROLLER_COUNT;
}
