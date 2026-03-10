# ZPHR-STM32-0007

* Board: Blackpill + display
* MCU: STM32F411RE

# Zephyr

* Zephyr Version: v4.3.0
* Zephyr SDK: v0.17.4

# Peripherals

* Display: ILI9341 (320*240)
* sensor Touch: FT6236

## Display Pinout (2.8" TFT SPI 240x320 Capacitive Touch V1.0)

| No | Display Pin | MCU Pin | Function                 |
|----|-------------|---------|--------------------------|
| 14 | SD_CS       | -       | SD card (not used)       |
| 13 | CTP_INT     | PB6     | Touch interrupt          |
| 12 | CTP_SDA     | PB3     | I2C2 SDA (touch)         |
| 11 | CTP_RST     | PB2     | Touch reset              |
| 10 | CTP_SCL     | PB10    | I2C2 SCL (touch)         |
| 9  | SDO(MISO)   | PA6     | SPI1 MISO                |
| 8  | LED         | PB7     | Backlight (TIM4 CH2 PWM) |
| 7  | SCK         | PA5     | SPI1 SCK                 |
| 6  | SDI(MOSI)   | PA7     | SPI1 MOSI                |
| 5  | LCD_RS      | PB0     | Data/Command (DC)        |
| 4  | LCD_RST     | PB1     | Display reset            |
| 3  | LCD_CS      | PA15     | SPI1 CS (chip select)    |
| 2  | GND         | GND     | Ground                   |
| 1  | VCC         | 3.3V    | Power                    |

## Arquitetura da Aplicacao (Padrao MVP)

Os modulos de tela seguem o padrao **MVP (Model-View-Presenter)**, separando
interface grafica, logica de negocio e ligacao em arquivos distintos. Cada tela
(ex: `protimer`, `clockalarm`) e composta por tres camadas:

```
app/src/screen/
├── inc/                          # Headers publicos
│   ├── protimer_model.h
│   ├── protimer_view.h
│   ├── protimer_screen.h
│   ├── clockalarm_model.h
│   ├── clockalarm_view.h
│   ├── clockalarm_screen.h
│   └── menu_screen.h
└── src/                          # Implementacao
    ├── protimer_model.c
    ├── protimer_view.c
    ├── protimer_screen.c
    ├── clockalarm_model.c
    ├── clockalarm_view.c
    ├── clockalarm_screen.c
    └── menu_screen.c
```

### Model (`*_model.c/h`)

Contem toda a **logica de negocio** sem nenhuma dependencia do framework de UI (LVGL).

- Maquina de estados (Zephyr SMF) com handlers de entry/run/exit
- Definicao de eventos e fila de mensagens (`k_msgq`)
- Timers (`k_timer`) para eventos periodicos
- Dados: contadores, valores de tempo, flags
- Notifica o presenter atraves de uma struct de callbacks (`*_model_cb`)

O model nunca inclui `<lvgl.h>`. Pode ser testado unitariamente de forma independente.

### View (`*_view.c/h`)

Contem toda a **interface grafica** sem nenhuma logica de negocio.

- Criacao de widgets LVGL (labels, botoes, containers, estilos)
- Layout e posicionamento
- Funcoes publicas de atualizacao: `set_time()`, `set_status()`, `set_state()`
- Callback de botao que mapeia eventos de toque para um enum
- Nunca toma decisoes sobre transicoes de estado ou transformacoes de dados

A view nunca inclui `<zephyr/smf.h>`. Pode ser substituida por outro
framework de UI sem alterar a logica.

### Screen/Presenter (`*_screen.c/h`)

A **camada fina de ligacao** que conecta o Model a View.

- Cria a view, passando um callback de botao
- Cria o model, passando os setters da view como callbacks de notificacao
- Mapeia IDs de botao da view para enums de evento do model
- Gerencia o ciclo de vida: `create()`, `destroy()`, `process()`

Tipicamente menos de 50 linhas de codigo.

### Fluxo de dados

```
  Usuario toca no botao
        │
        ▼
  View ──(btn callback)──► Presenter ──(send_event)──► Model
                                                         │
                                                    SMF processa
                                                         │
  View ◄──(set_time, etc)── Presenter ◄──(on_time, etc)──┘
        │
        ▼
  LVGL atualiza o display
```

### Por que isso escala

- **Adicionar nova tela**: criar 3 arquivos (model + view + screen), registrar
  no `menu_screen` e `main.c`. Nenhuma alteracao no codigo existente.
- **Trocar framework de UI**: substituir apenas os arquivos `*_view.c`. Model e
  presenter permanecem intactos.
- **Testes unitarios**: o model pode ser testado no host (sem LVGL, sem hardware).
- **Multiplos desenvolvedores**: um trabalha na UI (view), outro na logica (model),
  com o presenter como contrato entre eles.
