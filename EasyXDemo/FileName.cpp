#include<graphics.h>
#include<iostream>

int main() {
	initgraph(1280, 720);
	int x = 100, y = 100;

	BeginBatchDraw();

	while (true)
	{
		ExMessage msg;
		while (peekmessage(&msg))
		{
			if (msg.message == WM_MOUSEMOVE)
			{
				x = msg.x;
			    y = msg.y;
			}
		}

		cleardevice();
		solidcircle(x, y, 100);
		FlushBatchDraw();
	}

	EndBatchDraw();
	return 0;
}