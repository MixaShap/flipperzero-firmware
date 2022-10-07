#include <furi.h>
#include <furi-hal.h>
#include <gui/gui.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    osMessageQueueId_t event_queue;
    InputEvent event;
    uint16_t counter;
} CounterApp;

void counter_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    osMessageQueuePut((osMessageQueueId_t)ctx, input_event, 0, 0);
}

void counter_draw_callback(Canvas* canvas, void* ctx) {
    CounterApp* app = (CounterApp*)ctx;
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Counter");

    char string[6];
    sprintf(string, "%d", app->counter);

    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str(canvas, 53, 38, string);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 15, 60, "Long press back for exit");
}

void counter_app_free(CounterApp* app) {
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    osMessageQueueDelete(app->event_queue);
    furi_record_close("gui");
    free(app);
}

CounterApp* counter_app_alloc() {
    CounterApp* app = furi_alloc(sizeof(CounterApp));
    app->view_port = view_port_alloc();
    app->gui = furi_record_open("gui");
    app->event_queue = osMessageQueueNew(8, sizeof(InputEvent), NULL);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    view_port_draw_callback_set(app->view_port, counter_draw_callback, app);
    view_port_input_callback_set(app->view_port, counter_input_callback, app->event_queue);
    return app;
}

int32_t counter_app(void* p) {
    CounterApp* app = counter_app_alloc();

    while(1) {
        furi_check(osMessageQueueGet(app->event_queue, &app->event, NULL, osWaitForever) == osOK);
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
