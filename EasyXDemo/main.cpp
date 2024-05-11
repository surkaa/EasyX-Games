﻿#include<graphics.h>
#include<iostream>
#include<string>
#include<vector>

#pragma comment(lib, "MSIMG32.LIB")

// 窗口大小
const int width = 1280;
const int height = 720;
// 目标帧率
const int TARGET_FPS = 60;
// 目标帧率下每帧应绘画的最大时间
const int SLEEP_TIME = 1000 / TARGET_FPS;
// 玩家位置
POINT player_loc = { width / 2, height / 2 };
// 玩家移动速度
const int PLAYER_SPEED = 8;
// 玩家高度
const int PLAYER_WIDTH = 80;
// 玩家宽度
const int PLAYER_HEIGHT = 80;
// 阴影宽度
const int SHADOW_WIDTH = 32;

inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(
		GetImageHDC(NULL),
		x,
		y,
		w,
		h,
		GetImageHDC(img),
		0,
		0,
		w,
		h,
		{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA }
	);
}

class Animation
{
public:
	Animation(LPCTSTR path, int num, int interval)
	{
		this->interval_ms = interval;
		this->timer = 0;
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);
			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			this->frame_list.push_back(frame);
		}
	}
	~Animation()
	{
		for (size_t i = 0; i < this->frame_list.size(); i++)
		{
			delete this->frame_list[i];
		}
	}

	void play(POINT loc, int delta)
	{
		this->timer += delta;
		if (this->timer > this->interval_ms)
		{
			this->index_frame = (this->index_frame + 1) % this->frame_list.size();
			this->timer = 0;
		}
		putimage_alpha(loc.x, loc.y, this->frame_list[this->index_frame]);
	}
private:
	int timer = 0;
	int index_frame = 0;
	int interval_ms = 0;
	std::vector<IMAGE*> frame_list;
};

Animation anim_player_left(_T("img/player_left_%d.png"), 6, 45);
Animation anim_player_right(_T("img/player_right_%d.png"), 6, 45);

void DrawPlayer(int delta, int dx, IMAGE* shadow)
{
	int loc_shadow_x = player_loc.x + (PLAYER_WIDTH - SHADOW_WIDTH) / 2;
	int loc_shadow_y = player_loc.y + PLAYER_HEIGHT - 12;
	putimage_alpha(loc_shadow_x, loc_shadow_y, shadow);

	static bool is_facing_left = false;
	if (dx < 0)
	{
		is_facing_left = true;
	}
	else if (dx > 0)
	{
		is_facing_left = false;
	}

	if (is_facing_left) {
		anim_player_left.play(player_loc, delta);
	}
	else {
		anim_player_right.play(player_loc, delta);
	}
}

// 绘制提示信息(FPS)
void DrawTipText(const int fps) {
	static TCHAR str[64];
	_stprintf_s(str, _T("FPS: %d"), fps);
	settextcolor(RGB(225, 175, 45));
	outtextxy(0, 0, str);
}

int main() {
	initgraph(width, height);
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height= GetSystemMetrics(SM_CYSCREEN);
	
	SetWindowPos(GetHWnd(), NULL, (screen_width - width) / 2, (screen_height - height) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	bool runing = true;

	ExMessage msg;
	IMAGE background_img;
	IMAGE shadow_img;
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

	loadimage(&background_img, _T("img/background.png"));
	loadimage(&shadow_img, _T("img/shadow_player.png"));

	BeginBatchDraw();

	while (runing)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = true;
					break;
				case VK_DOWN:
					is_move_down= true;
					break;
				case VK_LEFT:
					is_move_left= true;
					break;
				case VK_RIGHT:
					is_move_right= true;
					break;
				}
			}
			else if (msg.message == WM_KEYUP) {
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = false;
					break;
				case VK_DOWN:
					is_move_down = false;
					break;
				case VK_LEFT:
					is_move_left = false;
					break;
				case VK_RIGHT:
					is_move_right = false;
					break;
				case VK_ESCAPE:
					runing = false;
					break;
				}
			}
		}

		cleardevice();

		putimage(0, 0, &background_img);
		int dx = is_move_right - is_move_left;
		int dy = is_move_down - is_move_up;
		// 移动向量的模
		double len = sqrt(dx * dx + dy * dy);
		if (len != 0)
		{
			// 向量分量乘以移动速度为分量上的移动距离
			player_loc.x += (int)((dx / len) * PLAYER_SPEED);
			player_loc.y += (int)((dy / len) * PLAYER_SPEED);
		}
		DrawPlayer(SLEEP_TIME, dx, &shadow_img);

		DWORD end_time = GetTickCount();
		DWORD delete_time = end_time - start_time;

		if (delete_time < SLEEP_TIME)
		{
			Sleep(SLEEP_TIME - delete_time);
			DrawTipText(TARGET_FPS);
		}
		else {
			DrawTipText(1000 / delete_time);
		}
		FlushBatchDraw();
	}

	EndBatchDraw();
	return 0;
}