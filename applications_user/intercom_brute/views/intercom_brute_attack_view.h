#pragma once

#include <gui/view.h>
#include "assets_icons.h"
#include <input/input.h>
#include <gui/elements.h>
#include <gui/icon.h>
#include <subghz/types.h>
#include "../intercom_brute_custom_event.h"

typedef void (*IntercomBruteAttackViewCallback)(IntercomBruteCustomEvent event, void* context);
typedef struct IntercomBruteAttackView IntercomBruteAttackView;

void intercom_brute_attack_view_set_callback(
    IntercomBruteAttackView* instance,
    IntercomBruteAttackViewCallback callback,
    void* context);
IntercomBruteAttackView* intercom_brute_attack_view_alloc();
void intercom_brute_attack_view_free(IntercomBruteAttackView* instance);
View* intercom_brute_attack_view_get_view(IntercomBruteAttackView* instance);
void intercom_brute_attack_view_set_current_step(IntercomBruteAttackView* instance, uint64_t current_step);
void intercom_brute_attack_view_set_worker_type(IntercomBruteAttackView* instance, bool is_continuous_worker);
void intercom_brute_attack_view_init_values(
    IntercomBruteAttackView* instance,
    uint8_t index,
    uint64_t max_value,
    uint64_t current_step,
    bool is_attacking);