#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include <queue>
using namespace std;

const int INF = 99999999;
const int ThreatScore = 9999999;
const int MAXDEPTH = 7;
const int n = 15;
const int EMPTY = 0;
const int OutBoard = 3;
const int limitTime = 900;
const int HashSize = 1024 * 1024;
int Threat[n][n];
const int FourThreat = 4, ThreeHoleThreat = 3, ThreeThreat = 2, ThreeBlockThreat = 1;
const int TSize = 5;
int ThreatNum[2][TSize];
int startTime;
int board[n][n];
double PointBoard[n][n];
int FourBlockCount[n][n];
int ThreeCount[n][n];

unsigned long HashKey32 = 0;
unsigned long long HashKey64 = 0;
unsigned long HashTable32[2][n][n];
unsigned long long HashTable64[2][n][n];

const double PosWeight[n][n]=
{
  {0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86},
  {0.86,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.86},
  {0.86,0.88,0.90,0.90,0.90,0.90,0.90,0.90,0.90,0.90,0.90,0.90,0.90,0.88,0.86},
  {0.86,0.88,0.90,0.92,0.92,0.92,0.92,0.92,0.92,0.92,0.92,0.92,0.90,0.88,0.86},
  {0.86,0.88,0.90,0.92,0.94,0.94,0.94,0.94,0.94,0.94,0.94,0.92,0.90,0.88,0.86},
  {0.86,0.88,0.90,0.92,0.94,0.96,0.96,0.96,0.96,0.96,0.94,0.92,0.90,0.88,0.86},
  {0.86,0.88,0.90,0.92,0.94,0.96,0.98,0.98,0.98,0.96,0.94,0.92,0.90,0.88,0.86},
  {0.86,0.88,0.90,0.92,0.94,0.96,0.98,1.00,0.98,0.96,0.94,0.92,0.90,0.88,0.86},
  {0.86,0.88,0.90,0.92,0.94,0.96,0.98,0.98,0.98,0.96,0.94,0.92,0.90,0.88,0.86},
  {0.86,0.88,0.90,0.92,0.94,0.96,0.96,0.96,0.96,0.96,0.94,0.92,0.90,0.88,0.86},
  {0.86,0.88,0.90,0.92,0.94,0.94,0.94,0.94,0.94,0.94,0.94,0.92,0.90,0.88,0.86},
  {0.86,0.88,0.90,0.92,0.92,0.92,0.92,0.92,0.92,0.92,0.92,0.92,0.90,0.88,0.86},
  {0.86,0.88,0.90,0.90,0.90,0.90,0.90,0.90,0.90,0.90,0.90,0.90,0.90,0.88,0.86},
  {0.86,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.88,0.86},
  {0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86,0.86}
};

enum HASH_TYPE {hashEXACT, hashLOWER, hashUPPER, hashEMPTY};

struct HashItem
{
  long long checksum;
  HASH_TYPE key_type;
  int depth;
  double eval;

	HashItem()
	{
		key_type = hashEMPTY;
	}
};
HashItem *hash[2];

struct axis
{
  int x, y;
};

axis FourBlockPos[2];

struct PointType
{
	//int depth;
  double val;
  axis stone;
};

void CalThreat(int index);

int ThreatFind(int OPlayer, int MPlayer);

bool cmp(const PointType &lhs, const PointType &rhs);

void CalculateInitHashKey();

void Hash_MakeMove(axis stone);

void HashTableAdd(HASH_TYPE key_type, double eval, int depth,int index);

bool LookUpHashTable(int depth, double alpha, double beta, int index, double &value);

void InitializeHashKey();

unsigned long long rand64();

unsigned long rand32();

bool checkLine(axis stone, int xa, int ya, int xb, int yb);

bool GameOver(axis stone, int num);

bool TimeIsOver();

void UpdateStone(axis &leftStone, axis &rightStone, axis stone);

int pos(int x, int y);

double evaluate(int type, double weight, axis leftStone, axis rightStone);

vector<axis> BestMoves(axis leftStone, axis rightStone, int OPlayer, int MPlayer, int retSize);

PointType MaxNode(const PointType &a, const PointType &b);

PointType MaxValue(int depth, double alpha, double beta, int OPlayer, int MPlayer, axis leftStone, axis rightStone, axis lastStone);

PointType MinNode(const PointType &a, const PointType &b);

PointType MinValue(int depth, double alpha, double beta, int OPlayer, int MPlayer, axis leftStone, axis rightStone, axis lastStone);

axis AlphaBeta(int depth, int OPlayer, int MPlayer, axis leftStone, axis rightStone);

PointType EvalBoard(int OPlayer, int MPlayer, const axis &leftStone, const axis &rightStone, int type);

int main()
{
  char op[10];
  int MPlayer, OPlayer;
  int x, y;
  axis leftStone, rightStone;
  leftStone.x = -1;
	memset(board, 0, sizeof(board));

  InitializeHashKey();

	scanf("%s", op);
	if (strcmp(op, "[START]") == 0)
	{
		scanf("%d", &MPlayer);
		if (MPlayer == 1)
			OPlayer = 2;
		else
			OPlayer = 1;
	}

  while(scanf("%s", op))
  {
    if (strcmp(op, "[PUT]") == 0)
    {
			scanf("%d%d", &x, &y);
      if (x == -1 && y == -1)
      {
        board[7][7] = MPlayer;
				printf("7 7\n");
				fflush(stdout);
        axis stone;
        stone.x = stone.y = 7;
        UpdateStone(leftStone, rightStone, stone);
      }
      else
      {
        axis stone;
        stone.x = x;
        stone.y = y;
        board[x][y] = OPlayer;
        UpdateStone(leftStone, rightStone, stone);

        startTime = clock();

        axis move = AlphaBeta(MAXDEPTH, OPlayer, MPlayer, leftStone, rightStone);

				printf("%d %d\n", move.x, move.y);
				fflush(stdout);

        board[move.x][move.y] = MPlayer;

        UpdateStone(leftStone, rightStone, move);
      }
    }
  }

	/*
	//test code for board.txt
	MPlayer = 2;
	OPlayer = 1;

	int testNum;
	cin >> testNum;
	char c;
	for(int t = 0; t < testNum; t++)
	{
		vector<axis> st;
		for(int i = 0; i < n; i++)
			for(int j = 0; j < n; j++)
			{
				cin >> c;
				if (c == '*')
				{
					board[i][j] = OPlayer;
					axis stone;
					stone.x = i;
					stone.y = j;
					UpdateStone(leftStone, rightStone, stone);
				}
				else if (c == 'o')
				{
					board[i][j] = MPlayer;
					axis stone;
					stone.x = i;
					stone.y = j;
					UpdateStone(leftStone, rightStone, stone);
					st.push_back(stone);
				}
				else
					board[i][j] = EMPTY;
			}
		//axis ret = AlphaBeta(MAXDEPTH, OPlayer, MPlayer, leftStone, rightStone);
		//cout << ret.x << ' ' << ret.y << endl;
		cout << "testCase:" << t << endl;
		for(int i = 0; i < st.size(); i++)
			cout << "x:" << st[i].x << " y:" << st[i].y << ' ' << GameOver(st[i], 4) << endl;
	}
	*/

  return 0;
}

axis AlphaBeta(int depth, int OPlayer, int MPlayer, axis leftStone, axis rightStone)
{
  double alpha = LLONG_MIN;
  double beta = LLONG_MAX;

  axis lastStone;
  lastStone.x = lastStone.y = -1;

  CalculateInitHashKey();

  PointType ret = MaxValue(depth, alpha, beta, OPlayer, MPlayer, leftStone, rightStone, lastStone);

  return ret.stone;
}

PointType MinNode(const PointType &a, const PointType &b)
{
  if (a.val < b.val)
    return a;
  return b;
}

PointType MinValue(int depth, double alpha, double beta, int OPlayer, int MPlayer, axis leftStone, axis rightStone, axis lastStone)
{
  if (GameOver(lastStone, 5))
  {
    PointType ret;

    ret.val = INF;

    return ret;
  }

  double value;

  if (LookUpHashTable(depth, alpha, beta, depth % 2, value))
  {
    PointType ret;

    ret.val = value;

    return ret;
  }

  if (depth == 0 || clock() - startTime > limitTime)
		return EvalBoard(OPlayer, MPlayer, leftStone, rightStone, 0);

  const int retSize = 7;
	const int eps = 1;
  vector<axis> stone(BestMoves(leftStone, rightStone, OPlayer, MPlayer, retSize));
  PointType node;
  node.val = LLONG_MAX;
  node.stone.x = stone[0].x;
  node.stone.y = stone[0].y;

	if (depth != MAXDEPTH)
	{
		int TF = ThreatFind(MPlayer % 2, OPlayer % 2);

		if (TF == -1)
		{
			node.val = -ThreatScore;
			HashTableAdd(hashEXACT, -ThreatScore, depth, depth % 2);
			return node;
		}
		else if (TF == 1)
		{
			node.val = ThreatScore;
			HashTableAdd(hashEXACT, ThreatScore, depth, depth % 2);
			return node;
		}
	}

  bool eval_is_exact = false;

  for(vector<axis>::const_iterator iter = stone.begin(); iter != stone.end(); iter++)
  {
    board[iter->x][iter->y] = MPlayer;

    axis ls = leftStone;
    axis rs = rightStone;
    UpdateStone(leftStone, rightStone, *iter);

    Hash_MakeMove(*iter);

    PointType ret = MaxValue(depth - 1, alpha, beta, MPlayer, OPlayer, leftStone, rightStone, *iter);

    if (ret.val < node.val)
    {
      node.val = ret.val;
      node.stone.x = iter->x;
      node.stone.y = iter->y;
    }

    board[iter->x][iter->y] = 0;

    leftStone = ls;
    rightStone = rs;

    Hash_MakeMove(*iter);

		if (node.val <= alpha)
    {
      HashTableAdd(hashUPPER, alpha, depth, depth % 2);
      return node;
    }

    if (node.val < beta)
    {
      beta = node.val;
      eval_is_exact = true;
    }
  }

  if (eval_is_exact)
    HashTableAdd(hashEXACT, beta, depth, depth % 2);
  else
    HashTableAdd(hashLOWER, beta, depth, depth % 2);

  return node;
}

PointType MaxNode(const PointType &a, const PointType &b)
{
  if (a.val > b.val)
    return a;
  return b;
}

PointType MaxValue(int depth, double alpha, double beta, int OPlayer, int MPlayer, axis leftStone, axis rightStone, axis lastStone)
{
  if (GameOver(lastStone, 5))
  {
    PointType ret;

    ret.val = -INF;

    return ret;
  }

  double value;

  if (LookUpHashTable(depth, alpha, beta, depth % 2, value))
  {
    PointType ret;

    ret.val = value;

    return ret;
  }

  if (depth == 0 || clock() - startTime > limitTime)
		return EvalBoard(OPlayer, MPlayer, leftStone, rightStone, 1);

  const int retSize = 7;
	const int eps = 1;
  vector<axis> stone(BestMoves(leftStone, rightStone, OPlayer, MPlayer, retSize));
  PointType node;
  node.val = LLONG_MIN;
  node.stone.x = stone[0].x;
  node.stone.y = stone[0].y;

	if (depth != MAXDEPTH)
	{
		int TF = ThreatFind(MPlayer % 2, OPlayer % 2);

		if (TF == -1)
		{
			node.val = ThreatScore;
			HashTableAdd(hashEXACT, ThreatScore, depth, depth % 2);
			return node;
		}
		else if (TF == 1)
		{
			node.val = -ThreatScore;
			HashTableAdd(hashEXACT, -ThreatScore, depth, depth % 2);
			return node;
		}
	}

  bool eval_is_exact = false;

  for(vector<axis>::const_iterator iter = stone.begin(); iter != stone.end(); iter++)
  {
    board[iter->x][iter->y] = MPlayer;

    axis ls = leftStone;
    axis rs = rightStone;
    UpdateStone(leftStone, rightStone, *iter);

    Hash_MakeMove(*iter);

    PointType ret = MinValue(depth - 1, alpha, beta, MPlayer, OPlayer, leftStone, rightStone, *iter);

    if (node.val < ret.val)
    {
      node.val = ret.val;
      node.stone.x = iter->x;
      node.stone.y = iter->y;
    }
		/*
		else if (node.val > 0 && node.depth < ret.depth && abs(node.val - ret.val) < eps)
		{
			node.stone.x = iter->x;
			node.stone.y = iter->y;
			node.depth = ret.depth;
		}
		*/

    board[iter->x][iter->y] = 0;

    leftStone = ls;
    rightStone = rs;

    Hash_MakeMove(*iter);

		if (node.val >= beta)
    {
      HashTableAdd(hashLOWER, beta, depth, depth % 2);
      return node;
    }

    if (node.val > alpha)
    {
      alpha = node.val;
      eval_is_exact = true;
    }
  }

  if (eval_is_exact)
    HashTableAdd(hashEXACT, alpha, depth, depth % 2);
  else
    HashTableAdd(hashUPPER, alpha, depth, depth % 2);

  return node;
}

void UpdateStone(axis &leftStone, axis &rightStone, axis stone)
{
  if (leftStone.x == -1)
  {
    leftStone.x = stone.x;
    leftStone.y = stone.y;
    rightStone = leftStone;
  }
  else
  {
    if (stone.x < leftStone.x)
      leftStone.x = stone.x;
    if (stone.y < leftStone.y)
      leftStone.y = stone.y;
    if (stone.x > rightStone.x)
      rightStone.x = stone.x;
    if (stone.y > rightStone.y)
      rightStone.y = stone.y;
  }
}

int pos(int x, int y)
{
  if (x < 0 || y < 0 || x >= n || y >= n)
		return OutBoard;
  return (board[x][y]);
}

double evaluate(int type, double weight, axis leftStone, axis rightStone)
{
  /*
  # = EMPTY
  _ = CURRENT SQUARE
  X = OWN MARKS
  O = OPP MARKS
  * == X or #
  */

  const double FIVE = 999999;
  const double FOUR = 9999;
  const double THREE = 500;
  const double THREEHOLE = 450;
  const double FOURBLOCK = 450;
  const double FOURBLOCKHOLE = 400;
  const double TWO = 50;
  const double TWOHOLE = 45;
  const double THREEBLOCK = 40;
  const double TWOBLOCK = 3;
	const double FourBlockDouble = 1000;
	const double ThreeFourBlock = 800;
	const double ThreeDouble = 600;

  int mx[8] = { 1, -1, 0, 0, 1, -1, 1, -1 };
	int my[8] = { 0, 0, 1, -1, 1, 1, -1, -1 };
  double points = 0;

	memset(FourBlockCount, 0, sizeof(FourBlockCount));
	memset(ThreeCount, 0, sizeof(ThreeCount));
	memset(Threat, 0, sizeof(Threat));

	int player = type % 2;

  for (int x = max(0, leftStone.x - 2); x <= min(n - 1, rightStone.x + 2); x++)
    for (int y = max(0, leftStone.y - 2); y <= min(n - 1, rightStone.y + 2); y++)
    {
      if (pos(x, y) == EMPTY)
        for (int i = 0; i < 8; i++)
        {
          if (pos(x + 1 * mx[i], y + 1 * my[i]) == type)
          {
            if (pos(x + 2 * mx[i], y + 2 * my[i]) == type)
            {
              if (pos(x + 3 * mx[i], y + 3 * my[i]) == type)
              {
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // _XXXX XXXX_
                {
                  points += FIVE;
                  PointBoard[x][y] += FIVE * weight;

									Threat[x][y] = max(Threat[x][y], FourThreat);
									FourBlockPos[player].x = x;
									FourBlockPos[player].y = y;
                  continue;
                }
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY)
                {
                  if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) // X_XXX#    #XXX_X
                  {
                    points += FIVE;
                    PointBoard[x][y] += FIVE * weight;

										Threat[x][y] = max(Threat[x][y], FourThreat);
										FourBlockPos[player].x = x;
										FourBlockPos[player].y = y;
                    continue;
                  }
                  if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_XXX#   #XXX_#
                  {
                    points += FOUR;
                    PointBoard[x][y] += FOUR * weight;

										Threat[x][y] = max(Threat[x][y], ThreeThreat);
                    continue;
                  }
                  // O_XXX#   #XXX_O
                  FourBlockCount[x][y]++;
									double p = (FOURBLOCK + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

                  PointBoard[x][y] += p * weight;
                  points += p;

									if (pos(x + 5 * mx[i], y + 5 * my[i]) == EMPTY)
										Threat[x][y] = max(Threat[x][y], ThreeThreat);
                  continue;
                }
                // pos(x + 4 * mx[i], y + 4 * my[i]) = fi || edge
                if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) // X_XXXO   OXXX_X
                {
                  points += FIVE;
                  PointBoard[x][y] += FIVE * weight;

									Threat[x][y] = max(Threat[x][y], FourThreat);
									FourBlockPos[player].x = x;
									FourBlockPos[player].y = y;
                  continue;
                }
                if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_XXXO   OXXX_#
                {
                  FourBlockCount[x][y]++;
                  double p = (FOURBLOCK + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

                  PointBoard[x][y] += p * weight;
                  points += p;

									Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                  continue;
                }
                // O_XXXO     OXXX_O
                continue;
              }
              // ---------------------------- pos(x + 3 * mx[i], y + 3 * my[i]) != type
              if (pos(x + 3 * mx[i], y + 3 * my[i]) == EMPTY)
              {
                if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_XX#    #XX_#
                {
									ThreeCount[x][y]++;
									double p = (THREE + ThreeDouble * (ThreeCount[x][y] - 1) + 
											ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

                  PointBoard[x][y] += p * weight;
                  points += p;
                  continue;
                }
                if (pos(x - 1 * mx[i], y - 1 * my[i]) == type)
                {
                  if (pos(x - 2 * mx[i], y - 2 * my[i]) == EMPTY) // #X_XX#   #XX_X#
                  {
                    points += FOUR;
                    PointBoard[x][y] += FOUR * weight;

										Threat[x][y] = max(Threat[x][y], ThreeHoleThreat);
                    continue;
                  }
                  if (pos(x - 2 * mx[i], y - 2 * my[i]) == type) // XX_XX#    #XX_XX   Dual
                  {
                    points += FIVE;
                    PointBoard[x][y] += FIVE * weight;

										Threat[x][y] = max(Threat[x][y], FourThreat);
										FourBlockPos[player].x = x;
										FourBlockPos[player].y = y;
                    continue;
                  }
                  // OX_XX#    #XX_XO
                  FourBlockCount[x][y]++;
									double p = (FOURBLOCK + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

                  PointBoard[x][y] += p * weight;
                  points += p;
									
									Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                  continue;
                }
                // pos(x - 1 * mx[i], y - 1 * my[i]) == fi || edge
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // O_XX##    ##XX_O
                {
                  points += THREEBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += THREEBLOCK * weight * PosWeight[x][y];
                  continue;
                }
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // O_XX#X   X#XX_O
                {
                  FourBlockCount[x][y]++;
									double p = (FOURBLOCK + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

                  PointBoard[x][y] += p * weight;
                  points += p;

									Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                  continue;
                }
                // O_XX#O   O#XX_O
                continue;
              }

              // pos(x + 3 * mx[i], y + 3 * my[i]) == fi || edge
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY)
              {
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == EMPTY) // ##_XXO    OXX_##
                {
                  points += THREEBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += THREEBLOCK * weight * PosWeight[x][y];
                  continue;
                }
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == type) // X#_XXO  OXX_#X
                {
                  FourBlockCount[x][y]++;
									double p = (FOURBLOCK + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

                  PointBoard[x][y] += p * weight;
                  points += p;

									Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                  continue;
                }
                // O#_XXO    OXX_#O
                continue;
              }
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) // X_XXO   OXX_X
              {
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == EMPTY) // #X_XXO  OXX_X#
                {
                  FourBlockCount[x][y]++;
									double p = (FOURBLOCK + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

                  PointBoard[x][y] += p * weight;
                  points += p;

									Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                  continue;
                }
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == type) // XX_XXO  OXX_XX   Dual
                {
                  points += FIVE;
                  PointBoard[x][y] += FIVE * weight;

									Threat[x][y] = max(Threat[x][y], FourThreat);
									FourBlockPos[player].x = x;
									FourBlockPos[player].y = y;
                  continue;
                }
                // OX_XXO    OXX_XO
                continue;
              }
              // O_XXO    OXX_O
              continue;
            }
            // --------------------- pos(x + 2 * mx[i], y + 2 * my[i]) != type
            if (pos(x + 2 * mx[i], y + 2 * my[i]) == EMPTY) // _X#
            {
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_X#
              {
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == type) // X#_X#
                {
                  if (pos(x - 3 * mx[i], y - 3 * my[i]) == EMPTY) // #X#_X#
                  {
										ThreeCount[x][y]++;
										double p = (THREEHOLE + ThreeDouble * (ThreeCount[x][y] - 1) + 
												ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

										PointBoard[x][y] += p * weight;
                    points += p;
                    continue;
                  }
                  if (pos(x - 3 * mx[i], y - 3 * my[i]) == type) // XX#_X#
                  {
                    FourBlockCount[x][y]++;
										double p = (FOURBLOCK + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
											ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

										PointBoard[x][y] += p * weight;
                    points += p;

										Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                    continue;
                  }
                  points += THREEBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += THREEBLOCK * weight * PosWeight[x][y];
                  // OX#_X#
                  continue;
                }
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == EMPTY) // ##_X#
                {
                  if (pos(x + 3 * mx[i], y + 3 * my[i]) == EMPTY) // ##_X##
                  {
                    points += TWO * PosWeight[x][y];
                    PointBoard[x][y] += TWO * weight * PosWeight[x][y];
                    continue;
                  }
                  if (pos(x + 3 * mx[i], y + 3 * my[i]) == type) // ##_X#X
                  {
                    if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // ##_X#X#
                    {
											ThreeCount[x][y]++;
											double p = (THREEHOLE + ThreeDouble * (ThreeCount[x][y] - 1) + 
													ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

											PointBoard[x][y] += p * weight;
                      points += p;
                      continue;
                    }
                    if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // ##_X#XX
                    {
                      FourBlockCount[x][y]++;
											double p = (FOURBLOCK + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
												ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

											PointBoard[x][y] += p * weight;
                      points += p;

											Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                      continue;
                    }
                    // ##_X#XO
                    points += THREEBLOCK * PosWeight[x][y];
                    PointBoard[x][y] += THREEBLOCK * weight * PosWeight[x][y];
                    continue;
                  }
                  // ##_X#O
                  points += TWO * PosWeight[x][y];
                  PointBoard[x][y] += TWO * weight * PosWeight[x][y];
                  continue;
                }
                // O#_X#
                if (pos(x + 3 * mx[i], y + 3 * my[i]) == EMPTY) // O#_X##
                {
                  points += TWO * PosWeight[x][y];
                  PointBoard[x][y] += TWO * weight * PosWeight[x][y];
                  continue;
                }
                if (pos(x + 3 * mx[i], y + 3 * my[i]) == type)
                {
                  if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // O#_X#X#
                  {
										ThreeCount[x][y]++;
										double p = (THREEHOLE + ThreeDouble * (ThreeCount[x][y] - 1) + 
												ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

										PointBoard[x][y] += p * weight;
                    points += p;
                    continue;
                  }
                  if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // O#_X#XX
                  {
                    FourBlockCount[x][y]++;
										double p = (FOURBLOCK + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
											ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

										PointBoard[x][y] += p * weight;
                    points += p;

										Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                    continue;
                  }
                  // O#_X#XO
                  points += THREEBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += THREEBLOCK * weight * PosWeight[x][y];
                  continue;
                }
                // O#_X#O
                continue;
              }
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == type)
              {
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == type) // XX_X#
                  continue; // Already processed
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == EMPTY) // #X_X# // Dual
                {
                  points += THREE / 2 * PosWeight[x][y];
                  PointBoard[x][y] += THREE * weight / 2 * PosWeight[x][y];
                  continue;
                }
                if (pos(x + 3 * mx[i], y + 3 * my[i]) == EMPTY) // OX_X##
                {
                  points += THREEBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += THREEBLOCK * weight * PosWeight[x][y];
                  continue;
                }
                if (pos(x + 3 * mx[i], y + 3 * my[i]) == type) // OX_X#X
                {
									FourBlockCount[x][y]++;
									double p = (FOURBLOCKHOLE + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

									PointBoard[x][y] += p * weight;
                  points += p;

									Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                  continue;
                }
                // OX_X#O
                continue;
              }
              if (pos(x + 3 * mx[i], y + 3 * my[i]) == EMPTY)
              {
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // O_X###
                {
                  points += TWOBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += TWOBLOCK * weight * PosWeight[x][y];
                  continue;
                }
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // O_X##X
                {
                  points += TWOBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += TWOBLOCK * weight * PosWeight[x][y];
                  continue;
                }
                // O_X##O
                continue;
              }
              if (pos(x + 3 * mx[i], y + 3 * my[i]) == type)
              {
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // O_X#X#
                {
                  points += THREEBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += THREEBLOCK * weight * PosWeight[x][y];
                  continue;
                }
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // O_X#XX
                {
                  FourBlockCount[x][y]++;
									double p = (FOURBLOCKHOLE + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

									PointBoard[x][y] += p * weight;
                  points += p;

									Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                  continue;
                }
                // O_X#XO
                continue;
              }
              // O_X#O
              continue;
            }
            // pos(x + 2 * mx[i], y + 2 * my[i]) == fi || edge
            if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_XO
            {
              if (pos(x - 2 * mx[i], y - 2 * my[i]) == type)
              {
                if (pos(x - 3 * mx[i], y - 3 * my[i]) == EMPTY) // #X#_XO
                {
                  points += THREEBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += THREEBLOCK * weight * PosWeight[x][y];
                  continue;
                }
                if (pos(x - 3 * mx[i], y - 3 * my[i]) == type) // XX#_XO
                {
                  FourBlockCount[x][y]++;
									double p = (FOURBLOCKHOLE + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

									PointBoard[x][y] += (FOURBLOCKHOLE + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * weight;
                  points += FOURBLOCKHOLE;

									Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                  continue;
                }
                // OX#_XO
                continue;
              }
              if (pos(x - 2 * mx[i], y - 2 * my[i]) == EMPTY)
              {
                if (pos(x - 3 * mx[i], y - 3 * my[i]) == EMPTY) // ###_XO
                {
                  points += TWOBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += TWOBLOCK * weight * PosWeight[x][y];
                  continue;
                }
                if (pos(x - 3 * mx[i], y - 3 * my[i]) == type) // X##_XO
                {
                  points += TWOBLOCK * PosWeight[x][y];
                  PointBoard[x][y] += TWOBLOCK * weight * PosWeight[x][y];
                  continue;
                }
                // O##_XO
                continue;
              }
              // O#_XO
              continue;
            }
            //		      if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) continue; // Already processed
            // O_XO
            continue;
          }
          //-------------------- pos(x + 1 * mx[i], y + 1 * my[i]) != type
          if (pos(x + 1 * mx[i], y + 1 * my[i]) == EMPTY && pos(x + 2 * mx[i], y + 2 * my[i]) == type) // _#X
          {
            if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY && pos(x + 3 * mx[i], y + 3 * my[i]) == EMPTY) // #_#X#
            {
              points += TWOHOLE * PosWeight[x][y];
              PointBoard[x][y] += TWOHOLE * weight * PosWeight[x][y];
              continue;
            }
            if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) continue; // X_#X# Already processed
            if (pos(x + 3 * mx[i], y + 3 * my[i]) == type) // _#XX
            {
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) continue; // X_#XX // Already processed
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_#XX
              {
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // #_#XX#
                {
									ThreeCount[x][y]++;
									double p = (THREE + ThreeDouble * (ThreeCount[x][y] - 1) + 
											ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

                  PointBoard[x][y] += p * weight;
                  points += p;
                  continue;
                }
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // #_#XXX
                {
                  FourBlockCount[x][y]++;
									double p = (FOURBLOCK + FourBlockDouble * (FourBlockCount[x][y] - 1) + 
										ThreeFourBlock * (FourBlockCount[x][y] + ThreeCount[x][y] - 1)) * PosWeight[x][y];

                  PointBoard[x][y] += p * weight;
                  points += p;

									Threat[x][y] = max(Threat[x][y], ThreeBlockThreat);
                  continue;
                }
                // #_#XXO
                points += THREEBLOCK * PosWeight[x][y];
                PointBoard[x][y] += THREEBLOCK * weight * PosWeight[x][y];
                continue;
              }
              // O_#XX
              points += THREEBLOCK * PosWeight[x][y];
              PointBoard[x][y] += THREEBLOCK * weight * PosWeight[x][y];
              continue;
            }
            if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_#XO
            {
              points += TWOBLOCK * PosWeight[x][y];
              PointBoard[x][y] += TWOBLOCK * weight * PosWeight[x][y];
              continue;
            }
            // O_#X#
            points += TWOBLOCK * PosWeight[x][y];
            PointBoard[x][y] += TWOBLOCK * weight * PosWeight[x][y];
            continue;
          }
          if (pos(x + 1 * mx[i], y + 1 * my[i]) == EMPTY && pos(x + 2 * mx[i], y + 2 * my[i]) == EMPTY && pos(x + 3 * mx[i], y + 3 * my[i]) == type) // _##X
          {
            if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY && pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // #_##X#
            {
              points += TWOHOLE * PosWeight[x][y];
              PointBoard[x][y] += TWOHOLE * weight * PosWeight[x][y];
              continue;
            }
            if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) continue; // X_##X# // Already processed
            if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // _##XX
            {
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_##XX
              {
                points += TWOHOLE * PosWeight[x][y];
                PointBoard[x][y] += TWOHOLE * weight * PosWeight[x][y];
                continue;
              }
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) continue; // X_##XX // Already processed
              // O_##XX
              points += TWOBLOCK * PosWeight[x][y];
              PointBoard[x][y] += TWOBLOCK * weight * PosWeight[x][y];
              continue;
            }

            if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_##XO
            {
              points += TWOBLOCK * PosWeight[x][y];
              PointBoard[x][y] += TWOBLOCK * weight * PosWeight[x][y];
              continue;
            }
            // O_##X#
            points += TWOBLOCK * PosWeight[x][y];
            PointBoard[x][y] += TWOBLOCK * weight * PosWeight[x][y];
            continue;
          }
        }
    }
  return points;
}

//OPlayer : Opponent, MPlayer: My
vector<axis> BestMoves(axis leftStone, axis rightStone, int OPlayer, int MPlayer, int retSize)
{
	memset(PointBoard, 0, sizeof(PointBoard));

  double MPoints = evaluate(MPlayer, 1, leftStone, rightStone);

	CalThreat(MPlayer % 2);

	memset(PointBoard, 0, sizeof(PointBoard));

  double OPoints = evaluate(OPlayer, 1, leftStone, rightStone);

	CalThreat(OPlayer % 2);

  double weight;

  double eps = 0.00001;

  if (OPoints > eps)
  {
    weight = MPoints / OPoints * 2.5;

    if (weight > 10)
      weight = 10;
  }
  else
    weight = 10;

  evaluate(MPlayer, weight, leftStone, rightStone);

  PointType point;
  vector<PointType> MaxPoint;

  for(int i = max(leftStone.x - 2, 0); i <= min(rightStone.x + 2, n - 1); i++)
    for(int j = max(leftStone.y - 2, 0); j <= min(rightStone.y + 2, n - 1); j++)
      if (board[i][j] == EMPTY)
      {
        point.stone.x = i;
        point.stone.y = j;
        point.val = PointBoard[i][j];

				MaxPoint.push_back(point);
      }

	sort(MaxPoint.begin(), MaxPoint.end(), cmp);

	vector<axis> ret(min((int)MaxPoint.size(), retSize));
	for(int i = 0; i < ret.size(); i++)
		ret[i] = MaxPoint[i].stone;

  return ret;
}

bool checkLine(axis stone, int num, int xa, int ya, int xb, int yb)
{
	int type = board[stone.x][stone.y];

	axis aStone = stone;
	while(pos(aStone.x + xa, aStone.y + ya) == type)
	{
		aStone.x = aStone.x + xa;
		aStone.y = aStone.y + ya;
	}

	axis bStone = stone;
	while(pos(bStone.x + xb, bStone.y + yb) == type)
	{
		bStone.x = bStone.x + xb;
		bStone.y = bStone.y + yb;
	}
	
	if (num == 5)
		return (max(abs(aStone.x - bStone.x), abs(aStone.y - bStone.y)) >= 4);
	else if (num == 4)
	{
		int maxNum = max(abs(aStone.x - bStone.x), abs(aStone.y - bStone.y));
		if (maxNum >= 3)
			return (pos(aStone.x + xa, aStone.y + ya) == EMPTY || pos(bStone.x + xb, bStone.y + yb) == EMPTY);
		else
		{
			bool aCheck = false;
			bool bCheck = false;
			if (pos(aStone.x + xa, aStone.y + ya) == EMPTY && pos(aStone.x + 2 * xa, aStone.y + 2 * ya) == type)
			{
				axis st;
				st.x = aStone.x + 2 * xa;
				st.y = aStone.y + 2 * ya;
				aCheck = checkLine(st, 4 - (maxNum + 1), xa, ya, xb, yb);
			}
			if (pos(bStone.x + xb, bStone.y + yb) == EMPTY && pos(bStone.x + 2 * xb, bStone.y + 2 * yb) == type)
			{
				axis st;
				st.x = bStone.x + 2 * xb;
				st.y = bStone.y + 2 * yb;
				bCheck = checkLine(st, 4 - (maxNum + 1), xa, ya, xb, yb);
			}
			return aCheck || bCheck;
		}
	}
	else if (num == 3)
		return max(abs(aStone.x - bStone.x), abs(aStone.y - bStone.y)) >= 2;
	else if (num == 2)
		return max(abs(aStone.x - bStone.x), abs(aStone.y - bStone.y)) >= 1;
	else
		return true;
}

bool GameOver(axis stone, int num)
{
  int type = board[stone.x][stone.y];

  if (type == EMPTY || type == OutBoard)
    return false;


	return (checkLine(stone, num, -1, 0, 1, 0) || checkLine(stone, num, 0, 1, 0, -1) || checkLine(stone, num, -1, 1, 1, -1) || 
			checkLine(stone, num, -1, -1, 1, 1));
}

unsigned long long rand64()
{
  return rand() ^ ((long long)rand() << 15) ^ ((long long)rand() << 30) ^
         ((long long)rand() << 45) ^ ((long long)rand() << 60);
}

unsigned long rand32()
{
  return rand() ^ ((long)rand() << 15) ^ ((long)rand() << 30);
}

void InitializeHashKey()
{
  srand((unsigned)time(NULL));

  for (int k = 0; k < 2; k++)
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
      {
        HashTable32[k][i][j] = rand32();
        HashTable64[k][i][j] = rand64();
      }

  hash[0] = new HashItem[HashSize];
  hash[1] = new HashItem[HashSize];
}

void CalculateInitHashKey()
{
  HashKey32 = 0;
  HashKey64 = 0;

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
    {
      int type = board[i][j];
      if (type != EMPTY)
      {
        HashKey32 = HashKey32 ^ HashTable32[type % 2][i][j];
        HashKey64 = HashKey64 ^ HashTable64[type % 2][i][j];
      }
    }
}

void Hash_MakeMove(axis stone)
{
  int type = board[stone.x][stone.y];

  HashKey32 = HashKey32 ^ HashTable32[type % 2][stone.x][stone.y];
  HashKey64 = HashKey64 ^ HashTable64[type % 2][stone.x][stone.y];
}

bool LookUpHashTable(int depth, double alpha, double beta, int index, double &value)
{
  HashItem *pht = &hash[index][HashKey32 % HashSize];

  if (pht->depth >= depth && pht->checksum == HashKey64)
  {
    HASH_TYPE type = pht->key_type;

    if (type == hashEXACT)
    {
      value = pht->eval;
      return true;
    }
    else if(type == hashLOWER)
    {
      if (pht->eval >= beta)
      {
        value = beta;
        return true;
      }
      else
        return false;
    }
    else
    {
      if (pht->eval <= alpha)
      {
        value = alpha;
        return true;
      }
      else
        return false;
    }
  }

  return false;
}

void HashTableAdd(HASH_TYPE key_type, double eval, int depth,int index)
{
  HashItem *pht = &hash[index][HashKey32 % HashSize];

	if (pht->key_type != hashEMPTY && pht->depth > depth)
		return;

  pht->checksum = HashKey64;
  pht->key_type = key_type;
  pht->eval = eval;
  pht->depth = depth;
}

bool cmp(const PointType &lhs, const PointType &rhs)
{
	return lhs.val > rhs.val;
}

void CalThreat(int index)
{
	for(int i = 0; i < TSize; i++)
		ThreatNum[index][i] = 0;

	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			ThreatNum[index][Threat[i][j]]++; 

	ThreatNum[index][ThreeThreat] /= 2;
}

// MPlayer Win: 1, OPlayer Win: -1, No Result: 0
int ThreatFind(int OPlayer, int MPlayer)
{
	if (ThreatNum[OPlayer][FourThreat] > 0)
		return -1;

	if (ThreatNum[MPlayer][FourThreat] > 0)
	{
		if (ThreatNum[MPlayer][FourThreat] >= 2)
			return 1;
		else
		{
			// bug：并且对方不会因封堵我的FourBlock而炼成四子
			int x = FourBlockPos[MPlayer].x;
			int y = FourBlockPos[MPlayer].y;
			int type = board[x][y];
			board[x][y] = OPlayer;

			if (ThreatNum[MPlayer][ThreeThreat] + ThreatNum[MPlayer][ThreeHoleThreat] > 0 && !GameOver(FourBlockPos[MPlayer], 4))
			{
				board[x][y] = type;
				return 1;
			}

			board[x][y] = type;
		}
	}
	else
	{
		if (ThreatNum[OPlayer][ThreeThreat] + ThreatNum[OPlayer][ThreeHoleThreat] > 0)
			return -1;
		else
		{
			if (ThreatNum[MPlayer][ThreeThreat] + ThreatNum[MPlayer][ThreeHoleThreat] >= 2 && ThreatNum[OPlayer][ThreeBlockThreat] == 0)
				return 1;
		}
	}

	return 0;
}

PointType EvalBoard(int OPlayer, int MPlayer, const axis &leftStone, const axis &rightStone, int type)
{
	PointType ret;

	// type = 0 -> MinValue, 1-> MaxValue
	if (type == 0)
	{
		ret.val = evaluate(OPlayer, 1, leftStone, rightStone);

		CalThreat(OPlayer % 2);

		ret.val -= evaluate(MPlayer, 1.5, leftStone, rightStone);

		CalThreat(MPlayer % 2);

		int TF = ThreatFind(MPlayer % 2, OPlayer % 2);

		if (TF == 1)
		{
			ret.val = ThreatScore;
		}
		else if (TF == -1)
		{
			ret.val = -ThreatScore;
		}
	}
	else
	{
    ret.val = evaluate(MPlayer, 1.5, leftStone, rightStone);

		CalThreat(MPlayer % 2);

    ret.val -= evaluate(OPlayer, 1, leftStone, rightStone);

		CalThreat(OPlayer % 2);
		
		int TF = ThreatFind(MPlayer % 2, OPlayer % 2);

		if (TF == 1)
		{
			ret.val = -ThreatScore;
		}
		else if (TF == -1)
		{
			ret.val = ThreatScore;
		}
	}

	return ret;
}
