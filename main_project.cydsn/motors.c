/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "motors.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

extern uint8 DISTANCE_TRIGGERED; //what does this do?
extern uint8 isPollingUltrasonics; //What does this do?
int TURN_90 = 14765;
int TURN_180 = 14765 * 2;
int MOTOR_SYNC_THRESH = 2;

void Motors_Forward(const int count) {
    _Motors_Stop();
    QuadDec_L_SetCounter(0);
    QuadDec_R_SetCounter(0);
    int32 cnt_left, cnt_right, diff;
    diff = 10;
    
    while (((QuadDec_R_GetCounter() < count)&&(QuadDec_L_GetCounter() < count))||(abs(diff) > MOTOR_SYNC_THRESH)) { //What happens when cnt_left - cnt_right > 40 but QuadDec_L_GetCounter() >= count?
        //if (DISTANCE_TRIGGERED) break;
        
        //moving individual motors may contribute to difference in quad decoder
        //may also make movement not entirely straight
        _Motor_Right_Forwards();
        _Motor_Left_Forwards();                
        
        cnt_left = QuadDec_L_GetCounter();
        cnt_right = QuadDec_R_GetCounter();
        diff = cnt_left - cnt_right;
        
        if (diff > MOTOR_SYNC_THRESH) { //left in front
            _Motors_Stop();
            _Motor_Right_Forwards();
        }
        else if (diff < -MOTOR_SYNC_THRESH) { //right in front
            _Motors_Stop();
            _Motor_Left_Forwards(); 
        }
    }
    //need correction code at the end in case robot goes overboard trying to correct the wheels
    while ((QuadDec_R_GetCounter() > count)&&(QuadDec_L_GetCounter() > count)) {
        //if (DISTANCE_TRIGGERED) break;
        
        _Motor_Right_Reverse();
        _Motor_Left_Reverse();                
        
        cnt_left = QuadDec_L_GetCounter(); 
        cnt_right = QuadDec_R_GetCounter();
        diff = cnt_left - cnt_right;
        
        if (diff > MOTOR_SYNC_THRESH) { //left in front
            _Motors_Stop();
            _Motor_Left_Reverse();
        }
        else if (diff < -MOTOR_SYNC_THRESH) { //right in front
            _Motors_Stop();
            _Motor_Right_Reverse();
        }
        
    }
    _Motors_Stop();
    //isPollingUltrasonics = 0;
    //DISTANCE_TRIGGERED = 0;
    cnt_left = QuadDec_L_GetCounter();
    cnt_right = QuadDec_R_GetCounter();
    char leftCount[100], rightCount[100];
    snprintf(leftCount, 100, "Left count %li\n", cnt_left);
    snprintf(rightCount, 100, "Right count %li\n", cnt_right);
    UART_PutString(leftCount);
    UART_PutString(rightCount);
    return;
}
    
void Motors_Reverse(const int count) {
    _Motors_Stop();
    QuadDec_L_SetCounter(0);
    QuadDec_R_SetCounter(0);
    int32 cnt_left, cnt_right, diff;
    diff = 10;
    
    // Expect QuadDec to return negative values
    while (((abs(QuadDec_L_GetCounter()) < count)&&(abs(QuadDec_R_GetCounter()) < count))||(abs(diff) > MOTOR_SYNC_THRESH)) {
        //if (DISTANCE_TRIGGERED) break;
        
        _Motor_Right_Reverse();
        _Motor_Left_Reverse();
        
        cnt_left = abs(QuadDec_L_GetCounter());
        cnt_right = abs(QuadDec_R_GetCounter());
        diff = cnt_left - cnt_right;
        
        if (diff > MOTOR_SYNC_THRESH) { //left in front
            _Motors_Stop();
            _Motor_Right_Reverse();
        }
        else if (diff < -MOTOR_SYNC_THRESH) { //right in front
            _Motors_Stop();
            _Motor_Left_Reverse();
        }
    }
    //need correction code at the end in case robot goes overboard
    while ((abs(QuadDec_L_GetCounter()) > count)&&(abs(QuadDec_R_GetCounter()) > count)) {
        //if (DISTANCE_TRIGGERED) break;
        
        _Motor_Right_Forwards();
        _Motor_Left_Forwards();
        
        cnt_left = abs(QuadDec_L_GetCounter());
        cnt_right = abs(QuadDec_R_GetCounter());
        diff = cnt_left - cnt_right;
        
        if (diff > MOTOR_SYNC_THRESH) { //left in back
            _Motors_Stop();
            _Motor_Left_Forwards();
        }
        else if (diff < -MOTOR_SYNC_THRESH) { //right in back
            _Motors_Stop();
            _Motor_Right_Forwards();
        }
    }
    _Motors_Stop();
    cnt_left = abs(QuadDec_L_GetCounter());
    cnt_right = abs(QuadDec_R_GetCounter());
    //isPollingUltrasonics = 0;
    //DISTANCE_TRIGGERED = 0;
    char leftCount[100], rightCount[100];
    snprintf(leftCount, 100, "Backwards left count %li\n", cnt_left);
    snprintf(rightCount, 100, "Backwards right count %li\n", cnt_right);
    UART_PutString(leftCount);
    UART_PutString(rightCount);
    return;
}

void Motors_Turn_Left(const int count) {
    _Motors_Stop();
    QuadDec_L_SetCounter(0);
    QuadDec_R_SetCounter(0);
    int32 cnt_left, cnt_right, diff;
    diff = 10;
    
    // Expect QuadDec_L to return negative value
    while (((QuadDec_R_GetCounter() < count)&&(abs(QuadDec_L_GetCounter()) < count))||(abs(diff) > MOTOR_SYNC_THRESH)) {
        _Motor_Right_Forwards();
        _Motor_Left_Reverse();
       
        cnt_left = abs(QuadDec_L_GetCounter());
        cnt_right = QuadDec_R_GetCounter();
        diff = cnt_left - cnt_right;
       
        if (diff > MOTOR_SYNC_THRESH) { //left in front
            _Motors_Stop();
            _Motor_Right_Forwards();
        }
        else if (diff < -MOTOR_SYNC_THRESH) { //right in front
            _Motors_Stop();
            _Motor_Left_Reverse();
        }
    }
    //need correction code at the end in case robot goes oveboard
    while (((QuadDec_R_GetCounter() > count)&&(abs(QuadDec_L_GetCounter()) > count))) {
        _Motor_Right_Reverse();
        _Motor_Left_Forwards();
       
        cnt_left = abs(QuadDec_L_GetCounter());
        cnt_right = QuadDec_R_GetCounter();
        diff = cnt_left - cnt_right;
       
        if (diff > MOTOR_SYNC_THRESH) { //left in front
            _Motors_Stop();
            _Motor_Left_Forwards();
        }
        else if (diff < -MOTOR_SYNC_THRESH) { //right in front
            _Motors_Stop();
            _Motor_Right_Reverse();
        }
    }
    _Motors_Stop();
    cnt_left = QuadDec_L_GetCounter();
    cnt_right = QuadDec_R_GetCounter();
    char leftCount[100], rightCount[100];
    snprintf(leftCount, 100, "Turn left left count %li\n", cnt_left);
    snprintf(rightCount, 100, "Turn left right count %li\n", cnt_right);
    UART_PutString(leftCount);
    UART_PutString(rightCount);
    return;
}

void Motors_Turn_Right(const int count) {  
    _Motors_Stop();
    QuadDec_L_SetCounter(0);
    QuadDec_R_SetCounter(0);
    int32 cnt_left, cnt_right, diff;
    diff = 10;
    
    // Expect QuadDec_R to return negative value
    while (((QuadDec_L_GetCounter() < count)&&(abs(QuadDec_L_GetCounter()) < count))||(abs(diff) > MOTOR_SYNC_THRESH)) {
        _Motor_Right_Reverse();
        _Motor_Left_Forwards();

        cnt_left = QuadDec_L_GetCounter();
        cnt_right = abs(QuadDec_R_GetCounter());
        diff = cnt_left - cnt_right;
       
        if (diff > MOTOR_SYNC_THRESH) { //left in front
            _Motors_Stop();
            _Motor_Right_Reverse();
        }
        else if (diff < -MOTOR_SYNC_THRESH) { //right in front
            _Motors_Stop();
            _Motor_Left_Forwards(); 
        }
    }
    //need correction code in case robot goes overboard
    while (((QuadDec_L_GetCounter() > count)&&(abs(QuadDec_L_GetCounter()) > count))) {
        _Motor_Right_Forwards();
        _Motor_Left_Reverse();

        cnt_left = QuadDec_L_GetCounter();
        cnt_right = abs(QuadDec_R_GetCounter());
        diff = cnt_left - cnt_right;
       
        if (diff > MOTOR_SYNC_THRESH) { //left in front
            _Motors_Stop();
            _Motor_Left_Reverse();
        }
        else if (diff < -MOTOR_SYNC_THRESH) { //right in front
            _Motors_Stop();
            _Motor_Right_Forwards();
        }
    }
    _Motors_Stop();
    char leftCount[100], rightCount[100];
    cnt_left = QuadDec_L_GetCounter();
    cnt_right = QuadDec_R_GetCounter();
    snprintf(leftCount, 100, "Turn right left count %li\n", cnt_left);
    snprintf(rightCount, 100, "Turn right right count %li\n", cnt_right);
    UART_PutString(leftCount);
    UART_PutString(rightCount);
    return;
}

void _Motors_Stop(void) {
    motor_L_1_Write(0);
    motor_L_2_Write(0);
    motor_R_1_Write(0);
    motor_R_2_Write(0);
}

void _Motor_Left_Forwards(void) {
    motor_L_1_Write(0);
    motor_L_2_Write(1);
}

void _Motor_Left_Reverse(void) {
    motor_L_1_Write(1);
    motor_L_2_Write(0);
}

void _Motor_Right_Forwards(void) {
    motor_R_1_Write(0);
    motor_R_2_Write(1);
}

void _Motor_Right_Reverse(void) {
    motor_R_1_Write(1);
    motor_R_2_Write(0);
}

/* [] END OF FILE */
