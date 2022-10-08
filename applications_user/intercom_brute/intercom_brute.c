#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/view_stack.h>
#include <gui/scene_manager.h>
#include <gui/modules/text_input.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>
#include <gui/modules/loading.h>

#include <dialogs/dialogs.h>

#include "intercom_brute.h"
#include "intercom_brute_i.h"
#include "intercom_brute_custom_event.h"

#define TAG "IntercomBruteApp"

static const char* intercom_brute_menu_names[] = {
    [METAKOM_CYFRAL] = "METAKOM_CYFRAL",
    [METAKOM_1] = "METAKOM_1",
    [CYFRAL_1] = "CYFRAL_1",
    [VIZIT_1] = "VIZIT_1",
    [VIZIT_2] = "VIZIT_2",
    [ELTIS] = "ELTIS",
    [LIFT] = "LIFT",
    [TOILET] = "TOILET",
    [IntercomBruteAttackChamberlain9bit390] = "UNUSED",
    [IntercomBruteAttackLinear10bit300] = "UNUSED",
    [IntercomBruteAttackLinear10bit310] = "UNUSED",
    [IntercomBruteAttackLoadFile] = "EXISTING",
    [IntercomBruteAttackTotalCount] = "Total Count",
};

static const char* intercom_brute_menu_names_small[] = {
    [METAKOM_CYFRAL] = "METAKOM_CYFRAL",
    [METAKOM_1] = "METAKOM_1",
    [CYFRAL_1] = "CYFRAL_1",
    [VIZIT_1] = "VIZIT_1",
    [VIZIT_2] = "VIZIT_2",
    [ELTIS] = "ELTIS",
    [LIFT] = "LIFT",
    [TOILET] = "TOILET",
    [IntercomBruteAttackChamberlain9bit390] = "UNUSED",
    [IntercomBruteAttackLinear10bit300] = "UNUSED",
    [IntercomBruteAttackLinear10bit310] = "UNUSED",
    [IntercomBruteAttackLoadFile] = "EXISTING",
    [IntercomBruteAttackTotalCount] = "Total Count",
};

static bool intercom_brute_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    IntercomBruteState* instance = context;
    return scene_manager_handle_custom_event(instance->scene_manager, event);
}

static bool intercom_brute_back_event_callback(void* context) {
    furi_assert(context);
    IntercomBruteState* instance = context;
    return scene_manager_handle_back_event(instance->scene_manager);
}

static void intercom_brute_tick_event_callback(void* context) {
    furi_assert(context);
    IntercomBruteState* instance = context;
    scene_manager_handle_tick_event(instance->scene_manager);
}

IntercomBruteState* intercom_brute_alloc() {
    IntercomBruteState* instance = malloc(sizeof(IntercomBruteState));

    instance->scene_manager = scene_manager_alloc(&intercom_brute_scene_handlers, instance);
    instance->view_dispatcher = view_dispatcher_alloc();

    instance->gui = furi_record_open(RECORD_GUI);

    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);
    view_dispatcher_set_custom_event_callback(
        instance->view_dispatcher, intercom_brute_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        instance->view_dispatcher, intercom_brute_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        instance->view_dispatcher, intercom_brute_tick_event_callback, 10);

    //Dialog
    instance->dialogs = furi_record_open(RECORD_DIALOGS);

    // Notifications
    instance->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Devices
    instance->device = intercom_brute_device_alloc();

    // Worker
    instance->worker = intercom_brute_worker_alloc();

    // TextInput
    instance->text_input = text_input_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher,
        IntercomBruteViewTextInput,
        text_input_get_view(instance->text_input));

    // Custom Widget
    instance->widget = widget_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, IntercomBruteViewWidget, widget_get_view(instance->widget));

    // Popup
    instance->popup = popup_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, IntercomBruteViewPopup, popup_get_view(instance->popup));

    // ViewStack
    instance->view_stack = view_stack_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, IntercomBruteViewStack, view_stack_get_view(instance->view_stack));

    // IntercomBruteMainView
    instance->view_main = intercom_brute_main_view_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher,
        IntercomBruteViewMain,
        intercom_brute_main_view_get_view(instance->view_main));

    // IntercomBruteAttackView
    instance->view_attack = intercom_brute_attack_view_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher,
        IntercomBruteViewAttack,
        intercom_brute_attack_view_get_view(instance->view_attack));

    // Loading
    instance->loading = loading_alloc();
    //instance->flipper_format = flipper_format_string_alloc();
    //instance->environment = subghz_environment_alloc();

    return instance;
}

void intercom_brute_free(IntercomBruteState* instance) {
    furi_assert(instance);

    // IntercomBruteWorker
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free IntercomBruteDevice");
#endif
    intercom_brute_worker_stop(instance->worker);
    intercom_brute_worker_free(instance->worker);

    // IntercomBruteDevice
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free IntercomBruteDevice");
#endif
    intercom_brute_device_free(instance->device);

    // Notifications
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free Notifications");
#endif
    notification_message(instance->notifications, &sequence_blink_stop);
    furi_record_close(RECORD_NOTIFICATION);
    instance->notifications = NULL;

    // Loading
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free loading");
#endif
    loading_free(instance->loading);

    // View Main
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free IntercomBruteViewMain");
#endif
    view_dispatcher_remove_view(instance->view_dispatcher, IntercomBruteViewMain);
    intercom_brute_main_view_free(instance->view_main);

    // View Attack
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free IntercomBruteViewAttack");
#endif
    view_dispatcher_remove_view(instance->view_dispatcher, IntercomBruteViewAttack);
    intercom_brute_attack_view_free(instance->view_attack);

    // TextInput
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free IntercomBruteViewTextInput");
#endif
    view_dispatcher_remove_view(instance->view_dispatcher, IntercomBruteViewTextInput);
    text_input_free(instance->text_input);

    // Custom Widget
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free IntercomBruteViewWidget");
#endif
    view_dispatcher_remove_view(instance->view_dispatcher, IntercomBruteViewWidget);
    widget_free(instance->widget);

    // Popup
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free IntercomBruteViewPopup");
#endif
    view_dispatcher_remove_view(instance->view_dispatcher, IntercomBruteViewPopup);
    popup_free(instance->popup);

    // ViewStack
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free IntercomBruteViewStack");
#endif
    view_dispatcher_remove_view(instance->view_dispatcher, IntercomBruteViewStack);
    view_stack_free(instance->view_stack);

    //Dialog
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free RECORD_DIALOGS");
#endif
    furi_record_close(RECORD_DIALOGS);
    instance->dialogs = NULL;

    // Scene manager
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free scene_manager");
#endif
    scene_manager_free(instance->scene_manager);

    // View Dispatcher
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free view_dispatcher");
#endif
    view_dispatcher_free(instance->view_dispatcher);

    // GUI
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free RECORD_GUI");
#endif
    furi_record_close(RECORD_GUI);
    instance->gui = NULL;

    // The rest
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "free instance");
#endif
    free(instance);
}

void intercom_brute_show_loading_popup(void* context, bool show) {
    TaskHandle_t timer_task = xTaskGetHandle(configTIMER_SERVICE_TASK_NAME);
    IntercomBruteState* instance = context;
    ViewStack* view_stack = instance->view_stack;
    Loading* loading = instance->loading;

    if(show) {
        // Raise timer priority so that animations can play
        vTaskPrioritySet(timer_task, configMAX_PRIORITIES - 1);
        view_stack_add_view(view_stack, loading_get_view(loading));
    } else {
        view_stack_remove_view(view_stack, loading_get_view(loading));
        // Restore default timer priority
        vTaskPrioritySet(timer_task, configTIMER_TASK_PRIORITY);
    }
}

void intercom_brute_text_input_callback(void* context) {
    furi_assert(context);
    IntercomBruteState* instance = context;
    view_dispatcher_send_custom_event(
        instance->view_dispatcher, IntercomBruteCustomEventTypeTextEditDone);
}

void intercom_brute_popup_closed_callback(void* context) {
    furi_assert(context);
    IntercomBruteState* instance = context;
    view_dispatcher_send_custom_event(
        instance->view_dispatcher, IntercomBruteCustomEventTypePopupClosed);
}

const char* intercom_brute_get_menu_name(IntercomBruteAttacks index) {
    furi_assert(index < IntercomBruteAttackTotalCount);

    return intercom_brute_menu_names[index];
}

const char* intercom_brute_get_small_menu_name(IntercomBruteAttacks index) {
    furi_assert(index < IntercomBruteAttackTotalCount);

    return intercom_brute_menu_names_small[index];
}

// ENTRYPOINT
int32_t intercom_brute_app(void* p) {
    UNUSED(p);

    IntercomBruteState* instance = intercom_brute_alloc();
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(instance->scene_manager, IntercomBruteSceneStart);

    furi_hal_power_suppress_charge_enter();
    notification_message(instance->notifications, &sequence_display_backlight_on);
    view_dispatcher_run(instance->view_dispatcher);
    furi_hal_power_suppress_charge_exit();
    intercom_brute_free(instance);

    return 0;
}
