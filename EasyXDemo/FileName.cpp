#include<graphics.h>
#include<iostream>

/* ªÊ÷∆∆Â≈Ã */
void DrawBoard() {
	line(200, 0, 200, 600);
	line(400, 0, 400, 600);
	line(0, 200, 600, 200);
	line(0, 400, 600, 400);
}

/* ªÊ÷∆∆Â◊” */
void DrawPiece() {

}

int main() {
	initgraph(600, 600);

	bool runing = true;

	ExMessage msg;

	BeginBatchDraw();

	while (runing)
	{
		while (peekmessage(&msg))
		{
		}



		cleardevice();
		DrawBoard();
		FlushBatchDraw();
	}

	EndBatchDraw();
	return 0;
}