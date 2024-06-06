#include <malloc.h>
#include <stdlib.h>
#include "tim_irq_handler.h"


typedef struct {
    TIM_HandleTypeDef *p_tim_instance; // pointer to HAL timer structure
    void (*p_fcn_callback) (void); // pointer to user callback function
    void (*p_fcn_handler) (void *); // pointer to user interrupt handler function
    uint8_t irq_flag; // interrupt flag (0 - no interrupt, 1 - interrupt occured)
} tim_irq_t;

static tim_irq_t *p_tim_irq; // structure, using for managing the interrupts
static size_t cnt = 0; // added timers counter


int compare_timers(const tim_irq_t *v_0, const tim_irq_t *v_1) {
    if (v_0->p_tim_instance < v_1->p_tim_instance)
        return -1;
    else if ((tim_irq_t *) v_0->p_tim_instance == (tim_irq_t *) v_1->p_tim_instance)
        return 0;
    else 
        return 1;
}

static int8_t find_tim(tim_irq_t *p_tims, size_t beg_idx, size_t fin_idx, TIM_HandleTypeDef *p_tim_to_find) {
    if ((fin_idx - beg_idx) == 1 || fin_idx == beg_idx) {
        if (p_tim_to_find == p_tims[beg_idx].p_tim_instance) {
            return beg_idx;
        } else if (p_tim_to_find == p_tims[fin_idx].p_tim_instance) {
            return fin_idx;
        } else return -1;
    } else if (p_tim_to_find >= p_tims[beg_idx + ((fin_idx - beg_idx) >> 1) + 1].p_tim_instance) {
        return find_tim(p_tims, beg_idx + ((fin_idx - beg_idx) >> 1) + 1, fin_idx, p_tim_to_find);
    } else {
        return find_tim(p_tims, beg_idx, fin_idx >> 1, p_tim_to_find);
    }
}

int8_t tim_add_handlers(TIM_HandleTypeDef *p_tim, void (*p_irq_fcn) (void), void (*p_hndlr_fcn) (void *)) {
    if (!cnt) {
        p_tim_irq = (tim_irq_t *) malloc(sizeof(tim_irq_t));
        p_tim_irq->p_tim_instance = p_tim;
        p_tim_irq->p_fcn_callback = p_irq_fcn;
        p_tim_irq->p_fcn_handler = p_hndlr_fcn;
        p_tim_irq->irq_flag = 0;
        if (p_tim_irq == NULL) return -1;
        cnt = 1;
    } else {
    	tim_irq_t *bckp_p_tim_irq = p_tim_irq;
        cnt++;
        p_tim_irq = (tim_irq_t *) realloc(p_tim_irq, sizeof(tim_irq_t) * cnt);
        if (p_tim_irq == NULL) {
        	cnt--;
        	p_tim_irq = bckp_p_tim_irq;
        	return -1;
        }

        p_tim_irq[cnt - 1].p_tim_instance = p_tim;
        p_tim_irq[cnt - 1].p_fcn_callback = p_irq_fcn;
        p_tim_irq[cnt - 1].p_fcn_handler = p_hndlr_fcn;
        p_tim_irq->irq_flag = 0;
        qsort(p_tim_irq, cnt, sizeof(tim_irq_t), (int (*) (const void *, const void *)) compare_timers);
    }
    return 0;
}

void tim_start(TIM_HandleTypeDef *p_tim) {
    __HAL_TIM_SET_COUNTER(p_tim, 0); // zeroing the timer counter
    __HAL_TIM_CLEAR_FLAG(p_tim, TIM_FLAG_UPDATE);
    HAL_TIM_Base_Start_IT(p_tim);
}

void tim_stop(TIM_HandleTypeDef *p_tim) {
	HAL_TIM_Base_Stop_IT(p_tim);
}

void tim_handle_callback(TIM_HandleTypeDef *p_tim, void *args) {
    int8_t tim_num = find_tim(p_tim_irq, 0, cnt - 1, p_tim);
    if ((tim_num != -1) && (p_tim_irq[tim_num].p_fcn_handler != NULL) && (p_tim_irq[tim_num].irq_flag == 1)) {
        p_tim_irq[tim_num].irq_flag = 0;
        p_tim_irq[tim_num].p_fcn_handler(args);  
    }
}

uint8_t get_flag(TIM_HandleTypeDef *p_tim) {
	int8_t tim_num = find_tim(p_tim_irq, 0, cnt - 1, p_tim);
	if (tim_num != -1)
		return p_tim_irq[tim_num].irq_flag;
	else
		return 0xFF;
}

// HAL interrupt handler
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    int8_t tim_num = find_tim(p_tim_irq, 0, cnt - 1, htim);
    if ((tim_num != -1) && (p_tim_irq[tim_num].p_fcn_callback != NULL)) {
        p_tim_irq[tim_num].irq_flag = 1;
        p_tim_irq[tim_num].p_fcn_callback();  
    }
}
