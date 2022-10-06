#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>

#include "lib/toolbox/path.h"
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <lib/toolbox/stream/stream.h>
#include <stream_buffer.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/view_stack.h>
#include <gui/scene_manager.h>
#include <gui/modules/text_input.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>
#include <gui/modules/loading.h>

#include <dialogs/dialogs.h>

#include <lib/subghz/protocols/base.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/environment.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "intercom_brute_device.h"
#include "helpers/intercom_brute_worker.h"
#include "intercom_brute.h"
#include "scenes/intercom_brute_scene.h"
#include "views/intercom_brute_attack_view.h"
#include "views/intercom_brute_main_view.h"

typedef enum {
    IntercomBruteViewNone,
    IntercomBruteViewMain,
    IntercomBruteViewAttack,
    IntercomBruteViewTextInput,
    IntercomBruteViewDialogEx,
    IntercomBruteViewPopup,
    IntercomBruteViewWidget,
    IntercomBruteViewStack,
} IntercomBruteView;

struct IntercomBruteState {
    // GUI elements
    NotificationApp* notifications;
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    ViewStack* view_stack;
    TextInput* text_input;
    Popup* popup;
    Widget* widget;
    DialogsApp* dialogs;
    Loading* loading;

    // Views
    IntercomBruteMainView* view_main;
    IntercomBruteAttackView* view_attack;
    IntercomBruteView current_view;

    // Scene
    SceneManager* scene_manager;

    IntercomBruteDevice* device;
    IntercomBruteWorker* worker;

    //Menu stuff
    // TODO: Do we need it?
    uint8_t menu_index;
};

void intercom_brute_show_loading_popup(void* context, bool show);
void intercom_brute_text_input_callback(void* context);
void intercom_brute_popup_closed_callback(void* context);
const char* intercom_brute_get_menu_name(uint8_t index);
const char* intercom_brute_get_small_menu_name(uint8_t index);