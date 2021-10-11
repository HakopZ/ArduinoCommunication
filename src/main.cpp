#include <Arduino.h>

int disconnectCount = 0;

char Message[8];

char ReadMessage[8];
//int MessageID = 0;
char *Messages[8];
int MessageCount = 0;
long myTime;
long currentTime;

char NumArray[5];
char FrontOfEnvelope[3] = "/*";
char Seperator = '|';
char EndOfEnvelope[3] = "*/";
int ExpectedIndex = 0;
enum States
{
  WaitingForEnvelope = 0,
  Heartbeating = 1,
  Disconnected = 2,
};

enum EnvelopeStates
{
  FindStart = 0,
  ReadMessageLength = 1,
  ReadMessage = 2,
};

States currentState;
void setup()
{
  // put your setup code here, to run once:
  Serial1.begin(115200);
  Serial.begin(9600);
  //Serial1.begin(115200);
  myTime = millis();
  currentState = WaitingForEnvelope;
}

void SendToUno()
{

  memset(Message, 0, sizeof(Message));
  Message[0] = '|';
  for (int i = 0; i < 6; i++)
  {
    Message[i + 1] = (char)random(97, 120);
  }
  Message[7] = '|';
  //memcpy((void *)&Messages[MessageCount], (void *)Message, 8);
  //Messages[MessageCount] = Message;
  //MessageCount++;

  Serial1.write(Message, 8);
}

bool GetMessageFromUno()
{
  Serial1.read();
  // memset(ReadMessage, 0, sizeof(ReadMessage));
  char Start[1];
  Serial1.readBytes(Start, 1);
  char MessageLength[5];
  if (Start[0] != '/')
  {
    return false;
  }
  if (Start[0] == '/')
  {
    Serial1.readBytesUntil('|', MessageLength, 5);
  }

  Serial.write(MessageLength, 8);
  int length = 0;
  sscanf(MessageLength, "%d", length);
  // Serial1.readBytesUntil('', ReadMessage, length);

  if (ReadMessage == "SensorOn" || ReadMessage == "Beating")
  {
    return true;
  }
  else
  {
    return false;
  }
}
int NumArrayCount = 0;
int Length;
char *Message;
void loop()
{
  currentTime = millis();
  switch (currentState)
  {
  case WaitingForEnvelope:

    EnvelopeStates es = FindStart;
    switch (es)
    {
    case FindStart:
      if (ExpectedIndex > 1)
      {
        ExpectedIndex = 0;
        es = ReadMessageLength;
      }
      if ((char)Serial1.read() != FrontOfEnvelope[ExpectedIndex])
      {
        ExpectedIndex++;
      }
      break;
    case ReadMessageLength:
      char temp = (char)Serial1.read();
      if (temp != Seperator)
      {
        NumArray[NumArrayCount] = temp;
        NumArrayCount++;
      }
      else
      {
        sscanf(NumArray, "%d", &Length);
        es = ReadMessage;
      }
      break;
    case ReadMessage:
      if ((char)Serial1.read() != EndOfEnvelope[ExpectedIndex])
      {
      
      }
    default:
      break;
    }
    break;
  case Heartbeating:
    if (disconnectCount > 10)
    {
      currentState = Disconnected;
    }
    if (currentTime - myTime >= 1000)
    {
      myTime = millis();
      if (!GetMessageFromUno())
      {
        disconnectCount++;
      }
      else
      {
        disconnectCount = 0;
      }
    }
  }
  /*
  if(disconnectCount > 0 && disconnectCount < 10)
  {
    Serial.println("Reconnect");
  }
  else if(disconnectCount > 10)
  {
    Serial.println("Failed to connect");
  }
  if(!GetMessageFromUno())
  {
    disconnectCount++;
  }
  else
  {
    disconnectCount = 0;
    Serial.println("Connected");
  }*/
}
}
