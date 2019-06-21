//##################################################//
//-------------------Snake Game---------------------//
//##################################################//
//
//       Author: Iestyn Gage
//    File Name: Snake4
// File Purpose: To play classic game snake
//  Version num: 4
//  File Created: 05/12/2017
//   Last Edited: 07/12/2017
//
//==================================================//
//                  1.Code organisation             //
//==================================================//

//        1.Code Organisation
//        2.Pre-Processor
//            2.a|Included Libraries
//            2.b|Define Constants
//            2.c|Debug Tools
//        3.Declaration of strut
//        4.Declaration of variables
//            4.a| Bool Variables
//            4.b| Integer Variables
//            4.c| unsigned long variables
//        5.Declaration of lists
//        6.Declaration of functions
//            6.a| State Functions
//            6.b| Food Functions
//            6.c| Snake Functions
//            6.d| Wall Functions
//            6.e| High Score Functions
//        7.Setup
//        8.Loop
//        9.Notes
//        10.References

//==================================================//
//                  2.Pre-Processor                 //
//==================================================//

//---------------------------------------------------//
//              Include Libraries                    //
//---------------------------------------------------//

#include <AberLED.h>
//This includes the AberLED functions which allows me to interact with LED screen

#include <EEPROM.h>
//Allows me to save information on EEPROM of ardunio

//---------------------------------------------------//
//              Define Constants                     //
//---------------------------------------------------//

// What each object represents in colour,
#define NOTHING           BLACK
#define FOOD              YELLOW
#define SNAKE             GREEN
#define WALL              RED

// The button inputs as constants instead integer form
#define BUTTONONE         1
#define BUTTONTWO         2
#define BUTTONTHREE       3
#define BUTTONFOUR        4
#define BUTTONFIVE        5

//Constant variable wall food counter, for the number of food needed to eat for new counter to be created
#define WALLFOODCOUNTER   3

//the starting speed of the snake
#define SNAKESPEED        300

//this stores EEPROM location for values
#define EEPROM_CHECKER    40
#define EEPROM_HIGHSCORE  41

#define CHECKVALUE        111
//---------------------------------------------------//
//              Debug Tools                          //
//---------------------------------------------------//

//#define DEBUG_STATE
//Prints information for state functions

//#define DEBUG_FOOD
//prints information for food functions

//#define DEBUG_SNAKE
//prints information for snake functions

//#define DEBUG_WALL
//prints information for wall functions

#define DEBUG_HIGHSCORE
//prints information for highScore functions

//#define DEBUG_NODEATH
//stops player from dying if defined

//=================================================//
//             3.Declaration of struct             //
//=================================================//

struct tail {
  int xCord;
  int yCord;
};
//this allows me to create a 3D array for location of the tail

struct wall {
  int xCord;
  int yCord;
};
//this is used to create 3D array for wall location

struct food {
  int xCord;
  int yCord;
};
//used to create 3D array for food, whistle they is only 1 food it gives greater
//coherent consistence withe the other object

//===================================================//
//             4.Declarationg of variables            //
//===================================================//

//---------------------------------------------------//
//              bool variables                       //
//---------------------------------------------------//

bool hitCorner;
//this bool is used to see if snake has hit corner
//false = no corner hit, true = corner hit

bool snakeDeath;
//this is used to tell if snake has collided with item that will result in it's death
// false = alive, true = dead

//---------------------------------------------------//
//              integer variables                    //
//---------------------------------------------------//

int NESW;
//NESW is the direction which the snake moves

int speedPoints;
//this is taken away from SNAKESPEED to get final speed

int tailLength;
//this is length of tail

int FoodPoint;
//This is amount of food that has been eaten

int wallCounter;
//this is amount of times food been eatern intill it reachs the wall counter, resets after it has been reached

int amountWall;
// this is the amount of wall that are in the game

//--------------------------------------------------//
//                unsigned long                     //
//--------------------------------------------------//

unsigned long lastMoveTime;

//unsigned long elaspedTime
// 'local' variable in main loop function to ensure everything is in sync

//==================================================//
//              5.list variables                    // 
//==================================================//

tail tailList[18];
//this creates 3D array used for tail location
// 18 otherwise they a wierd glitch due to taillength + 1 thing

wall wallList[8];
//used to create 3D array for walls location

food snakeFood[1];
//creates '3D' array of snake done, this is done for consistentie 

//====================================================//
//           6.Declaration of functions               //
//====================================================//

//--------------------------------------------------//
//          State Functions                         //
//--------------------------------------------------//

//.................................................//
//        setGame function                         //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void setGame(){
  AberLED.clear();
  AberLED.swap();
  lastMoveTime=millis();
  speedPoints = 0;
  tailLength  = 1;
  FoodPoint   = 0;
  wallCounter = 0;
  amountWall  = 0;
  snakeDeath = false;
  //credit this to ardunio thing
  tailList[0].xCord = random(1,7);  // set between 1-6 due to specification:
  tailList[0].yCord = random(1,7);  // set between 1-6 due to specification:
  NESW = random(1,5);
  
  #ifdef DEBUG_STATE
  Serial.print("Starting NESW");
  Serial.println(NESW);
  #endif
  
  createFood();
  createWall(amountWall);
}

//This function sets the relvent variables to they approriate starting values.
//I created this function as it would make going into a new game much easier as I don't have to reset...
//...each variable manually each time I want set a new game

//.................................................//
//        goToState function                       //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void goToState(int state){
  switch(state){
    case 1:
      // set new game state

        #ifdef DEBUG_STATE
      Serial.println("Snake died");
      #endif
      
      setGame();
      break;
      
    case 2:
    // death state
    
      #ifdef DEBUG_STATE
      Serial.println("Snake died");
      #endif
    
      deathState();
      break;
  }
}

// This function is used to go into different states

//.................................................//
//        renderGame function                      //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void render(){
    renderSnake(); // this is first so that all other objects are created ontop of the snake, even though it shouldn't, it would allow me to see it
    renderWall();
    renderFood();
    AberLED.swap();      
    AberLED.clear();
}

//This allows the main loop to only have one render function,...
//.. this makes main loop code look neater

//.................................................//
//        mainGame function                        //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void mainGame(){ 
    
    moveHead(NESW);
    moveTail();
    snakeHitWall();
    foodCollision();
    snakeCollision(tailList[0].xCord,tailList[0].yCord);
    
    #ifdef DEBUG_NODEATH
    snakeDeath = false;
    #endif
    
    if (snakeDeath == true){
      checkHighScore(FoodPoint);
      goToState(2);
    }
}

//This allows the main loop to only have one 'Processing' function,...
//.. this makes main loop code look neater

//.................................................//
//        deathState function                      //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void deathState(){
  for(int deathStateCounter = 0;deathStateCounter < 3; deathStateCounter++){
    skullImage(0);
    delay(1000);
    skullImage(1);
    delay(1000);
  }
  delay(1000);
  renderHighScore();
  delay(2500);
  AberLED.clear();
  AberLED.swap();
  delay(100);
  goToState(1);
}

//This is the death state, it displays skull on screen moving up and down for few seconds
//it uses a delay instead of timer as they no need to process anything else such has player input

//.................................................//
//        skullImage function                      //
//'''''''''''''''''''''''''''''''''''''''''''''''''//
void skullImage(int x){
  AberLED.clear();
    for(int i = 0;i<8;i++){
      for(int j = (0+x);j<(7+x);j++){
        AberLED.set(i,j,RED);
      }
    }
    
    AberLED.set(0,0+x,BLACK);
    AberLED.set(1,0+x,BLACK);
    AberLED.set(0,1+x,BLACK);
    
    AberLED.set(7,0+x,BLACK);
    AberLED.set(6,0+x,BLACK);
    AberLED.set(7,1+x,BLACK);

    AberLED.set(0,6+x,BLACK);
    AberLED.set(7,6+x,BLACK);

    AberLED.set(2,6+x,BLACK);
    AberLED.set(5,6+x,BLACK);

    AberLED.set(5,3+x,BLACK);
    AberLED.set(6,3+x,BLACK);

    AberLED.set(1,3+x,BLACK);
    AberLED.set(2,3+x,BLACK);
    AberLED.swap();

}

// This function creates the skull image on the player screen

//-------------------------------------------------//
//            Food Functions                       //
//-------------------------------------------------// 

//.................................................//
//      createFood function                        //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void createFood(){
  snakeFood[0].xCord = random(0,8);
  snakeFood[0].yCord = random(0,8); 
  FoodOnSnake();
  FoodOnWall();
}

//This function creates food, it firsts creates food and then checks if it's a good location
//

//..................................................//
//      FoodOnSnake function                       //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void FoodOnSnake(){
  for(int SnakeLength=tailLength+1;SnakeLength>=0 ;SnakeLength--){
  //tailLength+1 due to snake Movemement,due to way snake moves is remebering head position since tailLength+1 so that snake only needs to remeber certain amount
      if (snakeFood[0].xCord == tailList[SnakeLength].xCord && snakeFood[0].yCord == tailList[SnakeLength].yCord){
        #ifdef DEBUG_FOOD
        Serial.println("FoodOnSnake: Food spawning on Snake collision");
        #endif
        createFood();
      }
  }
}

// This checks if food is on snake, does this my having loop to check if
// snake tile is equal to food tile

//.................................................//
//      FoodOnWall function                        //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void FoodOnWall(){
  for(int FoodOnWallCounter = amountWall;FoodOnWallCounter>=0 ;FoodOnWallCounter--){
    if (snakeFood[0].xCord == wallList[FoodOnWallCounter].xCord && snakeFood[0].yCord == wallList[FoodOnWallCounter].yCord){
      createFood();
      
      #ifdef DEBUG_FOOD
      Serial.println("FoodOnWall: Food spawned on wall");
      #endif
    }
  }
}

//This checks if food been created on wall, it does this by running loop to check
//if food tile location is equal to wall tile location

//.................................................//
//      foodCollision function                     //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void foodCollision(){
  if( tailList[0].xCord == snakeFood[0].xCord && tailList[0].yCord == snakeFood[0].yCord){
    
    #ifdef DEBUG_FOOD
    Serial.println("Food Collision");
    #endif
          
    FoodPoint++;
    createFood();
    wallCounter++;
    Serial.println(wallCounter);
    if (tailLength < 16){
      tailLength++;

      #ifdef DEBUG_FOOD
      Serial.print("New Length: ");
      Serial.println(tailLength);
      #endif
             
    } 
    if (speedPoints < 150){
      speedPoints+=3; 
      // whistle it's highly likley that speedpoints will never get to 150, it's precation to put it in place

      #ifdef DEBUG_SNAKE
      Serial.print("Snake Speed: ");
      Serial.println(SNAKESPEED-speedPoints);
      #endif
    }
    if ((wallCounter/WALLFOODCOUNTER>=1) && amountWall <7){
      
      #ifdef DEBUG_FOOD
      Serial.print("Create New Wall ");
      Serial.println(amountWall);
      Serial.print(FoodPoint);
      #endif
      
      wallCounter = 0;      
      amountWall++;
      createWall(amountWall);      
    }
  }
}

//this checks if they has been a food collision, if they has then another food is created, speed inceased
// and if needed it recalls createwall function if enough food been eaten

//.................................................//
//        renderFood Function                      //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void renderFood(){
  AberLED.set(snakeFood[0].yCord,snakeFood[0].xCord,FOOD);
}

// this renders the food

//-------------------------------------------------//
//          Snake Functions                        //
//-------------------------------------------------//

//.................................................//
//        GameInput Function                         //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

int gameInput(){
  //Serial.print("SetNESW Function: ");
  if (AberLED.getButtonDown(BUTTONONE) && NESW != BUTTONTWO){
    return NESW = BUTTONONE;    
    
    #ifdef DEBUG_SNAKE
    Serial.println("Button 1");
    #endif
  }  
  else if (AberLED.getButtonDown(BUTTONTWO) && NESW != BUTTONONE){
    return NESW = BUTTONTWO;

    #ifdef DEBUG_SNAKE
    Serial.println("Button 2");
    #endif
  }
  else if (AberLED.getButtonDown(BUTTONTHREE) && NESW != BUTTONFOUR){
    return NESW = BUTTONTHREE;

    #ifdef DEBUG_SNAKE
    Serial.println("Button 3");
    #endif
  }
  else if (AberLED.getButtonDown(BUTTONFOUR) && NESW != BUTTONTHREE){
    return NESW = BUTTONFOUR;

    #ifdef DEBUG_SNAKE
    Serial.println("Button 4");
    #endif
  }
  else if (AberLED.getButtonDown(BUTTONFIVE)){
    
    #ifdef DEBUG_SNAKE
    Serial.println("Button 5");
    #endif

    checkHighScore(FoodPoint);
    goToState(1);
  }
}

// This sets NESW to correct tile, it also resets the game if button 5 been pressed
//

//.................................................//
//        boundryHit Function                      //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void boundryHit(int optionOne,int optionTwo){
  int randomNum = random(1,3);


  #ifdef DEBUG_SNAKE 
  Serial.print("boundryHit Function: ");
  Serial.println(randomNum);
  #endif
    
  if (randomNum==1){
    NESW = optionOne;   

    #ifdef DEBUG_SNAKE
    Serial.println("optionOne");
    #endif
    
  } else{
    NESW = optionTwo;

    #ifdef DEBUG_SNAKE
    Serial.println("optionTwo");
    #endif
  }
}

// If boundry is hit this function is used to select if it
// randomly turns left or right

//.................................................//
//        snakeCollision Function                  //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void snakeCollision(int x, int y){
  for(int SnakeLength=tailLength;SnakeLength>1 ;SnakeLength--){
        if (x == tailList[SnakeLength].xCord && y == tailList[SnakeLength].yCord){

           snakeDeath = true;

            #ifdef DEBUG_SNAKE
            Serial.println("SnakeCollision: Detection");
            #endif
        }
    }
}

//this uses loop to make comparison if x and y any part of the snake
// if they has been result that is equal then snake dies

//.................................................//
//        atCorner Function                        //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void atCorner(int xCornCord, int yCornCord,int Outcome){
  if(tailList[0].xCord == xCornCord && tailList[0].yCord == yCornCord){
    NESW = Outcome;
    moveHead(NESW);
    hitCorner = true;
    
    #ifdef DEBUG_SNAKE
    Serial.println("atCorner Function: Corner Hit");
    Serial.print("Corner set NESW as ");
    Serial.println(NESW);
    #endif

  }
}

// this function changes NESW when snake hits a corner

//.................................................//
//        atXRow Function                          //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

bool atXRow(int xRow, int boundryHitOne, int boundryHitTwo){
   if ( tailList[0].xCord == xRow && (tailList[0].yCord != 0 && tailList[0].yCord != 7) ){

        #ifdef DEBUG_SNAKE
        Serial.println("atRow Function: Wall Hit");
        #endif
        
        boundryHit(boundryHitOne,boundryHitTwo);
        moveHead(NESW);   
        
        return true;
      } else {
        return false;
      }
}

//This function checks if xROW has been hit and returns true if it has
// so that it can break the code (so it doesn't check if head at a corner

//.................................................//
//        atYRow Function                          //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

bool atYRow(int yRow, int boundryHitOne, int boundryHitTwo){
   if ((tailList[0].xCord != 0 && tailList[0].xCord != 7) && tailList[0].yCord == yRow){

        #ifdef DEBUG_SNAKE
        Serial.println("atRow Function: Wall Hit");
        #endif
        
        boundryHit(boundryHitOne,boundryHitTwo);
        moveHead(NESW);
        
        return true;
      } else {
        return false;
      }

}

//This function checks if Y Row has been hit and returns true if it has
// so that it can break the code (so it doesn't check if head at a corner

//.................................................//
//        moveHead Function                        //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void moveHead(int movingNESW){
  hitCorner = false;
  switch(movingNESW){
    case 1:     
      if (atXRow(0,4,3) == true){
        break;
      }
      atCorner(0,7,4);
      atCorner(0,0,3);
      if (hitCorner == false) {
        tailList[0].xCord--;
        
        #ifdef DEBUG_SNAKE
        Serial.println("Moving");
        #endif

      }
      break;
      
    case 2:     
       if (atXRow(7,3,4) == true){
        break;
       }
       atCorner(7,0,3);
       atCorner(7,7,4);

      if (hitCorner == false) {
        tailList[0].xCord++;

        #ifdef DEBUG_SNAKE
        Serial.println("Moving");
        #endif
      }
      break;
    case 3:        
        if (atYRow(7,1,2) == true){
          break;  
        }
        atCorner(0,7,2);
        atCorner(7,7,1);

      if (hitCorner == false) {
        tailList[0].yCord++;

        #ifdef DEBUG_SNAKE
        Serial.println("Moving");
        #endif
      }
      break;
    case 4: 
        if (atYRow(0,1,2) == true){
          break;      
        }
        atCorner(0,0,2);
        atCorner(7,0,1);
      
      if (hitCorner == false) {
        tailList[0].yCord--;

        #ifdef DEBUG_SNAKE
        Serial.println("Moving");
        #endif
      }
      break;
  }
}

// this function moves head, it does this by checking if it's hit a row or corner and reacts 
// accord to function if it has, but if it hasn't it moves one tile in direction

//.................................................//
//        moveTail Function                        //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void moveTail(){
  for (int SnakeLength = (tailLength+1);SnakeLength>0;SnakeLength--){
    //+1 so that random LED isn't light up when eatern
    tailList[SnakeLength].xCord = tailList[(SnakeLength-1)].xCord;
    tailList[SnakeLength].yCord = tailList[(SnakeLength-1)].yCord;
  }
}

// this uses a loop to move the tail

//.................................................//
//        renderFood Function                      //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void renderSnake(){
  for(int SnakeLength = tailLength;SnakeLength>0;SnakeLength--){
    AberLED.set(tailList[SnakeLength].yCord,tailList[SnakeLength].xCord,SNAKE);
  }
}

// this function  uses aloop to render the snake

//-------------------------------------------------//
//        Wall Functions                           //
//-------------------------------------------------//

//.................................................//
//        createWall Function                      //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void createWall(int obstaclenumber){
  wallList[obstaclenumber].xCord = random(0,8);
  wallList[obstaclenumber].yCord = random(0,8);

  //--------
  // checks if wall been created on snake
  //-------
    for(int SnakeLength=tailLength+1;SnakeLength>=0 ;SnakeLength--){
  //tailLength+1 due to snake Movemement,due to way snake moves is remebering head position since tailLength+1 so that snake only needs to remeber certain amount
      if (tailList[SnakeLength].xCord == wallList[obstaclenumber].xCord && tailList[SnakeLength].yCord == wallList[obstaclenumber].yCord){
        createWall(obstaclenumber);
        
        #ifdef DEBUG_WALL
        Serial.println("createWall Function: WallSpawned on Snake");
        #endif
      }
    }

  //------
  // Checks if walls been created on old walls
  //------
  for(int FoodOnWallCounter = (amountWall-1);FoodOnWallCounter>=0 ;FoodOnWallCounter--){
    if (wallList[obstaclenumber].xCord == wallList[FoodOnWallCounter].xCord && wallList[obstaclenumber].yCord == wallList[FoodOnWallCounter].yCord){
      createWall(obstaclenumber);
      
      #ifdef DEBUG_WALL
      Serial.println("createWall Function: Wall spawned on another wall");
      #endif
    }
  }
  //------
  // Checks if wall been created on food
  //------
    // checks if wall been created on food
    if (snakeFood[0].xCord == wallList[obstaclenumber].xCord && snakeFood[0].yCord == wallList[obstaclenumber].yCord) {
      createWall(obstaclenumber);

      #ifdef DEBUG_WALL
      Serial.println("createWall Function: WallSpawned on Snake");
      #endif
    }

    
  switch(wallList[obstaclenumber].xCord){
    case 0:
      if ( (wallList[obstaclenumber].yCord == 1) || (wallList[obstaclenumber].yCord == 6) ){
         createWall(obstaclenumber);
      }
      break;
      
    case 1:
      if ( (wallList[obstaclenumber].yCord == 0) || (wallList[obstaclenumber].yCord == 7) ){
         createWall(obstaclenumber);      
      }
      break;
      
    case 6:
      if ( (wallList[obstaclenumber].yCord == 1) || (wallList[obstaclenumber].yCord == 6) ){
         createWall(obstaclenumber);
      }
      break;
      
    case 7:
      if ( (wallList[obstaclenumber].yCord == 0) || (wallList[obstaclenumber].yCord == 7) ){
         createWall(obstaclenumber);      
      }
      break;
  }

  #ifdef DEBUG_WALL
  Serial.print("createWall: Created wall ");
  Serial.print(obstaclenumber);
  Serial.print(" at ");
  Serial.print(wallList[obstaclenumber].yCord);
  Serial.print(" ");
  Serial.println(wallList[obstaclenumber].xCord);
  #endif
}

// This function first creates new wall, it then checks if new walls has been created on
// Snake, other pieces of wall,food or locations which may stop the game

//.................................................//
//        snakeHitWall Function                    //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void snakeHitWall(){
  for(int hitWallCounter = amountWall; hitWallCounter>=0; hitWallCounter--){
    if( (tailList[0].xCord == wallList[hitWallCounter].xCord) && (tailList[0].yCord == wallList[hitWallCounter].yCord) ){
      
      #ifdef DEBUG_WALL
      Serial.println("Hit Wall Snake Dead");
      #endif
      
      snakeDeath = true;
    }
  }
}

//this uses a loop to see if head piece is equal to any other locations of walls
// if so it 'kills' the snake

//.................................................//
//        renderWall Function                      //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void renderWall(){
  for(int renderWallCounter = amountWall; renderWallCounter>=0; renderWallCounter--){
      AberLED.set(wallList[renderWallCounter].yCord,wallList[renderWallCounter].xCord,WALL);
  }
}

// this uses loop to redner the wall

//-------------------------------------------------//
//          High Score Functions                   //
//-------------------------------------------------//

//.................................................//
//        EEPROMChecker Function                   //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void EEPROMChecker(){
  int checkValue = EEPROM.read(EEPROM_CHECKER);
  if (checkValue != CHECKVALUE){
    EEPROM.write(EEPROM_CHECKER,CHECKVALUE);
    EEPROM.write(EEPROM_HIGHSCORE,0);

    #ifdef DEBUG_HIGHSCORE
    Serial.println("HighScore reset");
    #endif
    
  }
}

//this checks if Highscore EEPROM location was set by this Ardunio program
//does this by seeing if checker value is correct.

//.................................................//
//        checkHighScore Function                  //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void checkHighScore(int currentGameScore){
  
  int currentHighScore = EEPROM.read(EEPROM_HIGHSCORE);
  
  if ( (currentGameScore > currentHighScore) && currentGameScore > 64){
    // Since the max EEPROM value is 255, it stops possible wierd errors to happen
    EEPROM.update(EEPROM_HIGHSCORE,currentGameScore);
    
    #ifdef DEBUG_HIGHSCORE
    Serial.println("Max score of 64 reached");
    #endif
    
  } else if (currentGameScore > currentHighScore){
 
    EEPROM.update(EEPROM_HIGHSCORE,currentGameScore);
    
    #ifdef DEBUG_HIGHSCORE
    Serial.print("New HighScore Set ");
    Serial.println(currentGameScore);
    #endif
  } 
}

//this function sets new high score, it does this by comparing if the current score is better
//than current high score, if so uses EEPROM functions to update the score
//it also checks if score is too high for EEPROM and sets it at the maximum value

//.................................................//
//        renderHighScore Function                 //
//'''''''''''''''''''''''''''''''''''''''''''''''''//

void renderHighScore(){
  AberLED.clear();
  int renderHighScoreCounter = EEPROM.read(EEPROM_HIGHSCORE);
  
  #ifdef DEBUG_HIGHSCORE
  Serial.println(renderHighScoreCounter);
  #endif
  
  for (int rowNumber = 0; renderHighScoreCounter > 0; rowNumber++){
    for (int tileNumber = 0 ; (renderHighScoreCounter > 0 && tileNumber < 8); tileNumber++){
      AberLED.set(tileNumber,rowNumber,GREEN);
      renderHighScoreCounter--;
      
    }
  }
  AberLED.swap();
  // uses a daily as ardunio shouldn't be expecting a input, so no missed input
}

//this function is used to display the high score for the Ardunio
//

//===============================================//
//                7.Setup                        //
//===============================================//

void setup() {
  Serial.begin(9600);           // used to print messages
  AberLED.begin();              // allows me to set AberLED functions
  EEPROMChecker();             // checks if the highscore was set my this ardunio
  randomSeed(analogRead(0));  

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
  //    Code from Reference
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
  //
  //Code found from ardunio manual:
  //https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
  //
  //Code taken:
  //randomSeed(analogRead(0));
  //
  //What does it do:
  //Creates new seed so new random variables are created on startup
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
  
  goToState(1);               // sets the main game up
}

//===============================================//
//              8.Loop                           //
//===============================================//

void loop() {

  unsigned long elaspedTime = millis()-lastMoveTime;  // The timer in in the main loop to ensure all main functions are done 'in-sync'
  if(elaspedTime > (SNAKESPEED-speedPoints)){
    // the snakeSpeed-speedPoints allows the snake to speed when snake eats more food  
    // as speedPoint increases in the function Foodcollison
    lastMoveTime = millis();
    
    gameInput(); 
    mainGame();
    render();
    } 
}

//================================================//
//            9.Notes                             //
//================================================//

//
// objects have set ABERLED as Y first then X
//
//
//
//
//================================================//
//            10.Reference                         //
//================================================//
//
//Arduino.cc. (2017). Arduino Reference. [online] Available at: https://www.arduino.cc/reference/en/language/functions/random-numbers/random/ [Accessed 28 Nov. 2017].
//
//
