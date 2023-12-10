#include "multi_button.h"

#define EVENT_CB(ev)   if(handle->cb[ev])handle->cb[ev]((void*)handle)
#define PRESS_REPEAT_MAX_NUM  15 /*!< The maximum value of the repeat counter */

//button handle list head.
static struct Button* head_handle = NULL;
 @@ -106,7 +107,9 @@ static void button_handler(struct Button* handle)
		if(handle->button_level == handle->active_level) { //press down again
			handle->event = (uint8_t)PRESS_DOWN;
			EVENT_CB(PRESS_DOWN);
			handle->repeat++;
			if(handle->repeat != PRESS_REPEAT_MAX_NUM) {
				handle->repeat++;
			}
			EVENT_CB(PRESS_REPEAT); // repeat hit
			handle->ticks = 0;
			handle->state = 3;
		} else if(handle->ticks > SHORT_TICKS) { //released timeout
			if(handle->repeat == 1) {
				handle->event = (uint8_t)SINGLE_CLICK;
				EVENT_CB(SINGLE_CLICK);
			} else if(handle->repeat == 2) {
				handle->event = (uint8_t)DOUBLE_CLICK;
				EVENT_CB(DOUBLE_CLICK); // repeat hit
			}
			handle->state = 0;
		}
		break;
	case 3:
		if(handle->button_level != handle->active_level) { //released press up
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(PRESS_UP);
			if(handle->ticks < SHORT_TICKS) {
				handle->ticks = 0;
				handle->state = 2; //repeat press
			} else {
				handle->state = 0;
			}
		} else if(handle->ticks > SHORT_TICKS) { // SHORT_TICKS < press down hold time < LONG_TICKS
			handle->state = 1;
		}
		break;
	case 5:
		if(handle->button_level == handle->active_level) {
			//continue hold trigger
			handle->event = (uint8_t)LONG_PRESS_HOLD;
			EVENT_CB(LONG_PRESS_HOLD);
		} else { //released
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(PRESS_UP);
			handle->state = 0; //reset
		}
		break;
	default:
		handle->state = 0; //reset
		break;
	}
}
/**
  * @brief  Start the button work, add the handle into work list.
  * @param  handle: target handle struct.
  * @retval 0: succeed. -1: already exist.
  */
int button_start(struct Button* handle)
{
	struct Button* target = head_handle;
	while(target) {
		if(target == handle) return -1;	//already exist.
		target = target->next;
	}
	handle->next = head_handle;
	head_handle = handle;
	return 0;
}
/**
  * @brief  Stop the button work, remove the handle off work list.
  * @param  handle: target handle struct.
  * @retval None
  */
void button_stop(struct Button* handle)
{
	struct Button** curr;
	for(curr = &head_handle; *curr; ) {
		struct Button* entry = *curr;
		if(entry == handle) {
			*curr = entry->next;
//			free(entry);
			return;//glacier add 2021-8-18
		} else {
			curr = &entry->next;
		}
	}
}