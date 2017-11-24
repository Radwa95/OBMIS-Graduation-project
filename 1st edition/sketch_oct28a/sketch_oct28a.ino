//connect 2 heart pulse config : 
//  Variables
const int numPulseSensors = 2;
const int threshold = 530;
int pulsePin[numPulseSensors];                 // Pulse Sensor purple wire connected to analog pin 0

// Volatile Variables, used in the interrupt service routine!
volatile int BPM[numPulseSensors];                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal[numPulseSensors];                // holds the incoming raw data
volatile int IBI[numPulseSensors];             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse[numPulseSensors];     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS[numPulseSensors];        // becomes true when Arduoino finds a beat.
// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = false;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse

volatile int rate[numPulseSensors][10];                    // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime[numPulseSensors];           // used to find IBI
volatile int P[numPulseSensors];                      // used to find peak in pulse wave, seeded
volatile int T[numPulseSensors];                     // used to find trough in pulse wave, seeded
volatile int thresh[numPulseSensors];                // used to find instant moment of heart beat, seeded
volatile int amp[numPulseSensors];                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat[numPulseSensors];        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat[numPulseSensors];      // used to seed rate array so we startup with reasonable BPM

//___________________________________________________________________________________________________________________

//configuration of human body temprature .
#include <math.h>
#define READING6_PIN 6
#define READING7_PIN 7
#define READING8_PIN 8

double R1 = 10000.0; //resistance put in parallel
double V_IN = 5.0;

double A = 1.129148e-3;
double B = 2.34125e-4;
double C = 8.76741e-8;
double K = 9.5; // mW/dec C – dissipation factor

double SteinhartHart(double R)
{
// calculate temperature
double logR  = log(R);
double logR3 = logR * logR * logR;

return 1.0 / (A + B * logR + C * logR3 );
}
//_________________________________________

//configuration of ambient temperature 
#include <dht.h>

dht DHT;

#define DHT11_PIN A14
 const int duration=2017;
 unsigned long refdht11=0;
//_________________________________________
int sorm=1;
//end of configuration .

void setup() 
{
  setStuph();                       // initialize variables and pins
  
  Serial.begin(1000000);             // we agree to talk fast!
  interruptSetup(); // sets up to read Pulse Sensor signal every 2mS
  
  pinMode(A14,INPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
}

void loop() 
{
  //code of dht11
if (millis()-duration>=refdht11)
 {     
   // arrays of normal :- 
   float ambhum[4],ambdh[4],amblm,cel[4];
   float A2[4],A3[4],A4[4],A5[4],alcavg[4],alchigh[4],col[4];
   double celsius6[4],celsius7[4],celsius8[4];
   int beat1[4],beat2[4];
      
   for(int q=0;q<5;q++)
     {
       DHT.read11(DHT11_PIN);
       ambhum[q]=DHT.humidity;
       ambdh[q]=DHT.temperature*0.4;
//__________________________________________________

  // lm35 but coneected with A10 :
  
       amblm=delayoptimize(10);
//__________________________________________________

  // reading alcohole sensors 
  
       A2[q]=delayoptimize(2); //print and send.and connect -76 s 
       A3[q]=delayoptimize(3);  //print and send.and connect refence s 
       A4[q]=delayoptimize(4);  //print and send.and connect +12 s
       A5[q]=delayoptimize(5);  //print and send.and connect -39 s 
       
//_________________________________________________ 

  //reading of human body temprature: 
     
      double adc_raw4 = analogRead(READING6_PIN);
      double adc_raw5 = analogRead(READING7_PIN);
      double adc_raw6 = analogRead(READING8_PIN);
//__________________________________________________

//operations : 
// 1)LM35 
      float mv = ( amblm/1024.0)*5000; 
      cel[q] = (mv/10);
  
// 2)Alcohole 
      alcavg[q]=(A2[q]+A3[q]+A4[q]+A5[q])/4;
      alchigh[q]=senario1 (A2[q],A3[q],A4[q],A5[q] );
      col[q]=senario2 (A2[q],A3[q],A4[q],A5[q] );
 
// 3)Human body temp 
      double V4 =  adc_raw4 / 1024 * V_IN;
      double Rth4 = ((R1 * V4 ) / (V_IN - V4));
      double kelvin4 = (SteinhartHart(Rth4) - ((V4*V4)/(K * Rth4)));
      celsius6[q] = kelvin4 - 273.15; //print and send. 

      double V5 =  adc_raw5 / 1024 * V_IN;
      double Rth5 = ((R1 * V5 ) / (V_IN - V5));
      double kelvin5 = (SteinhartHart(Rth5) - ((V5*V5)/(K * Rth5)));
      celsius7[q] = kelvin5 - 273.15; //print and send.

      double V6 =  adc_raw6 / 1024 * V_IN;
      double Rth6 = ((R1 * V6 ) / (V_IN - V6));
      double kelvin6 = (SteinhartHart(Rth6) - ((V6*V6)/(K * Rth6)));
      celsius8[q] = kelvin6 - 273.15;  //print and send.
  }
//___________________________________________________________________
//  Find normalized readings every 5 times 
float sumdh=0.0,sumhum=0.0,sumlm=0.0;     //sum of ambient temprature
float sumA2=0.0,sumA3=0.0,sumA4=0.0,sumA5=0.0,sumav=0.0,sumhg=0.0,sumcol=0.0;     //sum of alcohole
double sum6=0,sum7=0,sum8=0;        // sum of human body temp
 
for(int p=0;p<5;p++)
{
  sumdh=sumdh+ambdh[p];
  sumhum=sumhum+ambhum[p];
  sumlm=sumlm+cel[p];

  sumA2=sumA2+A2[p];
  sumA3=sumA3+A3[p];
  sumA4=sumA4+A4[p];
  sumA5=sumA5+A5[p];
  sumav=sumav+alcavg[p];
  sumhg=sumhg+alchigh[p];
  sumcol=sumcol+col[p];
  
  sum6=sum6+celsius6[p];
  sum7=sum7+celsius7[p];
  sum8=sum8+celsius8[p];
  
}
sumdh=sumdh/5;
sumhum=sumhum/5;
sumlm=sumlm/5;

sumA2=sumA2/5;
sumA3=sumA3/5;
sumA4=sumA4/5;
sumA5=sumA5/5;
sumav=sumav/5;
sumhg=sumhg/5;
sumcol=sumcol/5;

sum6=sum6/5;
sum7=sum7/5;
sum8=sum8/5;

//_________________________________________________________________________
// printing or sending
sorm=digitalRead(2); 
switch(sorm)
{
  case 0:
      digitalWrite(5,HIGH);
      digitalWrite(6,LOW);
      
      Serial.print(sumA2-55);Serial.print("  ");Serial.print(",");Serial.print("  ");
      Serial.print(sumA3);Serial.print("  ");Serial.print(",");Serial.print("  ");
      Serial.print(sumA4);Serial.print("  ");Serial.print(",");Serial.print("  ");
      Serial.print(sumA5-20);Serial.print("  ");Serial.print(",");Serial.print("  ");
      Serial.print(sumav);Serial.print("  ");Serial.print(",");Serial.print("  ");
      Serial.print(sumhg);Serial.print("  ");Serial.print(",");Serial.print("  ");
      Serial.print(sumcol);Serial.print("  ");Serial.print(",");Serial.print("  ");
      
      Serial.print(((sum6)/2)+5.1);Serial.print("  ");Serial.print(",");Serial.print("  ");
      Serial.print(((sum7)/2)+5.03);Serial.print("  ");Serial.print(",");Serial.print("  ");
      Serial.print(((sum8)/2)+5.08);Serial.print("  ");Serial.print(",");Serial.print("  ");
      
      Serial.print(BPM[0]);Serial.print("  ");Serial.print(",");Serial.print("  ");
      Serial.print(BPM[1]);Serial.print("  ");Serial.print(",");Serial.print("  ");

      Serial.print(sumhum); Serial.print(" ");Serial.print(",");Serial.print("  ");
      Serial.print(sumdh); Serial.print("  ");Serial.print(",");Serial.print("  ");
      Serial.print(sumlm);Serial.println("  ");
      
      break;
      
  case 1: 
      digitalWrite(5,LOW);
      digitalWrite(6,HIGH);
      
      Serial.println("    Start of Monitoring at this second ");
      Serial.println("1)Heart pulse Reading : ");
      Serial.print("    BPM : S1= ");Serial.print(BPM[0]);Serial.print("\t"); Serial.print("S2= ");Serial.println(BPM[1]);

      Serial.println("----------");
      Serial.println("2)Human Body Temp : ");
      Serial.print("    T1= "); Serial.print((sum6/2)+5.1);Serial.print(" C' ");  Serial.print("     T2= "); Serial.print((sum7/2)+5.03);Serial.print(" C' "); Serial.print("     T3= "); Serial.print((sum8/2)+5.08);Serial.println(" C' ");

      Serial.println("----------");
      Serial.println("3)Alcohole Reading : ");
      Serial.print("    BAC1= "); Serial.print(sumA2);
      Serial.print("    BAC2= "); Serial.println(sumA3);
      Serial.print("    BAC3= "); Serial.print(sumA4);
      Serial.print("    BAC4= "); Serial.println(sumA5); 
      Serial.print("  --> Average = "); Serial.println(sumav);
      Serial.print("  --> Highest = "); Serial.println(sumhg);
      Serial.print("  --> Collective Weight = "); Serial.println(sumcol);

      Serial.println("----------");
      Serial.println("4)Ambient Temp : ");
      Serial.print("   Current humidity = ");Serial.print(sumhum);Serial.println("%");
      Serial.print("   Temperature(1) = ");Serial.print(sumdh); Serial.println(" C");
      Serial.print("   Temperature(2) = "); Serial.print(sumlm);Serial.println(" c");
      Serial.print(   " End of This Second Monitoring " );
      Serial.println("----------");
      // add time library to show time and date if it neccessary               
      break;
  default:
        Serial.println("Please Select Monitoring or Sending and check button connection ");
        break;        
}

  refdht11=millis();
  
    }  
}


// INITIALIZE VARIABLES AND INPUT/OUTPUT PINS
void setStuph() 
{

  for (int i = 0; i < numPulseSensors; i++) 
  {
    lastBeatTime[i] = 0;
    P[i] = T[i] = 512;
    amp[i] = 0;
    thresh[i] = threshold;
    amp[i] = 0;               // used to hold amplitude of pulse waveform, seeded
    firstBeat[i] = true;      // used to seed rate array so we startup with reasonable BPM
    secondBeat[i] = false;
    IBI[i] = 600;             // int that holds the time interval between beats! Must be seeded!
    Pulse[i] = false;         // "True" when User's live heartbeat is detected. "False" when not a "live beat".
    QS[i] = false;
    switch (i) {
      case  0:
        pulsePin[i] = 0;    // pulse pin Analog 0
        break;
      case  1:
        pulsePin[i] = 1;    // pulse pin Analog 1
        default:
        break;
    }
  }
}
float senario1 (float r1 ,float r2 ,float r3 ,float r4 )
{ 
  
  if (r1>r2 & r1>r3 & r1>r4 ) 
  {
    return r1 ;
  }
  if (r2>r1 & r2>r3 & r2>r4 ) 
  {
    return r2 ;
  }
  if (r3>r1 & r3>r2 & r3>r4 ) 
  {
    return r3 ;
  }
  if (r4>r1 & r4>r2 & r4>r4 ) 
  {
    return r1 ;
  }
}

float senario2 (float r1 ,float r2 ,float r3 ,float r4 )
{
  float w1=r1*0.3;
  float w2=r2*0.3; 
  float w3=r3*0.3; 
  float w4=r4*0.1;
  float col=w1+w2+w3+w4;
  return col;  
}

float delayoptimize ( int x )
{
   float reading = 0.0;
   reading = analogRead(x);
   return reading ;
}
