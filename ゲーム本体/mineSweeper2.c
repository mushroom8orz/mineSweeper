#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#define BeginnerX 9
#define BeginnerY 9
#define BeginnerMine 10
#define IntermediateX 16
#define IntermediateY 16
#define IntermediateMine 40
#define ExpertX 30
#define ExpertY 16
#define ExpertMine 99

WINDOW *upWin, *mineWin;
int restartBotton;
int mine[ExpertX+2][ExpertY+2];
int sweep[ExpertX+2][ExpertY+2];
int wx;
int wy;
int wmine;

int getRandom(int min, int max){
  static int flag;
  
  if (flag == 0) {
    srand((unsigned int)time(NULL));
    flag = 1;
  }
  
  return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

void initScr(int difFlag){
  char tmp[255] = "resize -s ";
  char tmp2[245];

  if(difFlag == 0){
    wx = BeginnerX;
    wy = BeginnerY;
    wmine = BeginnerMine;
  }else if(difFlag == 1){
    wx = IntermediateX;
    wy = IntermediateY;
    wmine = IntermediateMine;
  }else if(difFlag == 2){
    wx = ExpertX;
    wy = ExpertY;
    wmine = ExpertMine;
  }

  sprintf(tmp2, "%d %d", wy + 9, wx * 2 + 2);
  strcat(tmp, tmp2);
  system(tmp);

  clear();
  refresh();
}

void init(){
  int i, j;

  setlocale(LC_ALL,""); // 日本語対応
  setenv("TERM", "xterm-256color", 1); // 環境変数 TERM を更新
  
  initScr(1);

  initscr(); // ncurse起動

  /* マウスイベント起動 */
  keypad(stdscr, TRUE);  
  cbreak();
  mousemask(ALL_MOUSE_EVENTS, NULL); // 全てのマウスイベントを検知
  
  noecho(); // 入力文字を出力しない
  curs_set(0); // カーソルを消す
  timeout(0); // 入力待ちなし

  /* カラー設定 */
  start_color();
  use_default_colors();
  init_pair(99,-1, -1); // デフォルト色
  init_pair(1, 4, -1);
  init_pair(2, 2, -1);
  init_pair(3, 1, -1);
  init_pair(4, 17, -1);
  init_pair(5, 52, -1);
  init_pair(6, 6, -1);
  init_pair(7, 0, -1);
  init_pair(8, 8, -1);
  init_pair(9, 3, -1);
  init_pair(10, 0, 1); //　文字黒、背景赤 
  init_pair(11, 3, -1); //　文字黄色
  init_pair(12, -1, 3); //　背景黄色
  init_pair(13, 1, -1); //　🚩
}

void initPrint(int flagnum){
  static char digit[][4] = {"　","１","２","３","４","５","６","７","８","★"};
  int i, j;

  clear();
  refresh();
  
  upWin = subwin(stdscr, 5, wx * 2 + 2, 1, 0);
  mineWin = subwin(stdscr, wy + 2, wx * 2 + 2, 6, 0);

  mvaddstr(0, 0, "Options");
  mvaddstr(0, 8, "Help");
  mvaddstr(0, 13, "Quit");

  wattron(upWin, COLOR_PAIR(10));
  mvwprintw(upWin, 2, 1, "%03d", wmine - flagnum);
  mvwaddstr(upWin, 2, wx * 2 + 2 - 4, "000");
  wattron(upWin, COLOR_PAIR(99));

  mvwaddstr(upWin, 1, (wx * 2 + 2)/2 - 3, "┌────┐");
  mvwaddstr(upWin, 2, (wx * 2 + 2)/2 - 3, "│");
  wattron(upWin, COLOR_PAIR(11));
  mvwaddstr(upWin, 2, (wx * 2 + 2)/2 - 1, "☺");
  wattron(upWin, COLOR_PAIR(99));
  mvwaddstr(upWin, 2, (wx * 2 + 2)/2 + 2, "│");
  mvwaddstr(upWin, 3, (wx * 2 + 2)/2 - 3, "└────┘");
  restartBotton = (wx * 2 + 2)/2 - 3;
  
  for(i = 1; i < wx + 1; i++){
    for(j = 1; j < wy + 1; j++){
      if(sweep[i][j] == 0){
        mvwaddstr(mineWin, j, i * 2 -1, "□");
      }else if(sweep[i][j] == 1){
        wattron(mineWin, COLOR_PAIR(mine[i][j]));
        mvwprintw(mineWin, j, i*2 - 1, "%s", digit[mine[i][j]]);
        wattron(mineWin, COLOR_PAIR(99));
      }else if(sweep[i][j] == 2){
        wattron(mineWin, COLOR_PAIR(13));
        mvwaddstr(mineWin, j, i*2 - 1, "🚩");
        wattron(mineWin, COLOR_PAIR(99));
      }
    }
  }

  box(upWin, 0,0);
  box(mineWin, 0,0);
}

void initMine(){
  int i, j, n;
  int x, y;

  for(i = 1; i < wx + 2; i++){
    for(j = 1; j < wy + 2; j++){
      mine[i][j] = 0;
      sweep[i][j] = 0;
    }
  }
}

void reMine(int fx, int fy){
  int i, j, n = 0;
  int x, y;

  while(n < wmine){
    x = getRandom(1, wx);
    y = getRandom(1, wy);
    if(mine[x][y] != 9 && (x != fx || y != fy)){
      mine[x][y] = 9;
      n++;
    }
  }
  for(i = 1; i < wx + 1; i++){
    for(j = 1; j < wy + 1; j++){
      if(mine[i][j] != 9){
        if (mine[i-1][j-1] == 9) mine[i][j]++;
        if (mine[i-1][j] == 9) mine[i][j]++;
        if (mine[i-1][j+1] == 9) mine[i][j]++;
        if (mine[i][j-1] == 9) mine[i][j]++;
        if (mine[i][j+1] == 9) mine[i][j]++;
        if (mine[i+1][j-1] == 9) mine[i][j]++;
        if (mine[i+1][j] == 9) mine[i][j]++;
        if (mine[i+1][j+1] == 9) mine[i][j]++;
      }
    }
  }
}

void sweeper(int x, int y){
  static char digit[][4] = {"　","１","２","３","４","５","６","７","８","★"};
  if(x < 1 || x > wx || y < 1 || y > wy){
    return;
  }else if(sweep[x][y]){
    return;
  }
  
  sweep[x][y] = 1;
  wattron(mineWin, COLOR_PAIR(mine[x][y]));
  mvwprintw(mineWin, y, x*2 - 1, "%s", digit[mine[x][y]]);
  wattron(mineWin, COLOR_PAIR(99));

  if(mine[x][y] == 0){
    sweeper(x-1, y-1);
    sweeper(x-1, y);
    sweeper(x-1, y+1);
    sweeper(x, y-1);
    sweeper(x, y+1);
    sweeper(x+1, y-1);
    sweeper(x+1, y);
    sweeper(x+1, y+1);
  }
}

void gameOver(){
  int i, j;
  for(i = 1; i < wx + 1; i++){
    for(j = 1; j < wy + 1; j++){
      if(sweep[i][j] == 2){
        if(mine[i][j] != 9){
          wattron(mineWin, COLOR_PAIR(13));
          mvwaddstr(mineWin, j, i*2 - 1, "✕");
          wattron(mineWin, COLOR_PAIR(99));
        }
      }else if(mine[i][j] == 9){
        sweeper(i, j);
      }
    }
  }
}

int checkClear(){
  int i, j;

  for(i = 1; i < wx + 1; i++){
    for(j = 1; j < wy + 1; j++){
      if(mine[i][j] != 9 && sweep[i][j] == 0){
        return 0;
      }
    }
  }
  return 1;
}

void flagUper(int x, int y, int *flagnum){
  if(x < 1 || x > wx || y < 1 || y > wy){
    return;
  }
  if(sweep[x][y] != 1){
    if(sweep[x][y] != 2){
      sweep[x][y] = 2;
      wattron(mineWin, COLOR_PAIR(13));
      mvwaddstr(mineWin, y, x*2 - 1, "🚩");
      wattron(mineWin, COLOR_PAIR(99));

      (*flagnum)++;
    }else{
      sweep[x][y] = 0;
      mvwaddstr(mineWin, y, x*2 - 1, "□");
      (*flagnum)--;
    }
  }
  wattron(upWin, COLOR_PAIR(10));
  mvwprintw(upWin, 2, 1, "%03d", wmine - *flagnum);
  wattron(upWin, COLOR_PAIR(99));
}

void printOption(WINDOW *optionWin, int difFlag){
  wclear(optionWin);

  wattron(stdscr, COLOR_PAIR(12));
  mvaddstr(0, 0, "Options");
  wattron(stdscr, COLOR_PAIR(99));

  mvwaddstr(optionWin, 1, 1, "new Game");
  mvwhline(optionWin, 2, 1, ACS_HLINE, 15);
  if(difFlag == 0){
    mvwaddstr(optionWin, 3, 1, "✔");
  }else if(difFlag == 1){
    mvwaddstr(optionWin, 4, 1, "✔");
  }else if(difFlag == 2){
    mvwaddstr(optionWin, 5, 1, "✔");
  }
  mvwaddstr(optionWin, 3, 3, "Beginner");
  mvwaddstr(optionWin, 4, 3, "Intermediate");
  mvwaddstr(optionWin, 5, 3, "Expert");
  mvwhline(optionWin, 6, 1, ACS_HLINE, 15);
  mvwaddstr(optionWin, 7, 1, "Best time");
  mvwhline(optionWin, 8, 1, ACS_HLINE, 15);
  mvwaddstr(optionWin, 9, 1, "Exit");

  box(optionWin, 0,0);
  wrefresh(optionWin);
}

void printRunking(WINDOW *optionWin, int difFlag){
  FILE *fp;
  int  mytime[3], cnt = 0, x, y;
  char dif[13], num[3][4], name[3][4];
  MEVENT event;

  if(difFlag == 0){
    strcpy(dif, "Beginner");
  }else if(difFlag == 1){
    strcpy(dif, "Intermediate");
  }else if(difFlag == 2){
    strcpy(dif, "Expert");
  }

  getmaxyx(optionWin, y, x);
  wclear(optionWin);

  if ((fp = fopen(dif, "r")) == NULL) {
    wclear(optionWin);
    mvwaddstr(optionWin, y/2 - 1, x/2 - 7, "file not found");
  }else{
    while(fscanf(fp,"%s %s %d", num[cnt], name[cnt], &mytime[cnt]) != EOF) cnt++;

    mvwprintw(optionWin, 1, x/2 - (strlen(dif) + 2)/2, "%s's", dif);
    mvwaddstr(optionWin, 2, x/2 - 4, "Best Time");
    mvwprintw(optionWin, 4, x/2 - 5, "%s %s %03d", num[0], name[0], mytime[0]);
    mvwprintw(optionWin, 5, x/2 - 5, "%s %s %03d", num[1], name[1], mytime[1]);
    mvwprintw(optionWin, 6, x/2 - 5, "%s %s %03d", num[2], name[2], mytime[2]);
  }
  mvwhline(optionWin, 8, 1, ACS_HLINE, 15);
  mvwaddstr(optionWin, 9, 1, "Exit");
  box(optionWin, 0,0);
  wrefresh(optionWin);

  while(1){
    if(getch() == KEY_MOUSE){
      getmouse(&event);
      if((event.x >= 1 && event.x <= 4 && event.y == 10) || event.x >= 17 || event.y <= 0 || event.y >= 12){
        printOption(optionWin, difFlag);
        break;
      }
    }
  }
}

int option(int *difFlag){
  WINDOW *optionWin;
  MEVENT event;

  optionWin = subwin(stdscr, 11, 17, 1, 0);
  printOption(optionWin, *difFlag);

  while(1){
    if(getch() == KEY_MOUSE){
      getmouse(&event);
      if(event.x >= 1 && event.x <= 8 && event.y == 2){
        wclear(optionWin);
  	    delwin(optionWin);
        return 1;
      }else if(event.x >= 3 && event.x <= 11 && event.y == 4){
        wclear(optionWin);
  	    delwin(optionWin);
        *difFlag = 0;
        initScr(*difFlag);
        return 1;
      }else if(event.x >= 3 && event.x <= 14 && event.y == 5){
        wclear(optionWin);
  	    delwin(optionWin);
        *difFlag = 1;
        initScr(*difFlag);
        return 1;
      }else if(event.x >= 3 && event.x <= 8 && event.y == 6){
        wclear(optionWin);
  	    delwin(optionWin);
        *difFlag = 2;
        initScr(*difFlag);
        return 1;
      }else if(event.x >= 1 && event.x <= 9 && event.y == 8){
  	    printRunking(optionWin, *difFlag);
      }else if((event.x >= 1 && event.x <= 4 && event.y == 10) || event.x >= 17 || event.y <= 0 || event.y >= 12){
        wclear(optionWin);
  	    delwin(optionWin);
        return 0;
      }
    }
  }
}

void printBestTime(char num[], char name[], int my){
  int ch, x, y, n = 0;
  char tmp[4];

  strcpy(name, "aaa");

  mvprintw(my - 1, 7, "%s time :%s", num, name);
  wmove(stdscr,my - 1, 17);
  getyx(stdscr, y, x);
  curs_set(1);

  while((ch = getch()) != '\n'){
    if(ch == KEY_UP){
      if(name[n] == 'z'){
        name[n] = 'A';
      }else if(name[n] == 'Z'){
        name[n] = 'a';
      }else{
        name[n]++;
      }
      mvaddch(y, x + n, name[n]);
    }else if(ch == KEY_DOWN){
      if(name[n] == 'a'){
        name[n] = 'Z';
      }else if(name[n] == 'A'){
        name[n] = 'z';
      }else{
        name[n]--;
      }
      mvaddch(y, x + n, name[n]);
    }else if(ch == KEY_LEFT && n != 0){
      n--;
    }else if(ch == KEY_RIGHT && n != 2){
      n++;
    }
    move(y, x + n);
  }
  curs_set(0);
}

void checkBestTime(int bestTime, int difFlag, int my){
  FILE *fp;
  int  cnt = 0, i = 0, mytime[3];
  char dif[13], num[3][4], name[3][4], bestName[4];

  if(difFlag == 0){
    strcpy(dif, "Beginner");
  }else if(difFlag == 1){
    strcpy(dif, "Intermediate");
  }else if(difFlag == 2){
    strcpy(dif, "Expert");
  }

  if ((fp = fopen(dif, "r")) == NULL) {
    return;
  }

  while(fscanf(fp,"%s %s %d", num[i], name[i], &mytime[i]) != EOF) {
    if(mytime[i] < bestTime){
      cnt++;
    }
    i++;
  }
  if(cnt > 2){
    return;
  }

  printBestTime(num[cnt], bestName, my);

  fclose(fp);
  fp = fopen(dif, "w");
  i = 0;
  while(cnt > 0){
    fprintf(fp, "%s %s %03d\n", num[i], name[i], mytime[i]);
    cnt--;
    i++;
  }
  fprintf(fp, "%s %s %03d\n", num[i], bestName, bestTime);
  while(i+1 < 3){
    fprintf(fp, "%s %s %03d\n", num[i+1], name[i], mytime[i]);
    i++;
  }

  fclose(fp);
}

void help(int my){
  FILE *fp;
  if ((fp = fopen("MineSweeperInstructions.html", "r")) == NULL) {
    mvaddstr(my-1, 0, "file not found");
    return;
  }
  fclose(fp);
  system("firefox MineSweeperInstructions.html");
}

void run(){
  int difficultyFlag = 1, gameOverFlag = 0, firstFlag = 1;
  int mx, my, sx, sy;
  int flagnum = 0;
  time_t start, end;
  MEVENT event;
  
  getmaxyx(stdscr, my, mx);
  initPrint(0);

  while(1){
    if(getch() == KEY_MOUSE){
      getmouse(&event);
      if(event.x >= restartBotton && event.x <= restartBotton + 5 &&event.y >= 2 && event.y <= 4){
	      gameOverFlag = 0;
        firstFlag = 1;
	      flagnum = 0;
        initMine();
 	      initPrint(0);
      }else if(event.x >= 1 && event.x <= mx - 1 && event.y >= 7 && event.y <= my - 3 && gameOverFlag == 0){
        sx = (event.x + 1)/2;
        sy = event.y - 6;
        if(event.bstate == BUTTON1_RELEASED || event.bstate == BUTTON1_CLICKED ){
          if(firstFlag){
            reMine(sx, sy);
            start = time(NULL);
            firstFlag = 0;
          }
          sweeper(sx, sy);
          if(mine[sx][sy] == 9 && sweep[sx][sy] != 2){
            gameOver();
            mvaddstr(my - 1, 0, "Game Over!    ");
            gameOverFlag = 1;
          }else if(checkClear()){
            mvaddstr(my - 1, 0, "Clear!        ");
            gameOverFlag = 1;
            wrefresh(mineWin);
            checkBestTime(((int)(end - start)), difficultyFlag, my);
          }
        }
        if(event.bstate == BUTTON3_RELEASED || event.bstate == BUTTON3_CLICKED ){
          flagUper(sx, sy, &flagnum);
        }
      }else if(event.x >= 0 && event.x <= 6 && event.y == 0){
        if(option(&difficultyFlag) || firstFlag){
          gameOverFlag = 0;
          firstFlag = 1;
	        flagnum = 0;
          initMine();
          getmaxyx(stdscr, my, mx);
        }
        initPrint(flagnum);
      }else if(event.x >= 8 && event.x <= 11 && event.y == 0){
        help(my);
      }else if(event.x >= 13 && event.x <= 16 && event.y == 0){
	      break;
      }
    }
  if(firstFlag == 0 && gameOverFlag == 0 && (end - start) < 1000){
    end = time(NULL);
    wattron(upWin, COLOR_PAIR(10));
    mvwprintw(upWin, 2, wx * 2 + 2 - 4, "%03d", (int)(end - start));
    wattron(upWin, COLOR_PAIR(99));
  }
  wrefresh(mineWin);
  wrefresh(upWin);
  }
}

int main(int argc, char **argv) {
  init();
  
  run();
  
  endwin();

  return 0;
}
