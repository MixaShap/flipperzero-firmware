#include "intercom_brute_attack_view.h"
#include "../intercom_brute_i.h"

#include "assets_icons.h"
#include <input/input.h>
#include <gui/elements.h>
#include <gui/icon_i.h>
#include <gui/icon_animation_i.h>

#define TAG "IntercomBruteAttackView"

struct IntercomBruteAttackView {
    View* view;
    IntercomBruteAttackViewCallback callback;
    void* context;
};

typedef struct {
    IntercomBruteAttacks index;
    uint64_t max_value;
    uint64_t current_step;
    bool is_attacking;
    bool is_continuous_worker;
    IconAnimation* icon;
} IntercomBruteAttackViewModel;

void intercom_brute_attack_view_set_callback(
    IntercomBruteAttackView* instance,
    IntercomBruteAttackViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

bool intercom_brute_attack_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "InputKey: %d", event->key);
#endif
    IntercomBruteAttackView* instance = context;

    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        instance->callback(IntercomBruteCustomEventTypeBackPressed, instance->context);
        with_view_model(
            instance->view, (IntercomBruteAttackViewModel * model) {
                model->is_attacking = false;
                model->is_continuous_worker = false;
                return true;
            });
        return true;
    }

    bool is_attacking = false;

    with_view_model(
        instance->view, (IntercomBruteAttackViewModel * model) {
            is_attacking = model->is_attacking;
            return false;
        });

    //    if(!is_attacking) {
    //        instance->callback(IntercomBruteCustomEventTypeTransmitNotStarted, instance->context);
    //    } else {
    //        instance->callback(IntercomBruteCustomEventTypeTransmitStarted, instance->context);
    //    }

    if(!is_attacking) {
        if(event->type == InputTypeShort && event->key == InputKeyOk) {
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "InputKey: %d OK", event->key);
#endif
            with_view_model(
                instance->view, (IntercomBruteAttackViewModel * model) {
                    model->is_attacking = true;
                    model->is_continuous_worker = false;
                    icon_animation_stop(model->icon);
                    icon_animation_start(model->icon);
                    return true;
                });
            instance->callback(IntercomBruteCustomEventTypeTransmitStarted, instance->context);
            /*if(event->type == InputTypeRepeat && event->key == InputKeyOk) {
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "InputKey: %d OK. IntercomBruteCustomEventTypeTransmitContinuousStarted", event->key);
#endif
            with_view_model(
                instance->view, (IntercomBruteAttackViewModel * model) {
                    model->is_attacking = true;
                    model->is_continuous_worker = true;
                    icon_animation_stop(model->icon);
                    icon_animation_start(model->icon);
                    return true;
                });
            instance->callback(IntercomBruteCustomEventTypeTransmitContinuousStarted, instance->context);
        } else if(event->type == InputTypeShort && event->key == InputKeyOk) {
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "InputKey: %d OK", event->key);
#endif
            with_view_model(
                instance->view, (IntercomBruteAttackViewModel * model) {
                    model->is_attacking = true;
                    model->is_continuous_worker = false;
                    icon_animation_stop(model->icon);
                    icon_animation_start(model->icon);
                    return true;
                });
            instance->callback(IntercomBruteCustomEventTypeTransmitStarted, instance->context);*/
        } else if(event->key == InputKeyUp) {
            instance->callback(IntercomBruteCustomEventTypeSaveFile, instance->context);
        } else if(event->key == InputKeyDown) {
            instance->callback(IntercomBruteCustomEventTypeTransmitCustom, instance->context);
        } else if(event->type == InputTypeShort) {
            if(event->key == InputKeyLeft) {
                instance->callback(IntercomBruteCustomEventTypeChangeStepDown, instance->context);
            } else if(event->key == InputKeyRight) {
                instance->callback(IntercomBruteCustomEventTypeChangeStepUp, instance->context);
            }
            //            with_view_model(
            //                instance->view, (IntercomBruteAttackViewModel * model) {
            //                    if(event->key == InputKeyLeft) {
            //                        model->current_step =
            //                            ((model->current_step - 1) + model->max_value) % model->max_value;
            //                    } else if(event->key == InputKeyRight) {
            //                        model->current_step = (model->current_step + 1) % model->max_value;
            //                    }
            //                    return true;
            //                });
            //            instance->callback(IntercomBruteCustomEventTypeChangeStep, instance->context);
        } else if(event->type == InputTypeRepeat) {
            if(event->key == InputKeyLeft) {
                instance->callback(IntercomBruteCustomEventTypeChangeStepDownMore, instance->context);
            } else if(event->key == InputKeyRight) {
                instance->callback(IntercomBruteCustomEventTypeChangeStepUpMore, instance->context);
            }
            /*with_view_model(
                instance->view, (IntercomBruteAttackViewModel * model) {
                    if(event->key == InputKeyLeft) {
                        model->current_step =
                            ((model->current_step - 100) + model->max_value) % model->max_value;
                    } else if(event->key == InputKeyRight) {
                        model->current_step = (model->current_step + 100) % model->max_value;
                    }
                    return true;
                });
            instance->callback(IntercomBruteCustomEventTypeChangeStep, instance->context);*/
        }
    } else {
        if((event->type == InputTypeShort || event->type == InputTypeRepeat) &&
           (event->key == InputKeyOk || event->key == InputKeyBack)) {
            with_view_model(
                instance->view, (IntercomBruteAttackViewModel * model) {
                    model->is_attacking = false;
                    model->is_continuous_worker = false;
                    icon_animation_stop(model->icon);
                    icon_animation_start(model->icon);
                    return true;
                });
            instance->callback(IntercomBruteCustomEventTypeTransmitNotStarted, instance->context);
        }
    }

    return true;
}

IntercomBruteAttackView* intercom_brute_attack_view_alloc() {
    IntercomBruteAttackView* instance = malloc(sizeof(IntercomBruteAttackView));

    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(IntercomBruteAttackViewModel));
    view_set_context(instance->view, instance);

    with_view_model(
        instance->view, (IntercomBruteAttackViewModel * model) {
            model->icon = icon_animation_alloc(&A_Sub1ghz_14);
            view_tie_icon_animation(instance->view, model->icon);
            return false;
        });

    view_set_draw_callback(instance->view, (ViewDrawCallback)intercom_brute_attack_view_draw);
    view_set_input_callback(instance->view, intercom_brute_attack_view_input);
    view_set_enter_callback(instance->view, intercom_brute_attack_view_enter);
    view_set_exit_callback(instance->view, intercom_brute_attack_view_exit);

    return instance;
}

void intercom_brute_attack_view_enter(void* context) {
    furi_assert(context);

#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "intercom_brute_attack_view_enter");
#endif
}

void intercom_brute_attack_view_free(IntercomBruteAttackView* instance) {
    furi_assert(instance);

#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "intercom_brute_attack_view_free");
#endif

    with_view_model(
        instance->view, (IntercomBruteAttackViewModel * model) {
            icon_animation_free(model->icon);
            return false;
        });

    view_free(instance->view);
    free(instance);
}

View* intercom_brute_attack_view_get_view(IntercomBruteAttackView* instance) {
    furi_assert(instance);
    return instance->view;
}

void intercom_brute_attack_view_set_current_step(IntercomBruteAttackView* instance, uint64_t current_step) {
    furi_assert(instance);
#ifdef FURI_DEBUG
    //FURI_LOG_D(TAG, "Set step: %d", current_step);
#endif
    with_view_model(
        instance->view, (IntercomBruteAttackViewModel * model) {
            model->current_step = current_step;
            return true;
        });
}

void intercom_brute_attack_view_set_worker_type(IntercomBruteAttackView* instance, bool is_continuous_worker) {
    furi_assert(instance);
    with_view_model(
        instance->view, (IntercomBruteAttackViewModel * model) {
            model->is_continuous_worker = is_continuous_worker;
            return true;
        });
}

// We need to call init every time, because not every time we calls enter
// normally, call enter only once
void intercom_brute_attack_view_init_values(
    IntercomBruteAttackView* instance,
    uint8_t index,
    uint64_t max_value,
    uint64_t current_step,
    bool is_attacking) {
#ifdef FURI_DEBUG
    FURI_LOG_D(
        TAG, "init, index: %d, max_value: %d, current_step: %d", index, max_value, current_step);
#endif
    with_view_model(
        instance->view, (IntercomBruteAttackViewModel * model) {
            model->max_value = max_value;
            model->index = index;
            model->current_step = current_step;
            model->is_attacking = is_attacking;
            if(is_attacking) {
                icon_animation_start(model->icon);
            } else {
                icon_animation_stop(model->icon);
            }
            return true;
        });
}

void intercom_brute_attack_view_exit(void* context) {
    furi_assert(context);
    IntercomBruteAttackView* instance = context;
#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "intercom_brute_attack_view_exit");
#endif
    with_view_model(
        instance->view, (IntercomBruteAttackViewModel * model) {
            icon_animation_stop(model->icon);
            return false;
        });
}

void elements_button_top_left(Canvas* canvas, const char* str) {
    const Icon* icon = &I_ButtonUp_7x4;

    const uint8_t button_height = 12;
    const uint8_t vertical_offset = 9; //
    const uint8_t horizontal_offset = 3;
    const uint8_t string_width = canvas_string_width(canvas, str);
    const uint8_t icon_h_offset = 3;
    const uint8_t icon_width_with_offset = icon->width + icon_h_offset;
    const uint8_t icon_v_offset = icon->height; //
    const uint8_t button_width = string_width + horizontal_offset * 2 + icon_width_with_offset + 1;

    const uint8_t x = 0;
    const uint8_t y = 0;

    canvas_draw_box(canvas, x, y, button_width, button_height);
#ifdef FURI_DEBUG
    FURI_LOG_D(
        TAG, "lbox, x: %d, y: %d, width: %d, height: %d", x, y, button_width, button_height);
#endif
    //    canvas_draw_line(canvas, x + button_width + 0, y, x + button_width + 0, y + button_height - 0); //
    //    canvas_draw_line(canvas, x + button_width + 1, y, x + button_width + 1, y + button_height - 1);
    //    canvas_draw_line(canvas, x + button_width + 2, y, x + button_width + 2, y + button_height - 2);

    canvas_invert_color(canvas);
    canvas_draw_icon(canvas, x + horizontal_offset, y + icon_v_offset, icon);
    canvas_draw_str(
        canvas, x + horizontal_offset + icon_width_with_offset, y + vertical_offset, str);
    canvas_invert_color(canvas);
}

void elements_button_top_right(Canvas* canvas, const char* str) {
    const Icon* icon = &I_ButtonDown_7x4;

    const uint8_t button_height = 12;
    const uint8_t vertical_offset = 9;
    const uint8_t horizontal_offset = 3;
    const uint8_t string_width = canvas_string_width(canvas, str);
    const uint8_t icon_h_offset = 3;
    const uint8_t icon_width_with_offset = icon->width + icon_h_offset;
    const uint8_t icon_v_offset = icon->height; // + vertical_offset;
    const uint8_t button_width = string_width + horizontal_offset * 2 + icon_width_with_offset + 1;

    const uint8_t x = canvas_width(canvas);
    const uint8_t y = 0;

    canvas_draw_box(canvas, x - button_width, y, button_width, button_height);
#ifdef FURI_DEBUG
    FURI_LOG_D(
        TAG,
        "rbox, x: %d, y: %d, width: %d, height: %d",
        x - button_width,
        y,
        button_width,
        button_height);
#endif
    //    canvas_draw_line(canvas, x - button_width - 1, y, x + button_width - 1, y + button_height - 0);
    //    canvas_draw_line(canvas, x - button_width - 2, y, x + button_width - 2, y + button_height - 1);
    //    canvas_draw_line(canvas, x - button_width - 3, y, x + button_width - 3, y + button_height - 2);

    canvas_invert_color(canvas);
    canvas_draw_str(canvas, x - button_width + horizontal_offset, y + vertical_offset, str);
    canvas_draw_icon(canvas, x - horizontal_offset - icon->width, y + icon_v_offset, icon);
    canvas_invert_color(canvas);
}

void intercom_brute_attack_view_draw(Canvas* canvas, void* context) {
    furi_assert(context);
    IntercomBruteAttackViewModel* model = (IntercomBruteAttackViewModel*)context;
    char buffer[26];

    const char* attack_name = NULL;
    attack_name = intercom_brute_get_menu_name(model->index);
    // Title
    if(model->is_attacking) {
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 2, AlignCenter, AlignTop, attack_name);
    }
    // Value
    canvas_set_font(canvas, FontBigNumbers);
    snprintf(buffer, sizeof(buffer), "%04d/%04d", (int)model->current_step, (int)model->max_value);
    canvas_draw_str_aligned(canvas, 64, 17, AlignCenter, AlignTop, buffer);
    canvas_set_font(canvas, FontSecondary);

    if(!model->is_attacking) {
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 44, AlignCenter, AlignBottom, attack_name);

        elements_button_left(canvas, "-1");
        elements_button_right(canvas, "+1");
        elements_button_center(canvas, "Start");
        elements_button_top_left(canvas, "Save");
        elements_button_top_right(canvas, "Resend");
    } else {
        if(model->is_continuous_worker) {
            canvas_invert_color(canvas);
        }
        // canvas_draw_icon_animation
        const uint8_t icon_h_offset = 0;
        const uint8_t icon_width_with_offset = model->icon->icon->width + icon_h_offset;
        const uint8_t icon_v_offset = model->icon->icon->height; // + vertical_offset;
        const uint8_t x = canvas_width(canvas);
        const uint8_t y = canvas_height(canvas);
        canvas_draw_icon_animation(
            canvas, x - icon_width_with_offset, y - icon_v_offset, model->icon);
        // Progress bar
        // Resolution: 128x64 px
        float progress_value = (float)model->current_step / model->max_value;
        elements_progress_bar(canvas, 8, 37, 110, progress_value > 1 ? 1 : progress_value);

        elements_button_center(canvas, "Stop");
        if(model->is_continuous_worker) {
            canvas_invert_color(canvas);
        }
    }
}
