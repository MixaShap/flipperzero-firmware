#include "../intercom_brute_i.h"
#include "../intercom_brute_custom_event.h"
#include "../views/intercom_brute_main_view.h"

#define TAG "IntercomBruteSceneStart"

void intercom_brute_scene_start_callback(IntercomBruteCustomEvent event, void* context) {
    furi_assert(context);

    IntercomBruteState* instance = (IntercomBruteState*)context;
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "intercom_brute_scene_start_callback");
#endif
    view_dispatcher_send_custom_event(instance->view_dispatcher, event);
}

void intercom_brute_scene_start_on_enter(void* context) {
    furi_assert(context);
#ifdef FURI_DEBUG
    FURI_LOG_I(TAG, "intercom_brute_scene_start_on_enter");
#endif
    IntercomBruteState* instance = (IntercomBruteState*)context;
    IntercomBruteMainView* view = instance->view_main;

    instance->current_view = IntercomBruteViewMain;
    intercom_brute_main_view_set_callback(view, intercom_brute_scene_start_callback, instance);
    intercom_brute_main_view_set_index(view, instance->device->attack, false, NULL);

    view_dispatcher_switch_to_view(instance->view_dispatcher, instance->current_view);
}

void intercom_brute_scene_start_on_exit(void* context) {
    UNUSED(context);
#ifdef FURI_DEBUG
    FURI_LOG_I(TAG, "intercom_brute_scene_start_on_exit");
#endif
}

bool intercom_brute_scene_start_on_event(void* context, SceneManagerEvent event) {
    IntercomBruteState* instance = (IntercomBruteState*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
#ifdef FURI_DEBUG
        FURI_LOG_D(TAG, "Event: %d", event.event);
#endif
        if(event.event == IntercomBruteCustomEventTypeMenuSelected) {
            IntercomBruteAttacks attack = intercom_brute_main_view_get_index(instance->view_main);

            intercom_brute_device_attack_set(instance->device, attack);
            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneSetupAttack);

            consumed = true;
        } else if(event.event == IntercomBruteCustomEventTypeLoadFile) {
            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneLoadFile);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(instance->scene_manager);
        view_dispatcher_stop(instance->view_dispatcher);
        consumed = true;
    }

    return consumed;
}