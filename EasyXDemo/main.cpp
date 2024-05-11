#include<graphics.h>
#include<iostream>

// 目标帧率
int target_fps = 60;
// 目标帧率下每帧应绘画的最大时间
int sleep_time = 1000 / target_fps;
// 当前关键帧动画的索引
int index_current_amin = 0;
// 关键帧动画的总数
const int PLAYER_AMIN_COUNT = 6;
IMAGE img_player_left[PLAYER_AMIN_COUNT];
IMAGE img_player_right[PLAYER_AMIN_COUNT];

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
	loadimage(&background_img, _T("img/background.png"));

	BeginBatchDraw();

	while (runing)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
			}
		}

		cleardevice();

		putimage(0, 0, &background_img);

		DWORD end_time = GetTickCount();
		DWORD delete_time = end_time - start_time;
		if (delete_time < sleep_time)
		{
			Sleep(sleep_time - delete_time);
			DrawTipText(target_fps);
		}
		else {
			DrawTipText(1000 / delete_time);
		}
		FlushBatchDraw();
	}

	EndBatchDraw();
	return 0;
}