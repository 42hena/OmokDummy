#pragma once

const int dy[] = { 1, 1, 1, 0 };
const int dx[] = { -1, 0, 1, 1 };

class CSimpleAI
{
public:
	enum MyEnum
	{
		BOARD_SIZE = 15
	};

	// static member function
	static inline bool IsValidPosition(int x, int y) { return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE; }
	static inline bool isMoveValid(int board[][BOARD_SIZE], int x, int y) { return (IsValidPosition(x, y)) && board[y][x] == 0; }
	static int FindOmokPattern(int board[][BOARD_SIZE], int x, int y, int type)
	{
		int cnt[5][2] = { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
		int total = 0;

		// 4방향 검색.
		for (int dir = 0; dir < 4; ++dir)
		{
			int sameCnt = 1;
			int close = 0;
			int open = 0;

			// + direction
			for (int len = 1; len <= 4; len++)
			{
				int nx = x + dx[dir] * len;
				int ny = y + dy[dir] * len;
				if (isMoveValid(board, nx, ny))
				{
					if (type == board[ny][nx])
						sameCnt++;
					else
					{
						// 공백
						if (board[ny][nx] == 0)
						{
							open++;
						}
						else
						{
							close++;
						}
						break;
					}
				}
				else
				{
					close++;
					break;
				}
			}

			// - direction
			for (int len = 1; len <= 4; len++)
			{
				int nx = x - dx[dir] * len;
				int ny = y - dy[dir] * len;
				if (isMoveValid(board, nx, ny))
				{
					if (type == board[nx][ny])
						sameCnt++;
					else
					{
						if (board[nx][ny] == 0)
							open++;
						else
							close++;
						break;
					}
				}
				else
				{
					close++;
					break;
				}
			}

			// 이미 막혔으면 0점.
			if (sameCnt <= 4 && close == 2)
				continue;

			// 이김.6목도 이기는 판정.
			if (sameCnt >= 5)
			{
				total += 1e6;
			}

			cnt[sameCnt][close]++;
		}

		return GetWeight(cnt) + posWeight[y][x];
	}
	static int GetWeight(int cnt[5][2])
	{
		if (cnt[4][0] >= 2)
			return 1800;
		else if (cnt[4][1] >= 1)
		{
			if (cnt[3][1])
				return 1700; // 4 3
			return 1600;// 열린 4
		}
		else if (cnt[4][0] >= 1)
		{
			if (cnt[3][1])
				return 1500;
			else if (cnt[3][0])
				return 1300;
			return 1000;
		}
		else if (cnt[3][1] >= 2)
		{
			return 900;
		}
		else if (cnt[3][1] >= 1)
		{
			if (cnt[2][1])
				return 420;
			return 400;
		}
		else if (cnt[3][0] >= 1)
		{
			return 60;
		}
		else if (cnt[2][1] >= 4)
		{
			return 450;
		}
		else if (cnt[2][1] >= 3)
		{
			return 430;
		}
		else if (cnt[2][1] >= 2)
		{
			return 60;
		}
		else if (cnt[2][1] >= 1)
		{
			return 40;
		}
		else if (cnt[2][0] >= 1)
		{
			return 30;
		}
		return 0;
	}

	// static member variable
	static int posWeight[15][15];
};
