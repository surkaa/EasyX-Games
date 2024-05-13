#include<graphics.h>
#include<iostream>
#include<string>
#include<vector>
#define PI 3.14159265358979323846

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "MSIMG32.LIB")

// 窗口大小
const int WINDOWS_WIDTH = 1280;
const int WINDOWS_HEIGHT = 720;
// 按钮大小
const int BUTTON_WIDTH = 192;
const int BUTTON_HEIGHT = 75;
// 目标帧率
const int TARGET_FPS = 60;
// 目标帧率下每帧应绘画的最大时间
const int SLEEP_TIME = 1000 / TARGET_FPS;

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

class Atlas
{
public:
	Atlas(LPCTSTR path, int num)
	{
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}
	~Atlas()
	{
		for (size_t i = 0; i < frame_list.size(); i++)
			delete frame_list[i];
	}
public:std::vector<IMAGE*> frame_list;
};

Atlas* atlas_player_left;
Atlas* atlas_player_right;
Atlas* atlas_enemy_left;
Atlas* atlas_enemy_right;

class Animation
{
public:
	Animation(Atlas* atlas, int interval)
	{
		this->anim_atlas = atlas;
		this->interval_ms = interval;
	}
	~Animation() = default;

	void play(POINT loc, int delta)
	{
		this->timer += delta;
		if (this->timer > this->interval_ms)
		{
			this->index_frame = (this->index_frame + 1) % this->anim_atlas->frame_list.size();
			this->timer = 0;
		}
		putimage_alpha(loc.x, loc.y, this->anim_atlas->frame_list[this->index_frame]);
	}
private:
	int timer = 0;
	int index_frame = 0;
	int interval_ms = 0;
	Atlas* anim_atlas;
};

class Player
{
public:
	// 玩家高度
	const int PLAYER_WIDTH = 80;
	// 玩家宽度
	const int PLAYER_HEIGHT = 80;
	// 得分
	int score = 0;
public:
	Player() {
		loadimage(&shadow_img, _T("img/shadow_player.png"));
		anim_left = new Animation(atlas_player_left, 45);
		anim_right = new Animation(atlas_player_right, 45);
	}
	~Player() {
		delete anim_left;
		delete anim_right;
	}
	// 处理事件
	void ProcessEvent(const ExMessage& msg, bool& runing, bool& is_puase) {
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
			case VK_UP:
			case 0x57:
				is_move_up = true;
				break;
			case VK_DOWN:
			case 0x53:
				is_move_down = true;
				break;
			case VK_LEFT:
			case 0x41:
				is_move_left = true;
				break;
			case VK_RIGHT:
			case 0x44:
				is_move_right = true;
				break;
			}
			break;
		case WM_KEYUP:
			switch (msg.vkcode)
			{
			case VK_UP:
			case 0x57:
				is_move_up = false;
				break;
			case VK_DOWN:
			case 0x53:
				is_move_down = false;
				break;
			case VK_LEFT:
			case 0x41:
				is_move_left = false;
				break;
			case VK_RIGHT:
			case 0x44:
				is_move_right = false;
				break;
			case VK_ESCAPE:
				runing = false;
				break;
			case VK_SPACE:
				is_puase = !is_puase;
				break;
			}
			break;
		}
	}
	// 处理移动
	void Move() {
		int dx = is_move_right - is_move_left;
		int dy = is_move_down - is_move_up;
		// 移动向量的模
		double len = sqrt(dx * dx + dy * dy);
		if (len != 0)
		{
			// 向量分量乘以移动速度为分量上的移动距离
			player_loc.x += (int)((dx / len) * PLAYER_SPEED);
			player_loc.y += (int)((dy / len) * PLAYER_SPEED);
			if (player_loc.x > WINDOWS_WIDTH - PLAYER_WIDTH)
			{
				player_loc.x = WINDOWS_WIDTH - PLAYER_WIDTH;
			}
			if (player_loc.x < 0)
			{
				player_loc.x = 0;
			}
			if (player_loc.y < 0)
			{
				player_loc.y = 0;
			}
			if (player_loc.y > WINDOWS_HEIGHT - PLAYER_HEIGHT)
			{
				player_loc.y = WINDOWS_HEIGHT - PLAYER_HEIGHT;
			}
		}
	}
	// 处理绘画
	void Draw(int delta)
	{
		int loc_shadow_x = player_loc.x + (PLAYER_WIDTH - SHADOW_WIDTH) / 2;
		int loc_shadow_y = player_loc.y + PLAYER_HEIGHT - 12;
		putimage_alpha(loc_shadow_x, loc_shadow_y, &shadow_img);

		static bool is_facing_left = false;
		int dx = is_move_right - is_move_left;
		if (dx < 0)
		{
			is_facing_left = true;
		}
		else if (dx > 0)
		{
			is_facing_left = false;
		}

		if (is_facing_left) {
			anim_left->play(player_loc, delta);
		}
		else {
			anim_right->play(player_loc, delta);
		}
	}
	const POINT& GetPosition() const
	{
		return player_loc;
	}
private:
	// 玩家移动速度
	const int PLAYER_SPEED = 5;
	// 阴影宽度
	const int SHADOW_WIDTH = 32;
private:
	IMAGE shadow_img;
	Animation* anim_left;
	Animation* anim_right;
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
	// 玩家位置
	POINT player_loc = { WINDOWS_WIDTH / 2, WINDOWS_HEIGHT / 2 };
};

class Bullet
{
public:
	POINT loc = { 0, 0 };
public:
	Bullet() = default;
	~Bullet() = default;
	void Draw() const
	{
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(loc.x, loc.y, RADIUS);
	}

private:
	const int RADIUS = 10;
};

class Enemy
{
public:
	Enemy() {
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));
		anim_left = new Animation(atlas_enemy_left, 45);
		anim_right = new Animation(atlas_enemy_right, 45);
		speed = rand() % 3 + 1;
		boold = rand() % 15 + 10;
		score = (boold + speed) / 10;

		// 敌人生成位置的枚举
		enum SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};

		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge)
		{
		case SpawnEdge::Up:
			loc.x = rand() % WINDOWS_WIDTH;
			loc.y = -FRAME_WIDTH;
			break;
		case SpawnEdge::Down:
			loc.x = rand() % WINDOWS_WIDTH;
			loc.y = WINDOWS_HEIGHT;
			break;
		case SpawnEdge::Left:
			loc.x = -FRAME_WIDTH;
			loc.y = rand() % WINDOWS_HEIGHT;
			break;
		case SpawnEdge::Right:
			loc.x = WINDOWS_WIDTH;
			loc.y = rand() % WINDOWS_HEIGHT;
			break;
		}
	}
	~Enemy() {
		delete anim_left;
		delete anim_right;
	}
	bool CheckBulletCollision(const Bullet& b)
	{
		return b.loc.x >= loc.x + (FRAME_WIDTH / 8) && b.loc.x <= loc.x + 7 * (FRAME_WIDTH / 8)
			&& b.loc.y >= loc.y + (FRAME_HEIGHT / 8) && b.loc.y <= loc.y + 7 * (FRAME_HEIGHT / 8);
	}
	bool CheckPlayerCollision(const Player& player)
	{
		const POINT& ploc = player.GetPosition();
		POINT check_loc = {
			loc.x + FRAME_WIDTH / 2,
			loc.y + FRAME_HEIGHT / 2,
		};
		return check_loc.x >= (ploc.x + player.PLAYER_WIDTH / 8) && check_loc.x <= ploc.x + 7 * player.PLAYER_WIDTH / 8
			&& check_loc.y >= (ploc.y + player.PLAYER_HEIGHT / 8) && check_loc.y <= ploc.y + 7 * player.PLAYER_HEIGHT / 8;
	}
	void Move(const Player& player)
	{
		// TODO 敌人有时候会突然停止
		const POINT& player_loc = player.GetPosition();
		int dx = player_loc.x - loc.x;
		if (dx > 0)
		{
			is_facing_left = false;
		}
		else if (dx < 0) {
			is_facing_left = true;
		}
		int dy = player_loc.y - loc.y;
		// 移动向量的模
		double len = sqrt(dx * dx + dy * dy);
		if (len != 0)
		{
			// 向量分量乘以移动速度为分量上的移动距离
			loc.x += (int)((dx / len) * speed);
			loc.y += (int)((dy / len) * speed);
		}
	}
	void Draw(int delta)
	{
		int loc_shadow_x = loc.x + (FRAME_WIDTH - SHADOW_WIDTH) / 2;
		int loc_shadow_y = loc.y + FRAME_HEIGHT - 35; 
		putimage_alpha(loc_shadow_x, loc_shadow_y, &img_shadow);

		if (is_facing_left) {
			anim_left->play(loc, delta);
		}
		else {
			anim_right->play(loc, delta);
		}
		static TCHAR str[4];
		_stprintf_s(str, _T("%d"), this->boold);
		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 83, 83));
		outtextxy(loc.x + 5, loc.y, str);
	}
	void Hurt()
	{
		boold--;
		if (boold < 0)
		{
			boold = 0;
		}
	}
	int GetBoold()
	{
		return boold;
	}
	int GetScore()
	{
		return score;
	}
private:
	// 敌人宽度
	const int FRAME_WIDTH = 80;
	// 敌人高度
	const int FRAME_HEIGHT = 80;
	// 阴影宽度
	const int SHADOW_WIDTH = 48;
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT loc = { 0, 0 };
	bool is_facing_left = false;
	bool is_move_left = false;
	bool is_move_right = false;
	int boold;
	int score;
	// 敌人移速
	int speed;
};

class Button
{
public:
	Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed) {
		region = rect;

		loadimage(&img_idle, path_img_idle);
		loadimage(&img_hovered, path_img_hovered);
		loadimage(&img_pushed, path_img_pushed);
	}
	~Button() = default;
	void Draw() {
		switch (status)
		{
		case Button::Status::Idle:
			putimage(region.left, region.top, &img_idle);
			break;
		case Button::Status::Hovered:
			putimage(region.left, region.top, &img_hovered);
			break;
		case Button::Status::Pushed:
			putimage(region.left, region.top, &img_pushed);
			break;
		default:
			break;
		}
	}
	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
				status = Status::Hovered;
			else if (status == Status::Hovered && !CheckCursorHit(msg.x, msg.y))
				status = Status::Idle;
			break;
		case WM_LBUTTONDOWN:
			if (CheckCursorHit(msg.x, msg.y))
				status = Status::Pushed;
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed)
				OnClick();
			break;
		default:
			break;
		}
	}

protected:
	virtual void OnClick() = 0;

private:
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
	enum class Status
	{
		Idle = 0,
		Hovered,
		Pushed
	};
	Status status = Status::Idle;
	bool CheckCursorHit(int x, int y)
	{
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
};

class StartGameButton : public Button
{
public:
	StartGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed, bool* igs)
		: Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {
		is_game_started = igs;
	}
	~StartGameButton() = default;
protected:
	void OnClick()
	{
		*is_game_started = true;
		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}
private:
	bool* is_game_started;
};
class QuitGameButton : public Button
{
public:
	QuitGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed, bool* r)
		: Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {
		running = r;
	}
	~QuitGameButton() = default;
protected:
	void OnClick()
	{
		*running = false;
	}
private:
	bool* running;
};

// 绘制提示信息(FPS)
void DrawTipText(const int fps, const int score, const int interval) {
	static TCHAR str[64];
	_stprintf_s(str, _T("得分: %d 帧率: %d 敌人生成速度: %d"), score, fps, interval);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(225, 175, 45));
	outtextxy(10, 10, str);
}

void DrawHelpText() {
	static TCHAR str[128];
	_stprintf_s(str, _T("游戏帮助:\n1. 你需要控制角色不被敌人吃掉你的脑子\n2. 控制角色上下左右分别对应wsad按键或者方向键\n3. 空格可以暂停/恢复游戏\n制作者: 杨伟宁"));
	setbkmode(TRANSPARENT);
	settextcolor(RGB(225, 175, 45));
	// 分割字符串
	TCHAR* next_line;
	TCHAR* token = _tcstok_s(str, _T("\n"), &next_line);
	int y = 10; // 初始Y坐标

	// 逐行输出
	while (token != NULL) {
		outtextxy(10, y, token);
		y += textheight(token) + 5; // 5是行间距
		token = _tcstok_s(NULL, _T("\n"), &next_line);
	}
}

void TryGenerateEnemy(std::vector<Enemy*>& emenies, int interval)
{
	static int counter = 0;
	if ((++counter) % interval == 0)
	{
		emenies.push_back(new Enemy());
		counter = 0;
	}
}

void UpdateBullets(std::vector<Bullet*>& bullets, const Player& player) {
	// 径向波动速度
	const double RADIAL_SPEED = 0.0045;
	// 切向波动速度
	const double TANGENT_SPEED = 0.0055;
	// 子弹之间的弧度间隔
	double radian_interval = 2 * PI / bullets.size();
	POINT ploc = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
	for (size_t i = 0; i < bullets.size(); i++) {
		// 当前子弹所在的弧度值
		double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;
		bullets[i]->loc.x = ploc.x + player.PLAYER_WIDTH / 2 + (int)radius * sin(radian);
		bullets[i]->loc.y = ploc.y + player.PLAYER_HEIGHT / 2 + (int)radius * cos(radian);
	}
}

int MyMain() {
	initgraph(WINDOWS_WIDTH, WINDOWS_HEIGHT);

	// 游戏是否已经开始
	bool igs = false;
	bool is_puase = false;
	bool runing = true;

	// 加载统一的资源
	atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);
	atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);
	atlas_enemy_left = new Atlas(_T("img/enemy_left_%d.png"), 6);
	atlas_enemy_right = new Atlas(_T("img/enemy_right_%d.png"), 6);

	// 设置随机数
	srand((unsigned int)time(NULL));

	// 居中
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(GetHWnd(), NULL, (screen_width - WINDOWS_WIDTH) / 2, (screen_height - WINDOWS_HEIGHT) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// 播放bgm
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);

	ExMessage msg;
	IMAGE img_menu;
	IMAGE background_img;
	Player player;
	std::vector<Enemy*> enemies;
	std::vector<Bullet*> bullets;
	bullets.push_back(new Bullet());
	bullets.push_back(new Bullet());
	bullets.push_back(new Bullet());

	RECT region_btn_start_game, region_btn_quit_game;

	region_btn_start_game.left = (WINDOWS_WIDTH - BUTTON_WIDTH) / 2;
	region_btn_start_game.right = region_btn_start_game.left + BUTTON_WIDTH;
	region_btn_start_game.top = 430;
	region_btn_start_game.bottom = region_btn_start_game.top + BUTTON_HEIGHT;

	region_btn_quit_game.left = (WINDOWS_WIDTH - BUTTON_WIDTH) / 2;
	region_btn_quit_game.right = region_btn_quit_game.left + BUTTON_WIDTH;
	region_btn_quit_game.top = 550;
	region_btn_quit_game.bottom = region_btn_quit_game.top + BUTTON_HEIGHT;

	StartGameButton btn_start_game = StartGameButton(
		region_btn_start_game,
		_T("img/ui_start_idle.png"),
		_T("img/ui_start_hovered.png"),
		_T("img/ui_start_pushed.png"),
		&igs
	);
	QuitGameButton btn_quit_game = QuitGameButton(
		region_btn_quit_game,
		_T("img/ui_quit_idle.png"),
		_T("img/ui_quit_hovered.png"),
		_T("img/ui_quit_pushed.png"),
		&runing
	);

	loadimage(&img_menu, _T("img/menu.png"));
	loadimage(&background_img, _T("img/background.png"));

	BeginBatchDraw();

	while (runing)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			if (igs)
			{
				player.ProcessEvent(msg, runing, is_puase);
			}
			else
			{
				btn_start_game.ProcessEvent(msg);
				btn_quit_game.ProcessEvent(msg);
			}
		}
		if (is_puase)
		{
			continue;
		}

		int interval = 10 * (pow(1.01, -player.score) * 5 + 1);

		if (igs)
		{
			player.Move();
			TryGenerateEnemy(enemies, interval);
			for (Enemy* enemy : enemies)
				enemy->Move(player);
			UpdateBullets(bullets, player);

			for (Enemy* enemy : enemies) {
				if (enemy->CheckPlayerCollision(player))
				{
					MessageBox(GetHWnd(), _T("你的脑子被吃了!"), _T("游戏失败"), MB_OK);
					runing = false;
					break;
				}
			}
			for (Enemy* e : enemies)
				for (const Bullet* b : bullets)
					if (e->CheckBulletCollision(*b))
					{
						e->Hurt();
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
					}
			// 移出死亡的敌人
			for (size_t i = 0; i < enemies.size(); i++)
			{
				Enemy* e = enemies[i];
				if (e->GetBoold() == 0)
				{
					player.score += e->GetScore();
					static bool s3 = false;
					static bool s10 = false;
					static bool s20 = false;
					static bool s50 = false;
					static bool s100 = false;
					static bool s200 = false;
					if (player.score > 3 && !s3)
					{
						bullets.push_back(new Bullet());
						s3 = true;
					}
					else if (player.score > 10 && !s10)
					{
						bullets.push_back(new Bullet());
						s10 = true;
					}
					else if (player.score > 20 && !s20)
					{
						bullets.push_back(new Bullet());
						s20 = true;
					}
					else if (player.score > 50 && !s50)
					{
						bullets.push_back(new Bullet());
						s50 = true;
					}
					else if (player.score > 100 && !s100)
					{
						bullets.push_back(new Bullet());
						s100 = true;
					}
					else if (player.score > 200 && !s200)
					{
						bullets.push_back(new Bullet());
						s200 = true;
					}
					std::swap(enemies[i], enemies.back());
					enemies.pop_back();
					delete e;
				}
			}
		}

		cleardevice();

		if (igs)
		{
			putimage(0, 0, &background_img);

			player.Draw(SLEEP_TIME);
			for (Enemy* enemy : enemies)
				if (enemy->GetBoold() > 0)
					enemy->Draw(SLEEP_TIME);
			for (Bullet* b : bullets)
				b->Draw();

			DWORD end_time = GetTickCount();
			DWORD delete_time = end_time - start_time;

			static int fps_timeer = 0;
			static int fps = TARGET_FPS;

			if (delete_time < SLEEP_TIME)
			{
				Sleep(SLEEP_TIME - delete_time);
				fps_timeer += SLEEP_TIME;
				if (fps_timeer > 800)
				{
					fps = TARGET_FPS;
					fps_timeer = 0;
				}
			}
			else {
				fps_timeer += delete_time;
				if (fps_timeer > 800)
				{
					fps = 1000 / delete_time;
					fps_timeer = 0;
				}
			}
			DrawTipText(fps, player.score, interval);
		}
		else
		{
			putimage(0, 0, &img_menu);
			btn_start_game.Draw();
			btn_quit_game.Draw();
			DrawHelpText();
		}
		FlushBatchDraw();
	}

	EndBatchDraw();

	delete atlas_player_left;
	delete atlas_player_right;
	delete atlas_enemy_right;
	delete atlas_enemy_left;

	return 0;
}

int main() {
	return MyMain();
}

//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
//	return MyMain();
//}