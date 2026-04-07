/*
 * Profiles.c
 *
 *  Created on: Jan 5, 2026
 *      Author: Jack Herron
 */

#include <stdint.h>
#include <math.h>
#include "App.h"
#include "Profiles.h"
#include "cmsis_os.h"

#define PROFILE_LENGTH          300U
#define INTERPOLATION_STEPS     36U
#define PROFILE_STEP_MS         5U

typedef struct
{
	uint16_t white;
	uint16_t red;
	uint16_t far_red;
	uint16_t purple;
	uint16_t lime;
} Lighting_Profile_Point;

static Lighting_Profile_Point profile[PROFILE_LENGTH];

static float Profiles___Clamp(float value, float min, float max)
{
	if (value < min)
	{
		return min;
	}
	if (value > max)
	{
		return max;
	}
	return value;
}

static uint16_t Profiles___To_Level(float value)
{
	value = Profiles___Clamp(value, 0.0f, 100.0f);
	return (uint16_t)((value / 100.0f) * 65535.0f + 0.5f);
}

static uint16_t Profiles___Interpolate_U16(uint16_t a, uint16_t b, uint16_t step, uint16_t max_steps)
{
	int32_t diff  = (int32_t)b - (int32_t)a;
	int32_t value = (int32_t)a + ((diff * (int32_t)step) / (int32_t)(max_steps + 1U));

	if (value < 0)
	{
		value = 0;
	}
	if (value > 65535)
	{
		value = 65535;
	}

	return (uint16_t)value;
}

/*
 * Smooth half-sine shape from 0 to 1 to 0
 * x should be in range 0.0 to 1.0
 */
static float Profiles___Bell_Curve(float x)
{
	float value;

	x = Profiles___Clamp(x, 0.0f, 1.0f);
	value = sinf(x * 3.14159265f);

	if (value < 0.0f)
	{
		value = 0.0f;
	}

	return value;
}

void Profiles___Init_Demo_Profile(void)
{
	for (uint16_t i = 0; i < PROFILE_LENGTH; i++)
	{
		float phase = (float)i / (float)(PROFILE_LENGTH - 1U);

		float red      = 0.0f;
		float far_red  = 0.0f;
		float purple   = 0.0f;
		float lime     = 0.0f;
		float white    = 0.0f;

		/*
		 * Extended night blanking:
		 * active day only between 10% and 90%
		 */
		if ((phase >= 0.10f) && (phase <= 0.90f))
		{
			float x = (phase - 0.10f) / 0.80f;   // 0 to 1 across active day

			/*
			 * Five pure bell curves.
			 * Each channel uses one fixed-width bell only.
			 * No piecewise regions, no changing scale factors over time.
			 */

			float red_curve      = powf(Profiles___Bell_Curve(x), 0.65f);  // widest
			float far_red_curve  = powf(Profiles___Bell_Curve(x), 0.80f);
			float white_curve    = powf(Profiles___Bell_Curve(x), 1.10f);
			float lime_curve     = powf(Profiles___Bell_Curve(x), 1.45f);
			float purple_curve   = powf(Profiles___Bell_Curve(x), 1.65f);  // narrowest

			red     = 90.0f * red_curve;
			far_red = 38.0f * far_red_curve;
			white   = 42.0f * white_curve;
			lime    = 95.0f * lime_curve;
			purple  = 95.0f * purple_curve;
		}

		profile[i].white   = Profiles___To_Level(white);
		profile[i].red     = Profiles___To_Level(red);
		profile[i].far_red = Profiles___To_Level(far_red);
		profile[i].purple  = Profiles___To_Level(purple);
		profile[i].lime    = Profiles___To_Level(lime);
	}
}


void Profiles___Start_Task(void const * argument)
{
	uint16_t profile_Index = 0;
	uint16_t interp_Step   = 0;

	Lighting_Profile_Point current_Point;
	Lighting_Profile_Point next_Point;
	Lighting_Profile_Point output_Point;

	Profiles___Init_Demo_Profile();

	osSignalWait(APP___PROFILES_TASK_START_FLAG, osWaitForever);

	for (;;)
	{
		osMutexWait(App___Profiles_State_Mutex, osWaitForever);

		if (App___Profiles_State.lighting_Mode == 0)
		{
			current_Point = profile[profile_Index];

			if ((profile_Index + 1U) >= PROFILE_LENGTH)
			{
				next_Point = profile[0];
			}
			else
			{
				next_Point = profile[profile_Index + 1U];
			}

			output_Point.white =
				Profiles___Interpolate_U16(current_Point.white, next_Point.white, interp_Step, INTERPOLATION_STEPS);

			output_Point.red =
				Profiles___Interpolate_U16(current_Point.red, next_Point.red, interp_Step, INTERPOLATION_STEPS);

			output_Point.far_red =
				Profiles___Interpolate_U16(current_Point.far_red, next_Point.far_red, interp_Step, INTERPOLATION_STEPS);

			output_Point.purple =
				Profiles___Interpolate_U16(current_Point.purple, next_Point.purple, interp_Step, INTERPOLATION_STEPS);

			output_Point.lime =
				Profiles___Interpolate_U16(current_Point.lime, next_Point.lime, interp_Step, INTERPOLATION_STEPS);

			osMutexWait(App___IO_Control_State_Mutex, osWaitForever);

			App___IO_Control_State.lighting.white   = output_Point.white;
			App___IO_Control_State.lighting.red     = output_Point.red;
			App___IO_Control_State.lighting.far_Red = output_Point.far_red;
			App___IO_Control_State.lighting.purple  = output_Point.purple;
			App___IO_Control_State.lighting.lime    = output_Point.lime;

			osMutexRelease(App___IO_Control_State_Mutex);

			interp_Step++;

			if (interp_Step > INTERPOLATION_STEPS)
			{
				interp_Step = 0;
				profile_Index++;

				if (profile_Index >= PROFILE_LENGTH)
				{
					profile_Index = 0;
				}
			}
		}
		else
		{
			osMutexWait(App___IO_Control_State_Mutex, osWaitForever);

			App___IO_Control_State.lighting.white   = 65535;
			App___IO_Control_State.lighting.red     = 0;
			App___IO_Control_State.lighting.far_Red = 0;
			App___IO_Control_State.lighting.purple  = 0;
			App___IO_Control_State.lighting.lime    = 0;

			osMutexRelease(App___IO_Control_State_Mutex);

			profile_Index = 0;
			interp_Step   = 0;
		}

		osMutexRelease(App___Profiles_State_Mutex);

		osDelay(PROFILE_STEP_MS);
	}
}
