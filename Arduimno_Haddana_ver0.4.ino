/*
 * Haddana ver 0.4 for Arduino
 * Created: 2/14/2017 
 *  Author: Moshtak ALLAH
 */ 
#include "Haddana.h"
#include <Button.h>
#include<LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN     8         // Pin which is connected to the DHT sensor.
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal LCD = LiquidCrystal( 2, 3, 4, 5, 6, 7);
Button Menu_But(A0) ;
Button plus_But(A1) ;
Button minus_But(A2);
Button ok_But(A3)  ; 

double temp;
uint16_t analval;
byte humidity ;
uint32_t start_time = millis();
uint32_t alldays;
volatile uint8_t WAIT = 1;
uint32_t last_storetime = eeprom_read_dword((uint32_t*)MEM_DELAY_TIME_IF_POWER_OFF); 
uint8_t currentday = 1 + last_storetime / (ONE_DAY*1000) ;
  
void setup() {
  DDRB = 0xDE;
  DDRC |= 1<<PORTC5;
  DDRD |=0X03;
  _delay_ms(1000);
 
  Menu_But.begin();
  plus_But.begin();
  minus_But.begin();
  ok_But.begin();
  LCD.begin(16, 2);
  dht.begin();
  make_mem_zeros();
}

void loop() {
  
// for flipping 
    if ( (millis() - start_time) >= (TWO_HOURS) ) {FLIPPING_MOTOR_ON;}
    if ( (millis() - start_time) >= (TWO_HOURS+FLIPP_10M) ) {FLIPPING_MOTOR_OFF; start_time = millis();}
  
    // to save last time before power cut
      alldays = millis() + last_storetime;
    if ( millis() >= HALF_HOUR ) {eeprom_write_dword((uint32_t*) MEM_DELAY_TIME_IF_POWER_OFF, alldays);}// 5 YEARS ONLY
        
    // for calc current day to display
    if ( millis() > (currentday * (ONE_DAY*1000)) ) {currentday++; }
  
    // FINISH CYCLE
    if ( PRESS_4_BUTTONS_TO_FINISH ) { 
      eeprom_write_dword((uint32_t*) MEM_DELAY_TIME_IF_POWER_OFF,  0);/* time from first start*/
      LCD.clear();
      _delay_ms(5);
        LCD.print("CYCLE IS ENDED");
      while (WAIT);
    }
    
    // main program
          // TEMP SECTION
    if (Menu_But.pressed()) adjust_temp();
    analval = analogRead(A4);
    temp = dht.readTemperature();;
    temp += (int16_t)(eeprom_read_word((uint16_t*)MEM_TEMP_CALIBRATE_ERROR)) / 10.0; // adding TEMP cal EEROR 
    LCD.setCursor(0,0);
    LCD.print("TEMP : ");
    LCD.print(temp);
    LCD.setCursor(14, 0);
    LCD.print(currentday);
    char status_T = compare_values(temp*10, MEM_MIN_TEMP21, MEM_MAX_TEMP21, 5); // +-0.5 C //temp*10 to convert float to integer 37.3 -> 373
    if (status_T == NORMAL)          {HEATER_OFF; WINDOW_TFLAG_CLR; TEMP_BUZZER_OFF;} 
    else if (status_T == LO_MINOR )  {HEATER_ON;  WINDOW_TFLAG_CLR; TEMP_BUZZER_OFF;}
    else if (status_T == LO_URGENT)  {HEATER_ON;  WINDOW_TFLAG_CLR; TEMP_BUZZER_ON;}
    else if (status_T == HI_MINOR )  {HEATER_OFF; WINDOW_TFLAG_SET; TEMP_BUZZER_OFF;} 
    else if (status_T == HI_URGENT)  {HEATER_OFF; WINDOW_TFLAG_SET; TEMP_BUZZER_ON;}
        // HUMUDITY SECTION
     
   humidity = dht.readHumidity();
    LCD.setCursor(0,1);
    LCD.print("HUMD : ");
    
    //humdity = DHT22_gethum();
    LCD.print(humidity); // for DHT22 divide /10
    char status_H = compare_values(humidity*10, MEM_MIN_HUMU21, MEM_MAX_HUMU21, 50); // +- 5 humu
    if (status_H == NORMAL)          {HUMU_FAN_OFF; WINDOW_HFLAG_CLR; HUMU_BUZZER_OFF;}
    else if (status_H == LO_MINOR )  {HUMU_FAN_ON;  WINDOW_HFLAG_CLR; HUMU_BUZZER_OFF;}
    else if (status_H == LO_URGENT)  {HUMU_FAN_ON;  WINDOW_HFLAG_CLR; HUMU_BUZZER_ON;}
    else if (status_H == HI_MINOR )  {HUMU_FAN_OFF; WINDOW_HFLAG_SET; HUMU_BUZZER_OFF;}
    else if (status_H == HI_URGENT)  {HUMU_FAN_OFF; WINDOW_HFLAG_SET; HUMU_BUZZER_ON;}
    
    // check water sensor 
    if (WATER_SENS) WATER_BUZZER_ON;
    else WATER_BUZZER_OFF;
      // test to open air windo for hi temp or hi humu
    if (WINDOW_TFLAG_VALUE || WINDOW_HFLAG_VALUE) OPEN_AIR_WINDOW;
    else CLOSE_AIR_WINDOW;  
  }

//--------------------------------------------------------------------------------
uint8_t compare_values(uint16_t val,uint16_t Address_min, uint16_t Address_max, uint8_t urgentval){
  
if (val < eeprom_read_word((uint16_t*)Address_min) - urgentval) return LO_URGENT; 
else if (val < eeprom_read_word((uint16_t*)Address_min)) return LO_MINOR; 

else if (val >  eeprom_read_word((uint16_t*)Address_max) + urgentval)  return HI_URGENT;
else if (val > eeprom_read_word((uint16_t*)Address_max)) return HI_MINOR;

else return NORMAL;

}

//------------------------------------------------------
void adjust_temp(){
  LCD.clear();
  _delay_ms(5);
  LCD.setCursor(0,0);
  LCD.print("TEMP : ");
  LCD.setCursor(0,1);
  LCD.print("MIN: ");
  update_limits(MEM_MIN_TEMP21);// read buttons and update values in memory
  
  LCD.setCursor(0,0);
  LCD.print("TEMP : ");
  LCD.setCursor(0,1);
  LCD.print("MAX : ");
  update_limits(MEM_MAX_TEMP21);// read buttons and update values in memory
  
  adjust_humud();
}
//---------------------------------------------------------

void adjust_humud(){
  LCD.clear();
  _delay_ms(5);
  LCD.setCursor(0,0);
  LCD.print("HUMUD : ");
  LCD.setCursor(0,1);
  LCD.print("MIN : ");
  update_limits(MEM_MIN_HUMU21);// read buttons and update values in memory
  
  LCD.setCursor(0,0);
  LCD.print("HUMUD : ");
  LCD.setCursor(0,1);
  LCD.print("MAX :  ");
  update_limits(MEM_MAX_HUMU21);// read buttons and update values in memory
  
  calibrate_temp();
}
//------------------------------------------------------------

void calibrate_temp(){
  LCD.clear();
  _delay_ms(5);
  LCD.setCursor(0,0);
  LCD.print("  START TEMP");
  LCD.setCursor(0,1);
  LCD.print("  CALIBRATION");
  _delay_ms(2000);
  LCD.clear();
  _delay_ms(5);
  LCD.setCursor(0,0);
  LCD.print("SENS TEMP: ");
  LCD.print(temp);
  LCD.setCursor(0,1);
  LCD.print("ERROR:  ");
  update_limits(MEM_TEMP_CALIBRATE_ERROR);
    
LCD.clear();
} 

// read buttons and update values in memory----------------------------------
 void update_limits(uint16_t memory_loc){
   int16_t temporary = eeprom_read_word((uint16_t*) memory_loc);
   for (;;){
     if (plus_But.pressed()) ++temporary;
     if (minus_But.pressed())  --temporary;
     if (ok_But.pressed()) { eeprom_write_word((uint16_t*) memory_loc,temporary); LCD.clear();_delay_ms(5); break;}
     LCD.setCursor(10,1);
     LCD.print((float)temporary/10);
   }
}
//---------------------------------------------------------------------------
// default values
void make_mem_zeros(){
  if (eeprom_read_byte((uint8_t*) MEM_ONLY_ONE) != 0)
  {
    eeprom_write_byte((uint8_t*)  MEM_ONLY_ONE, 0);
    eeprom_write_dword((uint32_t*) MEM_DELAY_TIME_IF_POWER_OFF,  0);// time from first start
    
    eeprom_write_word((uint16_t*) MEM_TEMP_CALIBRATE_ERROR, (int16_t) 0);// TEMP calibration error
    eeprom_write_word((uint16_t*) MEM_MIN_TEMP21, (uint16_t) 377);// 37.7C
    eeprom_write_word((uint16_t*) MEM_MAX_TEMP21, (uint16_t) 378);// 37.8C
    eeprom_write_word((uint16_t*) MEM_MIN_HUMU21, (uint16_t) 600);// 60 HUMU
    eeprom_write_word((uint16_t*) MEM_MAX_HUMU21, (uint16_t) 650);// 65 HUMU
  }
}

