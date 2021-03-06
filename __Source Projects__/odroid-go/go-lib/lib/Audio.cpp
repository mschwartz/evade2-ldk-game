#include "Audio.h"

// #include "odroid_settings.h"
#define I2S_NUM (I2S_NUM_0)
#define BUILTIN_DAC_ENABLED (1)

float audio_volume = .062f; // half way
bool audio_mute = false;
uint16_t sample_rate;


float Audio::GetVolume(){
  return audio_volume;
}

void Audio::SetVolume(float value) {
  // printf("%s(%f)\n", __func__, value);
  float newValue = audio_volume + value;
  // printf("newValue = %f\n", newValue);
  if (newValue > .124f) {
    audio_volume = 0;
  }
  else {
    audio_volume = newValue;
  }

  printf("audio_volume = %f\n", audio_volume);
}



void Audio::Init(uint16_t new_sample_rate) {
  sample_rate = new_sample_rate;

  // NOTE: buffer needs to be adjusted per AUDIO_SAMPLE_RATE

  i2s_config_t i2s_config = {
    .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = sample_rate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,               //2-channels
    .communication_format = I2S_COMM_FORMAT_PCM,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,                //Interrupt level 1
    .dma_buf_count = 48,
    .dma_buf_len = 8,  
    .use_apll = 0, //1
    .fixed_mclk = 1
  };

  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);

  i2s_set_pin(I2S_NUM, NULL);
  i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
}

void Audio::Terminate() {
  i2s_zero_dma_buffer(I2S_NUM);
  i2s_stop(I2S_NUM);

  i2s_start(I2S_NUM);


  esp_err_t err = rtc_gpio_init(GPIO_NUM_25);
  err = rtc_gpio_init(GPIO_NUM_26);
  
  if (err != ESP_OK){
    abort();
  }

  err = rtc_gpio_set_direction(GPIO_NUM_25, RTC_GPIO_MODE_OUTPUT_ONLY);
  err = rtc_gpio_set_direction(GPIO_NUM_26, RTC_GPIO_MODE_OUTPUT_ONLY);
  
  if (err != ESP_OK) {
    abort();
  }

  err = rtc_gpio_set_level(GPIO_NUM_25, 0);
  err = rtc_gpio_set_level(GPIO_NUM_26, 0);
  if (err != ESP_OK) {
    abort();
  }
}

void Audio::Mute() {
  // printf("%s\n", __func__);
  audio_mute = true;
}

void Audio::Unmute() {
  // printf("%s\n", __func__);
  audio_mute = false;
}

void Audio::Mixdown(short *stereoAudioBuffer, int frameCount) {
  // Convert for built in DAC
  for (short i = 0; i < frameCount; i += 2){
    int32_t dac0;
    int32_t dac1;

    if (audio_mute || audio_volume == 0.0f) {
      // Disable amplifier
      dac0 = 0;
      dac1 = 0;
    }
    else {
      // Down mix stero to mono
      int32_t sample = stereoAudioBuffer[i];
      sample += stereoAudioBuffer[i + 1];
      sample >>= 1;

      // Normalize
      const float sn = (float)sample / 0x8000;

      // Scale
      const int magnitude = 127 + 127;
      const float range = magnitude  * sn * audio_volume;

      // Convert to differential output
      if (range > 127) {
        dac1 = (range - 127);
        dac0 = 127;
      }
      else if (range < -127) {
        dac1  = (range + 127);
        dac0 = -127;
      }
      else{
        dac1 = 0;
        dac0 = range;
      }

      dac0 += 0x80;
      dac1 = 0x80 - dac1;

      dac0 <<= 8;
      dac1 <<= 8;
    }

    stereoAudioBuffer[i] = (int16_t)dac1;
    stereoAudioBuffer[i + 1] = (int16_t)dac0;
  }

}

void Audio::Submit(short* stereoAudioBuffer, int frameCount) {
  short currentAudioSampleCount = frameCount * 2;

  // Convert for built in DAC
  for (short i = 0; i < currentAudioSampleCount; i += 2){
    int32_t dac0;
    int32_t dac1;

    if (audio_volume == 0.0f || audio_mute) {
      // Disable amplifier
      dac0 = 0;
      dac1 = 0;
    }
    else {
      // Down mix stero to mono
      int32_t sample = stereoAudioBuffer[i];
      sample += stereoAudioBuffer[i + 1];
      sample >>= 1;

      // Normalize
      const float sn = (float)sample / 0x8000;

      // Scale
      const int magnitude = 127 + 127;
      const float range = magnitude  * sn * audio_volume;

      // Convert to differential output
      if (range > 127) {
        dac1 = (range - 127);
        dac0 = 127;
      }
      else if (range < -127) {
        dac1  = (range + 127);
        dac0 = -127;
      }
      else{
        dac1 = 0;
        dac0 = range;
      }

      dac0 += 0x80;
      dac1 = 0x80 - dac1;

      dac0 <<= 8;
      dac1 <<= 8;
    }

    stereoAudioBuffer[i] = (int16_t)dac1;
    stereoAudioBuffer[i + 1] = (int16_t)dac0;
  }


  int len = currentAudioSampleCount * sizeof(int16_t);
  int count = i2s_write_bytes(I2S_NUM, (const char *)stereoAudioBuffer, len, portMAX_DELAY);

  if (count != len)   {
    printf("i2s_write_bytes: count (%d) != len (%d)\n", count, len);
    abort();
  }
}

int Audio::GetSampleRate() {
  return sample_rate;
}
