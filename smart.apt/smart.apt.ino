
#include <PCF8574.h>

#define BTN_INT 50

#define IOExtender PCF8574
#define SZ(x) sizeof(x)/sizeof(x[0])
#define NA -1

#define LED 17
#define ZX_IRQ 7 // Zero Cross detection pin

unsigned int btn_millis;

// Input Buttons Extenders
IOExtender in_01(0x26);
IOExtender in_02(0x27);

// Output Relays Extenders
IOExtender out_20(0x20);
IOExtender out_21(0x21);
IOExtender out_22(0x22);
IOExtender out_23(0x23);

IOExtender in_arr[] = {in_01, in_02};
IOExtender out_arr[] = {out_20, out_21, out_22, out_23};

byte in[2];

boolean state[32] = { 0 }; // Buttons State
int counter[32] = { 0 };
byte st8[4] = { 0xff, 0xff, 0xff, 0xff }; // Relays State

volatile boolean ready2update = false;

// Rooms to Relays mapping
// -----------------------
typedef struct {
  int sw_buttons[16];
  int relays[32];
} room_lgt;

// Room Lights Declaration: { Switch Button IDs }, { Relay IDs }
room_lgt rooms[] = {
  {{ 0, 1,     NA }, { 14, 15,  NA }}, // hall
  {{ 2,        NA }, { 18, 19,  NA }}, // toilet
  {{ 3,        NA }, { 21,      NA }}, // kitchen
  {{ 4,        NA }, { 22,      NA }}, // kitchen
  {{ 5,        NA }, { 10, 11,  NA }}, // guest
  {{ 8, 9, 10, NA }, { 12, 13,  NA }}, // bedroom
  {{ 11,       NA }, { 2, 3, 4, NA }}, // bath
  {{ 12,       NA }, { 0, 1,    NA }}, // ware
  {{ 13, 14,   NA }, { 5, 6, 7, NA }}  // kids
};

void setup() {
  Serial.begin(115200);
  // Init Inputs  
  for (int i = 0; i < SZ(in_arr); ++i) {
    in_arr[i].begin();
  }
  // Init Outputs
  for (int i = 0; i < SZ(out_arr); ++i) {
    out_arr[i].begin();
  }
  
  pinMode(LED, OUTPUT);

  // ZeroX IRQ
  pinMode(ZX_IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ZX_IRQ), zerox_detected, RISING);
}

void loop() {
  
  if(millis() - btn_millis >= BTN_INT){
    btn_millis = millis();
    
    // Read Buttons
    for (int i = 0; i < SZ(in_arr); ++i)
    {
      in[i] = in_arr[i].read8();
    }

    // Update States
    for (int i = 0; i < SZ(rooms); ++i)
    {
      room_lgt room = rooms[i];
      
      boolean sw_st8 = false;
      for (int si = 0; room.sw_buttons[si] != NA; ++si)
      {
        sw_st8 |= sw_state(room.sw_buttons[si]);
      }
      
      if (i == 0) // DEBUG hall
      {
        check_room2(room);
        continue;
      }
      check_room(sw_st8, room.relays);
    }
  }
  
  // SET RELAYS STATE
  // -------------------
  if (ready2update) {
    ready2update = false;
    while (!ready2update) {
      // wait for next Zero Cross round
    }
    delay(4);
    update_relays();
    ready2update = false;
  }
}

void update_relays() {
  for (int i=0; i < SZ(out_arr); ++i)
  {
    out_arr[i].write8(st8[i]);
  }
}

void zerox_detected() {
  ready2update = true;
}

boolean sw_state(int swId) {
  return !bitRead(in[swId/8], swId%8);
}

void check_room(bool swState, int relayIds[]) {
      
  if (state[relayIds[0]] != swState) {
    if (state[relayIds[0]]) {
      for (int i=0; relayIds[i] != NA; ++i) {
        // Toggle i/8th relay state
        st8[relayIds[i]/8] ^= 1UL << (relayIds[i] % 8);
      }
    }
    
    for (int i=0; relayIds[i] != NA; ++i) {
      state[relayIds[i]] = !state[relayIds[i]];
    }
  }  
}

void check_room2(room_lgt room) {
  boolean sw_st8 = false;
  for (int si = 0; room.sw_buttons[si] != NA; ++si)
  {
    sw_st8 |= sw_state(room.sw_buttons[si]);
  }
  if (sw_st8) {
    
  }

  if (state[room.relays[0]] != sw_st8) {
    if (state[room.relays[0]]) {
      for (int i=0; room.relays[i] != NA; ++i) {
        // Toggle i/8th relay state
        st8[room.relays[i]/8] ^= 1UL << (room.relays[i] % 8);
      }
    }
    
    for (int i=0; room.relays[i] != NA; ++i) {
      state[room.relays[i]] = !state[room.relays[i]];
    }
  }  
}

