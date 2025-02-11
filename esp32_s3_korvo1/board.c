/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2021 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_log.h"
#include "board.h"
#include "audio_mem.h"
#include "periph_sdcard.h"
#include "periph_adc_button.h"
#include <esp_adc/adc_oneshot.h>
#include "tca9554.h"
#include "esp_event.h"

// At the top of board.c
#ifdef __cplusplus
extern "C" {
#endif

//extern void button_press_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);

#ifdef __cplusplus
}
#endif


static const char *TAG = "AUDIO_BOARD";

static audio_board_handle_t board_handle = 0;

audio_board_handle_t audio_board_init(void)
{
    ESP_LOGD(TAG, "Initializing the board");
    if (board_handle) {
        ESP_LOGW(TAG, "The board has already been initialized!");
        return board_handle;
    }
    board_handle = (audio_board_handle_t) audio_calloc(1, sizeof(struct audio_board_handle));
    AUDIO_MEM_CHECK(TAG, board_handle, return NULL);
    board_handle->audio_hal = audio_board_codec_init();
    board_handle->adc_hal = audio_board_adc_init();
    ESP_LOGD(TAG, "Initializing the board done");
    return board_handle;
}

audio_hal_handle_t audio_board_adc_init(void)
{
    ESP_LOGD(TAG, "Initializing the adc");
    audio_hal_codec_config_t audio_codec_cfg = AUDIO_CODEC_ES7210_CONFIG();
    audio_hal_handle_t adc_hal = audio_hal_init(&audio_codec_cfg, &AUDIO_CODEC_ES7210_DEFAULT_HANDLE);
    AUDIO_NULL_CHECK(TAG, adc_hal, return NULL);
    return adc_hal;
}

audio_hal_handle_t audio_board_codec_init(void)
{
    ESP_LOGD(TAG, "Initializing the codec");
    audio_hal_codec_config_t audio_codec_cfg = AUDIO_CODEC_ES8311_CONFIG();
    audio_hal_handle_t codec_hal = audio_hal_init(&audio_codec_cfg, &AUDIO_CODEC_ES8311_DEFAULT_HANDLE);
    AUDIO_NULL_CHECK(TAG, codec_hal, return NULL);
    return codec_hal;
}

//esp_err_t audio_board_key_init(esp_periph_set_handle_t set)
//{
//    periph_adc_button_cfg_t adc_btn_cfg = PERIPH_ADC_BUTTON_DEFAULT_CONFIG();
//    adc_arr_t adc_btn_tag = ADC_DEFAULT_ARR();
//    int btn_array[7] = {190, 600, 1000, 1375, 1775, 2195, 3000};
//    adc_btn_tag.adc_ch = INPUT_BUTOP_ID;
//    adc_btn_tag.adc_level_step = btn_array;
//    adc_btn_cfg.arr = &adc_btn_tag;
//    adc_btn_cfg.arr_size = 1;
//    esp_periph_handle_t adc_btn_handle = periph_adc_button_init(&adc_btn_cfg);
//    AUDIO_NULL_CHECK(TAG, adc_btn_handle, return ESP_ERR_ADF_MEMORY_LACK);
//    return esp_periph_start(set, adc_btn_handle);
//}

// Callback function to handle button press events
void button_press_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    periph_adc_button_event_id_t event = (periph_adc_button_event_id_t) id;
    periph_adc_button_event_t *button_event = (periph_adc_button_event_t *) event_data;

    switch (event) {
        case PERIPH_ADC_BUTTON_PRESSED:
            ESP_LOGI(TAG, "Button pressed, ID: %d", button_event->act_id);
            break;
        case PERIPH_ADC_BUTTON_RELEASE:
            ESP_LOGI(TAG, "Button released, ID: %d", button_event->act_id);
            break;
        default:
            ESP_LOGI(TAG, "Unknown button event");
            break;
    }

    // Additional handling based on button ID
    switch (button_event->act_id) {
        case BUTTON_VOLUP_ID:
            ESP_LOGI(TAG, "Volume Up button pressed");
            break;
        case BUTTON_VOLDOWN_ID:
            ESP_LOGI(TAG, "Volume Down button pressed");
            break;
        case BUTTON_SET_ID:
            ESP_LOGI(TAG, "Set button pressed");
            break;
        case BUTTON_PLAY_ID:
            ESP_LOGI(TAG, "Play button pressed");
            break;
        case BUTTON_MODE_ID:
            ESP_LOGI(TAG, "Mode button pressed");
            break;
        case BUTTON_REC_ID:
            ESP_LOGI(TAG, "Record button pressed");
            break;
        default:
            ESP_LOGI(TAG, "Unknown button ID");
            break;
    }
}

esp_err_t audio_board_key_init(esp_periph_set_handle_t set)
{
    periph_adc_button_cfg_t adc_btn_cfg = PERIPH_ADC_BUTTON_DEFAULT_CONFIG();
    adc_arr_t adc_btn_tag = ADC_DEFAULT_ARR();
    int btn_array[7] = {380, 820, 1100, 1650, 1980, 2410, 3000};
    adc_btn_tag.adc_ch = INPUT_BUTOP_ID;
    adc_btn_tag.adc_level_step = btn_array;
    adc_btn_tag.total_steps = 6;
    adc_btn_cfg.arr = &adc_btn_tag;
    adc_btn_cfg.arr_size = 1;
    esp_periph_handle_t adc_btn_handle = periph_adc_button_init(&adc_btn_cfg);
    AUDIO_NULL_CHECK(TAG, adc_btn_handle, return ESP_ERR_ADF_MEMORY_LACK);
    return esp_periph_start(set, adc_btn_handle);
}


esp_err_t audio_board_sdcard_init(esp_periph_set_handle_t set, periph_sdcard_mode_t mode)
{
    if (mode != SD_MODE_1_LINE) {
        ESP_LOGE(TAG, "Current board only support 1-line SD mode!");
        return ESP_FAIL;
    }
    periph_sdcard_cfg_t sdcard_cfg = {
        .root = "/sdcard",
        .card_detect_pin = get_sdcard_intr_gpio(),
        .mode = mode
    };
    esp_periph_handle_t sdcard_handle = periph_sdcard_init(&sdcard_cfg);
    esp_err_t ret = esp_periph_start(set, sdcard_handle);
    int retry_time = 5;
    bool mount_flag = false;
    while (retry_time --) {
        if (periph_sdcard_is_mounted(sdcard_handle)) {
            mount_flag = true;
            break;
        } else {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
    if (mount_flag == false) {
        ESP_LOGE(TAG, "Sdcard mount failed");
        return ESP_FAIL;
    }
    return ret;
}

audio_board_handle_t audio_board_get_handle(void)
{
    return board_handle;
}

esp_err_t audio_board_deinit(audio_board_handle_t audio_board)
{
    esp_err_t ret = ESP_OK;
    ret |= audio_hal_deinit(audio_board->audio_hal);
    ret |= audio_hal_deinit(audio_board->adc_hal);
    audio_free(audio_board);
    board_handle = NULL;
    return ret;
}
