/*
 * Board_AudioTest.h
 *
 * Dual-buffer audio test helpers.
 */

#ifndef INC_BOARD_AUDIOTEST_H_
#define INC_BOARD_AUDIOTEST_H_

#include <stdint.h>
#include "Audio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Total buffer size must be even because DMA divides it into two halves.
 *
 * At 48 kHz, 2048 samples gives approximately 21.3 ms per half.
 */
#define BOARD_AUDIO_TEST_BUFFER_SAMPLES       2048U
#define BOARD_AUDIO_TEST_HALF_SAMPLES         \
    (BOARD_AUDIO_TEST_BUFFER_SAMPLES / 2U)

/*
 * Starts continuous double-buffered test audio.
 */
Audio_StatusTypeDef Board_AudioTestStart(
    Audio_HandleTypeDef *AudioHandle
);

/*
 * Stops the test and returns the DAC to its midpoint.
 */
Audio_StatusTypeDef Board_AudioTestStop(void);

/*
 * Call repeatedly from the OS/superloop.
 *
 * This refills any buffer half released by the DMA interrupt callbacks.
 */
void Board_AudioTestUpdate(void);

/*
 * Returns nonzero if the OS failed to refill a half before DMA needed it
 * again. This indicates that Board_AudioTestUpdate() is being serviced
 * too slowly.
 */
uint8_t Board_AudioTestHasOverrun(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_BOARD_AUDIOTEST_H_ */
