#include<graphics.h>
#include<iostream>

/* �������� */
void DrawBoard() {
	line(200, 0, 200, 600);
	line(400, 0, 400, 600);
	line(0, 200, 600, 200);
	line(0, 400, 600, 400);
}

/* �������� */
void DrawPiece() {

}

// ������ʾ��Ϣ
void DrawTipText() {

}

// ����Ƿ��ʤ
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
			MessageBox(GetHWnd(), _T("X ��һ�ʤ"), _T("��Ϸ����"), MB_OK);
			runing = false;
		}
		else if (CheckWin('O')) {
			MessageBox(GetHWnd(), _T("O ��һ�ʤ"), _T("��Ϸ����"), MB_OK);
			runing = false;
		}
		else if (CheckDraw())
		{
			MessageBox(GetHWnd(), _T("ƽ��"), _T("��Ϸ����"), MB_OK);
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