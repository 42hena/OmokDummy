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
	static int FindOmokPattern(int board[][BOARD_SIZE], int x, int y, int type, bool opp)
	{
		int cnt[6][2] = { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
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
			if (sameCnt > 5)
			{
				sameCnt = 5;
			}

			cnt[sameCnt][close]++;
		}

		if (opp)
			return GetDefenseWeight(cnt) + posWeight[y][x];
		else
			return GetAttackWeight(cnt) + posWeight[y][x];
	}
	static int GetAttackWeight(int cnt[6][2])
	{
		if (cnt[5][0] || cnt[5][1])	// 5
			return 1'000'000;
		else if (cnt[4][0] >= 2)	// 44
			return 250'000;
		else if (cnt[4][0] && cnt[3][0]) // 43
			return 100'000;
		else if (cnt[3][0] >= 2)		// 33
			return 60'000;
		else if (cnt[4][0])				// 4
		{
			if (cnt[4][1])				// 4
				return 12000;
			if (cnt[3][1])				// 3
				return 10000;
		}
		else if (cnt[4][1])				// close 4
		{
			if (cnt[4][1] > 1)				// 4
				return 1500;
			if (cnt[3][1])				// 3
				return 1000;
		}
		else if (cnt[3][0])
		{
			if (cnt[4][1])				// 4
				return 900;
			if (cnt[3][1])				// 3
				return 850;
		}
		else if (cnt[3][1])
		{
			if (cnt[4][1] > 1)				// 4
				return 840;
			if (cnt[3][1] > 0)				// 3
				return 830;
			if (cnt[2][0] || cnt[2][1])
				return 820;
		}
		else if (cnt[2][0])
		{
			if (cnt[2][0] > 1)
				return 500;
			if (cnt[2][1])
				return 450;
			return 400;
		}
		return 100;
	}

	static int GetDefenseWeight(int cnt[6][2])
	{
		if (cnt[5][0] || cnt[5][1])	// 5
			return 500'000;
		else if (cnt[4][0] >= 2)	// 44
			return 125'000;
		else if (cnt[4][0] && cnt[3][0]) // 43
			return 90'000;
		else if (cnt[3][0] >= 2)		// 33
			return 30'000;
		else if (cnt[4][0])				// 4
		{
			if (cnt[4][1])				// 4
				return 11500;
			if (cnt[3][1])				// 3
				return 9500;
		}
		else if (cnt[4][1])				// close 4
		{
			if (cnt[4][1] > 1)				// 4
				return 1450;
			if (cnt[3][1])				// 3
				return 950;
		}
		else if (cnt[3][0])
		{
			if (cnt[4][1])				// 4
				return 890;
			if (cnt[3][1])				// 3
				return 840;
		}
		else if (cnt[3][1])
		{
			if (cnt[4][1] > 1)				// 4
				return 835;
			if (cnt[3][1] > 0)				// 3
				return 825;
			if (cnt[2][0] || cnt[2][1])
				return 815;
		}
		else if (cnt[2][0])
		{
			if (cnt[2][0] > 1)
				return 470;
			if (cnt[2][1])
				return 420;
			return 380;
		}
		return 50;
	}

	// static member variable
	static int posWeight[15][15];
};
