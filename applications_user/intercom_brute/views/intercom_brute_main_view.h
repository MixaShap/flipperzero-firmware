#pragma once

#include "../intercom_brute_custom_event.h"
#include <gui/view.h>
#include "assets_icons.h"
#include <input/input.h>
#include <gui/elements.h>
#include <gui/icon.h>

typedef void (*IntercomBruteMainViewCallback)(IntercomBruteCustomEvent event, void* context);
typedef struct IntercomBruteMainView IntercomBruteMainView;

void intercom_brute_main_view_set_callback(
    IntercomBruteMainView* instance,
    IntercomBruteMainViewCallback callback,
    void* context);

IntercomBruteMainView* intercom_brute_main_view_alloc();
void intercom_brute_main_view_free(IntercomBruteMainView* instance);
View* intercom_brute_main_view_get_view(IntercomBruteMainView* instance);
void intercom_brute_main_view_set_index(
    IntercomBruteMainView* instance,
    uint8_t idx,
    bool is_select_byte,
    const char* key_field);
uint8_t intercom_brute_main_view_get_index(IntercomBruteMainView* instance);
void intercom_brute_attack_view_enter(void* context);
void intercom_brute_attack_view_exit(void* context);
bool intercom_brute_attack_view_input(InputEvent* event, void* context);
void intercom_brute_attack_view_draw(Canvas* canvas, void* context);