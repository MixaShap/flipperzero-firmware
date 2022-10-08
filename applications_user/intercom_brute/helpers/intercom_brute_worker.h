#pragma once

#include <furi_hal_subghz.h>

typedef struct IntercomBruteWorker IntercomBruteWorker;
/**
 * Same like SubGhzTxRxWorkerStatus in subghz_tx_rx_worker.h
 * using just to not include that file

typedef enum {
    IntercomBruteWorkerStatusIDLE,
    IntercomBruteWorkerStatusTx,
    // IntercomBruteWorkerStatusRx,
} IntercomBruteWorkerStatus;

//typedef void (*IntercomBruteWorkerCallback)(IntercomBruteWorkerStatus event, void* context);
*/
IntercomBruteWorker* intercom_brute_worker_alloc();
void intercom_brute_worker_free(IntercomBruteWorker* instance);
bool intercom_brute_worker_start(
    IntercomBruteWorker* instance,
    uint32_t frequency,
    FuriHalSubGhzPreset preset,
    const char* protocol_name);
void intercom_brute_worker_stop(IntercomBruteWorker* instance);
bool intercom_brute_worker_get_continuous_worker(IntercomBruteWorker* instance);
void intercom_brute_worker_set_continuous_worker(IntercomBruteWorker* instance, bool is_continuous_worker);
//bool intercom_brute_worker_write(IntercomBruteWorker* instance, uint8_t* data, size_t size);
bool intercom_brute_worker_is_running(IntercomBruteWorker* instance);
bool intercom_brute_worker_can_transmit(IntercomBruteWorker* instance);
bool intercom_brute_worker_can_manual_transmit(IntercomBruteWorker* instance, bool is_button_pressed);
bool intercom_brute_worker_transmit(IntercomBruteWorker* instance, const char* payload);
bool intercom_brute_worker_init_manual_transmit(
    IntercomBruteWorker* instance,
    uint32_t frequency,
    FuriHalSubGhzPreset preset,
    const char* protocol_name);
bool intercom_brute_worker_manual_transmit(IntercomBruteWorker* instance, const char* payload);
void intercom_brute_worker_manual_transmit_stop(IntercomBruteWorker* instance);