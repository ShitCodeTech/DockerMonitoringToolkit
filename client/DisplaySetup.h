#ifndef DISPLAYSETUP_H
#define DISPLAYSETUP_H


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EncButton.h>



//init display
LiquidCrystal_I2C lcd(0x27, 20, 4);

void display_setup(){
  lcd.init();
  lcd.backlight();

}



//button survey
Button btn_next(27, INPUT_PULLUP);
Button btn_back(26, INPUT_PULLUP);

static byte mode = 0;

void btns_tick(){
  btn_next.tick();
  btn_back.tick();
  if (btn_next.click()){
    lcd.clear();
    if(++mode>=4) mode = 0;
  }
  if (btn_back.click()){
    lcd.clear();
    if(--mode==255) mode = 3;
  }
}


//functions for graphics
void initPlot() {
  // necessary symbols for operation
  // created in http://maxpromer.github.io/LCD-Character-Creator/
  byte row8[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
  byte row7[8] = {0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
  byte row6[8] = {0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
  byte row5[8] = {0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
  byte row4[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111};
  byte row3[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
  byte row2[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
  byte row1[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111};
  lcd.createChar(0, row8);
  lcd.createChar(1, row1);
  lcd.createChar(2, row2);
  lcd.createChar(3, row3);
  lcd.createChar(4, row4);
  lcd.createChar(5, row5);
  lcd.createChar(6, row6);
  lcd.createChar(7, row7);
}
void fillBar(byte start_pos, byte row, byte bar_length, String fill_percent_str) {
  byte fill_percent = fill_percent_str.toInt();
  byte infill = round((float)bar_length * fill_percent / 100);
  lcd.setCursor(start_pos, row);
  if (infill == 0) lcd.write(16);
  else lcd.write(255);
  for (int n = 1; n < bar_length - 1; n++) {
    if (n < infill) lcd.write(255);
    if (n >= infill) lcd.write(16);
  }
  if (infill == bar_length) lcd.write(255);
  else lcd.write(16);
}


int plot_array1[20];
int plot_array2[20];

void drawPlot1(byte pos, byte row, byte width, byte height, int min_val, int max_val, String fill_val_str) {
  int fill_val = fill_val_str.toInt();
  for (byte i = 0; i < width; i++) {
    plot_array1[i] = plot_array1[i + 1];
  }
  fill_val = constrain(fill_val, min_val, max_val);
  plot_array1[width] = fill_val;
  for (byte i = 0; i < width; i++) {                  
    byte infill, fract;
    infill = floor((float)(plot_array1[i] - min_val) / (max_val - min_val) * height * 10);
    fract = (infill % 10) * 8 / 10;               
    infill = infill / 10;
    for (byte n = 0; n < height; n++) {     
      if (n < infill && infill > 0) {      
        lcd.setCursor(i, (row - n));        
        lcd.write(0);
      }
      if (n >= infill) {                   
        lcd.setCursor(i, (row - n));
        if (fract > 0) lcd.write(fract);          
        else lcd.write(16);                       
        for (byte k = n + 1; k < height; k++) {   
          lcd.setCursor(i, (row - k));
          lcd.write(16);
        }
        break;
      }
    }
  }
}
void drawPlot2(byte pos, byte row, byte width, byte height, int min_val, int max_val, String fill_val_str) {
  int fill_val = fill_val_str.toInt();
  for (byte i = 0; i < width; i++) {
    plot_array2[i] = plot_array2[i + 1];
  }
  fill_val = constrain(fill_val, min_val, max_val);
  plot_array2[width] = fill_val;
  for (byte i = 0; i < width; i++) {                
    byte infill, fract;

    infill = floor((float)(plot_array2[i] - min_val) / (max_val - min_val) * height * 10);
    fract = (infill % 10) * 8 / 10;    
    infill = infill / 10;
    for (byte n = 0; n < height; n++) {    
      if (n < infill && infill > 0) {      
        lcd.setCursor(i, (row - n));
        lcd.write(0);
      }
      if (n >= infill) {                  
        lcd.setCursor(i, (row - n));
        if (fract > 0) lcd.write(fract);          
        else lcd.write(16);                       
        for (byte k = n + 1; k < height; k++) { 
          lcd.setCursor(i, (row - k));
          lcd.write(16);
        }
        break;
      }
    }
  }
}


//display output
void display1(String cpu, String mem, String uptime,String name){
  lcd.setCursor(0,0);
  lcd.print("CPU ");
  lcd.setCursor(4,0);
  lcd.print(cpu);lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("MEM ");
  lcd.setCursor(4,1);
  lcd.print(mem);lcd.print("%");
  lcd.setCursor(0,2);
  lcd.print("UPT");
  lcd.setCursor(4,2);
  lcd.print(uptime);lcd.print(" min");
  lcd.setCursor(0,3);
  lcd.print(name);
  fillBar(10, 0, 10, cpu);
  fillBar(10, 1, 10, mem);
}
void display2(String cpu){
  drawPlot1(1, 3, 10, 4, 0, 100, cpu);
  int max_value = -32000;
  int min_value = 32000;
  for (byte i = 0; i < 12; i++) {
    if (plot_array1[i] > max_value) max_value = plot_array1[i];
    if (plot_array1[i] < min_value) min_value = plot_array1[i];
  }
  lcd.setCursor(11, 0);
  lcd.print("CPU");
  lcd.setCursor(11, 1);
  lcd.print("Curr ");lcd.print(cpu);
  lcd.setCursor(18, 1);lcd.print("%");
  lcd.setCursor(11, 2);
  lcd.print("min "); lcd.print(min_value);
  lcd.setCursor(18, 2);lcd.print("%");
  lcd.setCursor(11, 3);
  lcd.print("max "); lcd.print(max_value);
  lcd.setCursor(18, 3);lcd.print("%");
}
void display3(String mem, String allmem, String usedmem){
  drawPlot2(0, 3, 10, 4, 0, 100, mem);
  lcd.setCursor(11,0);
  lcd.print("MEM");
  lcd.setCursor(11, 1);
  lcd.print("Curr ");lcd.print(mem);
  lcd.setCursor(18, 1);lcd.print("%");
  lcd.setCursor(11, 2);
  lcd.print("Use "); lcd.print("usedmem");
  lcd.setCursor(18, 2);lcd.print("gb");
  lcd.setCursor(11, 3);
  lcd.print("All "); lcd.print("allmem");
  lcd.setCursor(18, 3);lcd.print("gb");
}
void display4(String server_adress, String wifi_ssid, String ip){
  lcd.setCursor(0,0);
  lcd.print("CLoc");
  lcd.setCursor(5,0);
  lcd.print(WiFi.localIP());
  lcd.setCursor(0,1);
  lcd.print("SLoc");
  lcd.setCursor(5,1);
  lcd.print(server_adress);
  lcd.setCursor(0,2);
  lcd.print("SSID");
  lcd.setCursor(5,2);
  lcd.print(wifi_ssid);
  lcd.setCursor(0,3);
  lcd.print("SGl");
  lcd.setCursor(5,3);
  lcd.print(ip);
}


#endif
