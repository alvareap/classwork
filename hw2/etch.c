#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>
#include<signal.h>
#include<unistd.h>


#define BOARD_HEIGHT	50
#define BOARD_WIDTH	50


int etchBoard[BOARD_HEIGHT][BOARD_WIDTH] = { {0} };

struct curCoor {
  int X;
  int Y;
};

char getcha(void);


void signal_handler(int signal){

	if (signal == SIGINT){
	endwin();
	}

}



void main( void ){
  int inKey;
  struct curCoor map;
  map.X = BOARD_WIDTH/2;
  map.Y = BOARD_HEIGHT/2;
  

  initscr();

// Arrow key scan codes are as follows:
// 1b 5b XX 
// ^  3rd byte = 0x41
// v  3rd byte = 0x42
// -> 3rd byte = 0x43
// <- 3rd byte = 0x44

  while(1){    
    inKey = getcha();
    if ( inKey == 0x1b) { // we have a special key
      inKey = getcha();
      if (inKey == 0x5b){ // it can be an arrow key
	inKey = getcha();
      }
    } else { // handle normal key
	//printf("%x", inKey);	
	if (inKey == 'q'){
	  erase();
	}

    }
    
   // Up-Arrow handling
    if ( inKey == 0x41 ){
      if (map.Y > 0 ){
        map.Y -= 1;
      } else {
        continue;
      }
    }

   // Down-Arrow handling
    if ( inKey == 0x42 ){
      if (map.Y < (BOARD_HEIGHT-1) ){
        map.Y += 1;
      } else {
        continue;
      }
    }

   // Right-Arrow handling
    if ( inKey == 0x43 ){
      if (map.X < (BOARD_WIDTH-1) ){
        map.X += 1;
      } else {
        continue;
      }
    }

   // Left-Arrow handling
    if ( inKey == 0x44 ){
      if (map.X > 0 ){
        map.X -= 1;
      } else {
        continue;
      }
    }
  etchBoard[map.Y][map.X] = 1;

  mvaddch(map.Y, map.X, '1');

//  printf("%d, %d\n", map.X, map.Y);
  
  refresh();

  }



}



char getcha() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}




