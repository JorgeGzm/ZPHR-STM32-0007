#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

enum menu_selection {
	MENU_PROTIMER,
	MENU_CLOCKALARM,
};

typedef void (*menu_screen_cb_t)(enum menu_selection sel);

void menu_screen_create(menu_screen_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif /* MENU_SCREEN_H */
