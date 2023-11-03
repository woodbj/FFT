#include <microphone.h>
#include <Arduino.h>

/**
 * @brief This function initiates the I2S port
 *
 * @param settings Mic_Settings_t struct contains the necessary parameters to start the I2S
 *
 * @note doesn't seem to overlap as expected...
 */
Microphone::Microphone(Mic_Settings_t _settings)
{
    settings = _settings;

    pinMode(settings.sck, OUTPUT);
    pinMode(settings.ws, OUTPUT);
    pinMode(settings.sd, INPUT_PULLDOWN);

    micInstall();
}

void Microphone::micInstall()
{
    dma_buffer_count = 2 + settings.sample_count / SAMPLES_PER_DMA_BUFFER;

    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = settings.sample_rate,
        .bits_per_sample = i2s_bits_per_sample_t(BITS_PER_SAMPLE),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .dma_buf_count = dma_buffer_count,
        .dma_buf_len = SAMPLES_PER_DMA_BUFFER, // in samples
        .use_apll = true};

    const i2s_pin_config_t pin_config = {
        .bck_io_num = settings.sck,
        .ws_io_num = settings.ws,
        .data_out_num = -1,
        .data_in_num = settings.sd};


    esp_err_t install = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);


    esp_err_t pinset = i2s_set_pin(I2S_PORT, &pin_config);

    //     pinMode(settings.sck, OUTPUT);
    // pinMode(settings.ws, OUTPUT);
    // pinMode(settings.sd, INPUT_PULLDOWN);


    i2s_zero_dma_buffer(I2S_PORT);

}

/**
 * @brief Pulls the DMA buffer attached to the I2S and puts it into the output pointer.
 * if the function is called before the buffer can collect a full set of samples
 * the old values will be slid to the beginning and the new samples taken
 * will be placed at the end
 *
 * @param output Pointer to the array which this function will fill.
 *
 * !BEWARE! this parameter must not be altered between calls to this function or backfilling willl be compromised
 */
void Microphone::getBuffer(sampletype_t *input)
{
    size_t bytesCollected;
    size_t samplesCollected;
    int offset;
    const int numsamples = settings.sample_count;
    
    sampletype_t output[numsamples];

    esp_err_t read = i2s_read(I2S_PORT, &i2sread, settings.sample_count * BYTES_PER_SAMPLE, &bytesCollected, 0);


    samplesCollected = bytesCollected / 2;
    offset = settings.sample_count - samplesCollected;
    
    for (int i = 0; i < offset; i++)
    {
        output[i] = input[numsamples - offset + i];
    }
    for (int i = offset; i < numsamples; i++)
    {
        output[i] = i2sread[i - offset];
    }
    for (int i = 0; i < numsamples; i++)
    {
        input[i] = output[i];
    }
}

float Microphone::getSampleRate()
{
    return i2s_get_clk(I2S_PORT);
}

void Microphone::setSampleRate(uint32_t rate)
{
    settings.sample_rate = rate;
    esp_err_t uninstall = i2s_driver_uninstall(I2S_PORT);
    micInstall();
}