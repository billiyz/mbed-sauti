/************************************************************************
Copyright 2018 Ciira wa Maina
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

**************************************************************************/
#include "mbed.h"
#include <stdio.h>
#include <errno.h>
#include "SDBlockDevice.h"
#include "FATFileSystem.h"

#include "wav_files.h"


// Queue to schedule events
EventQueue sample_queue;
EventQueue write_queue;
EventQueue printf_queue;

// Analog input to simulate the microphone
static AnalogIn   signal_input(A1);

/* PWM parameters */
PwmOut pwm_output(PB_4);
const float SIGNAL_FREQ = 200.0;
const float DUTY_CYCLE = 0.5;
const float PERIOD_MS = 1000 * (1.0 / SIGNAL_FREQ);


const int  SAMPLING_RATE = 8000;
const int DURATION_IN_SEC = 4;

// Storage
SDBlockDevice bd(
    MBED_CONF_SD_SPI_MOSI,
    MBED_CONF_SD_SPI_MISO,
    MBED_CONF_SD_SPI_CLK,
    MBED_CONF_SD_SPI_CS);

FATFileSystem fs("fs");
int storage_status = 0;  // 0 -> unmounted , 1 -> mounted

// Buffer to store the samples
const int NUM_BUFFERS = 2;
const int BUFFER_SIZE = 1024;
static int16_t buffer[NUM_BUFFERS][BUFFER_SIZE];
int16_t *write_buffer;

// sampling variables
int current_buffer = 0;
int sample_number = 0;
uint32_t total_sample_number = 0;
int write_status = 0;
float sample_value = 0;
int16_t normalized_sample_value = 0;

// wav file parameter
FILE *fp;
static wav_file_header_t wav_file_header;
int file_open = 0;
int done_recording = 0;

static DigitalOut led(LED1);

void sample_signal() {
  if (total_sample_number < SAMPLING_RATE * DURATION_IN_SEC) {
    sample_value = signal_input.read();
    normalized_sample_value = (int16_t) (sample_value * 32767);

    buffer[current_buffer][sample_number] = normalized_sample_value;
    sample_number++;
    total_sample_number++;

    if (sample_number == BUFFER_SIZE) {
      sample_number %= BUFFER_SIZE;
      write_buffer = buffer[current_buffer];
      current_buffer = (current_buffer + 1) % NUM_BUFFERS;
      write_status = 1;
    }
  } else if (total_sample_number == (SAMPLING_RATE * DURATION_IN_SEC)) {
    done_recording = 1;
    fflush(stdout);
    printf_queue.call(printf, "Done recording ... \n");
    total_sample_number++;
  }

  if (file_open && storage_status && done_recording) {
    led = 0;
    file_open = 0;
  }

  if (!file_open && storage_status && done_recording) {
    storage_status = 0;
  }
}

void check_write_buffer() {
  int num_written_objs = 0;
  if (write_status) {
    write_status = 0;
    num_written_objs = fwrite(write_buffer,
                              sizeof(int16_t) * BUFFER_SIZE,
                              1,
                              fp);
    if (num_written_objs != 1) {
      printf_queue.call(printf, "writing error ... ");
    }
    led = !led;
  }
}

void mount_filesystem() {
  printf_queue.call(printf, "Mounting filesystem ... ");
  int err = fs.mount(&bd);
  if (!err) {
    storage_status = 1;
  } else  {
    // Reformat if we can't mount the filesystem
    printf_queue.call(printf, "No filesystem found, formatting... ");
    err = fs.reformat(&bd);
    printf_queue.call(printf, "%s\n", (err ? "Fail :(" : "OK"));
    if (err) {
      error("error: %s (%d)\n", strerror(-err), err);
    }
  }
  printf_queue.call(printf, "%s\n", (err ? "Fail :(" : "OK"));
  printf_queue.call(printf, "Opening wav file ... ");
  fp = fopen("/fs/audio_test_3.wav", "wb");
  // fclose(fp);
  if (fp) {
    file_open = 1;
  }
  printf_queue.call(printf, "%s\n", (!fp ? "Fail :(" : "OK"));
  // event_queue.call_in(10 * 1000, fclose, fp);
}




int main() {
  printf("\r--- Record Audio ---- \n");

  // Set up PWM parameters
  pwm_output.period_ms(PERIOD_MS);
  pwm_output.write(DUTY_CYCLE);
    
  // Set up sampling thread
  Thread sampler_thread(osPriorityRealtime);
  sampler_thread.start(callback(&sample_queue, &EventQueue::dispatch_forever));

  Thread write_thread;
  write_thread.start(callback(&write_queue, &EventQueue::dispatch_forever));

  Thread printf_thread(osPriorityNormal, 32 * 1024);
  printf_thread.start(callback(&printf_queue, &EventQueue::dispatch_forever));

  mount_filesystem();

  // initialize wav header
  initialize_wav_header(&wav_file_header,
                        SAMPLING_RATE,
                        SAMPLING_RATE * DURATION_IN_SEC);
  int num_written_objs;
  if (fp) {
    num_written_objs = fwrite(&wav_file_header, sizeof(wav_file_header), 1, fp);
  }

  if (num_written_objs != 1) {
    error("Failed to write wav file header\n");
  }

  Ticker sampler;
  sampler.attach_us(sample_queue.event(&sample_signal),
                      (1000000.0 / SAMPLING_RATE));

  Ticker writer;
  writer.attach_us(write_queue.event(&check_write_buffer),
                   (1000000.0 / SAMPLING_RATE) * (BUFFER_SIZE / 4));
  

  wait(2 * DURATION_IN_SEC);
  
  int err = fclose(fp);
  printf("Closing wave file ... %s\n", (err < 0 ? "Fail :(" : "OK"));
  if (err < 0) {
    error("error: %s (%d)\n", strerror(errno), -errno);
  }

  err = fs.unmount();
  printf("Unmounting ... %s\n", (err < 0 ? "Fail :(" : "OK"));
  if (err < 0) {
    error("error: %s (%d)\n", strerror(-err), err);
  }

  wait(osWaitForever);
}
