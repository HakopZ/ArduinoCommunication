#include <Arduino.h>

int disconnectCount = 0;


long currentTime;

char NumArray[5];
char FrontOfEnvelope[3] = "/*";
char Seperator = '|';
char EndOfEnvelope[3] = "*/";
char ExpectedMessage[] = "Heartbeat";
char* Message;


enum EnvelopeStates
{
  FindStart = 0,
  ReadMessageLength = 1,
  ReadMessage = 2,
};

void setup()
{
  // put your setup code here, to run once:
  Serial1.begin(115200);
  Serial.begin(9600);
  //Serial1.begin(115200);
  
  currentTime = millis();

}

int NumArrayCount = 0;
int Length;
char* ReadMessageWithLength(int length)
{

  char Message[length];
if(length == 1)
{
  Message[0] = (char)Serial.read();
  return Message;
}
  int count = 0;
  char Character = ' ';
  char NextCharacter = ' ';
  for (int i = 0; i < length - 1; i++)
  {
    Character = (char)Serial1.read();
    NextCharacter = (char)Serial1.read();
    if(Character == EndOfEnvelope[0] && NextCharacter == EndOfEnvelope[1])
    {
      break;
    }
    Message[i] = Character;
    Message[i+1] = NextCharacter;
  }
  return Message;

}
char* EnvelopeWait()
{
    EnvelopeStates es = FindStart;
    switch (es)
    {
    case FindStart:
      if ((char)Serial1.read() == FrontOfEnvelope[0] && (char)Serial1.read() == FrontOfEnvelope[1])
      {
        es = ReadMessageLength;
      }
      else
      {
        return "Failed";
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
      es = FindStart;
      return ReadMessageWithLength(Length);
    default:
      break;
    }
}

void loop()
{

  Message = EnvelopeWait();
  
  if(millis() - currentTime > 1000)
  {
    
    
    if(Message != ExpectedMessage)
    {
      disconnectCount++;
      if(disconnectCount>10)
      {
        Serial.println("Disconnected");
      }
      else
      {
        Serial.println("Try To Reconnect");
      }
    }
    else
    {
      Serial.println("Success!!");
      disconnectCount = 0;
    }

    currentTime = millis();
  }
 
  }

