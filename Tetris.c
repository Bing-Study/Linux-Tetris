#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/time.h>
#include<signal.h>
#include<termios.h>

#define Row 20
#define Col 14

#define X 5
#define Y 10

//
#define Base  7		//7个图形 
#define  Change 4	//每个图形四种变换 
#define N 4	//存储图形数组的大小 

//all color
enum {BLACK,RED,GREEN,YELLOW,BLUE,PURPLE,DARKGREEN};

//all active state
enum {DOWN,UP,RIGHT,LEFT};

struct blocks{
	int space[N][N];
}blocks[Base][Change];

int Tetris[Row][Col];	
int els_color[Row][Col];

//图形在棋盘的位置 
int x = 0,y = 6; 
//当前图形的
int cur_base;
//当前图形的变换
int cur_change;

int next_base;
int next_change;
int next_color;

int score=0;

//color
int cur_color;

int state=1;


struct itimerval itv;

void readme();
void next();
void setAlarm();
void cancelAlarm();
void showBg();		//初始化背景 
void blocksInit();	//初始化图形 
void debugBlock();	//测试图形 
void writeInBlock();//将图形显示在棋盘里
void printBlock();  //打印棋盘
void selectColor(int color);//随机生成方块颜色
void execCmd(int cmd);//控制方块移动方向
void cancelState(int state);//删除方块上一个状态
int judgeMove(int cmd);	
void lastInBlock(); 
void printNext();
void clearNext();
int judgeChange();
int judgeElimate();
void elimateT(int row);
void printScore();
int endGame();
void pauseGame();


void alarmHandler(int s){
	if(judgeMove(DOWN)){
		execCmd(DOWN);
	}
	else{
		lastInBlock();
		cancelAlarm();
		next();
	}

}

int main(void)
{
	struct termios old, new;
	char c;
	signal(SIGALRM,alarmHandler);
	printf("\033[?25l");
	showBg();
	blocksInit();
	srand(time(NULL));

	next_color=rand()%7;
	next_base=rand()%Base;
	next_change=rand()%Change;
	next();

	//`	debugBlock();

	writeInBlock();
	printBlock();
	readme();
	
	tcgetattr(0,&old);
	new = old;
	new.c_lflag = new.c_lflag & ~(ICANON | ECHO);
	tcsetattr(0,TCSANOW,&new);

	while(1){
		c=getchar();
		switch(c){
			case 'q':
			case 'Q':
				break;
			case 'A':
			case 'a':
				if(judgeMove(LEFT)){
					execCmd(LEFT);
				}
				break;
			case 's':
			case 'S':
				if(judgeMove(DOWN)){
					execCmd(DOWN);
				} else{
					lastInBlock();
					cancelAlarm();
					next();
				}
				break;

			case 'd':
			case 'D':
				if(judgeMove(RIGHT))
					execCmd(RIGHT);
				break;

			case 'w':
			case 'W':
				if(judgeChange())
					execCmd(UP);
				break;
			case 'p':
			case 'P':
				if(state==0){
					pauseGame();
					state=1;
					}
				else {
					pauseGame();
					state=0;
				}

		}
	}


	//	printBlock();




	return 0;
}


void next(){
	setAlarm();


	x = 1;
	y = 5;

	cur_base= next_base;
	cur_change= next_change;
	cur_color=next_color;

	next_base=rand()%Base;
	next_change=rand()%Change;
	next_color=rand()%7;
        printNext();

	if(endGame())
    	{
        	printf("\033[15;20H游戏结束");
        	printf("\n\033[0m");
        	cancelAlarm();
        	exit(0);
    	}
	printf("\033[%d;2H", Row+X + 5);

}

void setAlarm(){
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 500000;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL,&itv,NULL);
} 

void cancelAlarm(){
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL,&itv,NULL);

}

void showBg(){
	int i;
	printf("\033[2J");	//清屏
	printf ("\033[%d;%dH\033[45m--========================--==============--\033[0m\n",X,Y);
	for(i=0;i < Row-2;i++){
		// 左 ||
		printf("\033[%d;%dH\033[45m||\033[0m",X+i+1,Y);

		//zhong ||
		printf("\033[%d;%dH\033[45m||\033[0m",X+i+1,Y+(Col-1)*2);

		//右边界 
		printf("\033[%d;%dH\033[45m||\033[0m",X+i+1,Y+Col*2+14);
		if(i == 6 || i == 11){
			printf("\033[%d;%dH\033[45m--------------\033[0m",X+i+1,Y+Col*2);
		}
	}

	printf ("\033[%d;%dH\033[45m-=========================--==============--\033[0m\n",X+Row-1,Y);

	//初始化边界 
	for(i=0;i<Col;i++){
		Tetris[0][i]=2;
		Tetris[Row-1][i]=2;
	} 

	for(i=0;i<Row;i++){
		Tetris[i][0]=2;
		Tetris[i][Col-1]=2;
	}

}


//初始化图形 
void blocksInit(){
	int row;//数组所占行 
	int col; //列 
	int base; 
	int change;


	// 1 田
	for(row=1;row<=2;row++){
		for(col =1;col<=2;col++){
			blocks[0][0].space[row][col]=1;
		}
	}

	// 2.z  3.反z 
	for(col=0;col<2;col++){

		//z
		blocks[1][0].space[1][col]=1; 
		blocks[1][0].space[2][col+1]=1;

		//反z
		blocks[2][0].space[1][col+1]=1;
		blocks[2][0].space[2][col]=1;
	}

	//4.I
	for(row=0;row<4;row++){
		blocks[3][0].space[row][1]=1;
	}

	//5. 7    6.反7 
	for(row=0;row<3;row++){
		for(col=0;col<2;col++){
			//7 
			blocks[4][0].space[row][1]=1;
			blocks[4][0].space[0][0]=1;

			//反7
			blocks[5][0].space[row][1]=1;
			blocks[5][0].space[0][2]=1;
		}
	}

	//土
	blocks[6][0].space[0][1]=1;
	for(col=0;col<3;col++){
		blocks[6][0].space[1][col]=1;
	}

	//图形旋转
	for(base = 0;base <7;base++){
		for(change =0;change<Change-1;change ++){
			for(row =0;row < N;row++){
				for(col = 0;col<N;col++){
					blocks[base][change+1].space[row][col]=blocks[base][change].space[N-1-col][row];
				}
			}
		}
	}

}

void debugBlock(){
	int base,row,col,change;
	for(base =0;base < Base;base++){
		printf("第%d个图形",base+1);
		for(change = 0;change<Change-1;change++){
			printf("第%d种变换\n",change+1);
			for(row = 0;row < N;row++){
				for(col = 0;col < N;col++){printf("%d ",blocks[base][change].space[row][col]);
				}
				printf("\n");
			}
		}
	}
}

/*
   将图形写入棋盘里 
   */

void writeInBlock(){
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			if(Tetris[i+x][j+y]!=2){//非边界 非落下来的方块 
				Tetris[i+x][j+y]=blocks[cur_base][cur_change].space[i][j];
			}
		} 

	}
	printBlock();
} 

/*
   打印显示棋盘 
   */

void printBlock(){

	int i,j;
	printf("\033[%d;%dH",X+1,Y+2);
	for(i = 1;i<Row-1;i++){
		for(j=1;j<Col-1;j++){
			if(Tetris[i][j]==1){
				selectColor(cur_color);
				printf("[]\033[0m");
			}
			else if(Tetris[i][j]==2){
				selectColor(els_color[i][j]);
				printf("[]\033[0m");
			}
			else {
				printf("%d\033[0m ",Tetris[i][j]);
			}
		}
		printf("\n\033[%dC",Y+1);	//ding wei qi shi lie
	}
	printf("\033[0m");
}



/*
   color change

*/

void selectColor(int color){

	int n = 40;
	switch(color){
		case DARKGREEN:
			n++;
		case PURPLE:
			n++;
		case BLUE:
			n++;
		case YELLOW:
			n++;
		case GREEN:
			n++;
		case RED:
			n++;
			break;
	}

	printf("\033[%dm",n);
}


/*
   all active 
   */

void execCmd(int cmd){
	switch(cmd){
		case LEFT:
			y--;
			break;
		case RIGHT:
			y++;
			break;
		case DOWN:
			x++;
			break;
		default :
			break;
	}
	cancelState(cmd);
	writeInBlock();

}

/*
   clear the previous state*/

void cancelState(int state){	//
	int i,j;
	int last_row,last_col;
	last_row = last_col =0;
	switch(state){
		case LEFT:
			last_col = 1; 
			break;
		case RIGHT:
			last_col = -1;
			break;
		case DOWN:
			last_row = -1;
			break;
		case UP:
			cur_change=(cur_change+1)%Change;
		default :break;

	}
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			if(Tetris[i+x+last_row][j+y+last_col]!=2){
				Tetris[i+x+last_row][j+y+last_col]=0;
			}
		}
	}
}


int judgeMove(int cmd){

	int row = 0;
	int col = 0;
	switch(cmd){
		case LEFT:
			col=-1;
			break;
		case DOWN:
			row=1;
			break;
		case RIGHT:
			col = 1;
			break;
	}

	for(int i = 0;i < N;i++){
		for(int j = 0;j < N;j++){
			if(blocks[cur_base][cur_change].space[i][j]){
				if(Tetris[i+x+row][j+y+col]==2)
					return 0;
			}
		}
	}
	return 1;
}



/*
   ting liu qi pan ,zhi 2 	
   */

void lastInBlock(){
	int i,j;

	for(i = 1;i < Row-1;i++){
		for(j = 1;j < Col -1;j++){
			if(Tetris[i][j] == 1){
				Tetris[i][j] = 2;
				els_color[i][j]=cur_color;
			}
		}
	}
	judgeElimate();
	printBlock();
/*	if(endGame()){
		exit(0);
	}*/

}


 void printNext(){
 	int i,j;

	printf("\033[%d;%dH",X+2,Y+31);
	for(i = 0;i < N;i++){
		for(j = 0;j < N;j++){
			if(blocks[next_base][next_change].space[i][j]){
				selectColor(next_color);
				printf("[]\033[0m");
			}
			else 
				printf("  \033[0m");
		}
		printf("\n\033[%dC",Y+30);
	}
	printf("\033[0m");
 
 }

int judgeChange(){
	int i,j;
	int t;

	t=(cur_change+1)%Change;

	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			if(blocks[cur_base][t].space[i][j]){
				if(Tetris[x+i][y+j]==2)
					return 0;
			}
		}
	}
	return 1;

}


int judgeElimate(){
	int i,j;
	int flag=0;
	int t;
	for(i = Row-2;i > 0;i--){
		flag=0;
		for(j = 1;j < Col - 1;j++){
			if(Tetris[i][j]==2)
				flag++;
		}
		if(flag==Col-2){
			t=i;
			elimateT(i);
			i=t+1;
		}
	
	}
}

void elimateT(int line){
	int i,j;
	for(i = line; i > 1;i--){
		for(j = 1;j<Col-1;j++){
			Tetris[i][j]=Tetris[i-1][j];
		}
	}
	for(j=2;j<Col-1;j++){
		Tetris[1][j]=0;
	}
	printScore();
}

void printScore(){
	score=score+1;
	printf("\033[%d;%dH",X+9,Y+31);
	printf("\033[%dm",42);
	printf("score:%d",score);

}

int endGame(){
	int i,j;

	for(j=1; j< Col - 1;j++){
		if(Tetris[i][j] == 2){
			return 1;
		}
	}

	for(i = 0;i < N;i++){
		for(j=0;j < N;j++){
			if(blocks[cur_base][cur_change].space[i][j]){
				if(Tetris[x+i][y+j]==2){
					return 1;
				}
			}
		}
	}
	return 0;

}

void pauseGame(){
	
	if(state==0){
		cancelAlarm();
	}
		
	else 
		setAlarm();
}

void readme(){
	 printf("\033[%d;%dH",X+13,Y+32);
	printf("Read me");
	printf("\033[%d;%dH",X+14,Y+28);
	printf("W:Change shape\n");
	 printf("\033[%d;%dH",X+15,Y+28);
	printf("A:Left\n");
	 printf("\033[%d;%dH",X+16,Y+28);
	printf("S:Right\n");
       	printf("\033[%d;%dH",X+17,Y+28);
	printf("D:Down\n");
	 printf("\033[%d;%dH",X+18,Y+28);
	printf("p:Pause\n");

}
