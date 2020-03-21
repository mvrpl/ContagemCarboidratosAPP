#include "contcarboidratos.h"
#include <app_preference.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <Elementary.h>

const char* database = "/opt/usr/home/owner/data/config.db";

struct Pair {
  char *key;
  int value;
};

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	Evas_Object *confButton;
	Evas_Object *glicIn;
	Evas_Object *carbIn;
} appdata_s;

int glicIn, carbIn;

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
loads_prefs() {
	int callback(void *, int, char **, char **);
	sqlite3 *db;
	char *err = NULL;
	sqlite3_open(database, &db);
	/*char *droptb = "DROP TABLE IF EXISTS CONF;";
	sqlite3_exec(db, droptb, 0, 0, &err);*/
	char *sql = "CREATE TABLE IF NOT EXISTS CONF (row_id TINYINT PRIMARY KEY CHECK (row_id = 0),fator_sens INTEGER NOT NULL,uig INTEGER NOT NULL,glic_alv INTEGER NOT NULL);";
	sqlite3_exec(db, sql, 0, 0, &err);
	sqlite3_stmt* stmt = 0;
	sqlite3_prepare_v2(db, "SELECT fator_sens, uig, glic_alv FROM CONF", -1, &stmt, 0);
	sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0);
	while ( sqlite3_step(stmt) == SQLITE_ROW ) {
		for ( int colIndex = 0; colIndex < 3; colIndex++ ) {
			if (colIndex == 0){
				preference_set_int("fator_sens", sqlite3_column_int(stmt, colIndex));
			} else if (colIndex == 1) {
				preference_set_int("uig", sqlite3_column_int(stmt, colIndex));
			} else if (colIndex == 2) {
				preference_set_int("glic_alv", sqlite3_column_int(stmt, colIndex));
			}
		}
	}
	sqlite3_exec(db, "END TRANSACTION", 0, 0, &err);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

static void
set_pref(int vals[]) {
	sqlite3 *db;
	char *err = NULL;
	char sql[200];
	sprintf(sql, "INSERT OR REPLACE INTO CONF (row_id, fator_sens, uig, glic_alv) VALUES(0, %d, %d, %d);", vals[0], vals[1], vals[2]);
	sqlite3_open(database, &db);
	sqlite3_exec(db, sql, 0, 0, &err);
	sqlite3_close(db);
	struct Pair map[] = {{"fator_sens", vals[0]}, {"uig", vals[1]}, {"glic_alv", vals[2]}};
	int size = sizeof(map)/sizeof(map[0]);
	for (int i = 0; i < size; i++) {
		preference_set_int(map[i].key, map[i].value);
	}
}

static void
cal_uis(void *data, Evas_Object *obj, void *event_info) {
	appdata_s *ad = (appdata_s *) data;
	int glic_alv, uig, fator_sens;
	preference_get_int("glic_alv", &glic_alv);
	preference_get_int("uig", &uig);
	preference_get_int("fator_sens", &fator_sens);
	const char *valorGlic = elm_entry_entry_get(ad->glicIn);
	const char *valorCarb = elm_entry_entry_get(ad->carbIn);
	int glicIn = atoi(valorGlic);
	int carbIn = atoi(valorCarb);
	if (valorGlic != NULL && valorCarb != NULL){
		int result = ceil(((double)((float)glicIn - (float)glic_alv) / (float)fator_sens) + ((float)carbIn / (float)uig));
		char s[100];
		sprintf(s, "<align=center><font_size=60><color=#C0C0C0>%d</color></font_size></align><br>", result);
		elm_object_text_set(obj, s);
	}
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
	elm_table_homogeneous_set(table, EINA_TRUE);
	elm_object_content_set(ad->conform, table);
	evas_object_resize(table, 360, 360);
	evas_object_show(table);

	Evas_Object *editGlic = ad->glicIn = elm_entry_add(table);
	elm_entry_single_line_set(editGlic, EINA_TRUE);
	elm_entry_scrollable_set(editGlic, EINA_FALSE);
	elm_object_part_text_set(editGlic, "elm.guide", "<align=center>Glicemia</align>");
	elm_entry_input_panel_layout_set(editGlic,ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
	elm_table_pack(table, editGlic, 0, 0, 1, 2);
	elm_entry_text_style_user_push(editGlic, "DEFAULT='font=Tizen:style=Light font_size=50 color=#fff align=center'");
	evas_object_show(editGlic);

	Evas_Object *editCarb = ad->carbIn = elm_entry_add(table);
	elm_entry_single_line_set(editCarb, EINA_TRUE);
	elm_entry_scrollable_set(editCarb, EINA_FALSE);
	elm_object_part_text_set(editCarb, "elm.guide", "<align=center>Carboidratos</align>");
	elm_entry_input_panel_layout_set(editCarb,ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
	elm_table_pack(table, editCarb, 0, 1, 2, 1);
	elm_entry_text_style_user_push(editCarb, "DEFAULT='font=Tizen:style=Light font_size=50 color=#fff align=center'");
	evas_object_show(editCarb);

	//ad->confButton = elm_button_add(ad->conform);
	//evas_object_resize(ad->confButton, 360, 360 / 4);
	//elm_object_style_set(ad->confButton, "bottom");
	//elm_object_text_set(ad->confButton, "Config.");
	//evas_object_smart_callback_add(ad->confButton, "clicked", btn_clicked_cb, ad);
	//evas_object_show(ad->confButton);

	/* BUTTON */
	int confs[] = {10, 10, 90};
	set_pref(confs);

	char s[100];
	sprintf(s, "<align=center><font_size=60><color=#C0C0C0>0</color></font_size></align><br>");
	/* BUTTON */

	ad->label = elm_label_add(table);
	elm_object_text_set(ad->label, s);
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_table_pack(table, ad->label, 0, 2, 1, 2);
	evas_object_smart_callback_add(ad->label, "clicked", cal_uis, ad);
	evas_object_show(ad->label);
	//elm_object_content_set(ad->conform, ad->label);

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
	loads_prefs();

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
