#include <string.h>

#include "ev3api.h"
#include "app.h"

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

static FILE *serial = NULL;
static int last_updated_time;
static int left_motor_port = -1;
static int right_motor_port = -1;
static int enable_watchdog_task = 0;

/**
 * Get current time in [msec].
 */
ulong_t get_time()
{
                static ulong_t start = -1;
                ulong_t time;
                get_tim(&time); 
                if(start < 0){
                                start = time;
                }
                return time - start;
}

/**
 * This is run in the background every 1 seconds.
 * Send the stop command if idle time is more than 2 seconds.
 */
void watchdog_task(intptr_t exinf) {
                if (left_motor_port == -1 || right_motor_port == -1) return;
                if (enable_watchdog_task != 1) return;
                ulong_t now = get_time();
                if (now - last_updated_time > 1000) {
                                ev3_motor_steer(left_motor_port, right_motor_port, 0, 0);
                }
}

/// read byte
/// @param serial serial as file
/// @return uint8_t result.
uint8_t read_byte(FILE* serial) {
                uint8_t hi = fgetc(serial);
                uint8_t lo = fgetc(serial);
                uint8_t ret = hi*16 + lo;
                return ret;
}

void main_task(intptr_t unused) {
                // Draw information
                lcdfont_t font = EV3_FONT_MEDIUM;
                ev3_lcd_set_font(font);
                int32_t fontw, fonth;
                ev3_font_get_size(font, &fontw, &fonth);
                char lcdstr[100];

                // Open serial port
                serial = ev3_serial_open_file(EV3_SERIAL_UART);
                assert(serial != NULL);

                // main loop
                while (1) {
                                last_updated_time = get_time();
                                // sprintf(lcdstr, "%08d", last_updated_time);
                                // ev3_lcd_draw_string(lcdstr, 0, fonth * 5);

                                uint8_t header = read_byte(serial);
                                // sprintf(lcdstr, "header: %04d", header);
                                // ev3_lcd_draw_string(lcdstr, 0, fonth * 6);
                                if (header != 255) continue;

                                uint8_t cmd_id = read_byte(serial);
                                // sprintf(lcdstr, "cmd_id: %04d", cmd_id);
                                // ev3_lcd_draw_string(lcdstr, 0, fonth * 7);

                                // motor_config
                                if (cmd_id == 0) {
                                                int motor_port = read_byte(serial);
                                                int motor_type = read_byte(serial);
                                                ev3_motor_config(motor_port, motor_type);
                                                continue;
                                }

                                // enable_watchdog_task
                                if (cmd_id == 1) {
                                                enable_watchdog_task = 1;
                                                continue;
                                }

                                // motor_steer
                                if (cmd_id == 10) {
                                                left_motor_port = read_byte(serial);
                                                right_motor_port = read_byte(serial);
                                                int drive = read_byte(serial) - 100;
                                                int steer = read_byte(serial) - 100;
                                                if (drive > 100) drive=100;
                                                if (drive < -100) drive=-100;
                                                if (steer > 100) steer=100;
                                                if (steer < -100) steer=-100;
                                                ev3_motor_steer(left_motor_port, right_motor_port, drive, steer);
                                                continue;
                                }

                                // sensor_config
                                if (cmd_id == 100) {
                                                uint8_t sensor_port = read_byte(serial);
                                                uint8_t sensor_type = read_byte(serial);
                                                ev3_sensor_config(sensor_port, sensor_type);
                                                continue;
                                }

                                // touch_sensor_is_pressed
                                if (cmd_id == 110) {
                                                uint8_t touch_sensor_port = read_byte(serial);
                                                uint8_t touch = ev3_touch_sensor_is_pressed(touch_sensor_port);
                                                fputc((uint8_t)254, serial);
                                                fputc((uint8_t)touch, serial);
                                                continue;
                                }

                                // color_sensor_get_reflect
                                if (cmd_id == 120) {
                                                uint8_t color_sensor_port = read_byte(serial);
                                                uint8_t color = ev3_color_sensor_get_reflect(color_sensor_port);
                                                fputc((uint8_t)254, serial);
                                                fputc((uint8_t)color, serial);
                                                continue;
                                }

                                // lcd_draw_string
                                if (cmd_id == 200) {
                                                char str[100];
                                                uint8_t line = read_byte(serial);
                                                int i = 0;
                                                for (i=0; i<100; i++) {
                                                                uint8_t r = read_byte(serial);
                                                                str[i] = r;
                                                                if (r == 0) break;
                                                }
                                                strcpy(lcdstr, str);
                                                ev3_lcd_draw_string(lcdstr, 0, fonth * line);
                                                continue;
                                }

                                // button_is_pressed
                                if (cmd_id == 210) {
                                                uint8_t button = read_byte(serial);
                                                bool_t button_state = ev3_button_is_pressed(button);
                                                if (button_state) {
                                                                button_state = 1;
                                                }
                                                fputc((uint8_t)254, serial);
                                                fputc((uint8_t)button_state, serial);
                                }

                                // My program↓
                                // motor_rotate
                                if (cmd_id == 300) {
                                                int     port      = read_byte(serial);
                                                int     derection = read_byte(serial);
                                                uint8_t speed     = read_byte(serial);
                                                // for debug
                                                ev3_lcd_draw_string("%d %d %d %d", 300, port, derection, speed);
                                                ev3_motor_rotate(port, derection, speed, false);
                                }

                                // motor_set_power
                                if (cmd_id == 301) {
                                                int port  = read_byte(serial);
                                                int power = read_byte(serial);
                                                // for debug
                                                ev3_lcd_draw_string("%d %d %d", 301, port, power);
                                                ev3_motor_set_power(port, power);
                                }

                }
}
