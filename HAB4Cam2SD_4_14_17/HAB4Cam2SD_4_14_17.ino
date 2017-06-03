/*This is the final program for the cameras as of 4-12-17
  -This takes pitures with all six camersas
  -It will create a text document to save what picture it left off on when it powered off
  -I removed all things to do with serial output
*/

#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <SD.h>
#include "memorysaver.h"
//This demo can only work on OV2640_MINI_2MP and  OV5642_MINI_5MP and OV5642_MINI_5MP_BIT_ROTATION_FIXED platform.
#if !(defined OV5642_MINI_5MP || defined OV5642_MINI_5MP_BIT_ROTATION_FIXED || defined OV2640_MINI_2MP)
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif
#define SD_CS 9

// Set pin 4,5,6,7 as the slave select for SPI:
const int CS1 = 4;
const int CS2 = 5;
const int CS3 = 6;
const int CS4 = 7;
const int CS5 = 1;
const int CS6 = 3;

bool CAM1_EXIST = false;
bool CAM2_EXIST = false;
bool CAM3_EXIST = false;
bool CAM4_EXIST = false;
bool CAM5_EXIST = false;
bool CAM6_EXIST = false;



ArduCAM myCAM1(OV2640, CS1);
ArduCAM myCAM2(OV2640, CS2);
ArduCAM myCAM3(OV2640, CS3);
ArduCAM myCAM4(OV2640, CS4);
ArduCAM myCAM5(OV2640, CS5);
ArduCAM myCAM6(OV2640, CS6);
int k;

//Test the camera object passed to it, notice the pass by reference variables
bool CheckCamera(ArduCAM &camera, bool &camBool, int camNum) {
  camera.write_reg(ARDUCHIP_TEST1, 0x55);       //Write a value to the camera?
  uint8_t temp = camera.read_reg(ARDUCHIP_TEST1);     //Read that same value from the camera
  if (temp == 0x55)                           //If that value is not what was just written                                        //If the value was written and read correctly
  {
    camBool = true;
  }
  return camBool;
}


void setup()
{
  //Initialize SD Card
  while (!SD.begin(SD_CS))
  {
    delay(1000);
  }
  
  //Creates Save of Picture Progress ?but does not check for an existing one?
  File myFile;
  myFile = SD.open("pic.txt", FILE_WRITE);
  if (myFile.read() != 0)
    myFile.println("0");
  myFile.close();

  myFile = SD.open("pic.txt", FILE_READ);
  k = myFile.read();/////////////////////////////////////////
  myFile.close();

  // put your setup code here, to run once:
  uint8_t vid, pid;
  Wire.begin();

  // set the CS output:
  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);
  pinMode(CS3, OUTPUT);
  pinMode(CS4, OUTPUT);
  pinMode(CS5, OUTPUT);
  pinMode(CS6, OUTPUT);
  pinMode(SD_CS, OUTPUT);

  // initialize SPI:
  SPI.begin();
  
  //Check if the 4 ArduCAM Mini 2MP Cameras' SPI bus is OK
  myFile = SD.open("Cams.txt",FILE_WRITE);
  while (1)
  {
    if(CheckCamera(myCAM1, CAM1_EXIST, 1)) myFile.println("Camera 1 active.");
    if(CheckCamera(myCAM2, CAM2_EXIST, 2)) myFile.println("Camera 2 active.");
    if(CheckCamera(myCAM3, CAM3_EXIST, 3)) myFile.println("Camera 3 active.");
    if(CheckCamera(myCAM4, CAM4_EXIST, 4)) myFile.println("Camera 4 active.");
    if(CheckCamera(myCAM5, CAM5_EXIST, 5)) myFile.println("Camera 5 active.");
    if(CheckCamera(myCAM6, CAM6_EXIST, 6)) myFile.println("Camera 6 active.");

    if (!(CAM1_EXIST || CAM2_EXIST || CAM3_EXIST || CAM4_EXIST || CAM5_EXIST || CAM6_EXIST))
    {
      delay(1000); continue;
    }
    else
      break;
  }
  myFile.close();
  

  
  //Chris: I don't think we really need this next section
#if defined (OV2640_MINI_2MP)
  while (1)
  {
    //Check if the camera module type is OV2640
    myCAM1.wrSensorReg8_8(0xff, 0x01);
    myCAM1.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM1.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
    {
      delay(1000); continue;
    }
    else
    {
      break;
    }
  }
#else
  while (1)
  {
    //Check if the camera module type is OV5642
    myCAM1.wrSensorReg16_8(0xff, 0x01);
    myCAM1.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
    myCAM1.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
    if ((vid != 0x56) || (pid != 0x42))
    {
      delay(1000); continue;
    }
    else
    {
      break;
    }
  }
#endif
  //

  //Set camera output options
  myCAM1.set_format(JPEG);
  myCAM1.InitCAM();
#if defined (OV2640_MINI_2MP)
  myCAM1.OV2640_set_JPEG_size(OV2640_1600x1200);
  //Literally this whole next part isn't active
#else
  myCAM1.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
  myCAM2.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
  myCAM3.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
  myCAM4.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
  myCAM1.OV5642_set_JPEG_size(OV2640_1600x1200);
  //
#endif
  delay(1000);
  myCAM1.clear_fifo_flag();
  myCAM2.clear_fifo_flag();
  myCAM3.clear_fifo_flag();
  myCAM4.clear_fifo_flag();


}

void loop()
{
  //Take picture
  if (CAM1_EXIST) myCAMTakePicture(myCAM1);
  if (CAM2_EXIST) myCAMTakePicture(myCAM2);
  if (CAM3_EXIST) myCAMTakePicture(myCAM3);
  if (CAM4_EXIST) myCAMTakePicture(myCAM4);
  if (CAM5_EXIST) myCAMTakePicture(myCAM5);
  if (CAM6_EXIST) myCAMTakePicture(myCAM6);

  //Save Picture
  if (CAM1_EXIST) myCAMSaveToSDFile(myCAM1);
  if (CAM2_EXIST) myCAMSaveToSDFile(myCAM2);
  if (CAM3_EXIST) myCAMSaveToSDFile(myCAM3);
  if (CAM4_EXIST) myCAMSaveToSDFile(myCAM4);
  if (CAM5_EXIST) myCAMSaveToSDFile(myCAM5);
  if (CAM6_EXIST) myCAMSaveToSDFile(myCAM6);
}

void myCAMTakePicture(ArduCAM  myCAM)
{
  //Flush the FIFO
  myCAM.flush_fifo();
  
  //Clear the capture done flag
  myCAM.clear_fifo_flag();
  
  //Start capture
  myCAM.start_capture();
  
  //Wait for the camera to say it's finished
  while (!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
}

void myCAMSaveToSDFile(ArduCAM myCAM)
{
  //Create a string with length 7 (8-1)
  char str[8] = "";
  
  //Create a byte buffer
  byte buf[256];
  
  static int i = 0;
  uint8_t temp = 0, temp_last = 0;
  uint32_t length = 0;
  bool is_header = false;

  //Create a file object
  File outFile;
  
  length = myCAM.read_fifo_length();
  if (length >= MAX_FIFO_SIZE) //384K
  {
    return;
  }
  if (length == 0 ) //0 kb
  {
    return;
  }

  //Construct a file name
  k = k + 1;
  itoa(k, str, 36);
  
  //Creates Save of Picture Progress
  //Chris: We may want to do this only every camera sequence vs. every camera shot to save write cycles
  File myFile;
  myFile = SD.open("pic.txt", FILE_WRITE);
  myFile.println(k);
  myFile.close();

  //Add ".jpg" to the end of the file name
  strcat(str, ".jpg");

  //Open the new picture file
  outFile = SD.open(str, O_WRITE | O_CREAT | O_TRUNC);
  if (!outFile)
  {
    return;
  }

  //Select the camera
  myCAM.CS_LOW();

  //Get a fifo burst from the camera
  myCAM.set_fifo_burst();

  //Write the fifo burst to the .jpg file
  while ( length-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    //Read JPEG data from FIFO
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    {
      buf[i++] = temp;  //save the last  0XD9
      //Write the remain bytes in the buffer
      myCAM.CS_HIGH();
      outFile.write(buf, i);
      //Close the file
      outFile.close();
      is_header = false;
      i = 0;
    }
    if (is_header == true)
    {
      //Write image data to buffer if not full
      if (i < 256)
        buf[i++] = temp;
      else
      {
        //Write 256 bytes image data to file
        myCAM.CS_HIGH();
        outFile.write(buf, 256);
        i = 0;
        buf[i++] = temp;
        myCAM.CS_LOW();
        myCAM.set_fifo_burst();
      }
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      buf[i++] = temp_last;
      buf[i++] = temp;
    }
  }
  //Is the file ever closed?
  //Well, it is deconstructed after this function returns which includes .close()
}
