#include "motor.h"
#include "pid.h"
#include "uart_pid.h"

#define motor_max_duty 50000
#define MAX_DUTY 50000

Pid_t g_motorA;
Pid_t g_motorB;
Pid_t g_angle;
float Pitch, Roll, Yaw;
uint8_t RollL, RollH, PitchL, PitchH, YawL, YawH, VL, VH, SUM;
uint8_t ready1 = 1; // 灰度移植完删除

void pid_reset(Pid_t *pid)
{
    // --- 清空历史误差 ---
    // 这是PID“记忆”的核心部分
    pid->error[0] = 0.0f;
    pid->error[1] = 0.0f;
    pid->error[2] = 0.0f;

    // --- 清空各项输出和总输出 ---
    // 对于增量式PID，清空pid->out至关重要，因为它作为下一次计算的基础 u(k-1)
    // 对于位置式PID，清空pid->iout至关重要，因为它就是积分累加项
    pid->pout = 0.0f;
    pid->iout = 0.0f;
    pid->dout = 0.0f;
    pid->out = 0.0f;
}

void pid_init(Pid_t *pid, PidMode mode, float p, float i, float d)
{
    pid->pid_mode = mode;
    pid->p = p;
    pid->i = i;
    pid->d = d;
    pid_reset(pid);
}

// 设置两个电机的目标速度
void motor_target_set(int spe1, int spe2)
{
    if (spe1 >= 0)
    {
        g_motorA_dir = 1;
        g_motorA.target = spe1;
    }
    else
    {
        g_motorA_dir = 0;
        g_motorA.target = -spe1;
    }

    if (spe2 >= 0)
    {
        g_motorB_dir = 1;
        g_motorB.target = spe2;
    }
    else
    {
        g_motorB_dir = 0;
        g_motorB.target = -spe2;
    }
}

void angle_sudu(float target)
{
    g_angle.target = target;
    g_angle.now = Yaw; // 当前的偏航角作为反馈
    pid_cal(&g_angle);
    // 对PID输出进行限幅，防止转向过快
    if (g_angle.out >= 40)
        g_angle.out = 40;
    if (g_angle.out <= -40)
        g_angle.out = -40;

    // 将角度环的输出作为速度差，叠加到基础速度上
    // 例如，车要左转，angle.out为负，则左轮减速，右轮加速
    motor_target_set(260 - g_angle.out, 260 + g_angle.out);
}

/**
 * @brief  角度环（位置控制模式），用于控制小车转向
 * @brief  通过角度PID的输出，直接控制两个电机的速度，实现原地转向等
 * @param  target: 目标角度 (例如，Yaw角)
 */
void angle_cal(float target)
{
    g_angle.target = target;
    g_angle.now = Yaw;
    pid_cal(&g_angle);
    if (g_angle.out >= 100)
        g_angle.out = 100;
    if (g_angle.out <= -100)
        g_angle.out = -100;

    // 将角度环的输出直接作为两轮的速度目标值（方向相反），实现原地转向
    motor_target_set(-g_angle.out, g_angle.out);
}

int speed_TAGET = 0;

void pid_control()
{
    //--- 速度环 PID 计算 ---//

    // if(ready1==1)			//openmv中
    // track();
    if (g_motorA_dir)
    {
        g_motorA.now = g_encoder_A;
    }
    else
    {
        g_motorA.now = -g_encoder_A;
    } // 修改过
    if (g_motorB_dir)
    {
        g_motorB.now = g_encoder_B;
    }
    else
    {
        g_motorB.now = -g_encoder_B;
    }
    g_encoder_A = 0;
    g_encoder_B = 0;
    // 3.调用PID控制器进行计算
    pid_cal(&g_motorA);
    pid_cal(&g_motorB);
    pidout_limit(&g_motorA);
    pidout_limit(&g_motorB);
    // 4.将PID输出值赋给电机PWM驱动
    MotorA_Duty(g_motorA.out);
    MotorB_Duty(g_motorB.out);

    Serial_PID();
}

void pid_cal(Pid_t *pid)
{
    // 计算当前偏差
    pid->error[0] = pid->target - pid->now;
    //  pid->error[0]=x;
    // 根据PID模式进行计算
    if (pid->pid_mode == DELTA_PID) // 增量式PID
    {
        pid->pout = pid->p * (pid->error[0] - pid->error[1]);
        pid->iout = pid->i * pid->error[0];
        pid->dout = pid->d * (pid->error[0] - 2 * pid->error[1] + pid->error[2]);
        pid->out += pid->pout + pid->iout + pid->dout;
    }
    else if (pid->pid_mode == POSITION_PID) // 位置式PID
    {
        pid->pout = pid->p * pid->error[0];
        pid->iout += pid->i * pid->error[0];
        pid->dout = pid->d * (pid->error[0] - pid->error[1]);
        pid->out = pid->pout + pid->iout + pid->dout;
    }

    // 更新历史偏差，为下次计算做准备
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
}

void pidout_limit(Pid_t *pid)
{
    // 输出限幅
    if (pid->out >= MAX_DUTY)
        pid->out = MAX_DUTY;
    if (pid->out <= 0)
        pid->out = 0;
}

// 位置式PID控制器
void Position_PI(Pid_t *pid)
{
    pid->error[0] = pid->target - pid->now;
    pid->pout = pid->p * pid->error[0];
    pid->iout += pid->i * pid->error[0];

    // 积分分离：偏差较小时，清除积分项，防止过冲
    if (pid->error[0] < 20)
        pid->iout = 0;
    pid->dout = pid->d * (pid->error[0] - pid->error[1]);
    pid->error[1] = pid->error[0];
    pid->out = pid->pout + pid->iout + pid->dout;

    // 输出限幅
    if (pid->out >= MAX_DUTY)
        pid->out = MAX_DUTY;
    if (pid->out <= 0)
        pid->out = 0;
}
