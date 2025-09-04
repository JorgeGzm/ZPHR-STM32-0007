#include "main_screen.h"
#include <lvgl.h>
#include <zephyr/kernel.h>

static void main_screen_event_cb(lv_event_t *e);

/* Main screen */
static lv_obj_t *main_screen_container;
static lv_obj_t *heartbeat;
static lv_obj_t *capacity_label;
static lv_obj_t *load_label;
static lv_obj_t *gn_label;
static lv_obj_t *unit_label;
static lv_obj_t *id_label;
static lv_obj_t *connection_label;
static lv_obj_t *rbat_label;
static lv_obj_t *pilbat_label;
static lv_obj_t *bbat_label;
static lv_obj_t *rssi_label;
static lv_obj_t *bbat_value;
static lv_obj_t *pilbat_value;
static lv_obj_t *rbat_value;
static lv_obj_t *rssi_value;

/* Style*/
static lv_style_t style_amber;
static lv_style_t style_blue;
static lv_style_t style_red;
static lv_style_t style_black;

static void main_screen_event_cb(lv_event_t *e) {

    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
        case LV_EVENT_SCREEN_LOADED:
            break;
            
        case LV_EVENT_SCREEN_UNLOADED:
            break;
            
        default:
            break;
    }
}

void main_screen_create(void)
{
    /* PRIMEIRO: Inicializar estilos */
    lv_style_init(&style_amber);
    lv_style_init(&style_blue);
    lv_style_init(&style_red);
    lv_style_init(&style_black);

    lv_style_set_text_color(&style_amber, lv_palette_main(LV_PALETTE_AMBER));
    lv_style_set_text_color(&style_red, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_text_color(&style_blue, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_color(&style_black, lv_color_black());

    /* Criar container principal */
    main_screen_container = lv_obj_create(NULL);
    
    /* Configurar fundo (opcional) */
    lv_obj_set_style_bg_color(main_screen_container, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(main_screen_container, LV_OPA_COVER, 0);

    /* Adicionar event callbacks */
    lv_obj_add_event_cb(main_screen_container, main_screen_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
    lv_obj_add_event_cb(main_screen_container, main_screen_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);

    /* heartbeat indication */
    heartbeat = lv_label_create(main_screen_container);
    lv_label_set_recolor(heartbeat, true);
    lv_obj_add_style(heartbeat, &style_red, 0);  // Agora o estilo já foi inicializado
    lv_label_set_text(heartbeat, ".");
    lv_obj_align(heartbeat, LV_ALIGN_TOP_MID, 0, 2);

    /* Load label */
    load_label = lv_label_create(main_screen_container);
    lv_label_set_recolor(load_label, true);
    
    /* Verificar se a fonte está disponível */
    #if LV_FONT_MONTSERRAT_48
    lv_obj_set_style_text_font(load_label, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
    #else
    /* Use fonte padrão se Montserrat 48 não estiver disponível */
    lv_obj_set_style_text_font(load_label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    #endif
    
    lv_label_set_text(load_label, "");
    lv_label_set_long_mode(load_label, LV_LABEL_LONG_CLIP);
    lv_obj_align(load_label, LV_ALIGN_TOP_RIGHT, 0, 40);
    lv_obj_set_width(load_label, 230);
    lv_obj_set_style_text_align(load_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* capacity label */
    capacity_label = lv_label_create(main_screen_container);
    lv_label_set_recolor(capacity_label, true);
    lv_label_set_text(capacity_label, "---");
    lv_obj_align(capacity_label, LV_ALIGN_TOP_LEFT, 9, 10);

    /* Gross Net label */
    gn_label = lv_label_create(main_screen_container);
    lv_label_set_recolor(gn_label, true);
    lv_label_set_text(gn_label, "#000000 gross#");
    lv_obj_align(gn_label, LV_ALIGN_TOP_LEFT, 3, 100);

    /* Unit Net label */
    unit_label = lv_label_create(main_screen_container);
    lv_label_set_recolor(unit_label, true);
    lv_label_set_text(unit_label, "#255A9B t#");
    lv_obj_align(unit_label, LV_ALIGN_TOP_RIGHT, -3, 100);
    
    /* Battery level - Display Batt 1 */
    pilbat_label = lv_label_create(main_screen_container);
    lv_label_set_recolor(pilbat_label, true);
    lv_label_set_text(pilbat_label, " Display Batt 1:");
    lv_obj_align(pilbat_label, LV_ALIGN_TOP_LEFT, 3, 144);

    pilbat_value = lv_label_create(main_screen_container);
    lv_label_set_recolor(pilbat_value, true);
    lv_label_set_text(pilbat_value, "0");
    lv_obj_align(pilbat_value, LV_ALIGN_TOP_RIGHT, -3, 144);

    /* Battery level - Batt 2 */
    bbat_label = lv_label_create(main_screen_container);
    lv_label_set_recolor(bbat_label, true);
    lv_label_set_text(bbat_label, " Batt 2:");
    lv_obj_set_style_text_align(bbat_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(bbat_label, LV_ALIGN_TOP_MID, -10, 170);
    lv_obj_set_width(bbat_label, 130);

    bbat_value = lv_label_create(main_screen_container);
    lv_label_set_recolor(bbat_value, true);
    lv_label_set_text(bbat_value, "0");
    lv_obj_align(bbat_value, LV_ALIGN_TOP_RIGHT, -3, 170);

    /* Sensor Battery level */
    rbat_label = lv_label_create(main_screen_container);
    lv_label_set_recolor(rbat_label, true);
    lv_label_set_text(rbat_label, " Sensor Batt:");
    lv_obj_align(rbat_label, LV_ALIGN_TOP_LEFT, 23, 196);

    rbat_value = lv_label_create(main_screen_container);
    lv_label_set_recolor(rbat_value, true);
    lv_label_set_text(rbat_value, "0");
    lv_obj_align(rbat_value, LV_ALIGN_TOP_RIGHT, -3, 196);

    /* Signal */
    rssi_label = lv_label_create(main_screen_container);
    lv_label_set_recolor(rssi_label, true);
    lv_label_set_text(rssi_label, " Signal:");
    lv_obj_set_style_text_align(rssi_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(rssi_label, LV_ALIGN_TOP_MID, -10, 222);
    lv_obj_set_width(rssi_label, 130);

    rssi_value = lv_label_create(main_screen_container);
    lv_label_set_recolor(rssi_value, true);
    lv_label_set_text(rssi_value, "0");
    lv_obj_align(rssi_value, LV_ALIGN_TOP_RIGHT, -3, 222);

    /* ID Net label */
    id_label = lv_label_create(main_screen_container);
    lv_label_set_recolor(id_label, true);
    lv_label_set_text(id_label, "Paired ID:#255A9B  No Cell#");
    lv_obj_align(id_label, LV_ALIGN_TOP_MID, 0, 250);

    /* Sensor connection status label */
    connection_label = lv_label_create(main_screen_container);
    lv_label_set_recolor(connection_label, true);
    lv_label_set_text(connection_label, "#ff0000 Disconnected#");
    lv_obj_align(connection_label, LV_ALIGN_BOTTOM_MID, 0, -10);

    printk("SCREEN_MAIN screen created\r\n");
}

void main_screen_show(void) {
    if (main_screen_container) {
        lv_scr_load(main_screen_container);
        printk("show main screen \r\n");
    }
}
