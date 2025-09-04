#include <zephyr/kernel.h>

#include "basic_widgets.h"
#include "display_manager.h"
#include <lvgl.h>

/*
 * Hello world example
 */
void basic_widgets_ex1(void)
{
    #if 1

    /* Using current screen */
    lv_obj_t *splash_screen = lv_scr_act();
	
	/* Set background to black */
	lv_obj_set_style_bg_color(splash_screen, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(splash_screen, LV_OPA_COVER, 0);

	/* Create title label */
	lv_obj_t *title_label = lv_label_create(splash_screen);
	lv_label_set_text(title_label, "Hello World!");
	lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
	lv_obj_align(title_label, LV_ALIGN_CENTER, 0, -10);

#else 

    /* Creating new screen */
    lv_obj_t *splash_screen = lv_obj_create(NULL);
	
	/* Set background to black */
	lv_obj_set_style_bg_color(splash_screen, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(splash_screen, LV_OPA_COVER, 0);

	/* Create title label */
	lv_obj_t *title_label = lv_label_create(splash_screen);
	lv_label_set_text(title_label, "GZM Systems!");
	lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
	lv_obj_align(title_label, LV_ALIGN_CENTER, 0, -10);

    lv_scr_load(splash_screen);
#endif
    
}

/*
 * Basic widget
 */
void basic_widgets_ex2(void)
{
    /* Using current screen */
	/* Objeto pai */
    lv_obj_t *screen = lv_scr_act();
	lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

	/* objeto filho */
	lv_obj_t *obj = lv_obj_create(screen);
	lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
	lv_obj_set_width(obj, 100);
	lv_obj_set_height(obj, 100);
	lv_obj_set_x(obj, 50);
	lv_obj_set_y(obj, 50);
	display_update();

	/* Adiciona um label no objeto filho */
	lv_obj_t *label = lv_label_create(obj);
	lv_obj_set_style_text_color(label, lv_color_white(), 0);
	lv_label_set_text(label, "Hi!");
	lv_obj_set_y(label, 50);
	lv_obj_set_x(label, 10);
}

/*
 * Loading screen
 */
void basic_widgets_ex3(void)
{
    /* Using current screen */
	/* Objeto pai */
    lv_obj_t *screen = lv_scr_act();
	lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

	/* objeto filho */
	lv_obj_t *obj = lv_obj_create(screen);
	lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
	lv_obj_set_width(obj, 100);
	lv_obj_set_height(obj, 100);
	lv_obj_set_x(obj, 50);
	lv_obj_set_y(obj, 50);

	/* Executa comando pata atualizar tela */
	lv_timer_handler();
	k_sleep(K_SECONDS(3));

	/* Cria uma segunda tela */
	lv_obj_t *screen2 = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(screen2, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(screen2, LV_OPA_COVER, 0);

	lv_obj_t *label = lv_label_create(screen2);
	lv_obj_set_style_text_color(label, lv_color_white(), 0);
	lv_label_set_text(label, "Hi!");
	lv_obj_set_y(label, 50);
	lv_obj_set_x(label, 10);

	/* Carrega a segunda tela */
	lv_screen_load(screen2);
}

/*
 * Basic label example
 */
void basic_widgets_ex4(void)
{
	lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);
	lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);

	/* objeto filho */
	lv_obj_t *label = lv_label_create(lv_screen_active());
	lv_obj_set_style_bg_color(label, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);
	lv_label_set_text(label, "hello world!");

	lv_obj_t *label2 = lv_label_create(lv_screen_active());
	lv_obj_set_style_bg_color(label2, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(label2, LV_OPA_COVER, 0);
	lv_label_set_text_fmt(label2, "this is lable %d", 2);
	lv_obj_set_width(label2, 100);
	lv_obj_set_y(label2, 40);
	lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);

	lv_obj_t *label3 = lv_label_create(lv_screen_active());
	lv_obj_set_style_bg_color(label3, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(label3, LV_OPA_COVER, 0);
	lv_label_set_text_fmt(label3, "this is a very long message %d", 2);
	lv_obj_set_width(label3, 140);
	lv_obj_set_y(label3, 120);
	lv_obj_set_height(label3, 50);
	lv_label_set_long_mode(label3, LV_LABEL_LONG_SCROLL);
}

/*
 * Basic led
 */
void basic_widgets_ex5(void)
{
	/* Set background to white */
	lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), 0);
	lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);

  	lv_obj_t * led1  = lv_led_create(lv_screen_active());
    lv_obj_align(led1, LV_ALIGN_CENTER, -80, 0);
    lv_led_off(led1);

    /* Copy the previous LED and set a brightness */
    lv_obj_t * led2  = lv_led_create(lv_screen_active());
    lv_obj_align(led2, LV_ALIGN_CENTER, 0, 0);
    lv_led_set_brightness(led2, 150);
    lv_led_set_color(led2, lv_palette_main(LV_PALETTE_RED));

    /* Copy the previous LED and switch it ON */
    lv_obj_t * led3  = lv_led_create(lv_screen_active());
    lv_obj_align(led3, LV_ALIGN_CENTER, 80, 0);
    lv_led_on(led3);
}

/*
 * Basic line, este exemplo usa variaveis static
 */
void basic_widgets_ex6(void)
{
	/* Set background to white */
	lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), 0);
	lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);

	/* Desenhando um triangulo simples e sem efeito. */
	static lv_point_precise_t points[] = {{100,0}, {0,100}, {200,100}, {100,0}};
	lv_obj_t *line = lv_line_create(lv_screen_active());
	lv_line_set_points(line, points, 4);

	/* Desenhando um triango adicionando um style a linha, obs usar */
	static lv_point_precise_t points_2[] = {{100,100}, {0,200}, {200,200}, {100,100}};
	static lv_style_t line_style;
	lv_style_init(&line_style);
	lv_style_set_line_width(&line_style, 12);
	lv_style_set_line_color(&line_style, lv_palette_main(LV_PALETTE_INDIGO));
	lv_style_set_line_rounded(&line_style, true);

	lv_obj_t *line2 = lv_line_create(lv_screen_active());
	lv_line_set_points(line2, points_2, 4);
	lv_obj_add_style(line2, &line_style, 0);
}

/*
 * Basic bar
 */
void basic_widgets_ex7(void)
{
	/* Set background to white */
	lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), 0);
	lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);

	lv_obj_t *bar = lv_bar_create(lv_screen_active());
	lv_bar_set_value(bar, 50, LV_ANIM_ON); 
}

void basic_widgets_ex8(void)
{
	/* Set background to white */
	lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), 0);
	lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);

	lv_obj_t *bar = lv_bar_create(lv_screen_active());
	lv_bar_set_value(bar, 50, LV_ANIM_ON); 
}