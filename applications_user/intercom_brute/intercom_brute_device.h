#pragma once

#include <lib/toolbox/stream/stream.h>
#include <gui/gui.h>
#include <dialogs/dialogs.h>
#include <lib/subghz/protocols/base.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/environment.h>

#define INTERCOM_BRUTE_TEXT_STORE_SIZE 256

#define INTERCOM_BRUTE_MAX_LEN_NAME 64
#define INTERCOM_BRUTE_PATH EXT_PATH("subghz")
#define INTERCOM_BRUTE_FILE_EXT ".sub"

#define INTERCOM_BRUTE_PAYLOAD_SIZE 16

typedef enum {
    METAKOM_CYFRAL,
    METAKOM_1,
    CYFRAL_1,
    VIZIT_1,
    VIZIT_2,
    ELTIS,
    LIFT,
    TOILET,
    IntercomBruteAttackChamberlain9bit390,
    IntercomBruteAttackLinear10bit300,
    IntercomBruteAttackLinear10bit310,
    IntercomBruteAttackLoadFile,
    IntercomBruteAttackTotalCount,
} IntercomBruteAttacks;

typedef enum {
    IntercomBruteFileResultUnknown,
    IntercomBruteFileResultOk,
    IntercomBruteFileResultErrorOpenFile,
    IntercomBruteFileResultMissingOrIncorrectHeader,
    IntercomBruteFileResultFrequencyNotAllowed,
    IntercomBruteFileResultMissingOrIncorrectFrequency,
    IntercomBruteFileResultPresetInvalid,
    IntercomBruteFileResultMissingProtocol,
    IntercomBruteFileResultProtocolNotSupported,
    IntercomBruteFileResultDynamicProtocolNotValid,
    IntercomBruteFileResultProtocolNotFound,
    IntercomBruteFileResultMissingOrIncorrectBit,
    IntercomBruteFileResultMissingOrIncorrectKey,
    IntercomBruteFileResultMissingOrIncorrectTe,
    IntercomBruteFileResultBigBitSize,
} IntercomBruteFileResult;

typedef enum {
    IntercomBruteDeviceStateIDLE,
    IntercomBruteDeviceStateReady,
    IntercomBruteDeviceStateTx,
    IntercomBruteDeviceStateFinished,
} IntercomBruteDeviceState;

typedef struct {
    IntercomBruteDeviceState state;

    // Current step
    uint64_t key_index;
    FuriString* load_path;
    // Index of group to bruteforce in loaded file
    uint8_t load_index;

    SubGhzReceiver* receiver;
    SubGhzProtocolDecoderBase* decoder_result;
    SubGhzEnvironment* environment;

    // Attack state
    IntercomBruteAttacks attack;
    char file_template[INTERCOM_BRUTE_TEXT_STORE_SIZE];
    bool has_tail;
    char payload[INTERCOM_BRUTE_TEXT_STORE_SIZE * 2];
    uint64_t max_value;

    // Loaded info for attack type
    FuriHalSubGhzPreset preset;
    FuriString* preset_name;
    FuriString* protocol_name;
    uint32_t frequency;
    uint32_t repeat;
    uint32_t bit;
    char current_key[INTERCOM_BRUTE_PAYLOAD_SIZE];
    uint32_t te;

    char file_key[INTERCOM_BRUTE_MAX_LEN_NAME];
    char text_store[INTERCOM_BRUTE_PAYLOAD_SIZE];
} IntercomBruteDevice;

IntercomBruteDevice* intercom_brute_device_alloc();
void intercom_brute_device_free(IntercomBruteDevice* instance);
bool intercom_brute_device_save_file(IntercomBruteDevice* instance, const char* key_name);
const char* intercom_brute_device_error_get_desc(IntercomBruteFileResult error_id);
bool intercom_brute_device_create_packet_parsed(IntercomBruteDevice* context, uint64_t step, bool small);
IntercomBruteFileResult intercom_brute_device_attack_set(IntercomBruteDevice* context, IntercomBruteAttacks type);
uint8_t intercom_brute_device_load_from_file(IntercomBruteDevice* context, FuriString* file_path);
FuriHalSubGhzPreset intercom_brute_device_convert_preset(const char* preset);
void intercom_brute_device_attack_set_default_values(
    IntercomBruteDevice* context,
    IntercomBruteAttacks default_attack);
