#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "SDLFR.h"

const char Windows_Title[] = "Example";//���ڵı���
const int Windows_X = 0;//���ڳ�ʼλ��Xֵ
const int Windows_Y = 0;//���ڳ�ʼλ��Yֵ
const int Windows_Width = 1920;
const int Windows_Height = 1080;

struct ball
{
	int r;
	int x;
	int y;
	int v_x;
	int v_y;
};

struct MissionInf
{
	int IsUnlock;
	int QuickestTime;
};

struct SaveFile
{
	int MissionNow;
	MissionInf Mission[6];
};

SaveFile GameSaveFile;
SDL_Window *Main_Windows = nullptr;//����Ϊһ����Ϸֻ��Ҫһ������
SDL_Renderer *Main_Renderer = nullptr;//һ����Ҫ����Ⱦ�����൱����ʾҳ�棬��Ȼʹ�ö��ҳ���Ǻ�������
TTF_Font *Main_Font = nullptr;


void Game(void);
void ReadGameFile(void);
void SaveGameFile(void);
int IsTouch(int x1,int y1,int x2,int y2,ball MyBall);//�ж�Ŀ��λ���Ƿ�����Ӵ�
void ShowGallery();

char *IntToChar(int a)
{
	static char b[20];
	sprintf(b,"%d",a);
	return b;
}

void ReadGameFile(void)
{
	int i;
	FILE *fp;
	if ((fp = fopen("MRGameSave.sav","r")) == NULL)
	{
		GameSaveFile.MissionNow = 0;
		for (i = 0;i < 6;i ++)
		{
			GameSaveFile.Mission[i].IsUnlock = 0;
			GameSaveFile.Mission[i].QuickestTime = -1;
		}
	}
	else
	{
		fread(&GameSaveFile,sizeof(GameSaveFile),1,fp);
		fclose(fp);
	}
}

void SaveGameFile(void)
{
	FILE *fp;
	if ((fp = fopen("MRGameSave.sav","w")) == NULL)
	{
		printf("Fail to open file!\n");
		getchar();
		getchar();
	}
	else
	{
		fwrite(&GameSaveFile,sizeof(SaveFile),1,fp);
		fclose(fp);
	}
}

int main(int argc, char** argv)
{
	int Arrow_Id;
	int While_Quit;
	int Author_On = 0;

	assert(SDL_Init(SDL_INIT_EVERYTHING) != -1);//��ʼ��SDL

	Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,1024);
	
	Main_Windows = SDL_CreateWindow(Windows_Title,Windows_X,Windows_Y,Windows_Width,Windows_Height,SDL_WINDOW_FULLSCREEN);
	assert(Main_Windows != nullptr);

	SDL_Surface *WindowsIcon = SDL_LoadBMP("IMG\\icon.bmp");

	SDL_SetWindowIcon(Main_Windows,WindowsIcon);

	Main_Renderer = SDL_CreateRenderer(Main_Windows, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);//������һ��ҳ��,�������Ⱦ��
	assert(Main_Renderer != nullptr);

	assert(TTF_Init() != -1);//��ʼ��TTF��

	Main_Font = TTF_OpenFont("msyhbd.ttf", 72);

	SDL_Texture *BG_Pic = IMG_LoadTexture(Main_Renderer,"IMG\\main_menu_bg.png");//������һ������ͼƬָ��

	SDL_Texture *Arrow_Pic = IMG_LoadTexture(Main_Renderer,"IMG\\main_menu_arrow.png");

	Mix_Music *MusicTheme = Mix_LoadMUS("MUSIC\\theme.wav");

	Mix_PlayMusic(MusicTheme,-1);

	ReadGameFile();

	SDL_Event event;

	While_Quit = 0;
	Arrow_Id = 0;
	while (!While_Quit)
	{
		while(SDL_PollEvent(&event))
		{
			
			SDL_RenderClear(Main_Renderer);

			SDL_RenderCopy(Main_Renderer, BG_Pic, NULL, NULL);
			
			SDL_CopyTextureToRenderer(Main_Renderer,Arrow_Pic,1,0,0,0,0,1200,624 + Arrow_Id * 100,0,0);

			if (Author_On == 1)
				SDL_ShowTTFtoRenderer(Main_Font,Main_Renderer,"��������ʿHello Clyde����",61,978,0,0,0);

			SDL_RenderPresent(Main_Renderer);

			if (event.type == SDL_QUIT)
				While_Quit = 1;
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE )
					While_Quit = 1;
				else if(event.key.keysym.sym == SDLK_w)
				{
					if (Arrow_Id > 0)
						Arrow_Id --;
					else
						Arrow_Id = 3;
				}
				else if(event.key.keysym.sym == SDLK_s)
				{
					if (Arrow_Id < 3)
						Arrow_Id ++;
					else
						Arrow_Id = 0;
				}
				else if(event.key.keysym.sym == SDLK_RETURN)
				{
					if (Arrow_Id == 0)
						Game();
					else if (Arrow_Id == 1)
						ShowGallery();
					else if (Arrow_Id == 2)
					{
						if (Author_On == 1)
							Author_On = 0;
						else
							Author_On = 1;
					}
					else if (Arrow_Id == 3)
						While_Quit = 1;
				}
			}
         }
    }

	SDL_DestroyTexture(BG_Pic);
	SDL_DestroyTexture(Arrow_Pic);
	SDL_FreeSurface(WindowsIcon);
	Mix_CloseAudio();
	SDL_Quit();
	return 0;
}

void Game(void)
{
	char TempString[100];
	int BrickTotalNum,BrickBreakNum;
	int brick_x,brick_y;
	int bricks[6][26][15] = {{
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
						{0,0,0,0,0,0,0,0,0,1,1,1,1,0,0},
						{0,0,0,0,0,0,0,0,1,1,1,1,1,1,0},
						{0,0,0,0,0,1,1,1,1,1,1,1,1,1,0},
						{0,0,0,1,1,1,1,1,1,1,1,1,1,1,1},
						{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1},
						{0,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
						{0,0,1,1,1,1,1,1,1,1,1,1,1,0,0},
						{0,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},},
						{
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,1,1,0,0,0,0,0},
						{0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},
						{0,0,0,0,0,1,1,1,1,1,1,1,0,0,0},
						{0,0,0,0,1,1,1,1,1,1,1,1,0,0,0},
						{0,0,0,1,1,1,1,1,1,1,1,1,0,0,0},
						{0,0,0,1,1,1,1,1,1,1,1,1,1,0,0},
						{0,0,0,0,1,1,1,1,1,1,1,1,1,1,0},
						{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
						{0,0,0,0,0,0,1,0,1,1,1,1,1,1,1},
						{0,0,0,0,0,0,0,0,0,1,1,1,1,1,1},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},},
						{
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,1,0,0,0,1,1,0,0,0,0,0},
						{0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
						{0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
						{0,0,0,1,1,1,1,1,1,1,1,1,0,0,0},
						{0,0,0,0,1,1,1,1,1,1,1,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,1,1,1,1,1,0,0,0,0,0,0,0},
						{0,0,0,0,1,1,1,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},},
						{
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,1,1,0,0,0,0,0},
						{0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},
						{0,0,0,0,0,1,1,1,1,1,1,1,0,0,0},
						{0,0,0,0,1,1,1,1,1,1,1,1,0,0,0},
						{0,0,0,1,1,1,1,1,1,1,1,1,0,0,0},
						{0,0,0,1,1,1,1,1,1,1,1,1,1,0,0},
						{0,0,0,0,1,1,1,1,1,1,1,1,1,1,0},
						{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
						{0,0,0,0,0,0,1,0,1,1,1,1,1,1,1},
						{0,0,0,0,0,0,0,0,0,1,1,1,1,1,1},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},},
						{
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,1,1,1,0,0,0,1,1,1,1,1,1,0},
						{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
						{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
						{1,1,1,1,0,1,1,1,1,1,1,0,1,1,1},
						{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
						{0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
						{0,0,0,0,1,1,1,1,1,1,1,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},},
						{
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},
						{0,0,1,1,1,1,0,1,1,1,1,0,0,0,0},
						{0,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
						{0,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
						{0,0,1,1,1,1,1,1,1,1,1,0,0,0,0},
						{0,0,1,1,1,1,1,1,1,1,1,0,0,0,0},
						{0,0,0,1,1,1,1,1,1,1,0,0,0,0,0},
						{0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},},
	
	};
	/*
	{
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},},*/

	SDL_Texture *Game_BG_Pic = IMG_LoadTexture(Main_Renderer,"IMG\\game_desktop.png");
	
	strcpy(TempString,"IMG\\Mission\\00");
	strcat(TempString,IntToChar(GameSaveFile.MissionNow + 1));
	strcat(TempString,".png");

	SDL_Texture *Mission_Pic = IMG_LoadTexture(Main_Renderer,TempString);

	SDL_Texture *Board_Pic = IMG_LoadTexture(Main_Renderer,"IMG\\board.png");

	SDL_Texture *Brick_Pic = IMG_LoadTexture(Main_Renderer,"IMG\\brick.png");

	SDL_Texture *Ball_Pic = IMG_LoadTexture(Main_Renderer,"IMG\\ball.png");

	SDL_Texture *GameOver_Pic = IMG_LoadTexture(Main_Renderer,"IMG\\gameover.png");

	SDL_Texture *GameWin_Pic;

	Mix_Chunk *SoundBoom1 = Mix_LoadWAV("MUSIC\\boom1.wav");
	Mix_Chunk *SoundBoom2 = Mix_LoadWAV("MUSIC\\boom2.wav");
	Mix_Chunk *SoundWin = Mix_LoadWAV("MUSIC\\win.wav");
	Mix_Chunk *SoundLose = Mix_LoadWAV("MUSIC\\lose.wav");
	Mix_Chunk *SoundHeart = Mix_LoadWAV("MUSIC\\heart.wav");

	ball NewBall;
	NewBall.r = 23;
	NewBall.x = 959;
	NewBall.y = 1043 - 41 - NewBall.r - 1;
	NewBall.v_x = 2;
	NewBall.v_y = -8;


	SDL_Event event;

	int GameOverPosX;

	int Board_X = 829;//611~1300-260,�ײ�1043,��41

	long Time,OldTime;

	int While_Quit,WhileGameOver;

	//����ש������
	BrickTotalNum = 0;
	for (brick_y = 0; brick_y < 26;brick_y ++)
	{
		for (brick_x = 0; brick_x < 15; brick_x ++)
		{
			if (bricks[GameSaveFile.MissionNow][brick_y][brick_x] == 1)
				BrickTotalNum ++;
		}
	}

	BrickBreakNum = 0;

	While_Quit = 0;
	Time = 300;
	OldTime = SDL_GetTicks();

	while (While_Quit != 1)
	{

		SDL_RenderClear(Main_Renderer);

		SDL_RenderCopy(Main_Renderer, Game_BG_Pic, NULL, NULL);

		SDL_CopyTextureToRenderer(Main_Renderer,Mission_Pic,1,0,0,0,0,611,38,0,0);

		//��Ⱦש��
		for (brick_y = 0; brick_y < 26;brick_y ++)
		{
			for (brick_x = 0; brick_x < 15; brick_x ++)
			{
				if (bricks[GameSaveFile.MissionNow][brick_y][brick_x] == 1)
					SDL_CopyTextureToRenderer(Main_Renderer,Brick_Pic,1,0,0,0,0,612 + brick_x * 46,39 + brick_y * 46,0,0);
			}
		}

		//��Ⱦ����
		SDL_CopyTextureToRenderer(Main_Renderer,Ball_Pic,1,0,0,0,0,NewBall.x - NewBall.r,NewBall.y - NewBall.r,0,0);

		//����ʱ��
		SDL_ShowTTFtoRenderer(Main_Font,Main_Renderer,IntToChar(Time),1502,873,255,255,0);

		//�Ѿ��ƻ���ש����Ŀ
		SDL_ShowTTFtoRenderer(Main_Font,Main_Renderer,IntToChar(BrickBreakNum),1397,254,255,255,0);

		//��ǹ��
		if (GameSaveFile.Mission[GameSaveFile.MissionNow].QuickestTime != -1)
			SDL_ShowTTFtoRenderer(Main_Font,Main_Renderer,IntToChar(GameSaveFile.Mission[GameSaveFile.MissionNow].QuickestTime),150,942,255,255,0);
		else
			SDL_ShowTTFtoRenderer(Main_Font,Main_Renderer,"�ѵ������㣿",150,942,255,255,0);

		//��Ȼ��ͦ��ש��
		SDL_ShowTTFtoRenderer(Main_Font,Main_Renderer,IntToChar(BrickTotalNum - BrickBreakNum),1388,558,255,255,0);

		//��Ⱦľ��
		SDL_CopyTextureToRenderer(Main_Renderer,Board_Pic,1,0,0,0,0,Board_X,1043 - 41,0,0);

		SDL_RenderPresent(Main_Renderer);

		//��ʱ��
		if ((SDL_GetTicks() - OldTime) > 1000)
		{
			if (Time > 0)
			{
				Time --;
				OldTime = SDL_GetTicks();
			}
			else
				While_Quit = 1;
		}

		//����λ��
		NewBall.x += NewBall.v_x;
		NewBall.y += NewBall.v_y;

		//�߿���ײ
		if(NewBall.x < 612 + NewBall.r)//��߿�
		{
			NewBall.x = 612 + NewBall.r;
			NewBall.v_x = - NewBall.v_x;
		}
		else if (NewBall.x > 1300 - NewBall.r)//�ұ߿�
		{
			NewBall.x = 1300 - NewBall.r;
			NewBall.v_x = - NewBall.v_x;
		}
		else if (NewBall.y < 38 + NewBall.r)//�ϱ߿�
		{
			NewBall.y = 38 + NewBall.r;
			NewBall.v_y = - NewBall.v_y;
		}
		else if (NewBall.y > 1042 - NewBall.r)//�±߿�,����Ϸʧ��
		{
			/*
			NewBall.y = 1042 - NewBall.r;
			NewBall.v_y = - NewBall.v_y;
			*/
			Mix_PlayChannel(-1,SoundLose,0);
			GameOverPosX = 1920 - 810;
			SDL_CopyTextureToRenderer(Main_Renderer,GameOver_Pic,1,0,0,0,0,GameOverPosX,0,0,0);
			SDL_RenderPresent(Main_Renderer);
			While_Quit = 1;

			WhileGameOver = 0;
			while (!WhileGameOver)
			{
				while(SDL_PollEvent(&event))
				{
					if (event.type == SDL_QUIT)
					{
						WhileGameOver = 1;
					}
					else if (event.type == SDL_KEYDOWN)
					{
						WhileGameOver = 1;
					}
				 }
			}
		}
		//ש����ײ
		for (brick_y = 0; brick_y < 26;brick_y ++)
		{
			for (brick_x = 0; brick_x < 15; brick_x ++)
			{
				if (bricks[GameSaveFile.MissionNow][brick_y][brick_x] == 1)
				{
					if (IsTouch(612 + brick_x * 46,39 + brick_y * 46,612 + brick_x * 46 + 46,39 + brick_y * 46 + 46,NewBall))
					{
						bricks[GameSaveFile.MissionNow][brick_y][brick_x] = 0;//��������
						srand(time(NULL));
						if (rand() % 2)
							Mix_PlayChannel(-1,SoundBoom1,0);
						else
							Mix_PlayChannel(-1,SoundBoom2,0);

						BrickBreakNum ++;
						if (BrickBreakNum == BrickTotalNum - 3)
							Mix_PlayChannel(-1,SoundHeart,0);

						if (IsTouch(612 + brick_x * 46,39 + brick_y * 46,612 + brick_x * 46 + 46,39 + brick_y * 46 + 46,NewBall) == 1)
						{
							NewBall.y = 39 + brick_y * 46 - NewBall.r;
							NewBall.v_y = -NewBall.v_y;
						}
						else if (IsTouch(612 + brick_x * 46,39 + brick_y * 46,612 + brick_x * 46 + 46,39 + brick_y * 46 + 46,NewBall) == 2)
						{
							NewBall.y = 39 + brick_y * 46 + 46 + NewBall.r;
							NewBall.v_y = -NewBall.v_y;
						}
						else if (IsTouch(612 + brick_x * 46,39 + brick_y * 46,612 + brick_x * 46 + 46,39 + brick_y * 46 + 46,NewBall) == 3)
						{
							NewBall.x = 612 + brick_x * 46 - NewBall.r;
							NewBall.v_x = -NewBall.v_x;
						}
						else if (IsTouch(612 + brick_x * 46,39 + brick_y * 46,612 + brick_x * 46 + 46,39 + brick_y * 46 + 46,NewBall) == 4)
						{
							NewBall.x = 612 + brick_x * 46 + 46 + NewBall.r;
							NewBall.v_x = -NewBall.v_x;
						}
					}
				}
			}
		}



		//ľ����ײ
		if (IsTouch(Board_X,1043 - 41,Board_X + 260,1043,NewBall) == 1)
		{
			NewBall.y = 1043 - 41 - NewBall.r;
			NewBall.v_y = -NewBall.v_y;
		}
		else if (IsTouch(Board_X,1043 - 41,Board_X + 260,1043,NewBall) == 3)
		{
			NewBall.x = Board_X - NewBall.r;
			NewBall.v_x = -NewBall.v_x;
		}
		else if (IsTouch(Board_X,1043 - 41,Board_X + 260,1043,NewBall) == 4)
		{
			NewBall.x = Board_X + 260 + NewBall.r;
			NewBall.v_x = -NewBall.v_x;
		}

		//�ж��Ƿ�ʤ��
		if (BrickTotalNum == BrickBreakNum)
		{
			Mix_PlayChannel(-1,SoundWin,0);
			strcpy(TempString,"IMG\\Mission\\00");
			strcat(TempString,IntToChar(GameSaveFile.MissionNow + 1));
			strcat(TempString,"w.png");
			GameWin_Pic = IMG_LoadTexture(Main_Renderer,TempString);

			SDL_CopyTextureToRenderer(Main_Renderer,GameWin_Pic,1,0,0,0,0,0,0,0,0);
			SDL_RenderPresent(Main_Renderer);

			GameSaveFile.Mission[GameSaveFile.MissionNow].IsUnlock = 1;

			if (GameSaveFile.MissionNow == 5)
				GameSaveFile.MissionNow = 0;
			else
				GameSaveFile.MissionNow ++;

			if ((300- Time < GameSaveFile.Mission[GameSaveFile.MissionNow].QuickestTime) || (GameSaveFile.Mission[GameSaveFile.MissionNow].QuickestTime == -1))
				GameSaveFile.Mission[GameSaveFile.MissionNow].QuickestTime = 300 - Time;

			SaveGameFile();

			While_Quit = 1;

			WhileGameOver = 0;
			while (!WhileGameOver)
			{
				while(SDL_PollEvent(&event))
				{
					if (event.type == SDL_QUIT)
					{
						WhileGameOver = 1;
					}
					else if (event.type == SDL_KEYDOWN)
					{
						WhileGameOver = 1;
					}
				 }
			}
		}

		//�����¼�
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				While_Quit = 1;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE )
					While_Quit = 1;
			}
         }
		if (SDL_KeyBoardKeyPressed(SDL_SCANCODE_A))
		{
			if (Board_X > 610)
				Board_X -= 4;
		}
		else if (SDL_KeyBoardKeyPressed(SDL_SCANCODE_D))
		{
			if (Board_X < 1300 - 260)
				Board_X += 4;
		}
    }

	SDL_DestroyTexture(Board_Pic);
	SDL_DestroyTexture(Mission_Pic);
	SDL_DestroyTexture(Game_BG_Pic);
	SDL_DestroyTexture(Brick_Pic);
	SDL_DestroyTexture(Ball_Pic);
	SDL_DestroyTexture(GameOver_Pic);
}

//�ж�Ŀ��λ���Ƿ�����Ӵ���δ�Ӵ�����0���Ӵ��������ҷ���1234
int IsTouch(int x1,int y1,int x2,int y2,ball MyBall)
{
	if ((MyBall.y < y1 + MyBall.r && MyBall.y > y1 - MyBall.r) && (MyBall.x > x1 - MyBall.r && MyBall.x < x2+ MyBall.r))
		return 1;
	else if ((MyBall.y < y2 + MyBall.r && MyBall.y > y2 - MyBall.r) && (MyBall.x > x1 - MyBall.r && MyBall.x < x2 + MyBall.r))
		return 2;
	else if ((MyBall.x < x1 + MyBall.r && MyBall.x > x1 - MyBall.r) && (MyBall.y > y1 - MyBall.r && MyBall.y < y2 + MyBall.r))
		return 3;
	else if ((MyBall.x < x2 + MyBall.r && MyBall.x > x2 - MyBall.r) && (MyBall.y > y1 - MyBall.r && MyBall.y < y2 + MyBall.r))
		return 4;

	return 0;
}

void ShowGallery()
{
	SDL_Texture *GalleryBgPic = IMG_LoadTexture(Main_Renderer,"IMG\\gallery.png");

	char Draft[11][40] = {"��ʿ��Ϸ","�߻���HelloClyde","����HelloClyde","������HelloClyde","ͼƬ��������","Ů���ǣ�����������","�������֣�Τ�� �������� - ��������","��Ч��11��","ͼ�ο⣺SDL","����ƽ̨��VS2012","��л���ٶ�"};

	char TempString[100];

	int WhileQuit;

	int TextPosY;
	int i;
	int PicId;

	SDL_Event event;
	SDL_Texture *Mission_Pic;

	PicId = 1;
	TextPosY = 1080;
	WhileQuit = 0;

	while (!WhileQuit)
	{
		SDL_CopyTextureToRenderer(Main_Renderer,GalleryBgPic,1,0,0,0,0,0,0,0,0);

		strcpy(TempString,"IMG\\Mission\\00");
		strcat(TempString,IntToChar(PicId));
		strcat(TempString,".png");

		Mission_Pic = IMG_LoadTexture(Main_Renderer,TempString);

		SDL_CopyTextureToRenderer(Main_Renderer,Mission_Pic,1,0,0,0,0,1100,54,0,0);

		SDL_DestroyTexture(Mission_Pic);

		for (i = 0;i < 11;i ++)
		{
			SDL_ShowTTFtoRenderer(Main_Font,Main_Renderer,Draft[i],200,TextPosY + i * 200,0,0,0);
		}

		SDL_RenderPresent(Main_Renderer);

		TextPosY --;
		if (TextPosY < -2500)
			TextPosY = 1080;

		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				WhileQuit = 1;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN )
					WhileQuit = 1;
				else if (event.key.keysym.sym == SDLK_a && PicId > 1 && GameSaveFile.Mission[PicId - 2].IsUnlock == 1)
					PicId --;
				else if (event.key.keysym.sym == SDLK_d && PicId < 6 && GameSaveFile.Mission[PicId].IsUnlock == 1)
					PicId ++;
			}
		}
	}
	SDL_DestroyTexture(GalleryBgPic);
}