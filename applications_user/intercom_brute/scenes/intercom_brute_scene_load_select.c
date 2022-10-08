#include "../intercom_brute_i.h"
#include "../intercom_brute_custom_event.h"
#include "../views/intercom_brute_main_view.h"

#define TAG "IntercomBruteSceneStart"

void intercom_brute_scene_load_select_callback(IntercomBruteCustomEvent event, void* context) {
    furi_assert(context);

    IntercomBruteState* instance = (IntercomBruteState*)context;
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "intercom_brute_scene_load_select_callback");
#endif
    view_dispatcher_send_custom_event(instance->view_dispatcher, event);
}

void intercom_brute_scene_load_select_on_enter(void* context) {
    furi_assert(context);
#ifdef FURI_DEBUG
    FURI_LOG_I(TAG, "intercom_brute_scene_load_select_on_enter");
#endif
    IntercomBruteState* instance = (IntercomBruteState*)context;
    IntercomBruteMainView* view = instance->view_main;

    instance->current_view = IntercomBruteViewMain;
    intercom_brute_main_view_set_callback(view, intercom_brute_scene_load_select_callback, instance);
    intercom_brute_main_view_set_index(view, 7, true, instance->device->file_key);

    view_dispatcher_switch_to_view(instance->view_dispatcher, instance->current_view);
}

void intercom_brute_scene_load_select_on_exit(void* context) {
    UNUSED(context);
#ifdef FURI_DEBUG
    FURI_LOG_I(TAG, "intercom_brute_scene_load_select_on_exit");
#endif
}

bool intercom_brute_scene_load_select_on_event(void* context, SceneManagerEvent event) {
    IntercomBruteState* instance = (IntercomBruteState*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == IntercomBruteCustomEventTypeIndexSelected) {
            instance->device->load_index = intercom_brute_main_view_get_index(instance->view_main);
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "load_index: %d", instance->device->load_index);
#endif
            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneSetupAttack);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        if(!scene_manager_search_and_switch_to_previous_scene(
               instance->scene_manager, IntercomBruteSceneStart)) {
            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneStart);
        }
        consumed = true;
    }

    return consumed;
}