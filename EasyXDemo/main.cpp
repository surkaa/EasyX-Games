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
	void ProcessEvent(const ExMessage& msg, bool& runing) {
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = true;
				break;
			case VK_DOWN:
				is_move_down = true;
				break;
			case VK_LEFT:
				is_move_left = true;
				break;
			case VK_RIGHT:
				is_move_right = true;
				break;
			}
			break;
		case WM_KEYUP:
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
	const int PLAYER_SPEED = 6;
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
		return b.loc.x >= loc.x && b.loc.x <= loc.x + FRAME_WIDTH 
			&& b.loc.y >= loc.y && b.loc.y <= loc.y + FRAME_HEIGHT;
	}
	bool CheckPlayerCollision(const Player& player)
	{
		const POINT& ploc = player.GetPosition();
		POINT check_loc = {
			loc.x + FRAME_WIDTH / 2,
			loc.y + FRAME_HEIGHT / 2,
		};
		return check_loc.x >= ploc.x && check_loc.x <= ploc.x + player.PLAYER_WIDTH
			&& check_loc.y >= ploc.y && check_loc.y <= ploc.y + player.PLAYER_HEIGHT;
	}
	void Move(const Player& player)
	{
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
			loc.x += (int)((dx / len) * SPEED);
			loc.y += (int)((dy / len) * SPEED);
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
	}
	void Hurt()
	{
		alive = false;
	}
	bool CheckAlive()
	{
		return alive;
	}
private:
	// 敌人移速
	const int SPEED = 2;
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
	bool alive = true;
};

// 绘制提示信息(FPS)
void DrawTipText(const int fps, const int score) {
	static TCHAR str[64];
	_stprintf_s(str, _T("得分: %d 帧率: %d"), score, fps);
	settextcolor(RGB(225, 175, 45));
	outtextxy(0, 0, str);
}

void TryGenerateEnemy(std::vector<Enemy*>& emenies)
{
	const int INTERVAL = 50;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0)
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
	double radian_interval = 2 * 3.14159 / bullets.size();
	POINT ploc = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
	for (size_t i = 0; i < bullets.size(); i++) {
		// 当前子弹所在的弧度值
		double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;
		bullets[i]->loc.x = ploc.x + player.PLAYER_WIDTH / 2 + (int)radius * sin(radian);
		bullets[i]->loc.y = ploc.y + player.PLAYER_HEIGHT / 2 + (int)radius * cos(radian);
	}
}
 
int main() {
	initgraph(WINDOWS_WIDTH, WINDOWS_HEIGHT);

	// 加载统一的资源
	atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);
	atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);
	atlas_enemy_left = new Atlas(_T("img/enemy_left_%d.png"), 6);
	atlas_enemy_right= new Atlas(_T("img/enemy_right_%d.png"), 6);

	// 设置随机数
	srand((unsigned int)time(NULL));

	// 居中
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height= GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(GetHWnd(), NULL, (screen_width - WINDOWS_WIDTH) / 2, (screen_height - WINDOWS_HEIGHT) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// 播放bgm
	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
	mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);

	bool runing = true;

	ExMessage msg;
	IMAGE background_img;
	Player player;
	std::vector<Enemy*> enemies;
	std::vector<Bullet*> bullets;
	bullets.push_back(new Bullet());
	bullets.push_back(new Bullet());
	bullets.push_back(new Bullet());
	bullets.push_back(new Bullet());

	loadimage(&background_img, _T("img/background.png"));

	BeginBatchDraw();

	while (runing)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			player.ProcessEvent(msg, runing);
		}

		player.Move();
		TryGenerateEnemy(enemies);
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
					player.score++;
				}
		// 移出死亡的敌人
		for (size_t i = 0; i < enemies.size(); i++)
		{
			Enemy* e = enemies[i];
			if (!e->CheckAlive())
			{
				std::swap(enemies[i], enemies.back());
				enemies.pop_back();
				delete e;
			}
		}

		cleardevice();

		putimage(0, 0, &background_img);

		player.Draw(SLEEP_TIME);
		for (Enemy* enemy : enemies)
			if (enemy->CheckAlive())
				enemy->Draw(SLEEP_TIME);
		for (Bullet* b : bullets)
			b->Draw();

		DWORD end_time = GetTickCount();
		DWORD delete_time = end_time - start_time;

		if (delete_time < SLEEP_TIME)
		{
			Sleep(SLEEP_TIME - delete_time);
			DrawTipText(TARGET_FPS, player.score);
		}
		else {
			DrawTipText(1000 / delete_time, player.score);
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