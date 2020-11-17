#include <TimerOne.h>

volatile int sample; //Holder siste sample
volatile int lastSample; //Sample før siste sample


int  bn = 0; //Utgangsignalet b(t)
int  un = 0; //Utgangsignalet u(t)
float  xZero = 0;
float  xOne = 0;
float  sumL = 0;
float  sumH = 0;
float  aL = 0.2;
float  aH = -0.2;
float avgL, avgH;
int count = 0;
int numSamples = 10; //Antall samples som skal taes gjennomsnitt av


void setup() {
  
  // Oppsett av timer interrupt
  Timer1.initialize(500); // 500 mikrosekund mellom hver sample -> gir F_s = 2kHz
  // Argumentet i "attachInterrupt" bestemmer hvilken funskjon som er interrupt handler 
  Timer1.attachInterrupt(takeSample); 

  //Velger hvilke pins på arduino UNO som skal være output og input
  pinMode(8, OUTPUT); //Velger pin 8 som output for b(t)
  pinMode(7, OUTPUT); //Velger pin 7 som output for u(t)
  pinMode(A0, INPUT); //Velger pin A0 som input for r(t)
}

  
  
//En loop som kjører konstant og sørger for at det hele tiden blir sett etter ny sample
void loop() {  

    }

void takeSample(void){
   
  
  sample = analogRead(A0); //Får et nytt sample

  //Rekursive filter:
  xZero = (sample-lastSample) + aL*xZero; //Lavpassfilter
  xOne = (sample-lastSample) + aH*xOne; //Høypassfilter
  
  lastSample = sample; //Setter sample til forrige sample, ettersom det nå har vært gjennom filtrering. 

  //Absoluttverdi av utgangen på filtrene og legger til i sum av alle absoluttverdiene. Dette brukes videre til middelverdifilteret:
  sumL += abs(xZero); 
  sumH += abs(xOne);

  //Øker variabelen count med 1. 
  count++;

  //Sjekker om count er blitt lik antall ønskede samples, numSamples
  if(count == numSamples){
    
    //Middelverdi (estimat av amplituren) av de absoluttverdien til de filtrerte signalene:
    avgL = sumL/numSamples;
    avgH = sumH/numSamples;

  //Sjekker at estimatet av amplituden er over 20 for å sørge for å sjekke at det er et inngangssignal på FSK-demodulatoren.
    if (avgH < 20 && avgL < 20){
      //Setter utgangen u(t) til lav dersom ingen av middelverdiene er over.
      un = 0;
      digitalWrite(7, LOW);
    
      
    } else {
      //Setter u(t) til høy dersom det er en amplitude
          un = 1;
      //b(t) = 1 dersom amplituden gjennom høypassfilteret er større enn lavpassfilteret 
        if (avgH > avgL){
          bn = 1;
          digitalWrite(8, HIGH);
        } 

        //b(t) = 0 dersom amplituden gjennom lavpassfilteret er større enn høypassfilteret.
        else if (avgH < avgL){
          bn = 0;
          digitalWrite(8, LOW);
        }
      //Setter u(t) til høy dersom det er en amplitude
      digitalWrite(7, HIGH);
    }

  //Nullstilling av variabler for et nytt intervall med samples:
    sumL = 0;
    sumH = 0;
    xZero = 0;
    xOne = 0;
    avgH = 0;
    avgL = 0;
    count = 0;
  }

}
 
