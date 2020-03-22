#include "contcarboidratos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Elementary.h>

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *winConf;
	Evas_Object *conform;
	Evas_Object *conformConf;
	Evas_Object *label;
	Evas_Object *confButton;
	Evas_Object *glicIn;
	Evas_Object *carbIn;
	Evas_Object *glicAlv;
	Evas_Object *fatSens;
	Evas_Object *uig;
} appdata_s;

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

static void
cal_uis(void *data, Evas_Object *obj, void *event_info) {
	appdata_s *ad = (appdata_s *) data;

	const char *valorGlicAlv = elm_entry_entry_get(ad->glicAlv);
	const char *valorFatSens = elm_entry_entry_get(ad->fatSens);
	const char *valorUig = elm_entry_entry_get(ad->uig);
	int glic_alv = atoi(valorGlicAlv);
	int fator_sens = atoi(valorFatSens);
	int uig = atoi(valorUig);

	const char *valorGlic = elm_entry_entry_get(ad->glicIn);
	const char *valorCarb = elm_entry_entry_get(ad->carbIn);
	int glicIn = atoi(valorGlic);
	int carbIn = atoi(valorCarb);

	if (glicIn > 0 && carbIn >= 0){
		int result = ceil(((double)((float)glicIn - (float)glic_alv) / (float)fator_sens) + ((float)carbIn / (float)uig));
		char s[100];
		sprintf(s, "<align=center><font_size=45><color=#FFA500>%d</color></font_size></align><br>", result);
		elm_object_text_set(obj, s);
	} else {
		char s[100];
		sprintf(s, "<align=center><font_size=45><color=#C0C0C0>INSULINA UI</color></font_size></align><br>");
		elm_object_text_set(obj, s);
	}
}

static void
btn_clicked_cb(void *data, Evas_Object *obj, void *event_info) {
	appdata_s *ad = (appdata_s *) data;
	evas_object_show(ad->winConf);
}

static void
btn_clicked_ok_cb(void *data, Evas_Object *obj, void *event_info) {
	appdata_s *ad = (appdata_s *) data;
	evas_object_hide(ad->winConf);
}

static void
key_down(void *data, Evas *e, Evas_Object *obj, void *event_info) {
	Evas_Event_Key_Down *event = event_info;
	const char * key = "Return";
	if (strcmp(event->keyname, key) == 0) {
		elm_entry_input_panel_hide(obj);
	}
}

static void
create_conf_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->winConf = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->winConf, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->winConf)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->winConf, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->winConf, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->winConf, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conformConf = elm_conformant_add(ad->winConf);
	elm_win_indicator_mode_set(ad->winConf, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->winConf, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conformConf, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->winConf, ad->conformConf);
	evas_object_show(ad->conformConf);

	Evas_Object* table = elm_table_add(ad->conformConf);
	elm_table_homogeneous_set(table, EINA_FALSE);
	elm_object_content_set(ad->conformConf, table);
	evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(table, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_pack_end(ad->conformConf, table);
	evas_object_show(table);

	Evas_Object* exitConf = elm_button_add(table);
	elm_object_style_set(exitConf, "bottom/queue");
	elm_object_text_set(exitConf, "OK");
	elm_table_pack(table, exitConf, 0, 0, 1, 1);
	evas_object_smart_callback_add(exitConf, "clicked", btn_clicked_ok_cb, ad);
	evas_object_show(exitConf);

	static Elm_Entry_Filter_Limit_Size limit_size = {
		   .max_char_count = 3,
		   .max_byte_count = 0
   	};

   	char s1[100];
   	sprintf(s1, "<align=center><font_size=20><color=#C0C0C0>Fator Sensibilidade</color></font_size></align>");
   	Evas_Object *labelFatSens = elm_label_add(table);
   	elm_object_text_set(labelFatSens, s1);
   	evas_object_size_hint_weight_set(labelFatSens, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   	elm_table_pack(table, labelFatSens, 0, 1, 1, 1);
   	evas_object_show(labelFatSens);

	Evas_Object *editFatSens = ad->fatSens = elm_entry_add(table);
	elm_entry_single_line_set(editFatSens, EINA_TRUE);
	elm_entry_scrollable_set(editFatSens, EINA_FALSE);
	elm_entry_markup_filter_append(editFatSens, elm_entry_filter_limit_size, &limit_size);
	evas_object_size_hint_weight_set(editFatSens, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(editFatSens, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_entry_text_style_user_push(editFatSens, "DEFAULT='font=Tizen:style=Light font_size=50 color=#fff align=center'");
	elm_entry_input_panel_layout_set(editFatSens,ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
	evas_object_event_callback_add(editFatSens, EVAS_CALLBACK_KEY_DOWN, key_down, NULL);
	elm_entry_file_set(editFatSens, "/opt/usr/home/owner/data/contcarb_fs.cc", ELM_TEXT_FORMAT_MARKUP_UTF8);
	elm_entry_autosave_set(editFatSens, EINA_TRUE);
	elm_table_pack(table, editFatSens, 0, 2, 1, 1);
	evas_object_show(editFatSens);

   	char s2[100];
   	sprintf(s2, "<align=center><font_size=20><color=#C0C0C0>1UI Gramas Carb.</color></font_size></align>");
   	Evas_Object *labelGCarb = elm_label_add(table);
   	elm_object_text_set(labelGCarb, s2);
   	evas_object_size_hint_weight_set(labelGCarb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   	elm_table_pack(table, labelGCarb, 0, 3, 1, 1);
   	evas_object_show(labelGCarb);

	Evas_Object *editGCarb = ad->uig = elm_entry_add(table);
	elm_entry_single_line_set(editGCarb, EINA_TRUE);
	elm_entry_scrollable_set(editGCarb, EINA_FALSE);
	elm_entry_markup_filter_append(editGCarb, elm_entry_filter_limit_size, &limit_size);
	evas_object_size_hint_weight_set(editGCarb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(editGCarb, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_entry_text_style_user_push(editGCarb, "DEFAULT='font=Tizen:style=Light font_size=50 color=#fff align=center'");
	elm_entry_input_panel_layout_set(editGCarb,ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
	evas_object_event_callback_add(editGCarb, EVAS_CALLBACK_KEY_DOWN, key_down, NULL);
	elm_entry_file_set(editGCarb, "/opt/usr/home/owner/data/contcarb_ui.cc", ELM_TEXT_FORMAT_MARKUP_UTF8);
	elm_entry_autosave_set(editGCarb, EINA_TRUE);
	elm_table_pack(table, editGCarb, 0, 4, 1, 1);
	evas_object_show(editGCarb);

   	char s[100];
   	sprintf(s, "<align=center><font_size=20><color=#C0C0C0>Glicemia Alvo</color></font_size></align>");
   	Evas_Object *labelGlicAlv = elm_label_add(table);
   	elm_object_text_set(labelGlicAlv, s);
   	evas_object_size_hint_weight_set(labelGlicAlv, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   	elm_table_pack(table, labelGlicAlv, 0, 5, 1, 1);
   	evas_object_show(labelGlicAlv);

	Evas_Object *editGlicAlv = ad->glicAlv = elm_entry_add(table);
	elm_entry_single_line_set(editGlicAlv, EINA_TRUE);
	elm_entry_scrollable_set(editGlicAlv, EINA_FALSE);
	elm_entry_markup_filter_append(editGlicAlv, elm_entry_filter_limit_size, &limit_size);
	evas_object_size_hint_weight_set(editGlicAlv, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(editGlicAlv, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_entry_text_style_user_push(editGlicAlv, "DEFAULT='font=Tizen:style=Light font_size=50 color=#fff align=center'");
	elm_entry_input_panel_layout_set(editGlicAlv,ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
	evas_object_event_callback_add(editGlicAlv, EVAS_CALLBACK_KEY_DOWN, key_down, NULL);
	elm_entry_file_set(editGlicAlv, "/opt/usr/home/owner/data/contcarb_ga.cc", ELM_TEXT_FORMAT_MARKUP_UTF8);
	elm_entry_autosave_set(editGlicAlv, EINA_TRUE);
	elm_table_pack(table, editGlicAlv, 0, 6, 1, 1);
	evas_object_show(editGlicAlv);
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Label */
	/* Create an actual view of the base gui.
	   Modify this part to change the view. */

	Evas_Object* table = elm_table_add(ad->conform);
	elm_table_homogeneous_set(table, EINA_FALSE);
	elm_object_content_set(ad->conform, table);
	elm_table_padding_set(table, 5 * elm_config_scale_get(), 10 * elm_config_scale_get());
	evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(table, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_pack_end(ad->conform, table);
	evas_object_show(table);

	ad->confButton = elm_button_add(table);
	elm_object_style_set(ad->confButton, "bottom/queue");
	elm_object_text_set(ad->confButton, "Config");
	elm_table_pack(table, ad->confButton, 0, 0, 1, 1);
	evas_object_smart_callback_add(ad->confButton, "clicked", btn_clicked_cb, ad);
	evas_object_show(ad->confButton);

	static Elm_Entry_Filter_Limit_Size limit_size = {
		   .max_char_count = 3,
		   .max_byte_count = 0
	};

   	char s1[100];
   	sprintf(s1, "<align=center><font_size=20><color=#C0C0C0>Glicemia Agora</color></font_size></align>");
   	Evas_Object *labelGlic = elm_label_add(table);
   	elm_object_text_set(labelGlic, s1);
   	evas_object_size_hint_weight_set(labelGlic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   	elm_table_pack(table, labelGlic, 0, 1, 1, 1);
   	evas_object_show(labelGlic);

	Evas_Object *editGlic = ad->glicIn = elm_entry_add(table);
	elm_entry_single_line_set(editGlic, EINA_TRUE);
	elm_entry_scrollable_set(editGlic, EINA_FALSE);
	elm_entry_markup_filter_append(editGlic, elm_entry_filter_limit_size, &limit_size);
	evas_object_size_hint_weight_set(editGlic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(editGlic, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_entry_text_style_user_push(editGlic, "DEFAULT='font=Tizen:style=Light font_size=35 color=#fff align=center'");
	elm_entry_input_panel_layout_set(editGlic,ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
	evas_object_event_callback_add(editGlic, EVAS_CALLBACK_KEY_DOWN, key_down, NULL);
	elm_table_pack(table, editGlic, 0, 2, 1, 1);
	evas_object_show(editGlic);

   	char s2[100];
   	sprintf(s2, "<align=center><font_size=20><color=#C0C0C0>Gramas Carboidratos</color></font_size></align>");
   	Evas_Object *labelGCarb = elm_label_add(table);
   	elm_object_text_set(labelGCarb, s2);
   	evas_object_size_hint_weight_set(labelGCarb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   	elm_table_pack(table, labelGCarb, 0, 3, 1, 1);
   	evas_object_show(labelGCarb);

	Evas_Object *editCarb = ad->carbIn = elm_entry_add(table);
	elm_entry_single_line_set(editCarb, EINA_TRUE);
	elm_entry_scrollable_set(editCarb, EINA_FALSE);
	elm_entry_markup_filter_append(editCarb, elm_entry_filter_limit_size, &limit_size);
	evas_object_size_hint_weight_set(editCarb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(editCarb, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_entry_text_style_user_push(editCarb, "DEFAULT='font=Tizen:style=Light font_size=35 color=#fff align=center'");
	elm_entry_input_panel_layout_set(editCarb,ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
	evas_object_event_callback_add(editCarb, EVAS_CALLBACK_KEY_DOWN, key_down, NULL);
	elm_table_pack(table, editCarb, 0, 4, 1, 1);
	evas_object_show(editCarb);

	char s[100];
	sprintf(s, "<align=center><font_size=45><color=#C0C0C0>INSULINA UI</color></font_size></align><br>");

	ad->label = elm_label_add(table);
	elm_object_text_set(ad->label, s);
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ad->label, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_table_pack(table, ad->label, 0, 5, 1, 1);
	evas_object_smart_callback_add(ad->label, "clicked", cal_uis, ad);
	evas_object_show(ad->label);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);
	create_conf_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
