﻿// include the basic windows header files and the Direct3D header file
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

#define ENEMY_NUM 10  


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
int Time=0;
char g_strMessage[200]; // 게임 상태를 저장하는 문자열

						// sprite declarations
LPDIRECT3DTEXTURE9 sprite;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_Start;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_hero;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy_R;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy_L;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy_U;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy_D;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_bullet;    // the pointer to the sprite

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


// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;


enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };


//스테이지
enum Game_State { INIT, READY, RUNNING, SCORE };
Game_State g_GameState = INIT;
int g_nStage;


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


};

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
		x_pos -= 5;
		break;


	case MOVE_RIGHT:
		x_pos += 5;
		break;

	}

}




// 적 클래스 
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
	y_pos += 2;

}


void Enemy::move_up()
{
	y_pos -= 2;
}

void Enemy::move_right()
{
	x_pos += 2.5;
}

void Enemy::move_left()
{
	x_pos -= 2.5;
}



// 총알 클래스 
class Bullet :public entity {

public:
	bool bShow;

	void init(float x, float y);
	void move();
	bool show();
	void hide();
	void active();
	bool check_collision(float x, float y);


};


bool Bullet::check_collision(float x, float y)
{

	//충돌 처리 시 
	if (sphere_collision_check(x_pos, y_pos, 32, x, y, 32) == true)
	{
		bShow = false;
		return true;

	}
	else {

		return false;
	}
}




void Bullet::init(float x, float y)
{
	x_pos = x;
	y_pos = y;

}



bool Bullet::show()
{
	return bShow;

}


void Bullet::active()
{
	bShow = true;

}



void Bullet::move()
{
	y_pos += 6;
}

void Bullet::hide()
{
	bShow = false;

}

class Stage_ :public entity {

public:

	int Stage;
	void AddStage();
	int ShowStage();
};

void Stage_::AddStage()
{
	if (Stage <= 5)
		Stage++;
	else
		Stage = 1;
}

int Stage_ ::ShowStage()
{
	return Stage;
}




//객체 생성 
Hero hero;
Enemy enemy[ENEMY_NUM];
Enemy enemy_Down_S1[ENEMY_NUM];
Enemy enemy_Right_S1[ENEMY_NUM];
Enemy enemy_Left_S1[ENEMY_NUM];
Bullet bullet;
Stage_ S;

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

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"Our Direct3D Program",
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
		clock_t CurTime = clock();
		Time = CurTime / 1000;
		
		sprintf_s(g_strMessage, sizeof(g_strMessage),"버틴 시간 : %d 초",  Time);
		
		FMOD_System_Update(g_System);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		do_game_logic();
		S.Stage++;
		render_frame();

		// check the 'escape' key
		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);




		while ((GetTickCount() - starting_point) < 25);
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}


// this is the main message handler for the program
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
		L"Background.png",    // the file name
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
		L"bullet.png",    // the file name
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
		&sprite_bullet);    // load to sprite

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
	//객체 초기화 
	hero.init(150, 300);

	// 사운드
	
		FMOD_System_PlaySound(g_System, Sound, g_ChannelGroup, 0, &Channel);

	//적들 초기화 
	for (int i = 0; i<ENEMY_NUM; i++)
	{

		enemy[i].init((float)(10 + 60 * i), 0);
		enemy_Down_S1[i].init((float)(10 + 60 * i), 416);
		enemy_Right_S1[i].init(-64, (float)( 64 * i));
		enemy_Left_S1[i].init(800, (float)(64 * i));
	}

	//총알 초기화 
	bullet.bShow = true;
	bullet.init(enemy[3].x_pos, enemy[3].y_pos);


	//스테이지 초기화
	S.Stage = 1;

}


void do_game_logic(void)
{


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
	for (int i = 0; i<ENEMY_NUM; i++)
	{
		if (enemy[i].y_pos > 480)
		{	

			enemy[i].init((float)(10 + 60 * i), 0);
		}
		else
		

		enemy[i].move();


		if (enemy_Down_S1[i].y_pos < -64)
		{
			enemy_Down_S1[i].init((float)(10 + 60 * i), 416);
		}
		else
			enemy_Down_S1[i].move_up();

		if (enemy_Right_S1[i].x_pos > 800)
			enemy_Right_S1[i].init(-64, (float)(64 * i));
		else
			enemy_Right_S1[i].move_right();

		if (enemy_Left_S1[i].x_pos < -64)
			enemy_Left_S1[i].init(800, (float)(64 * i));
		else
			enemy_Left_S1[i].move_left();


	}

			

	//총알 처리 
		if (bullet.y_pos > 480)
			bullet.hide();
		else
			
			bullet.move();


//	//충돌 처리 
//	for (int i = 0; i<ENEMY_NUM; i++)
//	{
//		if (bullet.check_collision(enemy[i].x_pos, enemy[i].y_pos) == true)
//		{
//			enemy[i].init((float)(rand() % 300), rand() % 200 - 300);
//
//		}
//	}
	



}

// this is the function used to render a single frame
void render_frame(void)
{
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency
	
	//주인공 애니메이션 프레임
	static int frame = 0;
	if (KEY_DOWN(VK_LEFT)||(KEY_DOWN(VK_RIGHT))) frame += 1;
	if (frame == 4) frame = 0;
	
	int xpos = frame * 64;


	switch (g_GameState)
	{

	case INIT:
	{
		//배경
		RECT Back_Start;
		SetRect(&Back_Start, 0, 0, 800, 640);
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position(0, 0, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_Start, &Back_Start, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
		if (KEY_DOWN(VK_SPACE)) g_GameState = RUNNING;
		break;
	}
	case RUNNING:
	{//배경
		RECT Back;
		SetRect(&Back, 0, 0, 800, 640);
		D3DXVECTOR3 center0(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position0(0, 0, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite, &Back, &center0, &position0, D3DCOLOR_ARGB(255, 255, 255, 255));


		//타이머
		static RECT textbox;
		SetRect(&textbox, 0, 0, 720, 200); // create a RECT to contain the text
			// draw the Hello World text
		dxfont->DrawTextA(NULL,
			g_strMessage,
			-1,
			&textbox,
			DT_CENTER | DT_VCENTER,
			D3DCOLOR_ARGB(255, 0, 0, 0));


		//주인공 
		RECT part;
		SetRect(&part, xpos, 0, xpos + 65, 63);
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_hero, &part, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));


		////총알 
		if (bullet.bShow == true && S.Stage == 2)
		{
			RECT part1;
			SetRect(&part1, 0, 0, 64, 64);
			D3DXVECTOR3 center1(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 position1(bullet.x_pos, bullet.y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_bullet, &part1, &center1, &position1, D3DCOLOR_ARGB(255, 255, 255, 255));
		}


		////에네미 
		RECT part2;
		SetRect(&part2, 0, 0, 64, 64);
		D3DXVECTOR3 center2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		for (int i = 0; i < ENEMY_NUM; i++)
		{

			D3DXVECTOR3 position2(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_enemy_U, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
		RECT part3;
		SetRect(&part3, 0, 0, 64, 64);
		D3DXVECTOR3 center3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner

		for (int i = 0; i < ENEMY_NUM; i++)
		{

			D3DXVECTOR3 position3(enemy_Down_S1[i].x_pos, enemy_Down_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_enemy_D, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));
		}

		RECT part4;
		SetRect(&part4, 0, 0, 64, 64);
		D3DXVECTOR3 center4(0.0f, 0.0f, 0.0f);    // center at the upper-left corner

		for (int i = 0; i < ENEMY_NUM; i++)
		{

			D3DXVECTOR3 position4(enemy_Right_S1[i].x_pos, enemy_Right_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_enemy_R, &part4, &center4, &position4, D3DCOLOR_ARGB(255, 255, 255, 255));
		}


		RECT part5;
		SetRect(&part5, 0, 0, 64, 64);
		D3DXVECTOR3 center5(0.0f, 0.0f, 0.0f);    // center at the upper-left corner


		for (int i = 0; i < ENEMY_NUM; i++)
		{

			D3DXVECTOR3 position5(enemy_Left_S1[i].x_pos, enemy_Left_S1[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_enemy_L, &part5, &center5, &position5, D3DCOLOR_ARGB(255, 255, 255, 255));
		}




		break;
	}
	}
	

										
	
	





	d3dspt->End();    // end sprite drawing

	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;
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
	sprite_enemy_R->Release();
	sprite_enemy_L->Release();
	sprite_enemy_U->Release();
	sprite_enemy_D->Release();

	sprite_bullet->Release();
	

	return;
}
