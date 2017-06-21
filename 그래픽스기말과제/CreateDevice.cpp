// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h> 
#include <iostream>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <fmod.h> //사운드

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 640 
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)



#define ENEMY_NUM 8
#define ENEMY_NUM2 10



// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma  comment( lib, "fmod_vc.lib")


// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface
LPD3DXFONT dxfont; // 폰트 오브젝트의 포인터


//-----------------------------------------------
// 타이머
//-----------------------------------------------

char SecondMessage[200]; // 버틴 시간 : n초 저장하는 문자열
char ScoreMessage[200]; // 게임 종료 시 스코어를 나타내는 문자열
int Time; // 시간 저장 변수
int TimeScore; // 스코어 저장 변수
clock_t CurTime; // 게임 시작 부터 흘러 간 시간
clock_t OldTime; // 게임 종료 부터 흘러 간 시간

		
 //-----------------------------------------------
 // 스프라이트
 //-----------------------------------------------					
				 // sprite declarations
LPDIRECT3DTEXTURE9 sprite;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_Start;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_hero;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_hero2;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy_R;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy_L;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy_U;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy_D;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_heart;    // the pointer to the sprite

//----------------------------------------------------
// FMOD 사운드 시스템 생성과 초기화
//----------------------------------------------------

FMOD_SYSTEM *g_System;
FMOD_SOUND *Sound;
FMOD_CHANNEL *Channel;
FMOD_CHANNELGROUP *g_ChannelGroup;



									 // function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory

void init_game(void);
void do_game_logic(void);
bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1);
void Stage_Change();
void Stage_Change2();

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;


//-----------------------------------------------
// 캐릭터 이동 관련
//-----------------------------------------------
enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };
bool Move_L; // 왼쪽, 오른쪽 움직일 때 이미지 바꿔주기 위해



//----------------------------------------------------
// 스테이지
//----------------------------------------------------
enum Game_State { INIT, RUNNING, SCORE };
Game_State GameState=INIT;

/*총 4개의 레벨디자인을 위해*/
int n_Stage;
int n_Stage2;

/* 화살이 리셋될 때 마다 n_Stage를 바꿔주기 위해 bool 변수를 하나 더 만들어줘야 했음*/
bool S; 
bool S2;


//기본 클래스 
class entity {

public:
	float x_pos;
	float y_pos;
	int status;
	int HP;

};


bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1)
{

	if ((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1) < (size0 + size1) * (size0 + size1))
		return true;
	else
		return false;

}



//주인공 클래스 
class Hero :public entity {

public:
	void fire();
	void super_fire();
	void move(int i);
	void init(float x, float y);
	bool check_collision(float x, float y);

	int N_Death;
	bool Heart1;
	bool Heart2;
	bool Heart3;

};

bool Hero::check_collision(float x, float y)
{
	if (sphere_collision_check(x_pos, y_pos, 15, x, y, 15) == true)
	{
	//Heart1=false;
	//
	//if (Heart1 == false)
	//	Heart2 = false;
	//else if (Heart2 == false)		
	//	Heart3 = false;
	//
	//if (Heart1 == false && Heart2 == false && Heart3 == false)
	//	GameState = SCORE;
	//
		return true;

	}
	else
		return false;
}
//bool Bullet::check_collision(float x, float y)
//{
//
//	//충돌 처리 시 
//	if (sphere_collision_check(x_pos, y_pos, 32, x, y, 32) == true)
//	{
//		bShow = false;
//		return true;
//
//	}
//	else {
//
//		return false;
//	}
//}

void Hero::init(float x, float y)
{

	x_pos = x;
	y_pos = y;

}

void Hero::move(int i)
{
	switch (i)
	{
	case MOVE_UP:
		y_pos -= 5;
		break;

	case MOVE_DOWN:
		y_pos += 5;
		break;


	case MOVE_LEFT:
		Move_L = true;
		x_pos -= 5;
		break;


	case MOVE_RIGHT:
		Move_L = false;
		x_pos += 5;
		break;

	}

}


// 화살 클래스 
class Enemy :public entity {

public:
	void fire();
	void init(float x, float y);
	void move();
	void move_up();
	void move_right();
	void move_left();
};

void Enemy::init(float x, float y)
{

	x_pos = x;
	y_pos = y;

}


void Enemy::move()
{
	y_pos += 3.5;

}


void Enemy::move_up()
{
	y_pos -= 3.5;
}

void Enemy::move_right()
{
	x_pos += 4.5;
}

void Enemy::move_left()
{
	
	x_pos -= 4.5;
}




//객체 생성 
Hero hero;
Enemy enemy[ENEMY_NUM2];
Enemy enemy_Down_S1[ENEMY_NUM2];
Enemy enemy_Right_S1[ENEMY_NUM];
Enemy enemy_Left_S1[ENEMY_NUM];




// this is the main message handler for the program


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;


	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	D3DXCreateSprite(d3ddev, &d3dspt);    // create the Direct3D Sprite object

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Start_Background.png",    // the file name
		800,    // default width
		640,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_Start);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Background2.png",    // the file name
		800,    // default width
		640,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite);    // load to sprite



	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"hero1.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_hero);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"hero2.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_hero2);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"enemy_R.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy_R);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"enemy_L.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy_L);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"enemy_U.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy_U);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"enemy_D.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy_D);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Heart.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_heart);    // load to sprite



	D3DXCreateFont(d3ddev, // d3d디바이스
		40, // 폰트 높이
		0, // 폰트 넓이
		FW_NORMAL, //폰트 굵기
		D3DX_DEFAULT, // MipLevel
		TRUE, // italic font
		DEFAULT_CHARSET, // default character set
		OUT_DEFAULT_PRECIS, 
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Arial",
		&dxfont);


	//-----------------------------------------------
	// FMOD사운드 초기화
	//-----------------------------------------------
	FMOD_System_Create(&g_System);
	FMOD_System_Init(g_System, 32, FMOD_INIT_NORMAL, NULL);
	FMOD_System_CreateSound(g_System, "Sound.mp3", FMOD_DEFAULT, 0, &Sound);
	


	return;
}


void init_game(void)
{
	
	
	TimeScore = Time;
	
	//객체 초기화 
	hero.init(150, 300);
	hero.Heart1 = true;
	hero.Heart2 = true;
	hero.Heart3 = true;
	hero.N_Death = 0;

	// 사운드
	
	FMOD_System_PlaySound(g_System, Sound, g_ChannelGroup, 0, &Channel);

	//적들 초기화 
	for (int i = 0; i<ENEMY_NUM; i++)
	{

		enemy_Right_S1[i].init(-64, (float)( 80 * i));
		enemy_Left_S1[i].init(800, (float)(80 * i));
	}

	for (int i = 0; i < ENEMY_NUM2; i++)
	{

		enemy[i].init((float)(10 + 80 * i), -64);
		enemy_Down_S1[i].init((float)(10 + 80 * i), 640);
	}




	S = false;
	S2 = false;

	n_Stage = 1;
	n_Stage2 = 1;



	// 왼쪽오른쪽 초기화
	Move_L = false;

}

//----------------------------------------------------------------
// Stage_Change()
// 총 4개의 레벨디자인을 위해 render_frame에서 렌더링을 구분해 주기 위해 n_Stage가 필요했음.
//----------------------------------------------------------------


/* 왼 오 화살의 레벨관리 */
void Stage_Change()
{
	n_Stage++;
	if (n_Stage == 5)
		n_Stage = 1;
}
/* 위 아래 화살의 레벨관리 */
void Stage_Change2()
{
	n_Stage2++;
	if (n_Stage2 == 5)
		n_Stage2 = 1;
}


void do_game_logic(void)
{
	
	
	
	if (GameState == RUNNING)
	{
		sprintf_s(SecondMessage, sizeof(SecondMessage), "버틴 시간 : %d 초", Time);
		
		//주인공 처리 
		if (KEY_DOWN(VK_UP))
			hero.move(MOVE_UP);

		if (KEY_DOWN(VK_DOWN))
			hero.move(MOVE_DOWN);

		if (KEY_DOWN(VK_LEFT))
			hero.move(MOVE_LEFT);

		if (KEY_DOWN(VK_RIGHT))
			hero.move(MOVE_RIGHT);


		//적들 처리 
		for (int i = 0; i < ENEMY_NUM2; i++)
		{
			if (enemy[i].y_pos > 640)
			{
				S2 = true;
				enemy[i].init((float)(10 + 80 * i), -64);

			}
			else
			{
				S2 = false;
				enemy[i].move();

			}


			if (enemy_Down_S1[i].y_pos < -64)
			{
				enemy_Down_S1[i].init((float)(10 + 80 * i), 640);

			}

			else
				enemy_Down_S1[i].move_up();
		}

		for (int i = 0; i < ENEMY_NUM; i++)
		{


			if (enemy_Right_S1[i].x_pos > 800)
			{
				S = true;
				enemy_Right_S1[i].init(-64, (float)(80 * i));

			}
			else
				S = false;
			enemy_Right_S1[i].move_right();


			if (enemy_Left_S1[i].x_pos < -64)
			{

				enemy_Left_S1[i].init(800, (float)(80 * i));
			}

			else
				enemy_Left_S1[i].move_left();

		}





		//충돌 처리 
		for (int i = 0; i < ENEMY_NUM; i++)
		{
			if (hero.check_collision(enemy[i].x_pos, enemy[i].y_pos) == true)
			{
				hero.init(150, 300);
				hero.N_Death++;

				if (hero.N_Death == 1)
					hero.Heart1 = false;
				if (hero.N_Death == 2)
					hero.Heart2 = false;
				if (hero.N_Death == 3)
					hero.Heart3 = false;

				if (hero.N_Death == 4 && hero.Heart1 == false && hero.Heart2 == false && hero.Heart3 == false)
				{
					hero.N_Death = 0;
					GameState = SCORE;
					
				}
			}
		}

	}
			
	if (GameState == SCORE)
	{
		sprintf_s(ScoreMessage, sizeof(ScoreMessage), "\nGame Over!!\n버틴 시간 : %d 초", TimeScore);
	}

}

// this is the function used to render a single frame
void render_frame(void)
{
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(207, 192,54), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency             
	
	//주인공 애니메이션 프레임
	static int frame = 0;
	if (KEY_DOWN(VK_LEFT)||(KEY_DOWN(VK_RIGHT))) frame += 1;
	if (frame == 4) frame = 0;
	
	int xpos = frame * 64;


	switch (GameState)
	{

	case INIT:
	{
		//배경
		RECT Back_Start;
		SetRect(&Back_Start, 0, 0, 800, 640);
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position(0, 0, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_Start, &Back_Start, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
		if (KEY_DOWN(VK_SPACE)) GameState = RUNNING;
		break;
	}
	case RUNNING:
	{
		//배경
		RECT Back;
		SetRect(&Back, 0, 0, 800, 640);
		D3DXVECTOR3 center0(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position0(0, 0, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite, &Back, &center0, &position0, D3DCOLOR_ARGB(255, 255, 255, 255));


		//타이머
		static RECT textbox;
		SetRect(&textbox, 0, 0, 820, 120); // create a RECT to contain the text
			// draw the Hello World text
		dxfont->DrawTextA(NULL,
			SecondMessage,
			-1,
			&textbox,
			DT_CENTER | DT_VCENTER,
			D3DCOLOR_ARGB(255, 0, 0, 0));


		//주인공 
		if (Move_L == false)
		{
			RECT part;
			SetRect(&part, xpos, 0, xpos + 65, 63);
			D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 position(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_hero, &part, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
		}

		else
		{
			RECT part;
			SetRect(&part, xpos, 0, xpos + 65, 63);
			D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 position(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_hero2, &part, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
		}

		//하트
		RECT Heart;
		SetRect(&Heart, 0, 0, 64, 64);
		D3DXVECTOR3 center_h(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		for (int i = 0; i < 3; i++)
		{
			D3DXVECTOR3 position_h(10+74*i, 10, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_heart, &Heart, &center_h, &position_h, D3DCOLOR_ARGB(255, 255, 255, 255));

		}



		////화살_U
		RECT part2;
		SetRect(&part2, 0, 0, 35, 64);
		D3DXVECTOR3 center2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner

		////화살_D
		RECT part3;
		SetRect(&part3, 0, 0, 35, 64);
		D3DXVECTOR3 center3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner

		////화살_R
		RECT part4;
		SetRect(&part4, 0, 0, 64, 35);
		D3DXVECTOR3 center4(0.0f, 0.0f, 0.0f);    // center at the upper-left corner

		////화살_L
		RECT part5;
		SetRect(&part5, 0, 0, 64, 35);
		D3DXVECTOR3 center5(0.0f, 0.0f, 0.0f);    // center at the upper-left corner



				if (n_Stage == 2)
				{
					for (int i = 0; i < ENEMY_NUM; i++)
					{

						D3DXVECTOR3 position4(enemy_Right_S1[i].x_pos, enemy_Right_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_R, &part4, &center4, &position4, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
					for (int i = 0; i < ENEMY_NUM; i += 2)
					{

						D3DXVECTOR3 position5(enemy_Left_S1[i].x_pos, enemy_Left_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_L, &part5, &center5, &position5, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}

				if (n_Stage == 4)
				{
					for (int i = 0; i < ENEMY_NUM; i++)
					{

						D3DXVECTOR3 position4(enemy_Right_S1[i].x_pos, enemy_Right_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_R, &part4, &center4, &position4, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
					for (int i = 0; i < ENEMY_NUM; i ++)
					{

						D3DXVECTOR3 position5(enemy_Left_S1[i].x_pos, enemy_Left_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_L, &part5, &center5, &position5, D3DCOLOR_ARGB(255, 255, 255, 255));
					}

				}

			
				
				if (n_Stage == 1)
				{
					for (int i = 0; i < ENEMY_NUM; i += 2)
					{

						D3DXVECTOR3 position4(enemy_Right_S1[i].x_pos, enemy_Right_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_R, &part4, &center4, &position4, D3DCOLOR_ARGB(255, 255, 255, 255));
					}


					for (int i = 0; i < ENEMY_NUM; i+=2)
					{

						D3DXVECTOR3 position5(enemy_Left_S1[i].x_pos, enemy_Left_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_L, &part5, &center5, &position5, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}

				if (n_Stage == 3)
				{
					for (int i = 0; i < ENEMY_NUM; i +=2)
					{

						D3DXVECTOR3 position4(enemy_Right_S1[i].x_pos, enemy_Right_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_R, &part4, &center4, &position4, D3DCOLOR_ARGB(255, 255, 255, 255));
					}


					for (int i = 0; i < ENEMY_NUM; i++)
					{

						D3DXVECTOR3 position5(enemy_Left_S1[i].x_pos, enemy_Left_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_L, &part5, &center5, &position5, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}

	
				if (n_Stage2 == 2)
				{

					for (int i = 0; i < ENEMY_NUM2; i++)
					{

						D3DXVECTOR3 position2(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_U, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
					}

					for (int i = 0; i < ENEMY_NUM2; i += 2)
					{

						D3DXVECTOR3 position3(enemy_Down_S1[i].x_pos, enemy_Down_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_D, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}

				if (n_Stage2 == 4)
				{
					for (int i = 0; i < ENEMY_NUM2; i++)
					{

						D3DXVECTOR3 position2(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_U, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
					}

					for (int i = 0; i < ENEMY_NUM2; i ++)
					{

						D3DXVECTOR3 position3(enemy_Down_S1[i].x_pos, enemy_Down_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_D, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}

		
				if (n_Stage2 == 1)
				{

					for (int i = 0; i < ENEMY_NUM2; i += 2)
					{

						D3DXVECTOR3 position2(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_U, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
					}


					for (int i = 0; i < ENEMY_NUM2; i+=2)
					{

						D3DXVECTOR3 position3(enemy_Down_S1[i].x_pos, enemy_Down_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_D, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}

				if (n_Stage2 == 3)
				{
					for (int i = 0; i < ENEMY_NUM2; i += 2)
					{

						D3DXVECTOR3 position2(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_U, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
					}


					for (int i = 0; i < ENEMY_NUM2; i ++)
					{

						D3DXVECTOR3 position3(enemy_Down_S1[i].x_pos, enemy_Down_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
						d3dspt->Draw(sprite_enemy_D, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}


		

		break;

	}

	case SCORE:
	{
		
		init_game();

		static RECT textbox2;
		SetRect(&textbox2, 0, 160, 790, 330); // create a RECT to contain the text
										   // draw the Hello World text
		D3DXVECTOR3 position_S(200, 200, 0.0f);
		dxfont->DrawTextA(NULL,
			ScoreMessage,
			-1,
			&textbox2,
			DT_CENTER | DT_VCENTER,
			D3DCOLOR_ARGB(255, 0, 0, 0));

		if (KEY_DOWN(VK_SPACE))
		{
			GameState = INIT;
			
		}
		break;

	}


	}
	


	d3dspt->End();    // end sprite drawing

	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;
}


//----------------------------------------------------------------
// 메인루프
//----------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);
	//400,100
	hWnd = CreateWindowEx(NULL, L"WindowClass", L"HJ'S DIRECTX GAME",
		WS_OVERLAPPEDWINDOW, 400, 100, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	initD3D(hWnd);


	//게임 오브젝트 초기화 
	init_game();

	// enter the main loop:

	MSG msg;

	while (TRUE)
	{

		DWORD starting_point = GetTickCount();


		/* 
		게임이 끝나고 다시 Replay했을 때 버틴 시간을 0초로 초기화 해주기 위해
		Curtime은 Running 스테이지 처음 시작했을 때 부터 clock가동,
		Oldtime은 처음 게임이 끝난 Score 스테이지 시작했을 때 부터 clock가동해서
		Time은 Curtime-Oldtime으로 해주면 리플레이 했을 때 0초가 된다. 
		*/
		if (GameState == SCORE)
		{
			OldTime = clock();
		}

		if (GameState == RUNNING)
		{
			CurTime = clock();
			Time = CurTime / 1000  - OldTime / 1000;
		}

		/* 사운드 계속 재생되게 */
		FMOD_System_Update(g_System);


		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		do_game_logic();

		render_frame();


		/*
		화살들이 범위를 초과해서 다시 초기화 될 때 마다 나오는 갯수를 바꿔주기 위해.
		*/
		if (S == true)
			Stage_Change();

		if (S2 == true)
			Stage_Change2();




		// check the 'escape' key
		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);


		while ((GetTickCount() - starting_point) < 25);
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}



// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{	
	
	dxfont->Release();
	sprite->Release();
	d3ddev->Release();
	d3d->Release();



	//객체 해제 
	FMOD_Sound_Release(Sound);
	FMOD_System_Close(g_System);
	FMOD_System_Release(g_System);
	sprite_Start->Release();
	sprite_hero->Release();
	sprite_hero2->Release();
	sprite_enemy_R->Release();
	sprite_enemy_L->Release();
	sprite_enemy_U->Release();
	sprite_enemy_D->Release();
	sprite_heart->Release();
	

	

	return;
}
