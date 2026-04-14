#include "main.h"
#include "milis.h"
#include <stdbool.h>
#include <stm8s.h>
// #include "delay.h"
#include "daughterboard.h"
#include "uart1.h"
#include <stdio.h>

// Nucleo Kit
#ifdef STM8S208
#define LED_PORT GPIOC
#define LED_PIN GPIO_PIN_5
#define BTN_PORT GPIOE
#define BTN_PIN GPIO_PIN_4
#endif

//makra pro případný jednodušení - S3 můžu nahradit R atd atd.
#define BTNR_R_PORT DB_S3_PORT
#define BTNR_R_PIM DB_S3_PIN
#define BTNR_G_PORT DB_S2_PORT
#define BTNR_G_PIM DB_S2_PIN
#define BTNR_B_PORT DB_1_PORT
#define BTNR_B_PIM DB_S1_PIN

#define SET_COMPARE_R TIM2_SetCompare3;
#define SET_COMPARE_G TIM2_SetCompare2;
#define SET_COMPARE_B TIM2_SetCompare1;





void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    init_milis();
    init_uart1();

    // OC1 - Output channel 1
    GPIO_Init(DB_PWM_B_PORT, DB_PWM_B_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    // OC2 - Output channel 2
    GPIO_Init(DB_PWM_G_PORT, DB_PWM_G_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    // OC3 - Output channel 3
    GPIO_Init(DB_PWM_R_PORT, DB_PWM_R_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    GPIO_Init(DB_S1_PORT, DB_S1_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(DB_S2_PORT, DB_S2_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(DB_S3_PORT, DB_S3_PIN, GPIO_MODE_IN_PU_NO_IT);


    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 100 - 1); // f= 10kHz
    TIM2_Cmd(ENABLE);
    TIM2_OC1Init(TIM2_OCMODE_PWM1,        // PWM1 a PWM2 jsou navzájem negované
                 TIM2_OUTPUTSTATE_ENABLE, // povolí fyzický výstup
                 1,                       // počáteční hodnota compare registru
                 TIM2_OCPOLARITY_HIGH     // LED se spíná logickou 1
    );

    TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 1,
                 TIM2_OCPOLARITY_HIGH);

    TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 1,
                 TIM2_OCPOLARITY_HIGH);


    // ošetření nežádoucích jevů při změně compare registru
    TIM2_OC1PreloadConfig(ENABLE);
    TIM2_OC2PreloadConfig(ENABLE);
    TIM2_OC3PreloadConfig(ENABLE);
}


int main(void)
{

    uint32_t time = 0;
    uint32_t btn_time = 0;
    uint8_t R = 0, G = 0, B = 0;
    bool r_btn_is_press, g_btn_is_press, b_btn_is_press;

    typedef enum{
        no_change,
        display_change
    } status_type;
    status_type status= no_change;


    init();


    while (1) {
        if (milis() - time > 333) {
            time = milis();
            if(status ==display_change)
            printf("R: %d    G: %d   B: %d\n ", R, G, B);
            status=no_change;
        }
        if (milis() - btn_time > 30) {
            btn_time = milis();
            if (PUSH(DB_S3) && !r_btn_is_press) {
                status=display_change;
                R += 10;
                if(R>100){
                    R=0;
                }
            }
            if (PUSH(DB_S2) && !g_btn_is_press) {
                status=display_change;
                G += 10;
                if(G>100){
                    G=0;
                }
            }
            if (PUSH(DB_S1) && !b_btn_is_press) {
                status=display_change;
                B += 10;
                if(B>100){
                    B=0;
                }
            }
            r_btn_is_press = PUSH(DB_S3);
            g_btn_is_press = PUSH(DB_S2);
            b_btn_is_press = PUSH(DB_S1);


            TIM2_SetCompare1(B);
            TIM2_SetCompare2(G);
            TIM2_SetCompare3(R);
        }
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
