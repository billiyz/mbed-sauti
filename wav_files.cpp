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

#include<string.h>
#include "wav_files.h"


/* Initialize the wav header using given values of sampling rate and number of samples
   
   see wav_files.h for a description of the wav file header
*/
void initialize_wav_header(wav_file_header_t *wav_file_header,
                           int sampling_rate,
                           int num_samples) {
  memcpy(wav_file_header->chunk_id, "RIFF", 4);
  wav_file_header->chunk_size = (num_samples *
                                 NUM_CHANNELS * (BITS_PER_SAMPLE / 8) + 36);
  memcpy(wav_file_header->format, "WAVE", 4);
  memcpy(wav_file_header->sub_chunk_1_id, "fmt ", 4);
  wav_file_header->sub_chunk_1_size = SUB_CHUNK_1_SIZE;
  wav_file_header->audio_format = AUDIO_FORMAT;
  wav_file_header->num_channels = NUM_CHANNELS;
  wav_file_header->sampling_rate = sampling_rate;
  wav_file_header->byte_rate = (sampling_rate *
                                NUM_CHANNELS *
                                (BITS_PER_SAMPLE / 8));
  wav_file_header->block_align = NUM_CHANNELS * (BITS_PER_SAMPLE / 8);
  wav_file_header->bits_per_sample = BITS_PER_SAMPLE;
  memcpy(wav_file_header->sub_chunk_2_id, "data", 4);
  wav_file_header->sub_chunk_2_size = (num_samples *
                                       NUM_CHANNELS *
                                       (BITS_PER_SAMPLE / 8));
}
