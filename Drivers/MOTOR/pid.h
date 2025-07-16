#ifndef __PID_h_
#define __PID_h_
#include "main.h"

typedef enum {
    POSITION_PID = 0, // 位置式PID
    DELTA_PID,        // 增量式PID
} PidMode;

typedef struct {
    float target;
    float now;
    float error[3];
    float p, i, d;
    float pout, dout, iout;
    float out;

    PidMode pid_mode;
} Pid_t;

void pid_reset(Pid_t *pid);
void pid_cal(Pid_t *pid);
void pid_control(void);
void pid_init(Pid_t *pid, PidMode mode, float p, float i, float d);
void motor_target_set(int spe1, int spe2);
void Position_PI(Pid_t *pid);
void pidout_limit(Pid_t *pid);
void angle_cal(float target);
void angle_sudu(float target);

extern Pid_t g_motorA;
extern Pid_t g_motorB;
extern int speed_TAGET;
extern Pid_t g_angle;
extern float Pitch, Roll, Yaw;
extern uint8_t RollL, RollH, PitchL, PitchH, YawL, YawH, VL, VH, SUM;
extern  float yaw_filtered;

#endif
