# Audio Recording Using The Nucleo F446RE board

This repo contains code to record audio using the Nucleo F446RE board. Audio is obtained using a microphone and recorded to an SD card.

## Hardware Requirements

1. [Nucleo-F446RE](https://os.mbed.com/platforms/ST-Nucleo-F446RE/) board
1. [Microphone](https://www.adafruit.com/product/2716) (we will also record a PWM signal directly from the MCU)
1. [Micro SD card adapter](https://store.nerokas.co.ke/index.php?route=product/product&product_id=1082) (Catalex)
1. [Micro SD card](https://store.nerokas.co.ke/index.php?route=product/product&product_id=666)

### Connections

#### SD Card Adapter
The SD card adapter is connected as follows
1. VCC -> 5V
1. GND -> GND 
1. MISO -> D12
1. MOSI -> D11
1. CS -> D10
1. SCK -> D3. Note that you *cannot* use D13 when using LED1 which we do in this code. 

#### PWM Signal Recording
When recording the PWM signal, we connect the PWM output signal D5 (PB_4) to the analog input A1.
We create a 50% duty cycle signal at 200Hz. This is sampled at 8kHz and saved to a file *audio_test.wav*.

#### Microphone
[TODO]
## Software

The code in this repo relies on the SD card driver `SDBlockDevice` to record to the micro SD card. Code from the [mbed-os-example-filesystem](https://github.com/ARMmbed/mbed-os-example-filesystem) was used as a guide. The example can also be useful in ensuring that the SD card is correctly set up.

In addition it may be useful to format the SD card as shown [here](https://os.mbed.com/users/janjongboom/code/format-sd-card/).


### Recording a PWM signal
The PWM signal is set up as follows
```cpp
/* PWM parameters */
static PwmOut pwm_output(PB_4);
static const float SIGNAL_FREQ = 200.0;
static const float DUTY_CYCLE = 0.5;
static const float PERIOD_MS = 1000 * (1.0 / SIGNAL_FREQ);

```
...

```cpp
pwm_output.period_ms(PERIOD_MS);      
pwm_output.write(DUTY_CYCLE);
```

We record it for `DURATION_IN_SEC` seconds. 


## Usage
Ensure the Mbed development environment is set up as described in the [Get started with Mbed OS](https://os.mbed.com/docs/latest/tutorials/mbed-os-quick-start.html) page.
After wiring up the hardware, you can either import the repo into the [online compiler](https://os.mbed.com/docs/latest/tutorials/quick-start-online.html) or work offline with [Mbed CLI](https://os.mbed.com/docs/latest/tutorials/quick-start-offline.html).

## Sample Output

### PWM Signal
You can view the recorded 200Hz PWM signal in *audio_test.wav* using [Audacity](https://www.audacityteam.org/). A segment is shown below with 10 periods represented in the 50ms shown.

![Audacity screenshot](img/audacity_audio.png)

