#include<graphics.h>
#include<iostream>
#include<string>

#pragma comment(lib, "MSIMG32.LIB")

// 目标帧率
const int TARGET_FPS = 60;
// 目标帧率下每帧应绘画的最大时间
const int SLEEP_TIME = 1000 / TARGET_FPS;
// 当前关键帧动画的索引
int index_current_amin = 0;
// 关键帧动画的总数
const int PLAYER_AMIN_COUNT = 6;
// 玩家关键帧图片
IMAGE img_player_left[PLAYER_AMIN_COUNT];
IMAGE img_player_right[PLAYER_AMIN_COUNT];
// 玩家位置
POINT player_loc = { 640, 360 };
// 玩家移动速度
const int PLAYER_SPEED = 4;

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
		{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA}
	);
}

void LoadAnimation() {
	for (size_t i = 0; i < PLAYER_AMIN_COUNT; i++)
	{
		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());
	}
	for (size_t i = 0; i < PLAYER_AMIN_COUNT; i++)
	{
		std::wstring path = L"img/player_right_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_right[i], path.c_str());
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
	initgraph(1280, 720);

	bool runing = true;

	ExMessage msg;
	IMAGE background_img;
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

	LoadAnimation();
	loadimage(&background_img, _T("img/background.png"));

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

		if (is_move_up)
		{
			player_loc.y -= PLAYER_SPEED;
		}

		if (is_move_down)
		{
			player_loc.y += PLAYER_SPEED;
		}

		if (is_move_left)
		{
			player_loc.x -= PLAYER_SPEED;
		}

		if (is_move_right)
		{
			player_loc.x += PLAYER_SPEED;
		}

		static int counter = 0;
		if (++counter % 5 == 0) {
			index_current_amin++;
		}

		// 每五帧刷新玩家关键帧
		index_current_amin %= PLAYER_AMIN_COUNT;

		cleardevice();

		putimage(0, 0, &background_img);
		putimage_alpha(player_loc.x, player_loc.y, &img_player_left[index_current_amin]);

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