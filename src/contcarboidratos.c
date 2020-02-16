#include "contcarboidratos.h"
#include <app_preference.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

const char* database = "/opt/usr/home/owner/data/config.db";

/* C-DICT */
typedef struct dict_t_struct {
    char *key;
    int value;
    struct dict_t_struct *next;
} dict_t;

dict_t **dictAlloc(void) {
    return malloc(sizeof(dict_t));
}

void dictDealloc(dict_t **dict) {
    free(dict);
}

int getItem(dict_t *dict, char *key) {
    dict_t *ptr;
    for (ptr = dict; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            return ptr->value;
        }
    }

    return 0;
}

void delItem(dict_t **dict, char *key) {
    dict_t *ptr, *prev;
    for (ptr = *dict, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            if (ptr->next != NULL) {
                if (prev == NULL) {
                    *dict = ptr->next;
                } else {
                    prev->next = ptr->next;
                }
            } else if (prev != NULL) {
                prev->next = NULL;
            } else {
                *dict = NULL;
            }

            free(ptr->key);
            free(ptr);

            return;
        }
    }
}

void addItem(dict_t **dict, char *key, int value) {
    delItem(dict, key); /* If we already have a item with this key, delete it. */
    dict_t *d = malloc(sizeof(struct dict_t_struct));
    d->key = malloc(strlen(key)+1);
    strcpy(d->key, key);
    d->value = value;
    d->next = *dict;
    *dict = d;
}
/* C-DICT */

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
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
loads_prefs() {
	int callback(void *, int, char **, char **);
	sqlite3 *db;
	char *err = NULL;
	sqlite3_open(database, &db);
	/*char *droptb = "DROP TABLE IF EXISTS CONF;";
	sqlite3_exec(db, droptb, 0, 0, &err);*/
	char *sql = "CREATE TABLE IF NOT EXISTS CONF (row_id TINYINT PRIMARY KEY CHECK (row_id = 0),fator_sens INTEGER NOT NULL,uig INTEGER NOT NULL,glic_alv INTEGER NOT NULL);";
	sqlite3_exec(db, sql, 0, 0, &err);
	sqlite3_exec(db, "SELECT fator_sens, uig, glic_alv FROM CONF", callback, 0, &err);
	sqlite3_close(db);
}

int callback(void *NotUsed, int argc, char **argv, char **colNames){
	for(int i=0; i<argc; i++){
		preference_set_int(colNames[i], (int)argv[i]);
	}
	return 0;
}

static void
set_pref(dict_t dict) {
	sqlite3 *db;
	char *err = NULL;
	char sql[200];
	sprintf(sql, "INSERT OR REPLACE INTO CONF (row_id, fator_sens, uig, glic_alv) VALUES(0, %d, %d, %d);", getItem(&dict, "fator_sens"), getItem(&dict, "uig"), getItem(&dict, "glic_alv"));
	sqlite3_open(database, &db);
	sqlite3_exec(db, sql, 0, 0, &err);
	sqlite3_close(db);
	char *keys[] = {"fator_sens","uig","glic_alv",NULL}, **n;
	n = keys;
	while (*n != NULL) {
		char *key = *n++;
		preference_set_int(n++, getItem(&dict, key));
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
	ad->label = elm_label_add(ad->conform);

	/* BUTTON */
	dict_t **dict = dictAlloc();
	addItem(dict, "fator_sens", 10);
	addItem(dict, "uig", 10);
	addItem(dict, "glic_alv", 90);
	set_pref(**dict);
	dictDealloc(dict);

	const char *glic_alv = "glic_alv";
	int integer_output;
	preference_get_int(glic_alv, &integer_output);
	char s[100];
	sprintf(s, "<align=center>Hello %d!</align><br>", integer_output);
	/* BUTTON */

	elm_object_text_set(ad->label, s);
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform, ad->label);

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
