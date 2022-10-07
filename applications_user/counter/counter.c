#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>


typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    InputEvent event;
    uint16_t counter;
} CounterApp;


static void counter_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

//    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, input_event, 0);
}


void counter_draw_callback(Canvas* canvas, void* ctx) {
    CounterApp* app = (CounterApp*)ctx;
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Counter");

    char string[6];
    itoa(app->counter, string, 6);

    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str(canvas, 53, 38, string);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 15, 60, "Long press back for exit");
}


void counter_app_free(CounterApp* app) {
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_message_queue_free(app->event_queue);
    furi_record_close("gui");
    free(app);
}


int32_t counter_app(void* p) {
    UNUSED(p);

    CounterApp* app = malloc(sizeof(CounterApp));
    app->view_port = view_port_alloc();
    app->gui = furi_record_open("gui");
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    view_port_draw_callback_set(app->view_port, counter_draw_callback, app);
    view_port_input_callback_set(app->view_port, counter_input_callback, app->event_queue);

    while(1) {
        furi_check(furi_message_queue_get(app->event_queue, &app->event, 100) == FuriStatusOk);
        if(app->event.type == InputTypeLong && app->event.key == InputKeyBack) {
            break;
        }
        if(app->event.type == InputTypeShort) {
            app->counter++;
        }
        if(app->event.type == InputTypeLong && app->event.key == InputKeyOk) {
            app->counter = 0;
        }
        view_port_update(app->view_port);
    }

    counter_app_free(app);
    return 0;
}
