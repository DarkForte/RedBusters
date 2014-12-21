#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include<iostream>
#include<time.h>
using namespace std;

/*
  The map is 20x20, and each square is 24*24
  The initial coodernate of player is (10,10)
  Stage 1: Nothing
  Stage 2: Block
  Stage 3: Block+Trap
  Stage 4: Block+Trap+CleverWolf
  Stage 5: Block+Trap+CleverWolf(With WaveShooter)
*/

const int GRASS=1;
const int PLAYER=2;
const int WOLF=4;
const int BLOCK=8;
const int TRAP=16;
const int WAVE=32;

const int dx[4]={-1,1,0,0};
const int dy[4]={0,0,-1,1};

const int LEFT=0, RIGHT=1, UP=2, DOWN=3;
const int DRAW=1, DEL=2;
const int HELP=1, STAFF=2;

#define KEY_DOWN(vk_c) ( GetAsyncKeyState(vk_c) & 0x8000 ? 1 : 0)
#pragma comment (lib,"winmm.lib")

struct dtype
{
	int x; 
	int y;
};
struct ttype
{
	clock_t start;
	clock_t end;
	clock_t interval;
};

IMAGE button;
IMAGE frame;
IMAGE left_arrow;
IMAGE right_arrow;
IMAGE help[6];
IMAGE question;
IMAGE staff;
IMAGE info;
IMAGE loading;
IMAGE title;


void inkey()
{
	MOUSEMSG m;
	while(true)
	{
		if( kbhit() )
			break;
		else if(MouseHit())
		{
			m=GetMouseMsg();
			if(m.mkLButton==true)
				break;
		}
	}
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	FlushMouseMsgBuffer();
}

void OpenMusic()
{
	mciSendString("open \"music\\smooch.mp3\" alias stage1 ",NULL,0,NULL);
	mciSendString("open \"music\\LEVEL5.mp3\" alias stage2 ",NULL,0,NULL);
	mciSendString("open \"music\\Wily.mp3\" alias stage3 ",NULL,0,NULL);
	mciSendString("open \"music\\REDZONE.mp3\" alias stage4 ",NULL,0,NULL);
	mciSendString("open \"music\\UN.mp3\" alias stage5 ",NULL,0,NULL);
	mciSendString("open \"music\\StageClear.mp3\" alias clear ",NULL,0,NULL);
	mciSendString("open \"music\\StageSelect.mp3\" alias idle ",NULL,0,NULL);

	mciSendString("open \"se\\death.mp3\" alias death ",NULL,0,NULL);
	mciSendString("open \"se\\pause.mp3\" alias pause ",NULL,0,NULL);
	mciSendString("open \"se\\shoot.mp3\" alias shoot ",NULL,0,NULL);
	mciSendString("open \"se\\start.mp3\" alias start ",NULL,0,NULL);
	mciSendString("open \"se\\open1.mp3\" alias open1 ",NULL,0,NULL);
	mciSendString("open \"se\\open2.mp3\" alias open2 ",NULL,0,NULL);
	mciSendString("open \"se\\time.mp3\" alias time ",NULL,0,NULL);
}

void GetReady()
{
	mciSendString("open \"music\\title.mp3\" alias title ",NULL,0,NULL);
	mciSendString("play title",NULL,0,NULL);

	loadimage(&loading, "pictures\\loading.jpg");
	loadimage(&title,"pictures\\title.jpg");

	putimage(0,0,&loading);

	OpenMusic();
	setfillstyle(BLACK);
	bar(0,0,640,480);

	putimage(0,0,&title);

	inkey();
	
	Sleep(100);
	mciSendString("play start from 0", NULL,0,NULL);
	Sleep(200);
	mciSendString("stop all", NULL,0,NULL);
	bar(0,0,640,480);
	Sleep(100);
	return;
}

void PlayBGM(int now_stage)
{
	if(now_stage==1)
		mciSendString("play stage1 repeat from 0",NULL,0,NULL);
	else if(now_stage==2)
		mciSendString("play stage2 repeat from 0",NULL,0,NULL);
	else if(now_stage==3)
		mciSendString("play stage3 repeat from 0",NULL,0,NULL);
	else if(now_stage==4)
		mciSendString("play stage4 repeat from 0",NULL,0,NULL);
	else if(now_stage==5)
		mciSendString("play stage5 repeat from 0",NULL,0,NULL);
}

void ButtonDisplay(int style)//style: 1=idle 2=in game 3=allgray
{
	//button:70x30
	putimage(525,30,&button);
	putimage(525,70,&button);
	putimage(525,110,&button);
	putimage(525,150,&button);

	setfont(23,0,"微软雅黑");
	setcolor(BLACK);
	setbkmode(TRANSPARENT);
	
	if(style==1)
	{
		outtextxy(542, 32, "开始");
		outtextxy(542, 152, "退出");
		setcolor(LIGHTGRAY);
		outtextxy(542, 72, "暂停");
		outtextxy(542, 112, "放弃");
	}
	else if(style==2)
	{
		outtextxy(542, 72, "暂停");
		outtextxy(542, 112, "放弃");
		setcolor(LIGHTGRAY);
		outtextxy(542, 32, "开始");
	    outtextxy(542, 152, "退出");
	}
	else
	{
		setcolor(LIGHTGRAY);
		outtextxy(542, 72, "暂停");
		outtextxy(542, 112, "放弃");
		outtextxy(542, 32, "开始");
	    outtextxy(542, 152, "退出");
	}
}

void MenuDisplay() 
{
	loadimage(&button,"pictures\\start.jpg");
	loadimage(&frame ,"pictures\\frame.jpg");
	loadimage(&left_arrow, "pictures\\left.jpg");
	loadimage(&right_arrow, "pictures\\right.jpg");

	loadimage(&help[1], "pictures\\stage1help.jpg");
	loadimage(&help[2], "pictures\\stage2help.jpg");
	loadimage(&help[3], "pictures\\stage3help.jpg");
	loadimage(&help[4], "pictures\\stage4help.jpg");
	loadimage(&help[5], "pictures\\stage5help.jpg");
	loadimage(&question, "pictures\\question.jpg");
	loadimage(&staff, "pictures\\staff.jpg");
	loadimage(&info, "pictures\\info.jpg");

	setfillstyle(BLACK);
	bar(480,0,640,480);

	putimage(480,0,&frame);

	setfont(20,0,"Tahoma");
	setcolor(LIGHTRED);
	outtextxy(500, 200, "Win:");
	setcolor(WHITE);
	outtextxy(535, 200, "0");
	setcolor(LIGHTRED);
	outtextxy(570, 200, "Lose:");
	setcolor(WHITE);
	outtextxy(610, 200,"0");

	setcolor(LIGHTCYAN);
	outtextxy(500, 230, "Score:");
	outtextxy(500, 290, "Time Remain:");
	outtextxy(500, 350, "Now Level:");

	putimage(495,375, &left_arrow);
	putimage(567,375, &right_arrow);
	putimage(532,374,&question);
	putimage(597,435,&info);

	setcolor(WHITE);
	outtextxy(590,350,"1");

	ButtonDisplay(1);
}

void DrawLines()
{
	int i;
	setcolor(RGB(245,245,245));
	for(i=0;i<=480;i+=24)
		line(i,0,i,480);
    for(i=0;i<=480;i+=24)
		line(0,i,480,i);
    line(0,479,480,479);
}

int GenerateGrass(int map[21][21])
{
	srand((unsigned)time(NULL));
	int number;
	int low=24*6, high=24*12;
	number=rand()%(high-low) + low;

	int i;
	int nowx,nowy;
	for(i=1;i<=number;i++)
	{
		nowx=rand()%20+1;
		nowy=rand()%20+1;
		if((map[nowx][nowy]&GRASS) !=GRASS && (nowx!= 10 || nowy!=10) )
		{
			map[nowx][nowy]+=GRASS;
			setcolor(GREEN);
			setfont(16,0,"Tahoma");
			outtextxy(nowx*24-14,nowy*24-20,"/");
		}
		else
			i--;
	}
	return number;
}

dtype GenerateWolf ( int map[21][21] )
{
	srand(unsigned(time(NULL)));
	dtype wolf;
	int decide;
	decide=rand()%2;
	if(decide==0)
		wolf.x=1;
	else
		wolf.x=20;

	decide=rand()%2;
	if(decide==0)
		wolf.y=1;
	else
		wolf.y=20;

	map[wolf.x][wolf.y]+=WOLF;
	setfillstyle(LIGHTBLUE);
	bar((wolf.x-1)*24+3, (wolf.y-1)*24+3, wolf.x*24-3, wolf.y*24-3);
	
	return wolf;
}

void DrawRabbit(int x, int y, int style)//style==1: Draw; style==2:delete
{
	if(style==1)
	{
	    setfillstyle(RED);
		setcolor(RED);
		fillcircle(x*24-12, y*24-12,8);
	}
	else if(style==2)
	{
		setfillstyle(BLACK);
		setcolor(BLACK);
		fillcircle(x*24-12, y*24-12,8);
	}
}

void DrawWolf(int x, int y, int style, int grass)//style: 1=draw 2=delete
{
	if(style==1)
	{
	    setfillstyle(LIGHTBLUE);
		bar((x-1)*24+3, (y-1)*24+3, x*24-3, y*24-3);
	}
	else if(style==2)
	{
		setfillstyle(BLACK);
		bar((x-1)*24+3, (y-1)*24+3, x*24-3, y*24-3);
		if( grass==1)
		{
			setcolor(GREEN);
			setbkcolor(BLACK);
			setfont(16,0,"Tahoma");
			outtextxy(x*24-14,y*24-20,"/");
		}
	}
}

void GameOver()
{
	ButtonDisplay(3);
	mciSendString("stop all",NULL,0,NULL);
	mciSendString("play death from 0", NULL,0,NULL);
	Sleep(2000);
	setcolor(WHITE);
	//setbkcolor(CYAN);
	setfont(20,0,"Tahoma");
	outtextxy(500,400,"Game Over");
	Sleep(2000);
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	inkey();
	setfillstyle(BLACK);
	bar(500,400,620,430);
	return;
}

int TrackRabbit(int map[21][21], dtype Wolf, dtype Player)
{
	int q[410][3];// 0:x 1:y 2:value
	int head=0, tail=1;
	q[0][0]=Wolf.x;
	q[0][1]=Wolf.y;
	q[0][2]=0;

	bool mark[21][21];
	memset(mark, false , sizeof(bool) * 21*21);
	mark[Wolf.x][Wolf.y]=true;
	int i;
	while(head!=tail)
	{
		int nowx=q[head][0], nowy=q[head][1], nowv=q[head][2];
		if(nowx==Player.x && nowy==Player.y)
			return nowv;

		head++;

		for(i=0;i<=3;i++)
		{
			if(nowx+dx[i] >=1 && nowx+dx[i] <=20 && nowy+dy[i] >=1 && nowy+dy[i] <=20 //valid
				&& (map[ nowx + dx[i] ][ nowy + dy[i] ] & BLOCK) == 0 
				&& (map[ nowx + dx[i] ][ nowy + dy[i] ] & TRAP) == 0//can go
				&& mark[ nowx + dx[i] ][ nowy + dy[i] ] == false)//not gone
			{
				q[tail][0]=nowx+dx[i];
				q[tail][1]=nowy+dy[i];
				if(head==1)
					q[tail][2]=i;
				else
					q[tail][2]=nowv;

				mark[nowx+dx[i]][nowy+dy[i]]=true;
				tail++;
			}
		}
	}
}

int WolfMove(int map[21][21], dtype &Wolf, dtype Player, bool extend )
{
	map[Wolf.x][Wolf.y]-=WOLF;
	bool have_grass=false;
	if((map[Wolf.x][Wolf.y]&GRASS)==GRASS)
		have_grass=true;

	DrawWolf(Wolf.x, Wolf.y, 2,have_grass);

	if(extend==false)
	{
		if(Wolf.x>Player.x && (map[Wolf.x-1][Wolf.y]&BLOCK)==0 && (map[Wolf.x-1][Wolf.y]&TRAP)==0)
			Wolf.x--;
	
		else if(Wolf.x<Player.x && (map[Wolf.x+1][Wolf.y]&BLOCK)==0 && (map[Wolf.x+1][Wolf.y]&TRAP)==0)
			Wolf.x++;
	
		else if(Wolf.y>Player.y && (map[Wolf.x][Wolf.y-1]&BLOCK)==0 && (map[Wolf.x][Wolf.y-1]&TRAP)==0)
			Wolf.y--;

		else if(Wolf.y<Player.y && (map[Wolf.x][Wolf.y+1]&BLOCK)==0 && (map[Wolf.x][Wolf.y+1]&TRAP)==0)
			Wolf.y++;
	}
	else
	{
		int decide=TrackRabbit(map, Wolf, Player);//0=x-- 1=x++ 2=y-- 3=y++
		if(decide==0)
			Wolf.x--;
		else if(decide==1)
			Wolf.x++;
		else if(decide==2)
			Wolf.y--;
		else if(decide==3)
			Wolf.y++;
	}
	
	DrawWolf(Wolf.x, Wolf.y, 1,have_grass);

	if( (map[Wolf.x][Wolf.y]&PLAYER) ==PLAYER)
	{	
		GameOver();
		return 1;
	}
	
	map[Wolf.x][Wolf.y]+=WOLF;
	return 0;
}

void debugdisplay(int map[21][21])
{
	setcolor(LIGHTRED);
	int i,j;
	for(i=1;i<=20;i++)
		for(j=1;j<=20;j++)
			outtextxy(i*24-12, j*24-12, char(map[i][j]+'0') );
}

void haha()//debug
{
	return;
}

void OpenCountdown(int now_stage)
{
	char out[20]={0};
	int i=3;
	setfont(20,0,"Tahoma");
	while(i>0)
	{
		mciSendString("play open1 from 0",NULL,0,NULL);
		if(i==2)
			PlayBGM(now_stage);
		setcolor(WHITE);
		sprintf(out,"%d", i);
		outtextxy(580, 320, out);
		Sleep(1000);
		mciSendString("stop open1",NULL,0,NULL);
		setfillstyle(BLACK);
		bar(580, 320, 630, 350);
		i--;
	}
	mciSendString("play open2 from 0",NULL,0,NULL);
	outtextxy(570,320,"Go!!!");
	Sleep(1000);
	mciSendString("stop open2",NULL,0,NULL);
	setfillstyle(BLACK);
	bar(570, 320, 630, 350);
}

void ClockStop(ttype &t)
{
	t.end=clock();
	clock_t passed=t.end-t.start;
	t.interval-=passed;
	return;
}

void clear(MOUSEMSG &m)
{
	m.mkLButton=false;
	m.mkRButton=false;
	m.x=0;
	m.y=0;
}

bool CheckDelete(int map[21][21], int &GrassNum,dtype Wolf)
{
	bool connected[21][21];
	memset(connected, 0, sizeof(bool)*21*21);
	int queue[500][2]={0};//0=x 1=y
	int head=0,tail=1;

	queue[0][0]=10;
	queue[0][1]=10;
	connected[10][10]=true;

	int nowx, nowy;
	int i;

	while(head!=tail)
	{
		nowx=queue[head][0];
		nowy=queue[head][1];
		head++;

		for(i=0;i<=3;i++)
		{
			if(nowx+dx[i] >=1 && nowx+dx[i] <=20 && nowy+dy[i] >=1 && nowy+dy[i] <=20 //valid
				&& (map[ nowx + dx[i] ][ nowy + dy[i] ] & BLOCK) == 0 
				&& (map[ nowx + dx[i] ][ nowy + dy[i] ] & TRAP) == 0//can go
				&& connected[ nowx + dx[i] ][ nowy + dy[i] ] == false)//not gone
			{
				queue[tail][0]=nowx+dx[i];
				queue[tail][1]=nowy+dy[i];
				connected[nowx+dx[i]][nowy+dy[i]]=true;
				tail++;
			}
		}
	}

	if(connected[Wolf.x][Wolf.y]==false)
		return false;

	int j;
	for(i=1;i<=20;i++)
	{
		for(j=1;j<=20;j++)
		{
			if( ( map[i][j] & GRASS) !=0 && connected[i][j]==false)
			{
				GrassNum--;
				map[i][j]-=GRASS;
				setfillstyle(BLACK);
				bar((i-1)*24+3, (j-1)*24+3, i*24-3, j*24-3);
			}
		}
	}
	return true;
}

void GenerateBlock(int map[21][21], int &GrassNum, dtype Wolf, int now_stage)
{
	srand((unsigned)time(NULL));
	int num=(15-(GrassNum/24))*8;
	int trap_num=0;
	if(now_stage>=3)
		trap_num=num/4;
	int x,y;
	int i,j;
	int type;//0=block 1=trap
	while(true)
	{
		for(i=1;i<=num;i++)
		{
			if(trap_num==0)
				type=0;
			else
			    type=rand()%2;
			x=rand()%20+1;
			y=rand()%20+1;
			if(map[x][y]==0)
			{
				if(type==0)
				{
					setfillstyle(LIGHTGRAY);
					bar((x-1)*24+3, (y-1)*24+3, x*24-3, y*24-3);
					map[x][y]+=BLOCK;
				}
				else
				{
					setfillstyle(LIGHTMAGENTA);
					bar((x-1)*24+3, (y-1)*24+3, x*24-3, y*24-3);
					map[x][y]+=TRAP;
					trap_num--;
				}
			}
			else 
				i--;
		}
		if( CheckDelete(map,GrassNum,Wolf)==true )
			break;
		else
		{
			for(i=1;i<=20;i++)
			{
				for(j=1;j<=20;j++)
				{
					if( (map[i][j]&BLOCK) != 0 )
					{
						map[i][j]-=BLOCK;
						setfillstyle(BLACK);
						bar((i-1)*24+3, (j-1)*24+3, i*24-3, j*24-3);
					}
					else if( (map[i][j]&TRAP) !=0 )
					{
						map[i][j]-=TRAP;
						setfillstyle(BLACK);
						bar((i-1)*24+3, (j-1)*24+3, i*24-3, j*24-3);
					}
				}
			}
		}
	}
}

void DrawWave(int x, int y, int style, int map[21][21])
{
	if(style==DRAW)
	{
	    setfillstyle(LIGHTRED);
		bar((x-1)*24+3, (y-1)*24+3, x*24-3, y*24-3);
	}
	else if(style==DEL)
	{
		setfillstyle(BLACK);
		bar((x-1)*24+3, (y-1)*24+3, x*24-3, y*24-3);
		if((map[x][y]&GRASS)==GRASS)
		{
			setcolor(GREEN);
			setbkcolor(BLACK);
			setfont(16,0,"Tahoma");
			outtextxy(x*24-14,y*24-20,"/");
		}
		else if((map[x][y]&BLOCK)==BLOCK)
		{
			setfillstyle(LIGHTGRAY);
			bar((x-1)*24+3, (y-1)*24+3, x*24-3, y*24-3);
		}
		else if((map[x][y]&TRAP)==TRAP)
		{
			setfillstyle(LIGHTMAGENTA);
			bar((x-1)*24+3, (y-1)*24+3, x*24-3, y*24-3);
		}
		else if((map[x][y]&WOLF)==WOLF)
		{
			setfillstyle(BLUE);
			bar((x-1)*24+3, (y-1)*24+3, x*24-3, y*24-3);
		}
	}
}

void GenerateWave(int map[21][21], dtype Wolf, dtype Player, dtype &Wave, int &WaveDirect)
{
	if(Player.x==Wolf.x)
	{
		Wave.x=Player.x;
		if(Player.y>Wolf.y)
		{
			Wave.y=Wolf.y+1;
			WaveDirect=DOWN;
		}
		else 
		{
			Wave.y=Wolf.y-1;
			WaveDirect=UP;
		}
	}
	else
	{
		Wave.y=Player.y;
		if(Player.x>Wolf.x)
		{
			Wave.x=Wolf.x+1;
			WaveDirect=RIGHT;
		}
		else
		{
			Wave.x=Wolf.x-1;
			WaveDirect=LEFT;
		}
	}
	DrawWave(Wave.x, Wave.y, DRAW, map);
	map[Wave.x][Wave.y]+=WAVE;
}

int WaveMove(int map[21][21], dtype &Wave, int WaveDirect, bool &wave_exist)
{
	map[Wave.x][Wave.y]-=WAVE;
	DrawWave(Wave.x, Wave.y, DEL, map);
	Wave.x+=dx[WaveDirect];
	Wave.y+=dy[WaveDirect];
	if(Wave.x>=1 && Wave.x<=20 && Wave.y>=1 && Wave.y<=20)
	{
		DrawWave(Wave.x,Wave.y,DRAW,map);
		map[Wave.x][Wave.y]+=WAVE;
		if((map[Wave.x][Wave.y]&PLAYER)==PLAYER)
		{
			GameOver();
			return 1;
		}
	}
	else
		wave_exist=false;
	return 0;
}

void ShowHelp(int type,int now_stage)
{
	ButtonDisplay(3);
	mciSendString("play pause from 0",NULL,0,NULL);
	setfillstyle(BLACK);
	bar(0,0,480,480);
	if(type==HELP)
		putimage(0,0,&help[now_stage]);
	else if(type==STAFF)
		putimage(0,0,&staff);
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	Sleep(200);
	inkey();
	bar(0,0,480,480);
	DrawLines();
	ButtonDisplay(1);
	
}

bool InStage(int map[21][21], int now_stage)
{
	memset(map,0,sizeof(int)*21*21);
	int GrassNum;
	GrassNum=GenerateGrass(map);

	DrawRabbit(10,10,1);
	map[10][10]+=PLAYER;

	dtype Wolf;
	Wolf=GenerateWolf(map);
	bool extend=false;
	if(now_stage>=4)
		extend=true;

	if(now_stage>=2)
		GenerateBlock(map,GrassNum,Wolf,now_stage);
	
	dtype Player;
	Player.x=10;
	Player.y=10;

	bool wave_exist=false;
	dtype Wave;
	int WaveDirect=-1;

	int score=0;
	int PlayerGrass=0;
	char out[20]={0};

	setfillstyle(BLACK);
	bar(580,255,630,280);
	setcolor(WHITE);
	//setbkcolor(CYAN);
	setfont(20,0,"Tahoma");
	outtextxy(580,255,"0");

	ButtonDisplay(3);

	ttype WolfClock;
	WolfClock.start=clock();
	WolfClock.interval=200;

	ttype GameClock;
	GameClock.start=clock();
	GameClock.interval=1000;
	int countdown=70;
	if(now_stage==2)
		countdown=80;
	else if(now_stage==3)
		countdown=90;
	else if(now_stage==4)
		countdown=100;
	else if(now_stage==5)
		countdown=110;

	ttype WaveClock;
	WaveClock.interval=60;

	ttype InputClock;
	InputClock.start=clock();
	clock_t delta_time;

	OpenCountdown(now_stage);

    ButtonDisplay(2);

	MOUSEMSG m;
	FlushMouseMsgBuffer();
	while(true)
	{
		clear(m);
		if( MouseHit() )
			m=GetMouseMsg();
		//////////////////////////////////////////////////////  Control Part
		InputClock.end=clock();
		delta_time=InputClock.end-InputClock.start;

		if(KEY_DOWN(VK_UP) && Player.y>=2 && (map[Player.x][Player.y-1]&BLOCK)==0 && delta_time>=100)
		{
			map[Player.x][Player.y]-=PLAYER;
			DrawRabbit(Player.x, Player.y,2);
			Player.y--;
		    DrawRabbit(Player.x, Player.y,1);
			InputClock.start=clock();
		}
		else if(KEY_DOWN(VK_DOWN) && Player.y<=19 && (map[Player.x][Player.y+1]&BLOCK)==0 && delta_time>=100)
		{
			map[Player.x][Player.y]-=PLAYER;
			DrawRabbit(Player.x, Player.y,2);
			Player.y++;
		    DrawRabbit(Player.x, Player.y,1);
			InputClock.start=clock();
		}
		else if(KEY_DOWN(VK_LEFT) && Player.x>=2 && (map[Player.x-1][Player.y]&BLOCK)==0 && delta_time>=100)
		{
			map[Player.x][Player.y]-=PLAYER;
			DrawRabbit(Player.x, Player.y,2);
			Player.x--;
		    DrawRabbit(Player.x, Player.y,1);
			InputClock.start=clock();
		}
		else if(KEY_DOWN(VK_RIGHT) && Player.x<=19 && (map[Player.x+1][Player.y]&BLOCK)==0 && delta_time>=100)
		{
			map[Player.x][Player.y]-=PLAYER;
			DrawRabbit(Player.x, Player.y,2);
			Player.x++;
		    DrawRabbit(Player.x, Player.y,1);
			InputClock.start=clock();
		}
		//pause
		else if( KEY_DOWN(VK_SPACE) || (m.mkLButton && (m.x>=525 && m.y>=70 && m.x<=595 && m.y<=100) ) )
		{
			mciSendString("play pause from 0",NULL,0,NULL);
			ClockStop(WolfClock);
			ClockStop(GameClock);
			if(wave_exist)
				ClockStop(WaveClock);
			setcolor(WHITE);
			setfont(20,0,"Tahoma");
			outtextxy(500,400,"Pause");
			Sleep(200);
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
			while(true)
			{
				clear(m);
				if(MouseHit())
					m=GetMouseMsg();
				if( KEY_DOWN(VK_SPACE) || (m.mkLButton && (m.x>=525 && m.y>=70 && m.x<=595 && m.y<=100) ) )
				{
					mciSendString("play pause from 0",NULL,0,NULL);
					Sleep(200);
					FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
					break;
				}
			}
			setfillstyle(BLACK);
			bar(500,400,620,430);
			WolfClock.start=clock();
			GameClock.start=clock();
			if(wave_exist)
				WaveClock.start=clock();
		}
		else if( KEY_DOWN(VK_F1) || (m.mkLButton && (m.x>=525 && m.y>=110 && m.x<=595 && m.y<=140)) )
		{
			mciSendString("stop all",NULL,0,NULL);
			mciSendString("play death from 0", NULL,0,NULL);
		    return false;
		}

		//////////////////////////////////////// Judge Part
		if( (map[Player.x][Player.y]&GRASS)==GRASS)
		{
			score+=20;
			PlayerGrass++;
			map[Player.x][Player.y]-=GRASS;
			setcolor(WHITE);
			//setbkcolor(CYAN);
			sprintf(out, "%d", score);
			setfillstyle(BLACK);
			bar(580,255,630,280);
			setfont(20,0,"Tahoma");
			outtextxy(580,255,out);

			if(PlayerGrass==GrassNum)
			{
				ButtonDisplay(3);
				mciSendString("stop all",NULL,0,NULL);
				Sleep(500);
				mciSendString("play clear from 0",NULL,0,NULL);
				Sleep(2000);
				setfont(20,0,"Tahoma");
				outtextxy(500,400,"You Win!!");
				Sleep(2000);
				FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
				inkey();
				setfillstyle(BLACK);
				bar(500,400,620,430);
				return true;
			}
		}
		else if( (map[Player.x][Player.y]&WOLF)==WOLF || (map[Player.x][Player.y]&TRAP)==TRAP 
			|| (map[Player.x][Player.y]&WAVE)==WAVE)
		{
			GameOver();
			return false;
		}

		if((map[Player.x][Player.y]&PLAYER) == 0)
			map[Player.x][Player.y]+=PLAYER;
		//////////////////////////////////////////////////////////// Clock Part
		WolfClock.end=clock();
		if(WolfClock.end - WolfClock.start > WolfClock.interval)
		{
			if(now_stage>=5 && (Player.x==Wolf.x || Player.y==Wolf.y) && wave_exist==false 
				&& abs(Player.x-Wolf.x)+abs(Player.y-Wolf.y)>=7)
			{
				mciSendString("play shoot from 0",NULL,0,NULL);
				GenerateWave(map, Wolf, Player, Wave, WaveDirect);
				WaveClock.start=clock();
				wave_exist=true;
			}
			int judge=0;
			judge=WolfMove(map,Wolf,Player,extend);
			if(judge==1)//gameover
				return false;
			WolfClock.start=clock();
			WolfClock.interval=200;
		}

		GameClock.end=clock();
		if(GameClock.end - GameClock.start > GameClock.interval)
		{
			countdown--;
			setfillstyle(BLACK);
			bar(580, 320, 630, 350);
			sprintf(out, "%d", countdown);
			setcolor(WHITE);
			setfont(20,0,"Tahoma");
			outtextxy(580, 320, out);
			if(countdown==10)
				mciSendString("play time from 0", NULL,0,NULL);
			if(countdown==0)
			{
				mciSendString("stop all", NULL,0,NULL);
				outtextxy(500, 400, "Time up!");
				Sleep(3000);
				setfillstyle(BLACK);
				bar(500,400,620,430);
				GameOver();
				return false;
			}
			GameClock.start=clock();
			GameClock.interval=1000;
		}

		if(wave_exist)
		{
			WaveClock.end=clock();
			if(WaveClock.end-WaveClock.start > WaveClock.interval)
			{
				int judge=0;
				judge=WaveMove(map,Wave,WaveDirect, wave_exist);
				if(judge==1)//gameover
					return false;
				WaveClock.start=clock();
				WaveClock.interval=60;
			}
		}

		////////////////////////////////////////////////////////debug part
		//debugdisplay(map);

	}
	
}

int main()
{
	/*char PlayerName[100]={0};
	cout<<"欢迎新的挑战者！！"<<endl;
	cout<<"别着急，请先在这里输入您的姓名：";
	cin>>PlayerName;
	cout<<"请稍候，冒险马上开始！"<<endl;
	cout<<"Loading...";
	Sleep(1500);*/

	initgraph(640, 480);
	GetReady();

	int map[21][21];//map=20*20
	int win=0,lose=0;
	char out[20]={0};
	int now_stage=1;
	MOUSEMSG m;
	MenuDisplay();
	DrawLines();
	mciSendString("play idle repeat from 0",NULL,0,NULL);
	while(true)
	{
		clear(m);
		bool pass;
		if(MouseHit())
			m=GetMouseMsg();
		if( KEY_DOWN(VK_RETURN) || (m.mkLButton && (m.x>=525 && m.x<=595 && m.y>=30 && m.y<=60) )   )
		{
			mciSendString("stop idle",NULL,0,NULL);
			mciSendString("play start from 0",NULL,0,NULL);
			pass=InStage(map,now_stage);
			if(pass)
				win++;
			else
				lose++;

			setfillstyle(BLACK);
			setfont(20,0,"Tahoma");
			setcolor(WHITE);

			sprintf(out,"%d",win);
			bar(535,200,550,220);
			outtextxy(535, 200, out);

			sprintf(out,"%d",lose);
			bar(610,200,625,220);
			outtextxy(610,200,out);

			bar(580, 320, 630, 350);
			bar(0,0,480,480);
			DrawLines();
			ButtonDisplay(1);
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
			mciSendString("play idle from 0 repeat",NULL,0,NULL);
		}

		else if( KEY_DOWN(VK_ESCAPE) || (m.mkLButton && (m.x>=525 && m.y>=150 && m.x<=595 && m.y<=180)))
			break;

		else if(m.mkLButton && (m.x>=495 && m.y>=375 && m.x<=510 && m.y<=390) && now_stage>=2)
		{
			now_stage--;
			setfillstyle(BLACK);
			bar(590,350,610,370);
			sprintf(out,"%d", now_stage);
			setcolor(WHITE);
			setfont(20,0,"Tahoma");
			outtextxy(590,350,out);
			Sleep(300);
		}
		else if(m.mkLButton && (m.x>=567 && m.y>=375 && m.x<=583 && m.y<=390) && now_stage<=4)
		{
			now_stage++;
			setfillstyle(BLACK);
			bar(590,350,610,370);
			sprintf(out,"%d", now_stage);
			setcolor(WHITE);
			setfont(20,0,"Tahoma");
			outtextxy(590,350,out);
			Sleep(300);
		}
		else if(m.mkLButton && (m.x>=532 && m.y>=374 && m.x<=532+17 && m.y<=374+17))
		    ShowHelp(HELP, now_stage);
		else if(m.mkLButton && (m.x>=597 && m.y>=435 && m.x<=597+17 && m.y<=435+17))
			ShowHelp(STAFF,now_stage);
	}
	mciSendString("close all",NULL,0,NULL);
	closegraph();
}

