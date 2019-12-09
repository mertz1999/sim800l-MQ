
#include <SoftwareSerial.h>  //----------library for send and recive data serial with AT command------------

//---------Create software serial object to c//ommunicate with SIM800L-----------
//-----------SIM800L Tx & Rx is connected to Arduino #3 & #2------------
SoftwareSerial mySerial(3, 2);


//--------Define Variables -------------
int smokeMQ2 = A5;
int smokeMQ5 = A4;
int flag = 0;
int mq2Thres = 500;
int mq5Thres = 500;
String text = "MQ2 :";
String text2 = " MQ5 :";
String high = "High";
String low = "low";
String MQ2 = "";
String MQ5 = "";
String st;
int length1;

void setup() {
  //------set MQ Sensors Pin as Analog Input Pin---------
  pinMode(smokeMQ2, INPUT);
  pinMode(smokeMQ5, INPUT);
  
  //------Begin serial communication with Arduino and Arduino IDE (Serial Monitor)--------
  Serial.begin(9600);
  
  //------Begin serial communication with Arduino and SIM800L------
  mySerial.begin(9600);

  //-----Delay For First Setup--------
  Serial.println("Initializing..."); 
  delay(1000);

  //---------Ready For First recived Msg--------
  mySerial.println("AT");                   //-------Once the handshake test is successful, it will back to OK--------
  updateSerial();
  mySerial.println("AT+CMGF=1");            //------ Configuring TEXT mode --------
  updateSerial();
  mySerial.println("AT+CSQ");               //------ signal strength of the device --------
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0");    //-------- Decides how newly arrived SMS messages should be handled---------
  updateSerial();
  delay(1000);
}

void loop() {
  //Read MQ Sensors 
  int readSensorMQ2 = analogRead(smokeMQ2);
  int readSensorMQ5 = analogRead(smokeMQ5);

  //Print Sensor data in serial for test
  Serial.print("read MQ2: ");
  Serial.println(readSensorMQ2);
  Serial.print("read MQ5: ");
  Serial.println(readSensorMQ5);

  //Send Message if Recived Sensor Data is Higher that Sensor Thres
  if(readSensorMQ2 > mq2Thres || readSensorMQ5 > mq5Thres){
    if(flag = 0){
        if(readSensorMQ2 >= mq2Thres ){
             MQ2 = high;
           }else if(readSensorMQ2 < mq2Thres ){
             MQ2 = low;
           }
        if(readSensorMQ5 >= mq5Thres ){
            MQ5 = high;
           }else if(readSensorMQ5 < mq5Thres ){
            MQ5 = low;
        }
        sentMessage(text,text2,MQ2,MQ5);
        readyRecive();
    flag = 1;
     }
    }
    
  if(readSensorMQ2 <= 80  && readSensorMQ5 <= 300){
    flag = 0;
    delay(100);
  }


   //If Recive Message find *1 and then send sensor data and then be ready to recive another msg
   if (mySerial.available()) {
     st =  mySerial.readString();
     length1 = st.indexOf('*');
     length1 += 1;
     Serial.println(length1);
     Serial.println(st[length1]);
     Serial.println(st);
     if(st[length1] = '1'){
      if(readSensorMQ2 >= mq2Thres ){
        MQ2 = high;
      }else if(readSensorMQ2 < mq2Thres ){
        MQ2 = low;
      }
      if(readSensorMQ5 >= mq5Thres ){
        MQ5 = high;
      }else if(readSensorMQ5 < mq5Thres ){
        MQ5 = low;
      }
      sentMessage(text,text2,MQ2,MQ5);
      readyRecive();
     }
   }


  
  delay(1000);
}





//Function for update serial port
void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

//Function for Sent Msg
void sentMessage(String text,String text2,String readSensorMQ2,String readSensorMQ5){
      mySerial.println("AT");
      updateSerial();
      mySerial.println("AT+CMGF=1");                                 // Configuring TEXT mode
      updateSerial();
      mySerial.println("AT+CSMP=17,167,0,8");                        // Configuring TEXT mode
      updateSerial();
      mySerial.println("AT+CMGS=\"+989330091991\"");                 //change ZZ with country code and xxxxxxxxxxx with phone number to sms
      updateSerial();
      mySerial.print(text + readSensorMQ2 + text2 + readSensorMQ5);  //text content 
      updateSerial();
      mySerial.write(26);
      updateSerial();
      mySerial.print("AT+CMGDA=\"DEL ALL\"");                        //Delete msgs
      updateSerial();
      delay(5000);
  
}

//Function for be ready to Recive msg
void readyRecive(){
      mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
      updateSerial();
      mySerial.println("AT+CMGF=1"); // Configuring TEXT mode 
      updateSerial();
      mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
      updateSerial();
      delay(1000);
}
