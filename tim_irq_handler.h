#ifndef SRC_TIM_IRQ_HANDLER_H_
#define SRC_TIM_IRQ_HANDLER_H_

#include "tim.h"


/*
 * @brief Add callback and handler functions
 * @param p_tim Pointer to HAL timer structure
 * @param p_irq_fcn Pointer to user callback function
 * @param p_hndlr_fcn Pointer to user interrupt event handler function
 * @return Timer number if timer added, -1 otherwise
 * */
int8_t tim_add_handlers(TIM_HandleTypeDef *p_tim, void (*p_irq_fcn) (void), void (*p_hndlr_fcn) (void *));

/**
 * @brief Function for calling certain handler user function in the user's code
 * @param p_tim Pointer to HAL timer structure
 * @param *args Pointer to user arguments for the interrupt handler
 */
void tim_handle_callback(TIM_HandleTypeDef *p_tim, void *args);

/**
 * @brief Starting timer interrupt mode
 * @param p_tim Pointer to HAL timer structure
 */
void tim_start(TIM_HandleTypeDef *p_tim);

/**
 * @brief Stop timer interrupt mode
 * @param p_tim Pointer to HAL timer structure
 */
void tim_stop(TIM_HandleTypeDef *p_tim);

/**
 * @brief Get the interrupt flag
 * @param p_tim Pointer to HAL timer structure
 * @return Flag status if timer added, 0xFF otherwise
 */
uint8_t get_flag(TIM_HandleTypeDef *p_tim);

#endif /* SRC_TIM_IRQ_HANDLER_H_ */
