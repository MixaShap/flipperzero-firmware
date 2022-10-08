#include "../intercom_brute_i.h"
#include "../intercom_brute_custom_event.h"
#include <lib/subghz/protocols/registry.h>

#define TAG "IntercomBruteSceneLoadFile"

//void intercom_brute_scene_load_file_callback(IntercomBruteCustomEvent event, void* context) {
////    furi_assert(context);
////
////    IntercomBruteState* instance = (IntercomBruteState*)context;
////    view_dispatcher_send_custom_event(instance->view_dispatcher, event);
//}

void intercom_brute_scene_load_file_on_enter(void* context) {
    furi_assert(context);
    IntercomBruteState* instance = (IntercomBruteState*)context;

    // Input events and views are managed by file_browser
    FuriString* app_folder;
    FuriString* load_path;
    load_path = furi_string_alloc();
    app_folder = furi_string_alloc_set(INTERCOM_BRUTE_PATH);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, INTERCOM_BRUTE_FILE_EXT, &I_sub1_10px);

    IntercomBruteFileResult load_result = IntercomBruteFileResultUnknown;
    bool res =
        dialog_file_browser_show(instance->dialogs, load_path, app_folder, &browser_options);
#ifdef FURI_DEBUG
    FURI_LOG_D(
        TAG,
        "load_path: %s, app_folder: %s",
        furi_string_get_cstr(load_path),
        furi_string_get_cstr(app_folder));
#endif
    if(res) {
        load_result = intercom_brute_device_load_from_file(instance->device, load_path);
        if(load_result == IntercomBruteFileResultOk) {
            load_result = intercom_brute_device_attack_set(instance->device, IntercomBruteAttackLoadFile);
            if(load_result == IntercomBruteFileResultOk) {
                // Ready to run!
                instance->device->state = IntercomBruteDeviceStateReady;
                FURI_LOG_I(TAG, "Ready to run");
                res = true;
            }
        }

        if(load_result == IntercomBruteFileResultOk) {
            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneLoadSelect);
        } else {
            FURI_LOG_E(TAG, "Returned error: %d", load_result);

            FuriString* dialog_msg;
            dialog_msg = furi_string_alloc();
            furi_string_cat_printf(
                dialog_msg, "Cannot parse\nfile: %s", intercom_brute_device_error_get_desc(load_result));
            dialog_message_show_storage_error(instance->dialogs, furi_string_get_cstr(dialog_msg));
            furi_string_free(dialog_msg);
            scene_manager_search_and_switch_to_previous_scene(
                instance->scene_manager, IntercomBruteSceneStart);
        }
    } else {
        scene_manager_search_and_switch_to_previous_scene(
            instance->scene_manager, IntercomBruteSceneStart);
    }

    furi_string_free(app_folder);
    furi_string_free(load_path);
}

void intercom_brute_scene_load_file_on_exit(void* context) {
    UNUSED(context);
}

bool intercom_brute_scene_load_file_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}