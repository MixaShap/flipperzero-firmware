#pragma once

#include <stdint.h>
#include <stddef.h>

typedef enum {
    // Reserve first 100 events for button types and indexes, starting from 0
    IntercomBruteCustomEventTypeReserved = 100,

    IntercomBruteCustomEventTypeBackPressed,
    IntercomBruteCustomEventTypeIndexSelected,
    IntercomBruteCustomEventTypeTransmitStarted,
    IntercomBruteCustomEventTypeTransmitContinuousStarted,
    IntercomBruteCustomEventTypeTransmitFinished,
    IntercomBruteCustomEventTypeTransmitNotStarted,
    IntercomBruteCustomEventTypeTransmitCustom,
    IntercomBruteCustomEventTypeSaveFile,
    IntercomBruteCustomEventTypeUpdateView,
    IntercomBruteCustomEventTypeChangeStepUp,
    IntercomBruteCustomEventTypeChangeStepDown,
    IntercomBruteCustomEventTypeChangeStepUpMore,
    IntercomBruteCustomEventTypeChangeStepDownMore,

    IntercomBruteCustomEventTypeMenuSelected,
    IntercomBruteCustomEventTypeTextEditDone,
    IntercomBruteCustomEventTypePopupClosed,

    IntercomBruteCustomEventTypeLoadFile,
} IntercomBruteCustomEvent;