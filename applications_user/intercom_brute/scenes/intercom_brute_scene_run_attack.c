#include "../intercom_brute_i.h"
#include "../intercom_brute_custom_event.h"
#include "../views/intercom_brute_attack_view.h"
#include "../helpers/intercom_brute_worker.h"

#define TAG "IntercomBruteSceneRunAttack"

static void intercom_brute_scene_run_attack_callback(IntercomBruteCustomEvent event, void* context) {
    furi_assert(context);

    IntercomBruteState* instance = (IntercomBruteState*)context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, event);
}

//static void intercom_brute_scene_run_attack_worker_callback(void* context) {
//    IntercomBruteState* instance = (IntercomBruteState*)context;
//
//    if(instance->locked || instance->device->key_index + 1 > instance->device->max_value) {
//        return;
//    }
//    instance->locked = true;
//
//    if(intercom_brute_worker_can_manual_transmit(instance->worker)) {
//        // Blink
//        notification_message(instance->notifications, &sequence_blink_yellow_100);
//        intercom_brute_device_create_packet_parsed(instance->device, instance->device->key_index, true);
//
//#ifdef FURI_DEBUG
//        FURI_LOG_I(TAG, "intercom_brute_worker_manual_transmit");
//#endif
//        if(intercom_brute_worker_manual_transmit(instance->worker, instance->device->payload)) {
//#ifdef FURI_DEBUG
//            FURI_LOG_I(TAG, "transmit ok");
//#endif
//            // Make payload for new iteration or exit
//            if(instance->device->key_index + 1 <= instance->device->max_value) {
//                instance->device->key_index++;
//            } else {
//                view_dispatcher_send_custom_event(
//                    instance->view_dispatcher, IntercomBruteCustomEventTypeTransmitFinished);
//            }
//        }
//
//        // Stop
//        notification_message(instance->notifications, &sequence_blink_stop);
//    }
//
//    instance->locked = false;
//    intercom_brute_attack_view_set_current_step(instance->view_attack, instance->device->key_index);
//}

void intercom_brute_scene_run_attack_on_exit(void* context) {
    furi_assert(context);
    IntercomBruteState* instance = (IntercomBruteState*)context;
    //    IntercomBruteAttackState* state = (IntercomBruteAttackState*)scene_manager_get_scene_state(
    //        instance->scene_manager, IntercomBruteSceneRunAttack);
    //    furi_assert(state);
    //
    //    furi_timer_free(state->timer);
    //    free(state);

    if(intercom_brute_worker_get_continuous_worker(instance->worker)) {
        intercom_brute_worker_stop(instance->worker);
    }

    notification_message(instance->notifications, &sequence_blink_stop);
}

void intercom_brute_scene_run_attack_on_enter(void* context) {
    furi_assert(context);
    IntercomBruteState* instance = (IntercomBruteState*)context;
    IntercomBruteAttackView* view = instance->view_attack;
    //
    //    IntercomBruteAttackState* state = malloc(sizeof(IntercomBruteAttackState));
    //    scene_manager_set_scene_state(
    //        instance->scene_manager, IntercomBruteSceneRunAttack, (uint32_t)state);

    instance->current_view = IntercomBruteViewAttack;
    intercom_brute_attack_view_set_callback(view, intercom_brute_scene_run_attack_callback, instance);
    view_dispatcher_switch_to_view(instance->view_dispatcher, instance->current_view);

    intercom_brute_attack_view_init_values(
        view,
        (uint8_t)instance->device->attack,
        instance->device->max_value,
        instance->device->key_index,
        true);

    if(intercom_brute_worker_get_continuous_worker(instance->worker)) {
        // Init Continuous worker with values!
        if(!intercom_brute_worker_start(
               instance->worker,
               instance->device->frequency,
               instance->device->preset,
               furi_string_get_cstr(instance->device->protocol_name))) {
            FURI_LOG_W(TAG, "Worker Continuous init failed!");
        }
    } else {
        // Init worker with values
        if(!intercom_brute_worker_init_manual_transmit(
               instance->worker,
               instance->device->frequency,
               instance->device->preset,
               furi_string_get_cstr(instance->device->protocol_name))) {
            FURI_LOG_W(TAG, "Worker init failed!");
        }

        //    state->timer = furi_timer_alloc(
        //        intercom_brute_scene_run_attack_worker_callback, FuriTimerTypePeriodic, instance);
        //    furi_timer_start(state->timer, pdMS_TO_TICKS(100)); // 20 ms
    }
}

bool intercom_brute_scene_run_attack_on_event(void* context, SceneManagerEvent event) {
    IntercomBruteState* instance = (IntercomBruteState*)context;
    //    IntercomBruteAttackState* state = (IntercomBruteAttackState*)scene_manager_get_scene_state(
    //        instance->scene_manager, IntercomBruteSceneRunAttack);
    //    furi_assert(state);

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        IntercomBruteAttackView* view = instance->view_attack;

        if(event.event == IntercomBruteCustomEventTypeTransmitNotStarted ||
           event.event == IntercomBruteCustomEventTypeTransmitFinished ||
           event.event == IntercomBruteCustomEventTypeBackPressed) {
            //            furi_timer_stop(state->timer);
            // Stop transmit
            notification_message(instance->notifications, &sequence_display_backlight_on);
            notification_message(instance->notifications, &sequence_single_vibro);
            intercom_brute_attack_view_set_current_step(view, instance->device->key_index);
            scene_manager_search_and_switch_to_previous_scene(
                instance->scene_manager, IntercomBruteSceneSetupAttack);
            consumed = true;
        } else if(event.event == IntercomBruteCustomEventTypeUpdateView) {
            intercom_brute_attack_view_set_current_step(view, instance->device->key_index);
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        if(intercom_brute_worker_get_continuous_worker(instance->worker)) {
            if(intercom_brute_worker_can_transmit(instance->worker)) {
                // Blink
                notification_message(instance->notifications, &sequence_blink_yellow_100);

                intercom_brute_device_create_packet_parsed(
                    instance->device, instance->device->key_index, true);

                if(intercom_brute_worker_transmit(instance->worker, instance->device->payload)) {
                    // Make payload for new iteration or exit
                    if(instance->device->key_index + 1 > instance->device->max_value) {
                        // End of list
                        view_dispatcher_send_custom_event(
                            instance->view_dispatcher, IntercomBruteCustomEventTypeTransmitFinished);
                    } else {
                        instance->device->key_index++;
                        view_dispatcher_send_custom_event(
                            instance->view_dispatcher, IntercomBruteCustomEventTypeUpdateView);
                        //intercom_brute_attack_view_set_current_step(view, instance->device->key_index);
                    }
                }

                // Stop
                notification_message(instance->notifications, &sequence_blink_stop);
            }
        } else {
            if(intercom_brute_worker_can_manual_transmit(instance->worker, false)) {
                // Blink
                notification_message(instance->notifications, &sequence_blink_yellow_100);

                intercom_brute_device_create_packet_parsed(
                    instance->device, instance->device->key_index, true);

                if(intercom_brute_worker_manual_transmit(instance->worker, instance->device->payload)) {
                    // Make payload for new iteration or exit
                    if(instance->device->key_index + 1 > instance->device->max_value) {
                        // End of list
                        view_dispatcher_send_custom_event(
                            instance->view_dispatcher, IntercomBruteCustomEventTypeTransmitFinished);
                    } else {
                        instance->device->key_index++;
                        view_dispatcher_send_custom_event(
                            instance->view_dispatcher, IntercomBruteCustomEventTypeUpdateView);
                        //intercom_brute_attack_view_set_current_step(view, instance->device->key_index);
                    }
                }

                // Stop
                notification_message(instance->notifications, &sequence_blink_stop);
            }
        }

        consumed = true;
    }

    return consumed;
}
