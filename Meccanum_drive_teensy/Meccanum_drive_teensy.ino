
// Simple test of USB Host Mouse/Keyboard
//
// This example is in the public domain

#include "USBHost_t36.h"
//#include "debug_tt.h"


USBHost myusb;
JoystickController joystick1(myusb);
BluetoothController bluet(myusb, true, "0000");   // Version does pairing to device
//BluetoothController bluet(myusb);   // version assumes it already was paired
int user_axis[64];
uint32_t buttons_prev = 0;
int hat_prev = 0;
uint32_t buttons;
USBDriver *drivers[] = {&joystick1, &bluet};

#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"JOY1D", "Bluet"};

bool driver_active[CNT_DEVICES] = {false, false};

USBHIDInput *hiddrivers[] = {&joystick1};

#define CNT_HIDDEVICES (sizeof(hiddrivers)/sizeof(hiddrivers[0]))
const char * hid_driver_names[CNT_DEVICES] = {"Joystick1"};

bool hid_driver_active[CNT_DEVICES] = {false};

BTHIDInput *bthiddrivers[] = {&joystick1};
#define CNT_BTHIDDEVICES (sizeof(bthiddrivers)/sizeof(bthiddrivers[0]))
const char * bthid_driver_names[CNT_HIDDEVICES] = {"joystick"};
bool bthid_driver_active[CNT_HIDDEVICES] = {false};


bool show_short_form_data = false;
bool show_raw_data = false;

uint8_t joystick_left_trigger_value = 0;
uint8_t joystick_right_trigger_value = 0;
uint64_t joystick_full_notify_mask = (uint64_t) - 1;

int psAxis[64];
int psAxis_prev[64];
bool first_joystick_message = true;
uint8_t last_bdaddr[6] = {0, 0, 0, 0, 0, 0};

int m1_pwm = 1;
int m1_dir = 0;

int m2_pwm = 3;
int m2_dir = 2;

int m3_pwm = 5;
int m3_dir = 4;

int m4_pwm = 7;
int m4_dir = 6;

int max_pwm = 68; //6v--250rpm
//int dirPin4 = 2;
//int pwmPin4 = 3;
//=============================================================================
// Setup
//=============================================================================
int s1,s2,s3,s4;
void drive(int pwm_val, int pwmPin, int dirPin)
{
  if (pwm_val > 255)
    pwm_val = 255;
  else if (pwm_val < -255)
    pwm_val = -255;

  digitalWrite(dirPin, (pwm_val <= 0 ? LOW : HIGH));
  Serial.printf("Dir: %d\n", (pwm_val <= 0 ? LOW : HIGH));
  Serial.printf("PWM: %d\n", abs(pwm_val));
  analogWrite(pwmPin, abs(pwm_val));
}

void inverse_kine(int x, int y, int w, int z)
{
  int s1, s2, s3, s4;
  if (w != 0)
    s1 = s2 = s3 = s4 = w;
  else if(x!=0 || y!=0)
  {
    s1 = (int)(-x - y-1);
    s2 = (int)(-x + y-1);
    s3 = (int)(+x + y+1);
    s4 = (int)(x - y);

  }
  else
  s1=s2=s3=s4=0;
  Serial.printf("s1: %d  s2: %d  s3:%d  s4:%d\n", s1, s2, s3, s4);
  drive(s1, m1_pwm, m1_dir);
  drive(s2, m2_pwm, m2_dir);
  drive(s3, m3_pwm, m3_dir);
  drive(s4, m4_pwm, m4_dir);

}




void setup()
{

  Serial.begin(2000000);
  // while (!Serial) ; // wait for Arduino Serial Monitor

  Serial.println("\n\nUSB Host Testing - Joystick Bluetooth");
  if (CrashReport) Serial.print(CrashReport);
  myusb.begin();

  pinMode(m1_dir, OUTPUT);
  pinMode(m2_dir, OUTPUT);
  pinMode(m3_dir, OUTPUT);
  pinMode(m4_dir, OUTPUT);

  pinMode(13, OUTPUT);

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  digitalWrite(0, HIGH);
  pinMode(1, HIGH);
  analogWriteFrequency(m1_pwm, 9000);
  analogWriteFrequency(m2_pwm, 9000);
  analogWriteFrequency(m3_pwm, 9000);
  analogWriteFrequency(m4_pwm, 9000);

  //analogWriteFrequency(pwmPin4, 10000);
  //delay(2000);
  digitalWrite(13, HIGH);

  //  rawhid1.attachReceive(OnReceiveHidData);
  //  rawhid2.attachReceive(OnReceiveHidData);
}

//=============================================================================
// Loop
//=============================================================================
void loop()
{

  myusb.Task();

  //  if (Serial.available()) {
  //    int ch = Serial.read(); // get the first char.
  //    while (Serial.read() != -1) ;
  //    if ((ch == 'b') || (ch == 'B')) {
  //      Serial.println("Only notify on Basic Axis changes");
  //      joystick1.axisChangeNotifyMask(0x3ff);
  //    } else if ((ch == 'f') || (ch == 'F')) {
  //      Serial.println("Only notify on Full Axis changes");
  //      joystick1.axisChangeNotifyMask(joystick_full_notify_mask);
  //
  //    } else {
  //      if (show_short_form_data) {
  //        show_short_form_data = false;
  //        Serial.println("\n*** Show Full Data mode ***");
  //      } else {
  //        show_short_form_data = true;
  //        Serial.println("\n*** Show short form mode ***");
  //      }
  //    }
  //  }
  // check to see if the device list has changed:
  UpdateActiveDeviceInfo();

  if (joystick1.available()) {
    if (first_joystick_message) {
      Serial.printf("*** First Joystick message %x:%x ***\n",
                    joystick1.idVendor(), joystick1.idProduct());
      first_joystick_message = false;

      const uint8_t *psz = joystick1.manufacturer();
      if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
      psz = joystick1.product();
      if (psz && *psz) Serial.printf("  product: %s\n", psz);
      psz = joystick1.serialNumber();
      if (psz && *psz) Serial.printf("  Serial: %s\n", psz);

      // lets try to reduce number of fields that update
      joystick1.axisChangeNotifyMask(0xFFFFFl);
    }

    for (uint8_t i = 0; i < 64; i++) {
      psAxis_prev[i] = psAxis[i];
      psAxis[i] = joystick1.getAxis(i);
    }

    buttons = joystick1.getButtons();

    int left_x = psAxis[0] - 128;
    int left_y = psAxis[1] - 128;
    int right_x = psAxis[2] - 128;
    int right_y = psAxis[5] - 128;
    //    int pwm_r = 50;
    //    int pwm_l = 50;

    int pwm_r = psAxis[3];
    int pwm_l = psAxis[4];

    int x = map(left_x, -128, 128, max_pwm, -max_pwm);
    int y = map(left_y, -128, 128,max_pwm, -max_pwm);
    int w = map(right_x, -128, 128, max_pwm, -max_pwm);
    int z = map(right_y, -128, 128, 50, -50);

    Serial.printf("LX: %d, LY: %d, RX: %d, RY: %d \r\n", left_x, left_y, right_x, right_y);
    Serial.printf("L-Trig: %d, R-Trig: %d\r\n", pwm_l, pwm_r);

    //inverse_kine(x, y, w, z);
  drive(s1, m1_pwm, m1_dir);
  drive(s2, m2_pwm, m2_dir);
  drive(s3, m3_pwm, m3_dir);
  drive(s4, m4_pwm, m4_dir);



    delay(100);
  }
}








//=============================================================================
// UpdateActiveDeviceInfo
//=============================================================================
void UpdateActiveDeviceInfo() {

  for (uint8_t i = 0; i < CNT_DEVICES; i++) {
    if (*drivers[i] != driver_active[i]) {
      if (driver_active[i]) {
        Serial.printf("*** Device %s - disconnected ***\n", driver_names[i]);
        driver_active[i] = false;
      } else {
        Serial.printf("*** Device %s %x:%x - connected ***\n", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = drivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = drivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);

        if (drivers[i] == &bluet) {
          const uint8_t *bdaddr = bluet.myBDAddr();
          // remember it...
          Serial.printf("  BDADDR: %02X:%02X:%02X:%02X:%02X:%02X\n", bdaddr[5], bdaddr[4], bdaddr[3], bdaddr[2], bdaddr[1], bdaddr[0]);
          for (uint8_t i = 0; i < 6; i++) last_bdaddr[i] = bdaddr[i];
        }
      }
    }
  }

  for (uint8_t i = 0; i < CNT_HIDDEVICES; i++) {
    if (*hiddrivers[i] != hid_driver_active[i]) {
      if (hid_driver_active[i]) {
        Serial.printf("*** HID Device %s - disconnected ***\n", hid_driver_names[i]);
        hid_driver_active[i] = false;
      } else {
        Serial.printf("*** HID Device %s %x:%x - connected ***\n", hid_driver_names[i], hiddrivers[i]->idVendor(), hiddrivers[i]->idProduct());
        hid_driver_active[i] = true;

        const uint8_t *psz = hiddrivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = hiddrivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = hiddrivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);

        // See if this is our joystick object...
        if (hiddrivers[i] == &joystick1) {
          Serial.printf("  Joystick type: %d\n", joystick1.joystickType());
          if (joystick1.joystickType() == JoystickController::PS3_MOTION) {
            Serial.println("  PS3 Motion detected");
            //PS3_MOTION_timer = millis();  // set time for last event
            //PS3_MOTION_tried_to_pair_state = 0;
          }
        }

      }
    }
  }
  // Then Bluetooth devices̨
  for (uint8_t i = 0; i < CNT_BTHIDDEVICES; i++) {
    if (*bthiddrivers[i] != bthid_driver_active[i]) {
      if (bthid_driver_active[i]) {
        Serial.printf("*** BTHID Device %s - disconnected ***\n", hid_driver_names[i]);
        bthid_driver_active[i] = false;
      } else {
        Serial.printf("*** BTHID Device %s %x:%x - connected ***\n", hid_driver_names[i], hiddrivers[i]->idVendor(), hiddrivers[i]->idProduct());
        bthid_driver_active[i] = true;

        const uint8_t *psz = bthiddrivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = bthiddrivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = bthiddrivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);
      }
    }
  }
}
