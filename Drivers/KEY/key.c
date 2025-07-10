
#include "key.h"

// Make sure you have access to a global millisecond timer.
// If g_tick_ms is defined in main.c, you need this line in key.c

/**
 * @brief  Scans the 4x4 matrix keyboard and returns a single, debounced key
 * press event.
 * @return 1-16: A valid key press event for S1-S16 was detected.
 * @return 0   : No valid key press event.
 * @note   This function should be called repeatedly in your main loop.
 */
uint8_t Matrix_Key_Scan(void)
{

    uint8_t current_raw_key = 0; // The key currently being physically pressed

    // ===================================================================
    // Part 1: Corrected Raw Key Scanning Logic
    // ===================================================================

    // --- Scan Row 1 ---
    DL_GPIO_clearPins(GPIO_KEY_PIN_KEY_ROW1_PORT, GPIO_KEY_PIN_KEY_ROW1_PIN);
    DL_GPIO_setPins(GPIO_KEY_PIN_KEY_ROW2_PORT, GPIO_KEY_PIN_KEY_ROW2_PIN);
    DL_GPIO_setPins(GPIO_KEY_PIN_KEY_ROW3_PORT, GPIO_KEY_PIN_KEY_ROW3_PIN);
    DL_GPIO_setPins(GPIO_KEY_PIN_KEY_ROW4_PORT, GPIO_KEY_PIN_KEY_ROW4_PIN);
    if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL1_PORT, GPIO_KEY_PIN_KEY_COL1_PIN) ==
        0)
    {
        current_raw_key = 1;
    }
    else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL2_PORT,
                              GPIO_KEY_PIN_KEY_COL2_PIN) == 0)
    {
        current_raw_key = 2;
    }
    else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL3_PORT,
                              GPIO_KEY_PIN_KEY_COL3_PIN) == 0)
    {
        current_raw_key = 3;
    }
    else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL4_PORT,
                              GPIO_KEY_PIN_KEY_COL4_PIN) == 0)
    {
        current_raw_key = 4;
    }

    // --- Scan Row 2 (only if no key was found in row 1) ---
    if (current_raw_key == 0)
    {
        DL_GPIO_setPins(GPIO_KEY_PIN_KEY_ROW1_PORT, GPIO_KEY_PIN_KEY_ROW1_PIN);
        DL_GPIO_clearPins(GPIO_KEY_PIN_KEY_ROW2_PORT, GPIO_KEY_PIN_KEY_ROW2_PIN);
        if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL1_PORT,
                             GPIO_KEY_PIN_KEY_COL1_PIN) == 0)
        {
            current_raw_key = 5;
        }
        else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL2_PORT,
                                  GPIO_KEY_PIN_KEY_COL2_PIN) == 0)
        {
            current_raw_key = 6;
        }
        else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL3_PORT,
                                  GPIO_KEY_PIN_KEY_COL3_PIN) == 0)
        {
            current_raw_key = 7;
        }
        else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL4_PORT,
                                  GPIO_KEY_PIN_KEY_COL4_PIN) == 0)
        {
            current_raw_key = 8;
        }
    }

    // --- Scan Row 3 (only if no key was found yet) ---
    if (current_raw_key == 0)
    {
        DL_GPIO_setPins(GPIO_KEY_PIN_KEY_ROW2_PORT, GPIO_KEY_PIN_KEY_ROW2_PIN);
        DL_GPIO_clearPins(GPIO_KEY_PIN_KEY_ROW3_PORT, GPIO_KEY_PIN_KEY_ROW3_PIN);
        if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL1_PORT,
                             GPIO_KEY_PIN_KEY_COL1_PIN) == 0)
        {
            current_raw_key = 9;
        }
        else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL2_PORT,
                                  GPIO_KEY_PIN_KEY_COL2_PIN) == 0)
        {
            current_raw_key = 10;
        }
        else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL3_PORT,
                                  GPIO_KEY_PIN_KEY_COL3_PIN) == 0)
        {
            current_raw_key = 11;
        }
        else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL4_PORT,
                                  GPIO_KEY_PIN_KEY_COL4_PIN) == 0)
        {
            current_raw_key = 12;
        }
    }

    // --- Scan Row 4 (only if no key was found yet) ---
    if (current_raw_key == 0)
    {
        DL_GPIO_setPins(GPIO_KEY_PIN_KEY_ROW3_PORT, GPIO_KEY_PIN_KEY_ROW3_PIN);
        DL_GPIO_clearPins(GPIO_KEY_PIN_KEY_ROW4_PORT, GPIO_KEY_PIN_KEY_ROW4_PIN);
        if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL1_PORT,
                             GPIO_KEY_PIN_KEY_COL1_PIN) == 0)
        {
            current_raw_key = 13;
        }
        else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL2_PORT,
                                  GPIO_KEY_PIN_KEY_COL2_PIN) == 0)
        {
            current_raw_key = 14;
        }
        else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL3_PORT,
                                  GPIO_KEY_PIN_KEY_COL3_PIN) == 0)
        {
            current_raw_key = 15;
        }
        else if (DL_GPIO_readPins(GPIO_KEY_PIN_KEY_COL4_PORT,
                                  GPIO_KEY_PIN_KEY_COL4_PIN) == 0)
        {
            current_raw_key = 16;
        }
    }

    // After scanning, set all rows high to avoid conflicts
    DL_GPIO_setPins(GPIO_KEY_PIN_KEY_ROW1_PORT, GPIO_KEY_PIN_KEY_ROW1_PIN);
    DL_GPIO_setPins(GPIO_KEY_PIN_KEY_ROW2_PORT, GPIO_KEY_PIN_KEY_ROW2_PIN);
    DL_GPIO_setPins(GPIO_KEY_PIN_KEY_ROW3_PORT, GPIO_KEY_PIN_KEY_ROW3_PIN);
    DL_GPIO_setPins(GPIO_KEY_PIN_KEY_ROW4_PORT, GPIO_KEY_PIN_KEY_ROW4_PIN);

    // ===================================================================
    // Part 2: Non-Blocking Debounce and Edge-Detection Logic (Uncommented)
    // ===================================================================

    return current_raw_key;
}

extern uint8_t key_mode;

void Key_PID_MDF()
{
    static uint8_t Key_Val_Old = 0;
    static uint32_t key_ms = 0;

    if (tick_ms - key_ms >= 20)
    {
        key_ms = tick_ms;
        uint8_t key_val = Matrix_Key_Scan();

        if (key_val == Key_Val_Old)
            return;
        if (key_mode == 0) // 进入motorA调参模式
        {
            switch (key_val)
            {

            case 1:
                g_motorA.p += 0.5;
                break;

            case 2:
                g_motorA.p += 0.01;
                break;

            case 3:
                g_motorA.p -= 0.5;
                break;

            case 4:
                g_motorA.p -= 0.01;
                break;

            case 5:
                g_motorA.i += 0.5;
                break;

            case 6:
                g_motorA.i += 0.01;
                break;

            case 7:
                g_motorA.i -= 0.5;
                break;

            case 8:
                g_motorA.i -= 0.01;
                break;

            case 9:
                g_motorA.d += 0.5;
                break;

            case 10:
                g_motorA.d += 0.01;
                break;

            case 11:
                g_motorA.d -= 0.5;
                break;

            case 12:
                g_motorA.d -= 0.01;
                break;
            
            case 14:
                pid_init(&g_motorA,DELTA_PID,g_motorA.p,g_motorA.i,g_motorA.d);

                break;
            case 15:
                motor_status = 1;
                Motor_On();
                pid_init(&g_motorA, DELTA_PID, 2.05, 17.45, 0);
                pid_init(&g_motorB, DELTA_PID, 2.10, 17.45, 0);
                motor_target_set(100,100);
                motor_start = tick_ms;
                break;
            case 16:
                key_mode = 1;
                break;
            default:
                break;

            }    
        }
        else if(key_mode == 1)
        {
            switch (key_val)            
             { 
            case 1:
                g_motorB.p += 0.5;
                break;

            case 2:
                g_motorB.p += 0.01;
                break;

            case 3:
                g_motorB.p -= 0.5;
                break;

            case 4:
                g_motorB.p -= 0.01;
                break;
            
            case 5:
                g_motorB.i += 0.5;
                break;

            case 6:
                g_motorB.i += 0.01;
                break;

            case 7:
                g_motorB.i -= 0.5;
                break;

            case 8:
                g_motorB.i -= 0.01;
                break;
            
            case 9:
                g_motorB.d += 0.5;
                break;

            case 10:
                g_motorB.d += 0.01;
                break;

            case 11:
                g_motorB.d -= 0.5;
                break;

            case 12:
                g_motorB.d -= 0.01;
                break;
            case 14:
                pid_init(&g_motorB,DELTA_PID,g_motorB.p,g_motorB.i,g_motorB.d);
                
                break;

            case 16:
                key_mode = 2;
                break;
            default:
                break;
            }
        }

        else if (key_mode == 1)
        {
            switch (key_val)
            {
                switch (key_val)
                {
                case 1:
                    g_motorB.p += 0.5;
                    break;

                case 2:
                    g_motorB.p += 0.01;
                    break;

                case 3:
                    g_motorB.p -= 0.5;
                    break;

                case 4:
                    g_motorB.p -= 0.01;
                    break;

                case 5:
                    g_motorB.i += 0.5;
                    break;

                case 6:
                    g_motorB.i += 0.01;
                    break;

                case 7:
                    g_motorB.i -= 0.5;
                    break;

                case 8:
                    g_motorB.i -= 0.01;
                    break;

                case 9:
                    g_motorB.d += 0.5;
                    break;

                case 10:
                    g_motorB.d += 0.01;
                    break;

                case 11:
                    g_motorB.d -= 0.5;
                    break;

                case 12:
                    g_motorB.d -= 0.01;
                    break;

                case 16:
                    key_mode = 2;
                    break;
                default:
                    break;
                }
            }
        }
        else if (key_mode == 2)
        {

            switch (key_val)
            {
            case 16:
                key_mode = 0;
                break;
            default:
                break;
            }
        }
        Key_Val_Old = key_val;
    }
}