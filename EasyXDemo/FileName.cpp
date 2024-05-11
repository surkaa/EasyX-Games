#include<graphics.h>
#include<iostream>

/* 绘制棋盘 */
void DrawBoard() {
	line(200, 0, 200, 600);
	line(400, 0, 400, 600);
	line(0, 200, 600, 200);
	line(0, 400, 600, 400);
}

/* 绘制棋子 */
void DrawPiece() {

}

// 绘制提示信息
void DrawTipText() {

}

// 检查是否获胜
bool CheckWin(char c) {

}

bool CheckDraw() {

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

		if (CheckWin('X')) {
			MessageBox(GetHWnd(), _T("X 玩家获胜"), _T("游戏结束"), MB_OK);
			runing = false;
		}
		else if (CheckWin('O')) {
			MessageBox(GetHWnd(), _T("O 玩家获胜"), _T("游戏结束"), MB_OK);
			runing = false;
		}
		else if (CheckDraw())
		{
			MessageBox(GetHWnd(), _T("平局"), _T("游戏结束"), MB_OK);
			runing = false;
		}

		cleardevice();
		DrawBoard();
		DrawPiece();
		DrawTipText();
		FlushBatchDraw();
	}

	EndBatchDraw();
	return 0;
}