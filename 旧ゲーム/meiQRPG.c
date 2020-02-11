#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <time.h>

  /* 画面の並びは
     leftWin centerWin rightWin
     　　　　  underWin  commandWin */
WINDOW *leftWin, *centerWin, *underWin, *rightWin, *commandWin;

/* 表示する文字の設定 */
typedef struct mystr{
  char s[255];
  int x;
  int y;
  int w;
  WINDOW *win;
} mySTR;

typedef struct player{
  char num[5];
  char name[16];
  int namelen;
  int maxcon;
  int con;
  int pow;
  int str;
  int dex;
  int room;
} PLAYER;

typedef struct room{
  int up;
  int right;
  int left;
  int down;
  int after;
} ROOM;

/* 文字列の設定と表示 */
void setmySTR(mySTR *str, char s[], int y, int x, int w, WINDOW *win){
  strcpy(str->s, s);
  str->y = y;
  str->x = x;
  str->w = w;
  str->win = win;

  mvwaddstr(win, str->y, str->x, str->s);
}

int getRandom(int min, int max){
  static int flag;
  
  if (flag == 0) {
    srand((unsigned int)time(NULL));
    flag = 1;
  }
  
  return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

/* クリックの判定*/
int clickeMouse(MEVENT event, mySTR str){
  int by, bx;
  getbegyx(str.win, by, bx);
  event.x -= bx;
  event.y -= by;
  
  if(event.bstate == BUTTON1_RELEASED || event.bstate == BUTTON1_CLICKED ){
    if(event.x > str.x && event.x < str.x + str.w && event.y == str.y)
      return 1;
  }
  return 0;
}

void printWindow(){
  /* ウィンドウに枠を書く */
  box(leftWin, 0,0);
  box(underWin, 0,0);
  box(rightWin, 0,0);
  box(commandWin, 0,0);

  refresh();
}

/* 初期設定とウィンドウ作成 */
void initWindow(){
  system("resize -s 29 85"); // 画面サイズ変更
  setlocale(LC_ALL,""); // 日本語対応
  setenv("TERM", "xterm-256color", 1); // 環境変数 TERM を更新
  
  initscr(); // ncurse起動

  /* マウスイベント起動 */
  keypad(stdscr, TRUE);  
  cbreak();
  mousemask(ALL_MOUSE_EVENTS, NULL); // 全てのマウスイベントを検知
  
  noecho(); // 入力文字を出力しない
  curs_set(0); // カーソルを消す
  timeout(1); // 入力は1ms秒

  /* カラー設定 */
  start_color();
  use_default_colors();
  init_pair(1, -1, -1);
  init_pair(10, 8, -1); // 番号:10 文字グレー,背景デフォ
  init_pair(11, COLOR_YELLOW, -1); // 番号:11 文字黄色,背景デフォ
  init_pair(12, -1, COLOR_YELLOW); // 番号:12 文字デフォ,背景黄色
  init_pair(13, COLOR_BLUE, -1); // 番号:13 文字青色,背景デフォ
  init_pair(14, COLOR_GREEN, -1); // 番号:14 文字緑色,背景デフォ
  
  /* ウィンドウ作成*/
  leftWin = subwin(stdscr, 29, 20, 0, 0);
  centerWin = subwin(stdscr, 20, 47, 0, 20);
  underWin = subwin(stdscr, 9, 47, 20, 20);
  rightWin = subwin(stdscr, 20, 18, 0, 67);
  commandWin = subwin(stdscr, 9, 18, 20, 67);

  scrollok(underWin, 1);

  printWindow();
}

/* メニュー画面の表示と選択結果 */
void printMenu(mySTR str[]){
  int x,y;

  wclear(centerWin);
  
  getmaxyx(centerWin, y, x);
  y /= 2;
  x /= 2;
  setmySTR(&str[0], "meiQRPG", y - 4 , x - 7/2, 7, centerWin);
  setmySTR(&str[1], "製作者:マッシュ", y - 2, x - 15/2, 15, centerWin);
  setmySTR(&str[2], "画面サイズ変えないでね", y - 1, x - 22/2, 22, centerWin);
  setmySTR(&str[3], "[はじめから]", y + 1, x - 12/2, 12, centerWin);
  setmySTR(&str[4], "[つづきから]", y + 3, x - 12/2, 12, centerWin);
  setmySTR(&str[5], "[コンフィグ]", y + 5, x - 12/2, 12, centerWin);
  setmySTR(&str[6], "[ゲームを閉じる]", y + 7, x - 16/2, 16, centerWin);

  wrefresh(centerWin);
}

int setMenu(){
  MEVENT event;
  mySTR str[7];

  printMenu(str);
  
  while(1){
    if(getch() == KEY_MOUSE){
      getmouse(&event);
      if(clickeMouse(event, str[3])){
	return 1;
      }else if(clickeMouse(event, str[6])){
	return 4;
      }
    }
  }
}

int compare_int(const void *a, const void *b){
    return *(int*)a - *(int*)b;
}

void randomplayer(PLAYER *p){
  int a[3], b[5], i;

  b[0] = 0;
  b[4] = 150;

  for(i = 1; i < 4; i++){
    b[i] = getRandom(25, 75);
  }
  qsort(&b[1], 3, sizeof(int), compare_int);
  
}

PLAYER makePlayer(){
  PLAYER p1;
  
  strcpy(p1.num, "p1");
  strcpy(p1.name, "マッシュ");
  p1.namelen = 8;
  p1.maxcon = 2500;
  p1.con = 2500;
  p1.pow = 3000;
  p1.str = 50;
  p1.dex = 50;
  p1.room = getRandom(1,30);

  return p1;
}

void makeNPC(PLAYER *NPC, int num){
  sprintf(NPC->num, "NPC%d", num);
  strcpy(NPC->name, "");
}

void makemeiQ(ROOM room[]){
  int i, random, pre;
  
  for (i = 1; i <= 30; i++){
    if(i >= 7) room[i].up = 1;
    if(i % 6 != 1) room[i].left = 1;
    if(i % 6 != 0) room[i].right = 1;
    if(i < 25) room[i].down = 1;
  }
  
  pre = 0;
  for (i = 1; i <= 23; i++){
    if(i % 6 != 0){
      if(i < 6) random = getRandom(1,5);
      else random = getRandom(2,5);
      if(pre == 2 && random == 3 || pre == 3 && random == 2){
	i--;
      }else{
	pre = random;
	if(random == 1){ room[i].right--; room[i+1].left--;  }
	if(random == 2){ room[i].down--; room[i+6].up--;     }
	if(random == 3){ room[i+1].down--; room[i+7].up--;   }
	if(random == 4){ room[i+6].right--; room[i+7].left--;}
      }
    }else{
      pre = 0;
    }
  }
}

void printMeiQ(PLAYER p1, ROOM room[], mySTR str[]){
  int k, x = -1, y = -1;
  static char num[][10] = {"","１","２","３","４","５","６","７","８","９","10",
			   "11","12","13","14","15","16","17","18","19","20",
			   "21","22","23","24","25","26","27","28","29","30"};
  
  wclear(centerWin);

  for(k = 1; k <= 30; k++){    
    if(p1.room == k){
      wattron(centerWin, COLOR_PAIR(11));
    }else if(!room[k].after){
      wattron(centerWin, COLOR_PAIR(10));
    }else{
      wattron(centerWin, COLOR_PAIR(1));
    }

    mvwaddstr(centerWin, y+1, x+1, "┌─  ─┐");
    setmySTR(&str[k], num[k], y+2, x+3, 2, centerWin);
    mvwaddstr(centerWin, y+3, x+1, "└─  ─┘");

    if(room[k].after){
      if(room[k].up){
	mvwaddstr(centerWin, y+1, x+3, "││");
	mvwaddstr(centerWin, y, x+3, "││");
      }
      if(room[k].left){
	mvwaddstr(centerWin, y+2, x, "===");
      }
      if(room[k].right){
	mvwaddstr(centerWin, y+2, x+5, "===");
      }
      if(room[k].down){
	mvwaddstr(centerWin, y+3, x+3, "││");
	mvwaddstr(centerWin, y+4, x+3, "││");
      }
    }
    x += 8;
    if(k % 6 == 0){
      y += 4;
      x = -1;
    }
  }
  wattron(centerWin, COLOR_PAIR(1));
  /* for(i = 0; i < 20; i++){ // 20行まで
    for(j = 0; j < 46; j += 2){ // 46列まで
      if(p1.room == k){
	wattron(centerWin, COLOR_PAIR(11));
      }else if(!room[k].after){
	wattron(centerWin, COLOR_PAIR(10));
      }
      if(i % 4 == 0){
	if(j % 8 == 0){
	  mvwaddch(centerWin, i, j, ACS_ULCORNER);
	  waddch(centerWin, ACS_HLINE);
	}else if(j % 8 == 2){
	  if(room[k].up){
	    waddch(centerWin, ACS_VLINE);
	    waddch(centerWin, ACS_VLINE);
	  } else {
	    wmove(centerWin, i, j+2);
	  }
	}else if(j % 8 == 4){
	  waddch(centerWin, ACS_HLINE);
	  waddch(centerWin, ACS_URCORNER);
	  k++;
	}
      }else if(i % 4 == 1){
	if(j % 8 == 0){
	  if(room[k].left){
	    mvwaddstr(centerWin, i ,j-1, "===");
	  } else {
	    wmove(centerWin, i, j+2);
	  }
	}else if(j % 8 == 2){
	  setmySTR(&str[k], num[k], i, j, 2, centerWin);
	}else if(j % 8 == 4){
	  if(room[k].right){
	    waddstr(centerWin, "===");
	  } else {
	    wmove(centerWin, i, j+2);
	  }
	  k++;
	}
      }else if(i % 4 == 2){
	if(j % 8 == 0){
	  mvwaddch(centerWin, i, j, ACS_LLCORNER);
	  waddch(centerWin, ACS_HLINE);
	}else if(j % 8 == 2){
	  if(room[k].down){
	    waddch(centerWin, ACS_VLINE);
	    waddch(centerWin, ACS_VLINE);
	  } else {
	    wmove(centerWin, i, j+2);
	  }
	}else if(j % 8 == 4){
	  waddch(centerWin, ACS_HLINE);
	  waddch(centerWin, ACS_LRCORNER);
	  k++;
	}
      }else{
	if(j % 8 == 2){
	  if(room[k].down){
	    mvwaddch(centerWin, i, j, ACS_VLINE);
	    waddch(centerWin, ACS_VLINE);
	  }
	  k++;
	}
      }
    }
    if(i % 4 != 3){
      k -= 6;
    }
    wattron(centerWin, COLOR_PAIR(1));
  }
  */
  wrefresh(centerWin);
}

void printCommand(mySTR str[], int flag){
  wclear(commandWin);

  setmySTR(&str[0], "[探索]", 2, 2, 6, commandWin);
  setmySTR(&str[1], "[移動]", 2, 10, 6, commandWin);
  setmySTR(&str[2], "[道具]", 4, 2, 6, commandWin);
  if(!flag){
    wattron(commandWin, COLOR_PAIR(10));
  }
  setmySTR(&str[3], "[会話]", 4, 10, 6, commandWin);
  wattron(commandWin, COLOR_PAIR(1));
  setmySTR(&str[4], "[保存]", 6, 2, 6, commandWin);
  setmySTR(&str[5], "[終了]", 6, 10, 6, commandWin);

  box(commandWin, 0,0);
  wrefresh(commandWin);
}

void printLeft(PLAYER *party[], mySTR str[]){
  int x, y, n, i = 2, j, len = 1;
  char tmp[25];
  
  wclear(leftWin);
  getmaxyx(leftWin, y, x);

  if(party[1] != NULL) len = 2;
  if(party[2] != NULL) len = 3;
  
  for(n = 0; n < len; n++){
    mvwaddstr(leftWin, i, 1, party[n]->num);
    setmySTR(&str[0], party[n]->name, i, x-1-party[n]->namelen, party[n]->namelen, leftWin);
    sprintf(tmp, "体力   (%4d/%4d)", party[n]->maxcon, party[n]->con);    
    mvwaddstr(leftWin, i+1, 1, tmp);
    wmove(leftWin,i+2,1);
    wattron(leftWin, COLOR_PAIR(13));
    for(j = 0; j < party[n]->con; j += party[n]->maxcon/16)
      waddstr(leftWin, "■");
    wattron(leftWin, COLOR_PAIR(1));
    sprintf(tmp, "精神力 (3000/%4d)", party[n]->pow);    
    mvwaddstr(leftWin, i+3, 1, tmp);
    wmove(leftWin,i+4,1);
    wattron(leftWin, COLOR_PAIR(14));
    for(j = 0; j < 3000; j += party[n]->pow/16)
      waddstr(leftWin, "■");
    wattron(leftWin, COLOR_PAIR(1));
    mvwprintw(leftWin, i+5, 1, "攻撃 (%2d/%2d) 俊敏",party[n]->str, party[n]->dex);
    mvwaddstr(leftWin, i+6, 1, "------------------");
    i += 6;
  }
  box(leftWin, 0,0);
  wrefresh(leftWin);
}

void printUnder(PLAYER p1){
  int y ,x, i;

  getmaxyx(underWin, y, x);
  for(i = 0; i < x; i++) waddch(underWin, '-');
  wprintw(underWin, "現在 %d の部屋にいます\n", p1.room);
  wprintw(underWin, "どうする？\n", p1.room);

  wrefresh(underWin);
}

int setMeiQ(PLAYER p1, PLAYER *party[], ROOM room[]){
  mySTR meiQstr[31];
  mySTR comstr[6];
  mySTR leftstr[4];
  MEVENT event;
  int partyFlag = 0;

  if(party[1] != NULL){
    partyFlag = 1;
  }

  room[p1.room].after = 1;
  printCommand(comstr, partyFlag);
  printLeft(party, leftstr);
  printUnder(p1);
  printMeiQ(p1, room, meiQstr);
  
  while(1){
    if(getch() == KEY_MOUSE){
      getmouse(&event);
      if(clickeMouse(event, comstr[1])){
	return 1;
      }else if(clickeMouse(event, comstr[5])){
	return 99;
      }
    }
  }
}

void printMove(PLAYER *p1, ROOM room[], mySTR str[], int flag[]){
  int y, x, tx, ty, i, j;
  static char arrow[][6] ={"[上]","[左]","[右]","[下]"};
  
  getmaxyx(commandWin, y, x);
  wclear(commandWin);
  
  for(i = 0; i < 4; i++){
    if(i == 0) {j = room[p1->room].up; tx = x/2; ty = y/4;}
    if(i == 1) {j = room[p1->room].left; tx = x/4; ty = y/2;}
    if(i == 2) {j = room[p1->room].right; tx = x * 3/4+1; ty = y/2;}
    if(i == 3) {j = room[p1->room].down; tx = x/2; ty = y * 3/4;}
    if(!j){
      wattron(commandWin, COLOR_PAIR(10));
      flag[i] = 0;
    }
    setmySTR(&str[i], arrow[i], ty, tx - 2, 4, commandWin);
    wattron(commandWin, COLOR_PAIR(1));
  }
  setmySTR(&str[4], "[もどる]", y-2, x-1-8, 8, commandWin);
  
  box(commandWin, 0,0);
  wrefresh(commandWin);
}

void setMove(PLAYER *p1, ROOM room[]){
  mySTR movestr[5];
  int flag[4] = {1,1,1,1};
  MEVENT event;

  printMove(p1, room, movestr, flag);

  while(1){
    if(getch() == KEY_MOUSE){
      getmouse(&event);
      if(clickeMouse(event, movestr[0]) && flag[0]){
	p1->room -= 6;
	break;
      }else if(clickeMouse(event, movestr[1]) && flag[1]){
	p1->room -= 1;
	break;
      }else if(clickeMouse(event, movestr[2]) && flag[2]){
	p1->room += 1;
	break;
      }else if(clickeMouse(event, movestr[3]) && flag[3]){
	p1->room += 6;
	break;
      }else if(clickeMouse(event, movestr[4])){
	break;
      }
    }
  }
}

void runGame(PLAYER p1, PLAYER *party[], ROOM room[]){
  int flag;
  
  while(1){
    flag = setMeiQ(p1, party, room);
    if(flag == 1){
      setMove(&p1, room);
    }else if(flag == 99){
      clear();
      printWindow();
      break;
    }
  }
}

void newGame(){
  ROOM room[31] = {0,0,0,0,0};
  PLAYER p1, NPC[5];
  PLAYER *party[3];
  int i;
  
  p1 = makePlayer();
  party[0] = &p1;
  party[1] = NULL;
  party[2] = NULL;
  for(i = 0; i < 5; i++){
    makeNPC(&NPC[i], i);
  makemeiQ(room);
  wclear(underWin);
  runGame(p1, party, room);
  }
}

int main(int argc, char **argv) {
  int flag;

  initWindow();
 
  while(1){
    flag = setMenu();
    if(flag == 1){
      newGame();
    }else if(flag == 4){
      break;
    }
  }
  
  endwin();

  return 0;
}

// 覚書：strlen(msg)/3で文字の中央
