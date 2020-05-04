// ================== Game Settings ================== //
int ledPins[5] = {A0, A1, A2, A3, A4};
int inPins[5] = {6, 7, 8, 9, 10};
int val = 0;
int score = 0;
int turn;
int gamedur=10; // game duration in seconds
unsigned long time, gamestart, gametime, ledtime;
// ================== END of Game Settings ================== //

// ================== Audio Settings/Functions ================== //
//--define notes-
#define  c     1915    // 261 Hz
#define  d     1700    // 294 Hz
#define  e     1519    // 329 Hz
#define  f     1432    // 349 Hz
#define  g     1275    // 392 Hz
#define  a     1136    // 440 Hz
#define  b     1014    // 493 Hz
#define  C     956    // 523 Hz
int speakerPin = A5;
//--define sounds in game--
enum mel{
power_on,//sound when game powers on
start_sound,//sound when game starts
hit_sound,//sound when player correctly hits led in time
end_sound, //sound when game ends!
sil //silent
};
enum mel curr_mel=power_on; //variable that controls what sound is being played,set to power on.
//--define sound notes and beats--
int pow_mel[] = {e,c,e} ; 
int pow_beats []= {1,1,1} ;
int start_mel[] = {e,e,e,C} ; 
int start_beats[]={1,1,1,3};
int hit_mel[] = {g} ;
int hit_beats []= {1} ;
int end_mel[] = {C,0,C,C,C,0,g,0,C,C} ; 
int end_beats[]={2,1,1,1,1,1,2,2,2,2};
int tempo = 100;
		
//playes specified note by melody
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

//loops over melody notes
void playMelody(int melody[], int beats[], int length){
	for (int i = 0; i < length; i++) {
	  	  if (melody[i] == 0) { 
		      delay(beats[i] * tempo); // rest
		    } else {
		      playTone(melody[i],beats[i] * tempo);
		    }
	    // pause between notes
	    delay(tempo/2); 
	  }
}

//plays sound according to mode specified by curr_mel
void playSound() {
	switch (curr_mel) {
	  case power_on:
 		  playMelody(pow_mel,pow_beats,sizeof(pow_mel)/sizeof(int));
	    break;
	  case hit_sound:
 		  playMelody(hit_mel,hit_beats,sizeof(hit_mel)/sizeof(int));
	    break;
	  case end_sound:
		  playMelody(end_mel,end_beats,sizeof(end_mel)/sizeof(int));
	    break;
	  default:
		  break;
	}
	curr_mel=sil; //set after sound
}
// ================== END of Audio Settings/Functions ================== //

// ================== LCD Settings/Functions  ================== //
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Important that must be copied!!

// Function called before starting the game while waiting for pressing start button
void lcdBeforeStart()
{
  lcd.clear();
  lcd.print("-------HELLO!-------");
  // set the cursor to column 0, line 1
  lcd.setCursor(0, 1);
  lcd.print("Welcome to Led Game!");
}

// Function called after pressing start button to count down for game start
void lcdStartGame()
{
  lcd.clear();
  // Print a message to the LCD.
  lcd.print("-------HELLO!-------");
  // set the cursor to column 0, line 1
  lcd.setCursor(0, 1);
  lcd.print("Game starts in ");
  //Count down from 3 with start melody
  for (int i = 3; i >= 0; i--)
  {
    lcd.setCursor(15, 1);
	  playTone(start_mel[3-i],start_beats[3-i] * tempo);
    lcd.print(i);
    delay(500);
  }
}

// Function called every time unit to display remaining time and score
void lcdPrintScore(int time, int score)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("----GAME PLAYING----");
  lcd.setCursor(0, 1);
  lcd.print("Time:");
  lcd.print(time);
  lcd.print("   ");
  lcd.print("Score:");
  lcd.print(score);
}

// Function called after time ends to display score
void lcdGameOver(int score)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("-----GAME OVER!-----");
  lcd.setCursor(0, 1);
  lcd.print("Your score:");
  lcd.print(score);
}
// ================== END of LCD Settings/Functions ================== //

// ================== Game Setup ================== //
void setup()
{
  // set up the LCD's number of columns and rows
  lcd.begin(20, 2); 
  // set seaker pin
  pinMode(speakerPin, OUTPUT);
  //set buttons and LED pins
  for (int i = 0; i < 5; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    pinMode(inPins[i], INPUT);
  }
  randomSeed(analogRead(5));
}
// ================== END of Game Setup ================== //

// ================== Main Game Logic ================== //
void loop()
{

  //Show welcome screen and play power on sound
  lcdBeforeStart();
  curr_mel=power_on;
  playSound();
  delay(800);
  //Start Game with countdown
  lcdStartGame();
  //Initialize game timers
  gamestart = millis(); 
  ledtime = millis(); //time that last led was lit (to switch between them)
  score = 0;
  turn = 0; //led turn
  //Game loop
  while (true)
  {
    gametime = millis() - gamestart;
    //If game duration ended, break game loop
    if (gametime % (gamedur*1000) == 0 && gametime != 0)
      break;
    time = millis() - ledtime; //time elapsed with last led lit.
    //If LED on time elaped, switch to a different LED
    if (time % 800 == 0 && time != 0)
    {
      digitalWrite(ledPins[turn], LOW);
      turn = random(1000) % 5;
      digitalWrite(ledPins[turn], HIGH);
      ledtime = millis();
      lcdPrintScore(gamedur - (gametime / 1000), score);
    }
    else //player still has time to hit button
    {
      //check button asssociated with LED
      val = digitalRead(inPins[turn]); 
      if (val == 1) 
      {
        //Successfully pressed Lit LED button in time, play hit sound and increment score
		    curr_mel=hit_sound;
		    playSound();
        score++;
        //Then switch to another LED
        digitalWrite(ledPins[turn], LOW);
        turn = random(1000) % 5;
        digitalWrite(ledPins[turn], HIGH);
        ledtime = millis();
        lcdPrintScore(gamedur  - (gametime / 1000), score);
      }
    }
  }
  //Show final score and play game end melody
  lcdGameOver(score);	
	curr_mel=end_sound;
	playSound();
	delay(10000);
}
// ================== END of Main Game Logic ================== //
