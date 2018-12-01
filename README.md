# Audio Recording Using The Nucleo f446re board

This repo contains code to record audio using the Nucleo f446re board. Audio is obtained using a microphone and recorded to an SD card.

## Hardware Requirements

1. Nucleo f446re board
1. Microphone 
1. Micro SD card adapter (Catalex)
1. Micro SD card

### Connections

#### SD Card Adapter
The SD card is connected as follows
1. VCC -> 5V
1. GND -> GND 
1. MISO -> D12
1. MOSI -> D11
1. CS -> D10
1. SCK -> D3. Note that you *cannot* use D13 when using LED1 which we do in this code. 

#### PWM Signal Recording
When recording the PWM signal, we connect the PWM output signal D5 (PB_4) to the analog input A1.


#### Microphone
## Software

The code in this repo relies on the to record to the micro SD card. Code from the mbed-os-example-filesystem was used as a guide.


## Recording a PWM signal


