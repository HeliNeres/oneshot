#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/bootrom.h"

// Variáveis dos LEDs
#define LED_VERDE 11
#define LED_AZUL 12
#define LED_VERMELHO 13

//pino do botão
const uint BUTTON_A = 5;
const uint BUTTON_B = 6;

bool botao_on = false;
bool led_active = false;
absolute_time_t turn_off_time;
uint last_time = 0;
bool debounce = true;

//cabeçalhos da funções
void inicializar();
int64_t turn_off_green(alarm_id_t id, void *user_data);
int64_t turn_off_blue(alarm_id_t id, void *user_data);
int64_t turn_off_red(alarm_id_t id, void *user_data);
static void botao_boot(uint gpio, uint32_t events);

int main() {
    stdio_init_all();
    inicializar();

    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, 1, &botao_boot);

    while (true) {
        if (gpio_get(BUTTON_A) == 0 && !led_active) {
            sleep_ms(50); //debounce
            //printf("Botão\n");

            if (gpio_get(BUTTON_A) == 0) {
                gpio_put(LED_VERDE, 1);
                gpio_put(LED_AZUL, 1);
                gpio_put(LED_VERMELHO, 1);

                led_active = true;

                add_alarm_in_ms(3000, turn_off_green, NULL, false);
                add_alarm_in_ms(6000, turn_off_blue, NULL, false);
                add_alarm_in_ms(9000, turn_off_red, NULL, false);
            }
        }

        sleep_ms(10);
    }

    return 0;
}

void inicializar(){
	gpio_init(LED_VERDE);
	gpio_init(LED_AZUL);
	gpio_init(LED_VERMELHO);

	gpio_set_dir(LED_VERDE, GPIO_OUT);
	gpio_set_dir(LED_AZUL, GPIO_OUT);
	gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
}

// função de callback para desligar o LED verde após o tempo programado
int64_t turn_off_green(alarm_id_t id, void *user_data) {
    gpio_put(LED_VERDE, 0);

    return 0;
}

// função de callback para desligar o LED azul após o tempo programado
int64_t turn_off_blue(alarm_id_t id, void *user_data) {
    gpio_put(LED_AZUL, 0);

    return 0;
}

// função de callback para desligar o LED vermelho após o tempo programado
int64_t turn_off_red(alarm_id_t id, void *user_data) {
    gpio_put(LED_VERMELHO, 0);

    led_active = false;

    return 0;
}

//rotina da interrupção para boot
static void botao_boot(uint gpio, uint32_t events){
    printf("Interrupção ocorreu no pino %u, no evento %u\n", gpio, events);
    printf("HABILITANDO O MODO GRAVAÇÃO");
	reset_usb_boot(0,0); //habilita o modo de gravação do microcontrolador
}
