#include "../intercom_brute_i.h"
#include "../intercom_brute_custom_event.h"
#include "../views/intercom_brute_attack_view.h"

#define TAG "IntercomBruteSceneSetupAttack"

static void intercom_brute_scene_setup_attack_callback(IntercomBruteCustomEvent event, void* context) {
    furi_assert(context);

    IntercomBruteState* instance = (IntercomBruteState*)context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, event);
}

void intercom_brute_scene_setup_attack_on_enter(void* context) {
    furi_assert(context);
    IntercomBruteState* instance = (IntercomBruteState*)context;
    IntercomBruteAttackView* view = instance->view_attack;

#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "Enter Attack: %d", instance->device->attack);
#endif

    intercom_brute_attack_view_init_values(
        view,
        instance->device->attack,
        instance->device->max_value,
        instance->device->key_index,
        false);

    if(!intercom_brute_worker_init_manual_transmit(
           instance->worker,
           instance->device->frequency,
           instance->device->preset,
           furi_string_get_cstr(instance->device->protocol_name))) {
        FURI_LOG_W(TAG, "Worker init failed!");
    }

    instance->current_view = IntercomBruteViewAttack;
    intercom_brute_attack_view_set_callback(view, intercom_brute_scene_setup_attack_callback, instance);
    view_dispatcher_switch_to_view(instance->view_dispatcher, instance->current_view);
}

void intercom_brute_scene_setup_attack_on_exit(void* context) {
    furi_assert(context);
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "intercom_brute_scene_setup_attack_on_exit");
#endif
    IntercomBruteState* instance = (IntercomBruteState*)context;
    intercom_brute_worker_manual_transmit_stop(instance->worker);
    notification_message(instance->notifications, &sequence_blink_stop);
}

bool intercom_brute_scene_setup_attack_on_event(void* context, SceneManagerEvent event) {
    IntercomBruteState* instance = (IntercomBruteState*)context;
    IntercomBruteAttackView* view = instance->view_attack;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == IntercomBruteCustomEventTypeTransmitStarted) {
            intercom_brute_worker_set_continuous_worker(instance->worker, false);
            intercom_brute_attack_view_set_worker_type(view, false);
            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneRunAttack);
        } else if(event.event == IntercomBruteCustomEventTypeTransmitContinuousStarted) {
            // Setting different type of worker
            intercom_brute_worker_set_continuous_worker(instance->worker, true);
            intercom_brute_attack_view_set_worker_type(view, true);
            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneRunAttack);
        } else if(event.event == IntercomBruteCustomEventTypeSaveFile) {
            intercom_brute_worker_manual_transmit_stop(instance->worker);

            intercom_brute_attack_view_init_values(
                view,
                instance->device->attack,
                instance->device->max_value,
                instance->device->key_index,
                false);
            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneSaveName);
        } else if(event.event == IntercomBruteCustomEventTypeBackPressed) {
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "IntercomBruteCustomEventTypeBackPressed");
#endif
            instance->device->key_index = 0x00;
            //intercom_brute_attack_view_stop_worker(view);
            intercom_brute_attack_view_init_values(
                view,
                instance->device->attack,
                instance->device->max_value,
                instance->device->key_index,
                false);
            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneStart);
        } else if(event.event == IntercomBruteCustomEventTypeChangeStepUp) {
            // +1
            if((instance->device->key_index + 1) - instance->device->max_value == 1) {
                instance->device->key_index = 0x00;
            } else {
                uint64_t value = instance->device->key_index + 1;
                if(value == instance->device->max_value) {
                    instance->device->key_index = value;
                } else {
                    instance->device->key_index = value % instance->device->max_value;
                }
            }
            intercom_brute_attack_view_set_current_step(view, instance->device->key_index);
        } else if(event.event == IntercomBruteCustomEventTypeChangeStepUpMore) {
            // +50
            uint64_t value = instance->device->key_index + 50;
            if(value == instance->device->max_value) {
                instance->device->key_index += value;
            } else {
                instance->device->key_index = value % instance->device->max_value;
            }
            intercom_brute_attack_view_set_current_step(view, instance->device->key_index);
        } else if(event.event == IntercomBruteCustomEventTypeChangeStepDown) {
            // -1
            if(instance->device->key_index - 1 == 0) {
                instance->device->key_index = 0x00;
            } else if(instance->device->key_index == 0) {
                instance->device->key_index = instance->device->max_value;
            } else {
                uint64_t value = ((instance->device->key_index - 1) + instance->device->max_value);
                if(value == instance->device->max_value) {
                    instance->device->key_index = value;
                } else {
                    instance->device->key_index = value % instance->device->max_value;
                }
            }
            intercom_brute_attack_view_set_current_step(view, instance->device->key_index);
        } else if(event.event == IntercomBruteCustomEventTypeChangeStepDownMore) {
            // -50
            uint64_t value = ((instance->device->key_index - 50) + instance->device->max_value);
            if(value == instance->device->max_value) {
                instance->device->key_index = value;
            } else {
                instance->device->key_index = value % instance->device->max_value;
            }
            intercom_brute_attack_view_set_current_step(view, instance->device->key_index);
        } else if(event.event == IntercomBruteCustomEventTypeTransmitCustom) {
            if(intercom_brute_worker_can_manual_transmit(instance->worker, true)) {
                // Blink
                notification_message(instance->notifications, &sequence_blink_green_100);

                //                if(!intercom_brute_attack_view_is_worker_running(view)) {
                //                    intercom_brute_attack_view_start_worker(
                //                        view,
                //                        instance->device->frequency,
                //                        instance->device->preset,
                //                        furi_string_get_cstr(instance->device->protocol_name));
                //                }
                intercom_brute_device_create_packet_parsed(
                    instance->device, instance->device->key_index, false);
                intercom_brute_worker_manual_transmit(instance->worker, instance->device->payload);

                // Stop
                notification_message(instance->notifications, &sequence_blink_stop);
            }
        }

        consumed = true;
    }

    //    if(event.type == SceneManagerEventTypeCustom) {
    //        switch(event.event) {
    //        case IntercomBruteCustomEventTypeMenuSelected:
    //            with_view_model(
    //                view, (IntercomBruteMainViewModel * model) {
    //                    instance->menu_index = model->index;
    //                    return false;
    //                });
    //            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneLoadFile);
    //            consumed = true;
    //            break;
    //        case IntercomBruteCustomEventTypeLoadFile:
    //            with_view_model(
    //                view, (IntercomBruteMainViewModel * model) {
    //                    instance->menu_index = model->index;
    //                    return false;
    //                });
    //            scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneSetupAttack);
    //            consumed = true;
    //            break;
    //        }
    //    }

    return consumed;
}