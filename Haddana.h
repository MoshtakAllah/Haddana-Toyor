/*
 * Haddana.h
 *
 * Created: 2/14/2017 
 *  Author: Moshtak ALLAH
 */ 

#ifndef HADDANA_H_
#define HADDANA_H_

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>


#define MEM_ONLY_ONE   0
#define MEM_TEMP_CALIBRATE_ERROR 3

#define MEM_DELAY_TIME_IF_POWER_OFF 30
#define MEM_TEMP_WIND_FLAG 10
#define MEM_HUMU_WIND_FLAG 11

#define MEM_MIN_TEMP21 21  //FOR SECOND STAGE 21 DAYS
#define MEM_MAX_TEMP21 23
#define MEM_MIN_HUMU21 25
#define MEM_MAX_HUMU21 27

#define NORMAL 0
#define LO_MINOR  1
#define LO_URGENT 2
#define HI_MINOR  3
#define HI_URGENT 4

#define HEATER_ON			      PORTB |=  (1<<PB1)
#define HEATER_OFF			    PORTB &= ~(1<<PB1)
#define TEMP_BUZZER_ON		  PORTB |=  (1<<PB4)
#define TEMP_BUZZER_OFF		  PORTB &= ~(1<<PB4)
#define HUMU_BUZZER_ON		  PORTD |=  (1<<PD0)
#define HUMU_BUZZER_OFF		  PORTD &= ~(1<<PD0)
#define HUMU_FAN_ON			    PORTB |=  (1<<PB3)
#define HUMU_FAN_OFF		    PORTB &= ~(1<<PB3)
#define WATER_SENS			    PINB & (1<<PINB5)
#define WATER_BUZZER_ON		  PORTD |= (1<<PD1)
#define WATER_BUZZER_OFF	  PORTD &= ~(1<<PD1)
#define FLIPPING_MOTOR_ON   PORTB |= 1<<PB2
#define FLIPPING_MOTOR_OFF  PORTB &=~(1<<PB2)

#define WINDOW_TFLAG_CLR	  eeprom_write_byte((uint8_t*)(MEM_TEMP_WIND_FLAG), 0)
#define WINDOW_TFLAG_SET	  eeprom_write_byte((uint8_t*)(MEM_TEMP_WIND_FLAG), 1)
#define WINDOW_HFLAG_CLR	  eeprom_write_byte((uint8_t*)(MEM_HUMU_WIND_FLAG), 0)
#define WINDOW_HFLAG_SET	  eeprom_write_byte((uint8_t*)(MEM_HUMU_WIND_FLAG), 1)
#define WINDOW_TFLAG_VALUE  eeprom_read_byte((uint8_t*)MEM_TEMP_WIND_FLAG)
#define WINDOW_HFLAG_VALUE  eeprom_read_byte((uint8_t*)MEM_HUMU_WIND_FLAG)
#define OPEN_AIR_WINDOW		  PORTC |= 1<<PC5
#define CLOSE_AIR_WINDOW	  PORTC &= ~(1<<PC5)

#define TWO_HOURS           2*60*60*1000UL
#define FLIPP_10M           10*60*1000UL
#define HALF_HOUR           (30*60*1000UL)
#define ONE_DAY             (24*60*60UL)

#define PRESS_4_BUTTONS_TO_FINISH ( !( PINC & 0X0F) ) // 4 BUTTONS OK,MINUS,PLUS,MENU

extern "C" double Thermistor(int RawADC);
//extern "C" unsigned long millis();
static void adjust_temp();
static void adjust_humud();
void calibrate_temp();
static void update_limits(uint16_t);
uint8_t compare_values(uint16_t val,uint16_t Address_min, uint16_t Address_max, uint8_t urgentval);
static void make_mem_zeros();



#endif /* HADDANA_H_ */
