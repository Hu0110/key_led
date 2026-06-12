#include "./stm32f1xx_it.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"

/* ====== 跑马灯控制变量 ====== */
uint8_t run_mode = 0;       // 0=停止 1=运行
uint8_t led_index = 0;      // 当前LED
uint16_t speed = 500;       // ms（越小越快）
uint32_t last_time = 0;

void all_led_off(void)
{
    LED0(1);
    LED1(1);
}

/* 跑马灯逻辑 */
void marquee_update(void)
{
    if (!run_mode) return;

    if (HAL_GetTick() - last_time >= speed)
    {
        last_time = HAL_GetTick();

        /* 关掉所有灯 */
        all_led_off();

        /* 点亮当前LED */
        if (led_index == 0)
        {
            LED0(0);
        }
        else
        {
            LED1(0);
        }

        /* 切换下一个 */
        led_index ^= 1;
    }
}

int main(void)
{
    uint8_t key;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    key_init();

    all_led_off();

    while (1)
    {
        key = key_scan(0);

        if (key)
        {
            switch (key)
            {
                /* ===== KEY0：开启/关闭跑马灯 ===== */
                case KEY0_PRES:
                    run_mode = !run_mode;

                    if (run_mode)
                    {
                        led_index = 0;
                        last_time = HAL_GetTick();
                    }
                    else
                    {
                        all_led_off();
                    }
                    break;

                /* ===== KEY1：加速 ===== */
                case KEY1_PRES:
                    if (speed > 100)
                        speed -= 100;
                    else if (speed > 30)
                        speed -= 20;
                    break;

                /* ===== UP：关闭所有 ===== */
                case WKUP_PRES:
                    run_mode = 0;
                    speed = 500;
                    all_led_off();
                    break;
            }
        }

        /* 非阻塞更新跑马灯 */
        marquee_update();
    }
}