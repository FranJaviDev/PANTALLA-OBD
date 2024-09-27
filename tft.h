#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9486 _panel_instance;   // Panel para el ILI9486
  lgfx::Bus_Parallel8 _bus_instance;     // Bus paralelo de 8 bits

public:
  LGFX(void) {
    { // Configuración del bus paralelo 8 bits
      auto cfg = _bus_instance.config();
      cfg.freq_write = 20000000;         // Velocidad de escritura
      cfg.pin_wr = 4;                  // Pin WR (escritura), ajustado para D1 R32
      cfg.pin_rd = 2;                 // Pin RD (lectura) no utilizado
      cfg.pin_rs = 15;                  // Pin RS (Registro / Comando), ajustado para D1 R32
      cfg.pin_d0 = 12;                  // Pines D0 a D7 para datos
      cfg.pin_d1 = 13;
      cfg.pin_d2 = 26;
      cfg.pin_d3 = 25;
      cfg.pin_d4 = 17;
      cfg.pin_d5 = 16;
      cfg.pin_d6 = 27;
      cfg.pin_d7 = 14;
      _bus_instance.config(cfg);       // Aplicar configuración del bus
      _panel_instance.setBus(&_bus_instance);  // Asociar el bus al panel
    }

    { // Configuración del panel ILI9486
      auto cfg = _panel_instance.config();
      cfg.pin_cs = 33;                    // Pin CS (Chip Select), ajustado para D1 R32
      cfg.pin_rst = 32;                  // Pin Reset, ajustado para D1 R32
      cfg.pin_busy = -1;                 // Pin Busy no utilizado
      cfg.memory_width = 320;            // Ancho de la memoria de la pantalla
      cfg.memory_height = 480;           // Altura de la memoria de la pantalla
      cfg.panel_width = 320;             // Ancho del panel visible
      cfg.panel_height = 480;            // Altura del panel visible
      cfg.offset_x = 0;                  // Desplazamiento en X (ajustar si es necesario)
      cfg.offset_y = 0;                  // Desplazamiento en Y (ajustar si es necesario)
      cfg.dummy_read_pixel = 8;          // Lectura de píxeles en modo dummy
      cfg.dummy_read_bits = 1;           // Número de bits para la lectura dummy
      cfg.readable = false;              // No se necesita lectura de datos
      cfg.invert = false;                // Invertir los colores (opcional)
      cfg.rgb_order = false;             // Orden RGB, cambiar si es necesario
      cfg.bus_shared = false;            // No compartir el bus
      _panel_instance.config(cfg);       // Aplicar configuración del panel
    }

    setPanel(&_panel_instance);           // Asociar el panel a la clase LGFX
  }
};
