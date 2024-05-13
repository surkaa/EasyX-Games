#include<graphics.h>
#include<iostream>

char board_data[3][3] = {
	{ '-', '-', '-' },
	{ '-', '-', '-' },
	{ '-', '-', '-' },
};

char current_piece = 'O';

int gay = 4;

int target_fps = 60;

int sleep_time = 1000 / target_fps;

/* �������� */
void DrawBoard() {
	line(200, 0, 200, 600);
	line(400, 0, 400, 600);
	line(0, 200, 600, 200);
	line(0, 400, 600, 400);
}

/* �������� */
void DrawPiece() {

	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++) {
			if (board_data[i][j] == 'X')
			{
				line(gay + 200 * i, gay + 200 * j, 200 - gay + 200 * i, 200 - gay + 200 * j);
				line(gay + 200 * i, 200 - gay + 200 * j, 200 - gay + 200 * i, gay + 200 * j);
			}
			else if (board_data[i][j] == 'O') {
				circle(100 + 200 * i, 100 + 200 * j, 100 - gay);
			}
		}
	}
}

// ������ʾ��Ϣ
void DrawTipText(const int fps) {
	static TCHAR str[64];
	_stprintf_s(str, _T("��ǰ��������Ϊ: %c fps: %d"), current_piece, fps);
	settextcolor(RGB(225, 175, 45));
	outtextxy(0, 0, str);
}

// ����Ƿ��ʤ
bool CheckWin(char c) {
	if (board_data[0][0] == c && board_data[0][1] == c && board_data[0][2] == c)
		return true;
	if (board_data[1][0] == c && board_data[1][1] == c && board_data[1][2] == c)
		return true;
	if (board_data[2][0] == c && board_data[2][1] == c && board_data[2][2] == c)
		return true;
	if (board_data[0][0] == c && board_data[1][0] == c && board_data[2][0] == c)
		return true;
	if (board_data[0][1] == c && board_data[1][1] == c && board_data[2][1] == c)
		return true;
	if (board_data[0][2] == c && board_data[1][2] == c && board_data[2][2] == c)
		return true;
	if (board_data[0][0] == c && board_data[1][1] == c && board_data[2][2] == c)
		return true;
	if (board_data[2][0] == c && board_data[1][1] == c && board_data[0][2] == c)
		return true;
	return false;
}

// ����Ƿ�ƽ��
bool CheckDraw() {
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++) {
			if (board_data[i][j] == '-')
			{
				return false;
			}
		}
	}
	return true;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	initgraph(600, 600);

	bool runing = true;

	ExMessage msg;

	BeginBatchDraw();

	while (runing)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				int x = msg.x / 200;
				int y = msg.y / 200;
				if (board_data[x][y] == '-')
				{
					board_data[x][y] = current_piece;
					if (current_piece == 'X')
					{
						current_piece = 'O';
					}
					else if (current_piece == 'O') {
						current_piece = 'X';
					}
				}
			}
		}

		cleardevice();
		DrawBoard();
		DrawPiece();

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
	}

	EndBatchDraw();
	return 0;
}