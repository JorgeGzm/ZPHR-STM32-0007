
#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <string.h>

#define ADC_RESOLUTION       12  // Resolução do ADC (12 bits)
#define ADC_GAIN             ADC_GAIN_1
#define ADC_REFERENCE        ADC_REF_EXTERNAL0
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10)

#define ADC_MAX_VALUE         (4095)
#define ADC_VREF              (3.300)
#define ADC_RES_LIPO          (1.616187928)// divisao resistiva e drift(empirico) no canal 0
#define ADC_ALPHA_FILTER_COEF (0.1)
#define ADC_SAMPLES_COUNT     10

struct adc_measure_filter
{
    bool init;
    double voltage;
};

static float adc_voltage_to_soc_lipo(float voltage);
static float adc_voltage_to_soc_pil(float voltage);

// Buffer para armazenar o valor lido do ADC
static uint16_t adc_buffer[1];

// Configuração para o ADC (canal PA0 = ADC1_IN0 e PA1 = ADC1_IN1)
static struct adc_channel_cfg adc_channel_vad_bat = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .channel_id = 0,  // Canal 0 corresponde a PA0 (VAD_BAT)
    .differential = 0,
};

static struct adc_channel_cfg adc_channel_vpil = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .channel_id = 1,  // Canal 1 corresponde a PA1 (Vpil)
    .differential = 0,
};

static struct adc_sequence sequence = {
    .buffer = adc_buffer,
    .buffer_size = sizeof(adc_buffer),
    .resolution = ADC_RESOLUTION,
};

static struct adc_measure_filter g_bbat = {0};
static struct adc_measure_filter g_pilbat = {0};
static const double R1 = 330000.0; // em ohms
static const double R2 = 1000000.0; // em ohms
static double voltage_divider_factor =  (R1 + R2) / R2;
static const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc1));  // Obtém o dispositivo ADC1

/*
Tensão (V)	Estado de Carga (SoC)
4,20	100%
4,00	85%
3,85	75%
3,70	50%
3,50	25%
3,20	10%
3,00	0% (descarregada)

   Vin
    |
    |
    R1 = 330k
    |
    |---> Vout
    |
    R2 = 1M
    |
    |
   GND

vin	vout
3	2,25563909774436
3,1	2,33082706766917
3,2	2,40601503759398
3,3	2,4812030075188
3,4	2,55639097744361
3,5	2,63157894736842
3,6	2,70676691729323
3,7	2,78195488721804
3,8	2,85714285714286
3,9	2,93233082706767
4	3,00751879699248
4,1	3,08270676691729
4,2	3,15789473684211
*/

// Função para calcular SoC com base na tensão
static float adc_voltage_to_soc_lipo(float voltage) {
    // Tabela de tensões (V) e SoC correspondentes
    const float voltages[] = {3.0, 3.2, 3.5, 3.7, 3.85, 4.0, 4.2};
    const float socs[] = {0.0, 10.0, 25.0, 50.0, 75.0, 85.0, 100.0};
    const int size = sizeof(voltages) / sizeof(voltages[0]);

    // Verifica se a tensão está fora dos limites
    if (voltage <= voltages[0]) return socs[0]; // Descarregada
    if (voltage >= voltages[size - 1]) return socs[size - 1]; // Totalmente carregada

    // Interpolação linear para os valores intermediários
    for (int i = 0; i < size - 1; i++) {
        if (voltage >= voltages[i] && voltage <= voltages[i + 1]) {
            // Interpolação linear
            float soc = socs[i] + (voltage - voltages[i]) *
                        (socs[i + 1] - socs[i]) / (voltages[i + 1] - voltages[i]);
            return soc;
        }
    }

    // Caso inesperado (não deveria acontecer)
    return 0.0;
}

// Função para calcular o SoC com base na tensão da bateria moeda
static float adc_voltage_to_soc_pil(float voltage) {
    // Tabela de tensões (V) e SoC correspondentes
    const float voltages[] = {2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2};
    const float socs[] = {0.0, 10.0, 25.0, 50.0, 75.0, 90.0, 100.0};
    const int size = sizeof(voltages) / sizeof(voltages[0]);

    // Verifica se a tensão está fora dos limites
    if (voltage <= voltages[0]) return socs[0]; // Descarregada
    if (voltage >= voltages[size - 1]) return socs[size - 1]; // Totalmente carregada

    // Interpolação linear para os valores intermediários
    for (int i = 0; i < size - 1; i++) {
        if (voltage >= voltages[i] && voltage <= voltages[i + 1]) {
            // Interpolação linear
            float soc = socs[i] + (voltage - voltages[i]) *
                        (socs[i + 1] - socs[i]) / (voltages[i + 1] - voltages[i]);
            return soc;
        }
    }

    // Caso inesperado (não deveria acontecer)
    return 0.0;
}

static int adc_read_channel_average(uint8_t channel_id, uint32_t *adc_value) {
    int ret = -EINVAL;
    int count_reads = 0;
    uint32_t sum = 0;

    sequence.channels = BIT(channel_id);

    for (int i = 0; i < ADC_SAMPLES_COUNT; i++) {
        ret = adc_read(adc_dev, &sequence);
        if (!ret == 0) {
            return ret;
        }
        
        sum += adc_buffer[0];
        count_reads++;

        k_msleep(5);
    }

    *adc_value = (sum / count_reads);
    return 0;
}

void adc_init(void) {

    if (!device_is_ready(adc_dev)) {
        printk("ADC device not ready\n");
        return;
    }

    memset(&g_bbat, 0, sizeof(struct adc_measure_filter));
    memset(&g_pilbat, 0, sizeof(struct adc_measure_filter));

    // Configura os canais do ADC
    adc_channel_setup(adc_dev, &adc_channel_vad_bat);
    adc_channel_setup(adc_dev, &adc_channel_vpil);
}

double adc_read_val_percent_bbat(void) {
    int ret;
    uint32_t adc_value;
    double val_volt = 0;
    double val_percent = 0;

    ret = adc_read_channel_average(0, &adc_value);
    if(ret)
    {
        printk("Failed to read BBAT\n");
        return 0;
    }

    val_volt = (((double)adc_value) / ADC_MAX_VALUE) * ADC_VREF * voltage_divider_factor;
    if(!g_bbat.init)
    {
        g_bbat.init = true;
        g_bbat.voltage = val_volt;
    } 
    else
    {
        g_bbat.voltage = ADC_ALPHA_FILTER_COEF * val_volt + (1.0 - ADC_ALPHA_FILTER_COEF) * g_bbat.voltage;
    }

    val_percent = adc_voltage_to_soc_lipo(g_bbat.voltage);
    // printk("BBat:  %d(buffer), %.2f V, %.2f %% \r\n", adc_value, val_volt, val_percent);

    return val_percent;
}


double adc_read_val_percent_pilbat(void) {
    int ret;
    uint32_t adc_value;
    double val_volt = 0;
    double val_percent = 0;

    ret = adc_read_channel_average(1, &adc_value);
    if(ret)
    {
        printk("Failed to read PilBAT\n");
        return 0;
    }

    val_volt = (((double)adc_value) / ADC_MAX_VALUE) * ADC_VREF;

    if(!g_pilbat.init)
    {
        g_pilbat.init = true;
        g_pilbat.voltage = val_volt;
    } 
    else
    {
        g_pilbat.voltage = ADC_ALPHA_FILTER_COEF * val_volt + (1.0 - ADC_ALPHA_FILTER_COEF) * g_pilbat.voltage;
    }

    val_percent = adc_voltage_to_soc_pil(g_pilbat.voltage);
    // printk("PilBat: %d(buffer), %.2f V, %.2f %% \r\n", adc_value, val_volt, val_percent);

    return val_percent;
}
