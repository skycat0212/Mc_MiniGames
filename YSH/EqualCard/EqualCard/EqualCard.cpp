#include "Header.h"
#include "EqualCard.h"

// 상수
const int CardCount = 12; // Card의 개수

// 구조체변수 & 열거형변수
CARD Card[CardCount]; // Card 생성
GAMESTATUS GameStatus;
BOARD Board;

// 전역변수
char PrintArray[6][3] = { "♥" , "★" , "♣" , "♠" , "◀" , "☎" }; // 타입에 따른 출력 배열
char TypeArray[6][10] = { "Heart", "Star", "Clover", "Spade" , "Triangle" , "Phone" }; // 타입 이름 배열 

// 함수

void AssignCoord()// 좌표 부여 함수
{
	for (int index = 0; index < CardCount; index++)
	{
		/*
		9 10 11 12
		5  6  7  8 
		1  2  3  4
		*/
		switch (Card[index].CellNum)
		{
		case 1: // X, Y 부여
			Card[index].X = 10;
			Card[index].Y = 15;
			break;

		case 2:
			Card[index].X = 13;
			Card[index].Y = 15;
			break;

		case 3:
			Card[index].X = 16;
			Card[index].Y = 15;
			break;

		case 4:
			Card[index].X = 19;
			Card[index].Y = 15;
			break;

		case 5:
			Card[index].X = 10;
			Card[index].Y = 12;
			break;

		case 6:
			Card[index].X = 13;
			Card[index].Y = 12;
			break;

		case 7:
			Card[index].X = 16;
			Card[index].Y = 12;
			break;

		case 8:
			Card[index].X = 19;
			Card[index].Y = 12;
			break;

		case 9:
			Card[index].X = 10;
			Card[index].Y = 9;
			break;

		case 10:
			Card[index].X = 13;
			Card[index].Y = 9;
			break;

		case 11:
			Card[index].X = 16;
			Card[index].Y = 9;
			break;

		case 12:
			Card[index].X = 19;
			Card[index].Y = 9;
			break;

		}
	}
}

void AssignCell(int start, int end) // start ~ end 범위의 랜덤 숫자 생성
{
	int randomnum;
	bool IsOverlapCell[CardCount + 1 ] = { false, }; // 중복 판별용 bool 배열
	srand((unsigned)time(NULL));

	for (int i = 0; i < CardCount; )
	{
		randomnum = (rand() % (end - start)) + start; // start ~ end 범위의 랜덤 숫자

		if (IsOverlapCell[randomnum] == false) // 중복이 없다면
		{
			IsOverlapCell[randomnum] = true; // 중복 표시
			Card[i].CellNum = randomnum; // 값 대입
			++i; // 다음 CellNum으로 넘어감
		} // if문 종료
	} // for문 종료

} // 함수 종료

void CreateCard() // 카드 생성 함수
{
	AssignCell(1, CardCount+1); // CellNum 부여 함수 (랜덤. 중복X)
	AssignCoord();// CellNum에 따른 좌표 부여
	// Type 부여 함수 (랜덤. 중복 1회) 
	// Type에 따른 PrintForm 부여
}

// 프레임워크 함수
void Init()
{
	CreateCard();	// 카드 생성 함수 ( GameStatue == INIT 에 넣을까? )

	// Board 초기화
	Board.leftX = 15;
	Board.rightX = BOARD_WIDTH - 15;
	Board.topY = 4;
	Board.bottomY = BOARD_HEIGH - 4;
	
	// Game 상태 초기화
	//GameStatus = START;
}

void Update()
{

}

void Render()
{

	ScreenClear();

	// GameStatus에 따른 화면 출력 함수
	
	// RUNNIG 시 출력 문구
	
	// Board 출력
	// 각모서리
	ScreenPrint(Board.leftX, Board.topY, "┌"); // 좌측 상단
	ScreenPrint(Board.rightX, Board.topY, "┐"); // 우측 상단
	ScreenPrint(Board.leftX, Board.bottomY, "└"); // 좌측 하단
	ScreenPrint(Board.rightX, Board.bottomY, "┘"); // 우측 하단

												 // 위아래벽
	for (int i = Board.leftX + 2; i < Board.rightX; i++)
	{
		ScreenPrint(i, Board.topY, "-");
		ScreenPrint(i, Board.bottomY, "-");
	}
	// 좌우벽
	for (int i = Board.topY + 1; i < Board.bottomY; i++)
	{
		ScreenPrint(Board.leftX, i, "│");
		ScreenPrint(Board.rightX, i, "│");
	}

	// Card 출력
	for (int i = 0; i < CardCount; i++)
	{
		ScreenPrint(Card[i].X, Card[i].Y, "T");
	}

	ScreenFlipping();
}

void Release()
{
}

void main()
{
	int key;

	Init();
	ScreenInit();

	while (1)
	{
		if (_kbhit()) // key 입력이 있으면
		{
			key = _getch();

			if ((key == 'q') || (key == 'Q'))
				break;
		}

		Update();
		Render();
	}

	ScreenRelease();
	Release();
}