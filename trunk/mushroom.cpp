#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include <queue>
using namespace std;

const int MAXDEPTH = 5;
const int n = 15;
int board[n][n];
double PointBoard[n][n];
int BoardStep[n][n];

int EMPTY = 0;
int OutBoard = 3;

int limitTime = 950;
int startTime;


struct axis
{
  int x, y;
};

struct PointType
{
  double val;
  axis stone;
};

struct cmp
{
	bool operator()(const PointType &lhs, const PointType &rhs)
	{
		return lhs.val < rhs.val;
	}
};

void GetTime();

void PrintBoard();

void BoardInit();

bool TimeIsOver();

void UpdateStone(axis &leftStone, axis &rightStone, axis stone);

int pos(int x, int y);

double evaluate(int type, double multiplier, axis leftStone, axis rightStone);

int ThreatSpace(int OPlayer, int MPlayer, axis leftStone, axis rightStone);

vector<axis> BestMoves(axis leftStone, axis rightStone, int OPlayer, int MPlayer, int retSize);

PointType MaxNode(const PointType &a, const PointType &b);

PointType MaxValue(int depth, double alpha, double beta, int OPlayer, int MPlayer, axis leftStone, axis rightStone);

PointType MinNode(const PointType &a, const PointType &b);

PointType MinValue(int depth, double alpha, double beta, int OPlayer, int MPlayer, axis leftStone, axis rightStone);

axis AlphaBeta(int depth, int OPlayer, int MPlayer, axis leftStone, axis rightStone);

int main()
{
  string op;
  int MPlayer, OPlayer;
  int x, y;
  axis leftStone, rightStone;
  leftStone.x = -1;
  //srand(time(0));
  BoardInit();
  while(cin >> op)
  {
    if (op == "[START]")
    {
      cin >> MPlayer;
      if (MPlayer == 1)
				OPlayer = 2;
      else
				OPlayer = 1;
    }
    else if (op == "[PUT]")
    {
      cin >> x >> y;
      if (x == -1 && y == -1)
      {
        board[7][7] = MPlayer;
        cout << 7 << ' ' << 7 << endl;
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

        cout << move.x << ' ' << move.y << endl;
        board[move.x][move.y] = MPlayer;

        UpdateStone(leftStone, rightStone, move);
      }
    }
  }

  return 0;
}

void PrintBoard()
{
  for(int i = 0; i < n; i++)
  {
    for(int j = 0; j < n; j++)
      cout << board[i][j] << ' ';
    cout << endl;
  }
}

void BoardInit()
{
  for(int i = 0; i < n; i++)
    for(int j = 0; j < n; j++)
      board[i][j] = EMPTY;
}

axis AlphaBeta(int depth, int OPlayer, int MPlayer, axis leftStone, axis rightStone)
{
	double alpha = LLONG_MIN;
	double beta = LLONG_MAX;

	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			BoardStep[i][j] = 0;

	PointType ret = MaxValue(depth, alpha, beta, OPlayer, MPlayer, leftStone, rightStone);

  return ret.stone;
}

PointType MinNode(const PointType &a, const PointType &b)
{
	if (a.val < b.val)
		return a;
	return b;
}

PointType MinValue(int depth, double alpha, double beta, int OPlayer, int MPlayer, axis leftStone, axis rightStone)
{
	//GetTime();

	if (depth == 0 || TimeIsOver())
	{
		PointType ret;

		int ts = ThreatSpace(OPlayer, MPlayer, leftStone, rightStone);
		if (ts == 1)
		{
			ret.val = LLONG_MIN * 100.0;
			return ret;
		}
		else if (ts == -1)
		{
			ret.val = LLONG_MAX * 100.0;
			return ret;
		}

		ret.val = evaluate(OPlayer, 1, leftStone, rightStone);

		ret.val -= evaluate(MPlayer, 1, leftStone, rightStone);

		return ret;
	}

	const int retSize = 5;
  vector<axis> stone = BestMoves(leftStone, rightStone, OPlayer, MPlayer, retSize);
	PointType node;
	node.val = LLONG_MAX;

  for(vector<axis>::const_iterator iter = stone.begin(); iter != stone.end(); iter++)
  {
		board[iter->x][iter->y] = MPlayer;

		axis ls = leftStone;
		axis rs = rightStone;
		UpdateStone(leftStone, rightStone, *iter);

		BoardStep[iter->x][iter->y] = MAXDEPTH - depth;

    double val = MaxValue(depth - 1, alpha, beta, MPlayer, OPlayer, leftStone, rightStone).val;
		if (val < node.val)
		{
			node.val = val;
			node.stone.x = iter->x;
			node.stone.y = iter->y;
		}

		board[iter->x][iter->y] = 0;

		leftStone = ls;
		rightStone = rs;

		BoardStep[iter->x][iter->y] = 0;

		if (node.val <= alpha)
			return node;

		beta = min(beta, node.val);
  }

	return node;
}

PointType MaxNode(const PointType &a, const PointType &b)
{
	if (a.val > b.val)
		return a;
	return b;
}

PointType MaxValue(int depth, double alpha, double beta, int OPlayer, int MPlayer, axis leftStone, axis rightStone)
{
	//GetTime();

	if (depth == 0 || TimeIsOver())
	{
		PointType ret;

		int ts = ThreatSpace(OPlayer, MPlayer, leftStone, rightStone);
		if (ts == 1)
		{
			ret.val = LLONG_MAX * 100.0;
			return ret;
		}
		else if (ts == -1)
		{
			ret.val = LLONG_MIN * 100.0;
			return ret;
		}

		ret.val = evaluate(MPlayer, 1, leftStone, rightStone);

		ret.val -= evaluate(OPlayer, 1, leftStone, rightStone);

		return ret;
	}

	const int retSize = 5;
  vector<axis> stone = BestMoves(leftStone, rightStone, OPlayer, MPlayer, retSize);
	PointType node;
	node.val = LLONG_MIN;

	/*
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
			cout << PointBoard[i][j] << ' ';
		cout << endl;
	}

	for(int i = 0; i < stone.size(); i++)
		cout << stone[i].x << ' ' << stone[i].y << endl;
		*/

  for(vector<axis>::const_iterator iter = stone.begin(); iter != stone.end(); iter++)
  {
		board[iter->x][iter->y] = MPlayer;

		axis ls = leftStone;
		axis rs = rightStone;
		UpdateStone(leftStone, rightStone, *iter);

		BoardStep[iter->x][iter->y] = MAXDEPTH - depth;

		double val = MinValue(depth - 1, alpha, beta, MPlayer, OPlayer, leftStone, rightStone).val;
		if (node.val < val)
		{
			node.val = val;
			node.stone.x = iter->x;
			node.stone.y = iter->y;
		}

		//cout << "lev" << depth << ":" << "val:" << node.val << " x:" << node.stone.x << " y:" << node.stone.y << endl;

		board[iter->x][iter->y] = 0;

		leftStone = ls;
		rightStone = rs;

		BoardStep[iter->x][iter->y] = 0;

		if (node.val >= beta)
			return node;

		alpha = max(alpha, node.val);
  }

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
	if (x < 0) return OutBoard;
	if (y < 0) return OutBoard;
	if (x >= n) return OutBoard;
	if (y >= n) return OutBoard;
	return (board[x][y]);
}

double evaluate(int type, double multiplier, axis leftStone, axis rightStone)
{
  /*
  # = EMPTY
  _ = CURRENT SQUARE
  X = OWN MARKS
  O = OPP MARKS
  * == X or #
  */

  double FIVE = 999999;
  double FOUR = 9999;
  double THREE = 500;
  double THREEHOLE = 450;
  double FOURBLOCK = 450;
  double FOURBLOCKHOLE = 400;
  double TWO = 50;
  double TWOHOLE = 45;
  double THREEBLOCK = 40;
  double TWOBLOCK = 3;

  int mx[] = { 1, -1, 0, 0, 1, -1, 1, -1 }, my[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
  double points = 0;
  int blockmult[n][n];

  for (int x = 0; x < n; x++)
    for (int y = 0; y < n; y++)
		{
      blockmult[x][y] = 1;
		}


  for (int x = leftStone.x - 4; x < rightStone.x + 4; x++)
    for (int y = leftStone.y - 4; y < rightStone.y + 4; y++)
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
                  PointBoard[x][y] += FIVE * multiplier;
                  continue;
                }
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY)
                {
                  if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) // X_XXX#    #XXX_X
                  {
                    points += FIVE;
                    PointBoard[x][y] += FIVE * multiplier;
                    continue;
                  }
                  if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_XXX#   #XXX_#
                  {
                    points += FOUR;
                    PointBoard[x][y] += FOUR * multiplier;
                    continue;
                  }
                  // O_XXX#   #XXX_O
                  points += FOURBLOCK;
                  PointBoard[x][y] += FOURBLOCK * multiplier * blockmult[x][y];
                  blockmult[x][y]++;
                  continue;
                }
                // pos(x + 4 * mx[i], y + 4 * my[i]) = fi || edge
                if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) // X_XXXO   OXXX_X
                {
                  points += FIVE;
                  PointBoard[x][y] += FIVE * multiplier;
                  continue;
                }
                if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_XXXO   OXXX_#
                {
                  points += FOURBLOCK;
                  PointBoard[x][y] += FOURBLOCK * multiplier * blockmult[x][y];
                  blockmult[x][y]++;
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
                  points += THREE;
                  PointBoard[x][y] += THREE * multiplier;
                  continue;
                }
                if (pos(x - 1 * mx[i], y - 1 * my[i]) == type)
                {
                  if (pos(x - 2 * mx[i], y - 2 * my[i]) == EMPTY) // #X_XX#   #XX_X#
                  {
                    points += FOUR;
                    PointBoard[x][y] += FOUR * multiplier;
                    continue;
                  }
                  if (pos(x - 2 * mx[i], y - 2 * my[i]) == type) // XX_XX#    #XX_XX   Dual
                  {
                    points += FIVE / 2;
                    PointBoard[x][y] += FIVE * multiplier / 2;
                    continue;
                  }
                  // OX_XX#    #XX_XO
                  points += FOURBLOCK;
                  PointBoard[x][y] += FOURBLOCK * multiplier * blockmult[x][y];
                  blockmult[x][y]++;
                  continue;
                }
                // pos(x - 1 * mx[i], y - 1 * my[i]) == fi || edge
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // O_XX##    ##XX_O
                {
                  points += THREEBLOCK;
                  PointBoard[x][y] += THREEBLOCK * multiplier;
                  continue;
                }
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // O_XX#X   X#XX_O
                {
                  points += FOURBLOCKHOLE;
                  PointBoard[x][y] += FOURBLOCKHOLE * multiplier * blockmult[x][y];
                  blockmult[x][y]++;
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
                  points += THREEBLOCK;
                  PointBoard[x][y] += THREEBLOCK * multiplier;
                  continue;
                }
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == type) // X#_XXO  OXX_#X
                {
                  points += FOURBLOCKHOLE;
                  PointBoard[x][y] += FOURBLOCKHOLE * multiplier * blockmult[x][y];
                  blockmult[x][y]++;
                  continue;
                }
                // O#_XXO    OXX_#O
                continue;
              }
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) // X_XXO   OXX_X
              {
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == EMPTY) // #X_XXO  OXX_X#
                {
                  points += FOURBLOCK;
                  PointBoard[x][y] += FOURBLOCK * multiplier * blockmult[x][y];
                  blockmult[x][y]++;
                  continue;
                }
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == type) // XX_XXO  OXX_XX   Dual
                {
                  points += FIVE / 2;
                  PointBoard[x][y] += FIVE * multiplier / 2;
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
                    points += THREEHOLE;
                    PointBoard[x][y] += THREEHOLE * multiplier;
                    continue;
                  }
                  if (pos(x - 3 * mx[i], y - 3 * my[i]) == type) // XX#_X#
                  {
                    points += FOURBLOCKHOLE;
                    PointBoard[x][y] += FOURBLOCKHOLE * multiplier * blockmult[x][y];
                    blockmult[x][y]++;
                    continue;
                  }
                  points += THREEBLOCK;
                  PointBoard[x][y] += THREEBLOCK * multiplier;
                  // OX#_X#
                  continue;
                }
                if (pos(x - 2 * mx[i], y - 2 * my[i]) == EMPTY) // ##_X#
                {
                  if (pos(x + 3 * mx[i], y + 3 * my[i]) == EMPTY) // ##_X##
                  {
                    points += TWO;
                    PointBoard[x][y] += TWO * multiplier;
                    continue;
                  }
                  if (pos(x + 3 * mx[i], y + 3 * my[i]) == type) // ##_X#X
                  {
                    if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // ##_X#X#
                    {
                      points += THREEHOLE;
                      PointBoard[x][y] += THREEHOLE * multiplier;
                      continue;
                    }
                    if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // ##_X#XX
                    {
                      points += FOURBLOCKHOLE;
                      PointBoard[x][y] += FOURBLOCKHOLE * multiplier * blockmult[x][y];
                      blockmult[x][y]++;
                      continue;
                    }
                    // ##_X#XO
                    points += THREEBLOCK;
                    PointBoard[x][y] += THREEBLOCK * multiplier;
                    continue;
                  }
                  // ##_X#O
                  points += TWO;
                  PointBoard[x][y] += TWO * multiplier;
                  continue;
                }
                // O#_X#
                if (pos(x + 3 * mx[i], y + 3 * my[i]) == EMPTY) // O#_X##
                {
                  points += TWO;
                  PointBoard[x][y] += TWO * multiplier;
                  continue;
                }
                if (pos(x + 3 * mx[i], y + 3 * my[i]) == type)
                {
                  if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // O#_X#X#
                  {
                    points += THREEHOLE;
                    PointBoard[x][y] += THREEHOLE * multiplier;
                    continue;
                  }
                  if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // O#_X#XX
                  {
                    points += FOURBLOCKHOLE;
                    PointBoard[x][y] += FOURBLOCKHOLE * multiplier * blockmult[x][y];
                    blockmult[x][y]++;
                    blockmult[x][y]++;
                    continue;
                  }
                  // O#_X#XO
                  points += THREEBLOCK;
                  PointBoard[x][y] += THREEBLOCK * multiplier;
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
                  points += THREE / 2;
                  PointBoard[x][y] += THREE * multiplier / 2;
                  continue;
                }
                if (pos(x + 3 * mx[i], y + 3 * my[i]) == EMPTY) // OX_X##
                {
                  points += THREEBLOCK;
                  PointBoard[x][y] += THREEBLOCK * multiplier;
                  continue;
                }
                if (pos(x + 3 * mx[i], y + 3 * my[i]) == type) // OX_X#X
                {
                  points += FOURBLOCKHOLE;
                  PointBoard[x][y] += FOURBLOCKHOLE * multiplier;
                  continue;
                }
                // OX_X#O
                continue;
              }
              if (pos(x + 3 * mx[i], y + 3 * my[i]) == EMPTY)
              {
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // O_X###
                {
                  points += TWOBLOCK;
                  PointBoard[x][y] += TWOBLOCK * multiplier;
                  continue;
                }
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // O_X##X
                {
                  points += TWOBLOCK;
                  PointBoard[x][y] += TWOBLOCK * multiplier;
                  continue;
                }
                // O_X##O
                continue;
              }
              if (pos(x + 3 * mx[i], y + 3 * my[i]) == type)
              {
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // O_X#X#
                {
                  points += THREEBLOCK;
                  PointBoard[x][y] += THREEBLOCK * multiplier;
                  continue;
                }
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // O_X#XX
                {
                  points += FOURBLOCKHOLE;
                  PointBoard[x][y] += FOURBLOCKHOLE * multiplier * blockmult[x][y];
                  blockmult[x][y]++;
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
                  points += THREEBLOCK;
                  PointBoard[x][y] += THREEBLOCK * multiplier;
                  continue;
                }
                if (pos(x - 3 * mx[i], y - 3 * my[i]) == type) // XX#_XO
                {
                  points += FOURBLOCKHOLE;
                  PointBoard[x][y] += FOURBLOCKHOLE * multiplier * blockmult[x][y];
                  blockmult[x][y]++;
                  continue;
                }
                // OX#_XO
                continue;
              }
              if (pos(x - 2 * mx[i], y - 2 * my[i]) == EMPTY)
              {
                if (pos(x - 3 * mx[i], y - 3 * my[i]) == EMPTY) // ###_XO
                {
                  points += TWOBLOCK;
                  PointBoard[x][y] += TWOBLOCK * multiplier;
                  continue;
                }
                if (pos(x - 3 * mx[i], y - 3 * my[i]) == type) // X##_XO
                {
                  points += TWOBLOCK;
                  PointBoard[x][y] += TWOBLOCK * multiplier;
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
              points += TWOHOLE;
              PointBoard[x][y] += TWOHOLE * multiplier;
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
                  points += THREEHOLE;
                  PointBoard[x][y] += THREEHOLE * multiplier;
                  continue;
                }
                if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // #_#XXX
                {
                  points += FOURBLOCKHOLE;
                  PointBoard[x][y] += FOURBLOCKHOLE * multiplier * blockmult[x][y];
                  blockmult[x][y]++;
                  continue;
                }
                // #_#XXO
                points += THREEBLOCK;
                PointBoard[x][y] += THREEBLOCK * multiplier;
                continue;
              }
              // O_#XX
              points += THREEBLOCK;
              PointBoard[x][y] += THREEBLOCK * multiplier;
              continue;
            }
            if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_#XO
            {
              points += TWOBLOCK;
              PointBoard[x][y] += TWOBLOCK * multiplier;
              continue;
            }
            // O_#X#
            points += TWOBLOCK;
            PointBoard[x][y] += TWOBLOCK * multiplier;
            continue;
          }
          if (pos(x + 1 * mx[i], y + 1 * my[i]) == EMPTY && pos(x + 2 * mx[i], y + 2 * my[i]) == EMPTY && pos(x + 3 * mx[i], y + 3 * my[i]) == type) // _##X
          {
            if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY && pos(x + 4 * mx[i], y + 4 * my[i]) == EMPTY) // #_##X#
            {
              points += TWOHOLE;
              PointBoard[x][y] += TWOHOLE * multiplier;
              continue;
            }
            if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) continue; // X_##X# // Already processed
            if (pos(x + 4 * mx[i], y + 4 * my[i]) == type) // _##XX
            {
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_##XX
              {
                points += TWOHOLE;
                PointBoard[x][y] += TWOHOLE * multiplier;
                continue;
              }
              if (pos(x - 1 * mx[i], y - 1 * my[i]) == type) continue; // X_##XX // Already processed
              // O_##XX
              points += TWOBLOCK;
              PointBoard[x][y] += TWOBLOCK * multiplier;
              continue;
            }

            if (pos(x - 1 * mx[i], y - 1 * my[i]) == EMPTY) // #_##XO
            {
              points += TWOBLOCK;
              PointBoard[x][y] += TWOBLOCK * multiplier;
              continue;
            }
            // O_##X#
            points += TWOBLOCK;
            PointBoard[x][y] += TWOBLOCK * multiplier;
            continue;
          }
        }
    }
  return points;
}

//OPlayer : Opponent, MPlayer: My
vector<axis> BestMoves(axis leftStone, axis rightStone, int OPlayer, int MPlayer, int retSize)
{
	double agression;
	priority_queue<PointType, vector<PointType>, cmp> MaxPoint;
	
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			PointBoard[i][j] = 0;

	double MPoints = evaluate(MPlayer, 1, leftStone, rightStone);

	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			PointBoard[i][j] = 0;

	double OPoints = evaluate(OPlayer, 1, leftStone, rightStone);

	double eps = 0.00001;
	if (OPoints > eps)
	{
		agression = MPoints / OPoints * 3.5;
		if (agression > 100)
			agression = 100;
	}
	else
		agression = 10;

	evaluate(MPlayer, agression, leftStone, rightStone);

	PointType point;

	for(int i = max(leftStone.x - 4, 0); i < min(rightStone.x + 4, n); i++)
		for(int j = max(leftStone.y - 4, 0); j < min(rightStone.y + 4, n); j++)
			if (board[i][j] == EMPTY)
			{
				point.stone.x = i;
				point.stone.y = j;
				point.val = PointBoard[i][j];

				MaxPoint.push(point);
			}

	vector<axis> ret;

	while(!MaxPoint.empty() && retSize)
	{
		retSize--;
		ret.push_back(MaxPoint.top().stone);
		//cout << ret[ret.size() - 1].x << ' ' << ret[ret.size() - 1].y << " score:" << MaxPoint.top().val << endl;
		MaxPoint.pop();
	}

	return ret;
}

int ThreatSpace(int OPlayer, int MPlayer, axis leftStone, axis rightStone)
{
  int mx[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
	int my[] = { 0, 0, 1, -1, 1, 1, -1, -1 };

	int OThreatLastStep = INT_MAX;
	int MThreatLastStep = INT_MAX;

	for(int i = leftStone.x; i <= rightStone.x; i++)
		for(int j = leftStone.y; j <= rightStone.y; j++)
			for(int k = 0; k < 8; k++)
			{
				if (pos(i, j) == MPlayer && pos(i + mx[k], j + my[k]) == MPlayer && pos(i + 2 * mx[k], j + 2 * my[k]) == MPlayer && 
						pos(i + 3 * mx[k], j + 3 * my[k]) == MPlayer && pos(i + 4 * mx[k], j + 4 * my[k]) == MPlayer)
				{
					MThreatLastStep = min(MThreatLastStep, max(BoardStep[i][j], max(BoardStep[i + mx[k]][j + my[k]], 
												max(BoardStep[i + 2 * mx[k]][j + 2 * my[k]], max(BoardStep[i + 3 * mx[k]][j + 3 * my[k]], 
														BoardStep[i + 4 * mx[k]][j + 4 * my[k]])))));
					continue;
				}
				if (pos(i, j) == OPlayer && pos(i + mx[k], j + my[k]) == OPlayer && pos(i + 2 * mx[k], j + 2 * my[k]) == OPlayer && 
						pos(i + 3 * mx[k], j + 3 * my[k]) == OPlayer && pos(i + 4 * mx[k], j + 4 * my[k]) == OPlayer)
				{
					OThreatLastStep = min(OThreatLastStep, max(BoardStep[i][j], max(BoardStep[i + mx[k]][j + my[k]], 
												max(BoardStep[i + 2 * mx[k]][j + 2 * my[k]], max(BoardStep[i + 3 * mx[k]][j + 3 * my[k]], 
														BoardStep[i + 4 * mx[k]][j + 4 * my[k]])))));
				}
			}

	if (MThreatLastStep < OThreatLastStep)
		return 1;
	else if (MThreatLastStep > OThreatLastStep)
		return -1;
	else
		return 0;
}

bool TimeIsOver()
{
	if (clock() - startTime > limitTime)
		return true;
	return false;
}

void GetTime()
{
	cout << clock() - startTime << endl;
}

