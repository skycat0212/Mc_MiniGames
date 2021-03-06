#include "Header.h"
#include "Brick.h"

namespace SH_Brick
{

	// 열거형 

	enum ControlKeys
	{
		UP = 72,
		DOWN = 80,
		LEFT = 75,
		RIGHT = 77,
		SPACE = 32,
		ESC = 27
	};

	// 구조체

	// 전역 변수

	//구조체 변수

	BALL Ball;
	BAR Bar;
	BLOCK Block[20];
	GAMESTATUS GameStatus;
	STAGE Stage;
	WALL Wall;

	// 출력 관련

	char StatString[500]; // 화면 출력 문구 저장용 char[]
	int PrintTime = 3 * 1000;
	clock_t Stat_OldTime = clock(); // PrintTime의 OldTime

	// 플레이 관련

	//int BlockCountTable[3] = { 1, 2, 3 }; // 발컨을 위한 테스트용 테이블
	int BlockCountTable[3] = { 10, 15, 20 }; // Stage.Level 에 따른 Block 갯수
	int brokenBlock; // Life 0 인 Block의 갯수
	int WallStateTable[4][6] = { // 벽과의 충돌 시 상태 변화 테이블
		{3, 2, -1, -1, -1, 4},
		{-1, 5, 4, -1, -1, -1},
		{-1, -1, 1, 0, 5, -1},
		{-1, -1, -1, -1, 2, 1}
	};
	int BlockStateTable[6] = { 3, 2, 1, 0, 5, 4 }; // 블럭과의 충돌 시 상태 변화 테이블

	// 함수

	// 상태에 따른 스크린 출력
	void StatusPrint()
	{
		clock_t CurTime = clock();

		switch (GameStatus)
		{
		case START:
			sprintf(StatString, "[벽돌깨기 게임] \n\n"
				"\t\t===================================\n\n"
				"\t\t주어진 벽돌을 깨는 게임입니다.\n"
				"\t\t각 스테이지 당 Life는 3 입니다.\n"
				"\t\t공의 이동 방향을 조작할 수 있습니다.\n"
				"\t\t벽돌을 다 깨면 다음 스테이지로 넘어갑니다.\n"
				"\t\t스테이지는 총 3레벨로 구성되어있습니다.\n\n"
				"\t\t===================================\n\n"
				"\t\t\t  - 조 작 법 -\n"
				"\t\t이동 : 방향키 | 일시정지 : ESC\n"
				"\t\t공 방향 조작 : \n"
				"\t\t\tA S D \n"
				"\t\t\tZ X C \n"
				"\t\t-----------------------------------\n"
				"\t\t게임 시작 : SPACE BAR | 게임 종료 : q\n\n\n\n");
			ScreenPrint(25, 10, StatString);
			break;

		case INIT:

			// 스테이지 초기화
			//Stage.BlockCount = BlockCountTable[Stage.Level]; // Level에 따라 BlockCount가 다름 
			brokenBlock = 0; // 파괴된 Block 갯수 0
			Bar.Life = 3; // Life 초기화
			Ball.Direction = TOP;

			// 화면 출력

			if (CurTime - Stat_OldTime < PrintTime)
			{
				sprintf(StatString, "[INIT 화면]");
				ScreenPrint(30, 10, StatString);
			}
			else
			{
				GameStatus = READY;
				Stat_OldTime = CurTime;
			}
			break;

		case READY:
			if (CurTime - Stat_OldTime < PrintTime)
			{
				sprintf(StatString, "[READY 화면]");
				ScreenPrint(30, 10, StatString);
			}
			else
			{
				GameStatus = RUNNING;
				Stat_OldTime = CurTime;
			}
			break;

		case RUNNING:
			if (brokenBlock == Stage.BlockCount)
			{
				Ball.IsReady = 1;
				GameStatus = SUCCESS;
			}
			if (Bar.Life == 0)
			{
				Ball.IsReady = 1;
				GameStatus = FAILED;
			}
			break;

		case STOP:
			sprintf(StatString, "[STOP 화면] \n"
				"\t\t\t\t ESC :: 게임으로 돌아감");
			ScreenPrint(30, 10, StatString);
			break;

		case SUCCESS:
			if (Stage.Level < 2)
			{
				sprintf(StatString, "[미션 성공] \n "
					"\t\t\t SPACE BAR :: 다음 스테이지 \n"
					"\t\t\t ESC :: 결과 화면");
			}
			else // Stage.Level == 3
			{
				sprintf(StatString, "[미션 성공] \n "
					"\t\t\t SPACE BAR :: 결과 화면");
			}
			ScreenPrint(30, 10, StatString);
			break;

		case FAILED:
			sprintf(StatString, "[미션 실패] \n"
				"\t\t\t SPACE BAR :: 재도전 \n"
				"\t\t\t ESC :: 결과 화면");
			ScreenPrint(30, 10, StatString);
			break;

		case RESULT:
			sprintf(StatString, "RESULT 화면");
			ScreenPrint(30, 10, StatString);
			break;

		}
	}

	// 충돌 체크
	int Collision(int x, int y)
	{
		int count = 0; // 블럭 충돌 횟수

		// Ball과 Bar의 충돌

		for (int i = 0; i < Bar.Length; i++)
		{
			if (y == Bar.Y)
			{

				// X[0]과 충돌
				if ((x >= Bar.X[0] && x <= Bar.X[0] + 1) || ((x + 1) >= Bar.X[0] && (x + 1) <= Bar.X[0] + 1))
				{
					Ball.Direction = (DIRECT)5;
					return 1;
				}
				// X[1]과 충돌
				if ((x >= Bar.X[1] && x <= Bar.X[1] + 1) || ((x + 1) >= Bar.X[1] && (x + 1) <= Bar.X[1] + 1))
				{
					Ball.Direction = (DIRECT)0;
					return 1;
				}
				// X[2]과 충돌
				if ((x >= Bar.X[2] && x <= Bar.X[2] + 1) || ((x + 1) >= Bar.X[2] && (x + 1) <= Bar.X[2] + 1))
				{
					Ball.Direction = (DIRECT)1;
					return 1;
				}
			}
		}

		// Ball과 Block의 충돌

		for (int i = 0; i < Stage.BlockCount; i++)
		{
			if (Block[i].Life > 0) // Life가 남은 Block에 한해서
			{
				if (Block[i].Y == y) // y가 동일
				{
					if (Block[i].X == x || Block[i].X == (x + 1) ||
						(Block[i].X + 1) == x || (Block[i].X + 1) == (x + 1)) // x 또는 x+1이 동일
					{
						Ball.Direction = (DIRECT)BlockStateTable[Ball.Direction];
						Block[i].Life--; // Block의 Life 감소
						count++; // 충돌 횟수 증가
						if (Block[i].Life == 0) // Block이 파괴되면
						{
							brokenBlock++; // 파괴된 Block 갯수 증가
						}
					}
				}
			}
		}

		if (count > 0)
			return 1;

		// Ball과 벽의 충돌

		// ↑ ([0])
		if (y < Wall.topY + 1)
		{
			Ball.Direction = (DIRECT)WallStateTable[0][Ball.Direction];
			return 1; // 충돌 O
		}

		// → ([1])
		if (x > Wall.rightX - 1)
		{
			Ball.Direction = (DIRECT)WallStateTable[1][Ball.Direction];
			return 1;
		}

		// ↓ ([2])
		if (y > Wall.bottomY - 1)
		{
			// Ball 상태 초기화
			Ball.Direction = TOP;
			Ball.IsReady = 1;
			Ball.X = Bar.X[1];
			Ball.Y = Bar.Y - 1;

			Bar.Life--; // Bar 라이프 감소
			if (Bar.Life < 1) // 라이프가 없으면
			{
				GameStatus = FAILED; // 미션 실패
			}
			return 1;
		}

		// ← ([3])
		if (x < Wall.leftX + 1)
		{
			Ball.Direction = (DIRECT)WallStateTable[3][Ball.Direction];
			return 1;
		}

		return 0; // 충돌 X
	}

	// 중복Block 존재?
	int OverlapBlock(int End, int x, int y)
	{
		for (int i = 0; i < End; i++)
		{
			if (Block[i].Y == y) // y동일
			{
				if (Block[i].X == x || (Block[i].X + 1) == x
					|| Block[i].X == (x + 1) || (Block[i].X + 1) == (x + 1)) // x동일
					return 1; // 중복 있음
			}
		} // 중복 검사 종료
		return 0; // 중복 없음
	}

	// Block 생성
	void SetBlock(int BlockCount)
	{
		int x, y;
		srand((unsigned)time(NULL));
		for (int i = 0; i < BlockCount; i++)
		{
			Block[i].Life = 1; // Block[i]의 Life 지정

			while (1)
			{
				x = (rand() % (Wall.rightX - Wall.leftX - 1)) + Wall.leftX + 1; // Wall.leftX + 1 <= x < Wall 너비 - 1 + Wall.letfX + 1
				y = (rand() % 10) + Wall.topY + 1; // Wall.topY + 1 <= y < Wall.topY + 10 + 1

				if (OverlapBlock(i, x, y) == 0) // 0~i번 까지의 블럭 중 중복이 없으면
				{
					Block[i].X = x;
					Block[i].Y = y;
					break;
				}
			} // while 문 끝
		} // for문 끝
	}

	// (RUNNING에서의) 키조작
	void KeyControl(int key)
	{
		clock_t CurTime = clock();
		int direction;

		if (CurTime - Bar.OldTime > Bar.MoveTime) // Ball 이동제한시간 경과 시 
		{
			switch (key)
			{
				// 방향키
			case LEFT:
				Bar.OldTime = clock();
				if (Bar.X[0] > Wall.leftX + 1)
				{
					Bar.X[0]--;
					Bar.X[1]--;
					Bar.X[2]--;
				}
				break;

			case RIGHT:
				Bar.OldTime = clock();
				if (Bar.X[2] < Wall.rightX - 1)
				{
					Bar.X[0]++;
					Bar.X[1]++;
					Bar.X[2]++;
				}
				break;

			case SPACE:
				//(Ball.IsReady == 1) ? (Ball.IsReady = 0) : (Ball.IsReady = 1); // Ball.IsReady 바꿈 
				if (Ball.IsReady == 1)
				{
					Ball.OldTime = clock();
					Ball.IsReady = 0; // 준비 O -> 준비 X
				}
				break;

				// 공 방향 
			case 'S': case 's': // ↑
				direction = 0;
				Ball.Direction = (DIRECT)direction;
				Ball.OldTime = clock();
				break;

			case 'D': case 'd': // ↗
				direction = 1;
				Ball.Direction = (DIRECT)direction;
				Ball.OldTime = clock();
				break;

			case 'C': case 'c': // ↘
				direction = 2;
				Ball.Direction = (DIRECT)direction;
				Ball.OldTime = clock();
				break;

			case 'X': case 'x': // ↓
				direction = 3;
				Ball.Direction = (DIRECT)direction;
				Ball.OldTime = clock();
				break;

			case 'Z': case 'z': // ↙
				direction = 4;
				Ball.Direction = (DIRECT)direction;
				Ball.OldTime = clock();
				break;

			case 'A': case 'a': // ↖
				direction = 5;
				Ball.Direction = (DIRECT)direction;
				Ball.OldTime = clock();
				break;

			default:
				break;
			}
		}

	}

	// Ball의 이동
	void BallMove(clock_t CurTime)
	{
		char temp[20];
		if (Ball.IsReady == 0) // 준비상태가 아니면
		{
			if (CurTime - Ball.OldTime > Ball.MoveTime) // 이동제한시간 경과
			{
				Ball.OldTime = CurTime;

				// 방향에 따른 좌표 변화 
				switch (Ball.Direction)
				{
				case TOP:
					sprintf(temp, "TOP");
					ScreenPrint(10, 2, temp);
					if (Collision(Ball.X, Ball.Y - 1) == 0) // 이동할 좌표에서 충돌이 안 일어나면 
					{
						Ball.Y--; // 이동한다
					}
					// 충돌이 일어났을 경우, Collision에서 Ball.Direction이 변경된다.
					break;

				case TOP_RIGHT:
					if (Collision(Ball.X + 1, Ball.Y - 1) == 0)
					{
						Ball.X++;
						Ball.Y--;
					}
					break;

				case BOT_RIGHT:
					if (Collision(Ball.X + 1, Ball.Y + 1) == 0)
					{
						Ball.X++;
						Ball.Y++;
					}
					break;

				case BOTTOM:
					if (Collision(Ball.X, Ball.Y + 1) == 0)
					{
						Ball.Y++;
					}
					break;

				case BOT_LEFT:
					if (Collision(Ball.X - 1, Ball.Y + 1) == 0)
					{
						Ball.X--;
						Ball.Y++;
					}
					break;

				case TOP_LEFT:
					if (Collision(Ball.X - 1, Ball.Y - 1) == 0)
					{
						Ball.X--;
						Ball.Y--;
					}
					break;

				}
			}
			if (Ball.X < 0 || Ball.X >BOARD_WIDTH || Ball.Y < 0 || Ball.Y > BOARD_HEIGH) // 벽에 충돌
			{
				Ball.IsReady = 1;
				Ball.Direction = TOP;
				Ball.X = Bar.X[1];
				Ball.Y = Bar.Y - 1;
			}
		}
		else // 준비 상태면
		{
			Ball.X = Bar.X[1];
			Ball.Y = Bar.Y - 1;
		}
	}

	// 프레임워크 함수

	void Init()
	{
		// 벽 초기화
		Wall.leftX = 12;
		Wall.rightX = BOARD_WIDTH - 12;
		Wall.topY = 2;
		Wall.bottomY = BOARD_HEIGH - 1;

		// 상태 초기화
		GameStatus = START;

		// 스테이지 초기화 -> StatusPrint() 의 case INIT: 
		Stage.Level = 0;
		Stage.BlockCount = BlockCountTable[Stage.Level];

		// 바 초기화
		Bar.X[0] = 30;
		Bar.X[1] = 32;
		Bar.X[2] = 34;
		Bar.Y = 20;
		Bar.Life = 3;
		Bar.Length = 3;
		Bar.OldTime = clock();
		Bar.MoveTime = 30;

		// 공 초기화
		Ball.X = Bar.X[1];
		Ball.Y = Bar.Y - 1;
		Ball.Direction = TOP;
		Ball.OldTime = clock();
		Ball.IsReady = 1;
		Ball.MoveTime = 130;

		// Block 생성
		SetBlock(Stage.BlockCount);

	}

	void Update()
	{
		clock_t CurTime = clock(); // 현재 시각

		if (GameStatus == RUNNING)
		{
			BallMove(CurTime);// 공 움직임
		}
	}

	void Render()
	{
		ScreenClear();

		StatusPrint();

		// 상단바 출력

		char TheTopBar[81];

		// 게임 실행 중 출력
		if (GameStatus == RUNNING || GameStatus == STOP)
		{
			sprintf(TheTopBar, "현 스테이지 : %d | 파괴된 블럭 : %d | Life : %d", Stage.Level + 1, brokenBlock, Bar.Life);
			ScreenPrint(17, 1, TheTopBar);

			// Wall 표시

			// 각모서리
			ScreenPrint(Wall.leftX, Wall.topY, "┌"); // 좌측 상단
			ScreenPrint(Wall.rightX, Wall.topY, "┐"); // 우측 상단
			ScreenPrint(Wall.leftX, Wall.bottomY, "└"); // 좌측 하단
			ScreenPrint(Wall.rightX, Wall.bottomY, "┘"); // 우측 하단

														 // 위아래벽
			for (int i = Wall.leftX + 2; i < Wall.rightX; i++)
			{
				ScreenPrint(i, Wall.topY, "-");
				ScreenPrint(i, Wall.bottomY, "-");
			}

			// 좌우벽
			for (int i = Wall.topY + 1; i < Wall.bottomY; i++)
			{
				ScreenPrint(Wall.leftX, i, "│");
				ScreenPrint(Wall.rightX, i, "│");
			}

			ScreenPrint(Ball.X, Ball.Y, "●"); // Ball 표시

			for (int i = 0; i < Bar.Length; i++) // Bar 표시
			{
				ScreenPrint(Bar.X[i], Bar.Y, "▣");
			}

			for (int i = 0; i < Stage.BlockCount; i++) // Block 표시
			{
				if (Block[i].Life > 0) // Life가 남아있으면
					ScreenPrint(Block[i].X, Block[i].Y, "■");
			}
		}

		ScreenFlipping();
	}

	void Release()
	{

	}

	void Brick()
	{
		int key;

		ScreenInit();
		Init();

		while (1)
		{


			if (_kbhit())
			{
				key = _getch();

				if ((key == 'q') || (key == 'Q'))
				{
					Stage.Level = 0;
					break;
				}

				// RUNNING 외 상태에서의 키조작

				if (key == ESC)
				{
					switch (GameStatus)
					{
					case RUNNING:
						GameStatus = STOP;
						break;

					case STOP:
						GameStatus = RUNNING;
						break;

					case SUCCESS:
						GameStatus = RESULT;
						break;

					case FAILED:
						GameStatus = RESULT;
						break;

					default:
						break;
					}
				}

				if (key == SPACE)
				{
					switch (GameStatus)
					{
					case START:
						GameStatus = INIT;
						break;

					case SUCCESS:
						Stage.Level++;
						if (Stage.Level < 3)
						{
							Stage.BlockCount = BlockCountTable[Stage.Level];
							SetBlock(Stage.BlockCount);
							GameStatus = INIT;
						}
						else
							GameStatus = RESULT;

						break;

					case FAILED:
					{
						Stage.BlockCount = BlockCountTable[Stage.Level];
						SetBlock(Stage.BlockCount);
						GameStatus = INIT;
					}
					break;

					default:
						break;
					}
				}

				// RUNNING 상태일 때의 키조작

				if (GameStatus == RUNNING)
				{
					KeyControl(key);
				}

			}

			Update();
			Render();
		}
	}
}