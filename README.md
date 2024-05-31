# tim_inerrupt_handlers

Interface for automatic STM32 timers interrupts processing with HAL callback (dynamic)

**Warning!!** Before using generation option with "*.c/*.h" configuration files should be enabled in STM32CubeMX.

Allows automatic calling of custom callback functions and handlers, calling in loop.

Source code uses heap allocation and reallocation when new timer is added. Timer numbers seuqence isn't matter.

Example
---------
```c
#include "tim_irq_handler.h"
/*
* TIM1 is set for 1 s period for this example
*/

void my_tim_1_callback(void) {
  // Stop after timer update
  tim_stop(&htim1);
}

void my_tim_1_evt_proc(void *ptr) {
  // Start after interrupt
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
  tim_start(&htim1);
}

int main() {
  // if handler (third arg) won't be used, use NULL instead, as well as for callback function 
  tim_add_handlers(&htim1, my_tim_1_callback, my_tim_1_evt_proc);
  tim_start(&htim1);
  while (1) {
    tim_handle_callback(&htim1, NULL); // second arg is used for using outer variables via pointer
  }
}

```
