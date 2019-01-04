#include<stdio.h>
#include<iostream>
#include<conio.h>
#include<math.h>
#include<time.h>
#include<iomanip>
#include<locale>
#include<Windows.h>
#include<string>
#include<cstring>
#include<fstream>
#pragma comment (lib,"winmm.lib")
using namespace std;
struct HUNTER //defines hunter properties
{
	int x; // coord x
	int y; // coord y
	int dir; // direction of view : 0 - axis x, 1 - axis y
	int ap; // action points , 2 - on each turn
};
struct HARE//defines hare properties
{
	int x; // coord x
	int y; // coord y
	int dx; // deltaX - difference between hare and hunter position on axis x
	int dy; // deltaY - difference between hare and hunter position on axis y
	int dist; // distance from hare to hunter (in fact:  dist = dx+dy)

};
struct GRAVE // grave is a dead hare, so it has same properties, only can't move and can't kill )))
{
	int x;
	int y;
	int dx;
	int dy;
};
struct TRAP //trap is place, where is hare of hunter will be dead
{
	int x;
	int y;
};
struct PLAYER
{
	char name[11];
	int score;
};

void gamePlay(int diffSelector, int field[30][60], HUNTER* hunter, HARE* hare, int hares, GRAVE* grave, int graves, TRAP* trap, int traps, int points);
void gameSave(int field[30][60], int points, int diffSelector);
void gameLoad(int field[30][60], int points, int diffSelector);

void dieMF(HARE** hare, GRAVE** grave, int* hares, int* graves, int field[30][60], int deadHare, int diffSelector , int* points);
void main();
void loseGame(int points);
void showRecords();
void menu();

void drawGray(int field[30][60]) //draws gray field (for hard levels)
{
	system("color 07");
	system("cls");
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 9,9 };
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);

	for (int i = 0; i < 32; i++)//рамка поля
	{
		for (int j = 0; j < 62; j++)
		{
			coord.X = 9 + j;
			coord.Y = 9 + i;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 32);
			if (i == 0 && j == 0)
				cout << char(201);
			else if (i == 0 && j == 61)
				cout << char(187);
			else if (i == 31 && j == 0)
				cout << char(200);
			else if (i == 31 && j == 61)
				cout << char(188);
			else if (i == 0 || i == 31)
				cout << char(205);
			else if (j == 0 || j == 61)
				cout << char(186);
			else
				cout << char(32);
		}
	}
	for (int i = 0; i < 30; i++) //draws blank elements
	{
		for (int j = 0; j < 60; j++)
		{
			coord.X = 10 + j;
			coord.Y = 10 + i;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 32);
			if (field[i][j] == 1)
				cout << char(26);
			else
				cout << char(178);
		}
	}
}

void drawField(int field[30][60]) //рисует поле на старте
{
	system("color 07");
	system("cls");
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 9,9 };
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);

	for (int i = 0; i < 32; i++)//рамка поля
	{
		for (int j = 0; j < 62; j++)
		{
			coord.X = 9 + j;
			coord.Y = 9 + i;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 32);
			if (i == 0 && j == 0)
				cout << char(201);
			else if (i == 0 && j == 61)
				cout << char(187);
			else if (i == 31 && j == 0)
				cout << char(200);
			else if (i == 31 && j == 61)
				cout << char(188);
			else if (i == 0 || i == 31)
				cout << char(205);
			else if (j == 0 || j == 61)
				cout << char(186);
			else
				cout << char(32);
		}
	}
	for (int i = 0; i < 30; i++) //расставляет на поле зайцев и охотника на старте
	{
		for (int j = 0; j < 60; j++)
		{
			coord.X = 10 + j;
			coord.Y = 10 + i;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 32);
			if (field[i][j] == 0)
				cout << char(32);
			else if (field[i][j] == 1)
				cout << char(26);
			else if (field[i][j] == 2)
				cout << char(2);
			else if (field[i][j] == 3)
				cout << char(134);
			else if (field[i][j] == 4)
				cout << char(35);
			
		}
	}
}

void generateHares(int field[30][60], HARE* hare) //генерирует координаты для зайцев
{
	int count = 0;
	int value = 0;
	while (count < 80)
	{
		value = rand() % 1800;//случайное число в диапазоне от 0 до x*y 
		if (value % 60 == 0 && value / 60 == 29) // если совпадает с координатами охотника, то пропускает
			continue;
		else
		{
			field[value / 60][value % 60] = 2;
			hare[count].x = value % 60; // координата х как остаток от деления
			hare[count].y = value / 60;  // координата y как частное целочисленного деления
			hare[count].dx = value % 60 - 0;       //стартовые дельты отсчитываются от стартовой позиции охотника
			hare[count].dy = value / 60 - 29;      // с координатами 0;29
			hare[count].dist = abs(hare[count].dx) + abs(hare[count].dy);
			count++;
		}
	}
}

bool searchTarget(HUNTER* hunter, HARE* hare, GRAVE* grave, int* hares, int* graves, int* deadHare) // search target of firing
{
	int minGraveDx = 99; //minimal distance to x for grave, default -  99
	int minGraveDy = 99; //minimal distance to y for grave, default -  99
	int minHareDx = 99; //minimal distance to x for hare, default - 99
	int minHareDy = 99; //minimal distance to y for hare, default - 99
	int minHareI = 99; // number of hare in array
	int value = 0; // for randomizer
	const int close = 4; // close range
	const int mid = 10; // middle range
	// far range is bigger than middle (>mid)

	const int valueClose = 95; // hit percent for  close range 
	const int valueMid = 45; // hit percent for middle range 
	const int valueFar = 5; // hit percent for far range 

	if (grave != NULL && *graves > 0)
	{
		if (hunter->dir == 0) // axis x
		{
			
			for (int i = 0; i < *graves; i++) // перебор могил
			{
				if (grave[i].dy == 0) // выбор тех, что на одной оси х
				{
					if (minGraveDx > grave[i].dx)
					{
						minGraveDx = grave[i].dx; // поиск ближайшей 
					}
				}
			} // если таких нет , то берется дефолтное значение 99
		}
		else if (hunter->dir == 1) // axis y
		{

			for (int i = 0; i < *graves; i++) // перебор могил
			{
				if (grave[i].dx == 0) // выбор тех, что на одной оси y
				{
					if (minGraveDy > grave[i].dy)
					{
						minGraveDy = grave[i].dy; // поиск ближайшей 
					}
				}
			} // если таких нет , то берется дефолтное значение 99
		}
	}

	if (hare != NULL && *hares > 0)
	{
		
		if (hunter->dir == 0) // axis x 
		{
		
			for (int i = 0; i < *hares; i++) //перебор зайцев
			{
				if (hare[i].dy == 0) // выбор только тех, кто на оси х
				{
					if (minHareDx > hare[i].dx)
					{
						minHareDx = hare[i].dx; // поиск ближайшего 
						minHareI = i; // запоминание номера в массиве
					}
				}
			} // если нету - берутся дефолтные значения
		}
		else if (hunter->dir == 1) // axis y
		{

			for (int i = 0; i < *hares; i++) //перебор зайцев
			{
				if (hare[i].dx == 0) // выбор только тех, кто на оси y
				{
					if (minHareDy > hare[i].dy)
					{
						minHareDy = hare[i].dy; // поиск ближайшего 
						minHareI = i; // запоминание номера в массиве
					}
				}
			} // если нету - берутся дефолтные значения
		}
	}
	if (hunter->dir == 0) // axis x - horizontal shhoting
	{
		if ((minGraveDx == 99 && minHareDx == 99) || minGraveDx < minHareDx)
			return false;
		
		else if (minHareDx < minGraveDx)
		{
			value = rand() % 100 + 1; // randomizing value
			if (hare[minHareI].dx <= close) // probability for close range
			{
				if (value <= valueClose)
				{
					*deadHare = minHareI; // number of dead hare in array
					return true;
				}
					
				else
					return false;
			}
			else if (hare[minHareI].dx <= mid) // for middle range
			{
				if (value <= valueMid)
				{
					*deadHare = minHareI;
					return true;
				}
				else
					return false;
			}
			else if (hare[minHareI].dx > mid) // for far range
			{
				if (value <= valueMid)
				{
					*deadHare = minHareI;
					return true;
				}
				else
					return false;
			}
		}

	}
	else if (hunter->dir == 1) // axis y - vertical shooting (similar to x)
	{
		if ((minGraveDy == 99 && minHareDy == 99) || minGraveDy < minHareDy) // if none hares found or grave closer than hare
			return false;
	
		else if (minHareDy < minGraveDy)
		{
			value = rand() % 100 + 1; // random value for chance to hit
			if (hare[minHareI].dy <= close)// for close range
			{
				if (value <= valueClose)
				{
					*deadHare = minHareI;
					return true;
				}
				else
					return false;
			}
			else if (hare[minHareI].dy <= mid)//for middle range
			{
				if (value <= valueMid)
				{
					*deadHare = minHareI;
					return true;
				}
				else
					return false;
			}
			else if (hare[minHareI].dy > mid)// for far range
			{
				if (value <= valueMid)
				{
					*deadHare = minHareI;
					return true;
				}
				else
					return false;
			}
		}

	}
	
}

void dieMF(HARE** hare, GRAVE** grave, int* hares, int* graves, int field[30][60], int deadHare, int diffSelector  , int* points)
{
	if (hare != NULL && *hare != NULL && *hares >0)
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD coord = { 0,0 };
		HARE* tmpHare = new HARE[(*hares) - 1];
		if (*grave == NULL && *graves == 0)// first grave
		{
			GRAVE* tmpGrave = new GRAVE;
			*graves = 1;
			tmpGrave->x = (*hare)[deadHare].x;
			tmpGrave->y = (*hare)[deadHare].y;
			tmpGrave->dx = (*hare)[deadHare].dx;
			tmpGrave->dy = (*hare)[deadHare].dy;
			coord.X = 10 + tmpGrave->x;
			coord.Y = 10 + tmpGrave->y;
			if (diffSelector == 0) // only on easy difficulty
			{
				SetConsoleCursorPosition(handle, coord);
				SetConsoleTextAttribute(handle, 32);
				cout << char(134);// draws grave instead of dead hare
			}
			
			(*grave) = tmpGrave;
		}
		else if (*grave != NULL && *graves > 0)// next graves
		{
			GRAVE* tmpGrave = new GRAVE[(*graves) + 1];
			for (int i = 0; i < (*graves) ; i++)
			{
				tmpGrave[i] = (*grave)[i];
			}
			tmpGrave[*graves].x = (*hare)[deadHare].x;
			tmpGrave[*graves].y = (*hare)[deadHare].y;
			tmpGrave[*graves].dx = (*hare)[deadHare].dx;
			tmpGrave[*graves].dy = (*hare)[deadHare].dy;
			coord.X = 10 + tmpGrave[*graves].x;
			coord.Y = 10 + tmpGrave[*graves].y;
			if (diffSelector == 0) // only on easy difficulty
			{
				SetConsoleCursorPosition(handle, coord);
				SetConsoleTextAttribute(handle, 32);
				cout << char(134);// draws grave instead of dead hare
			}
			(*graves) += 1;
		}
		field[(*hare)[deadHare].y][(*hare)[deadHare].x] = 3; // adds grave to field array

		for (int i = 0; i < *hares - 1; i++)
		{
			if (i < deadHare)
				tmpHare[i] = (*hare)[i];
			else if (i >= deadHare)
				tmpHare[i] = (*hare)[i + 1];
		}
		(*hare) = tmpHare;
		(*hares) -= 1;
		(*points) += 10;
	}

}

void addTrap(HUNTER* hunter, TRAP** trap, int* traps, int field[30][60])
{
	if (trap != nullptr && *trap != nullptr && *traps > 0)
	{
		TRAP* tmpTrap = new TRAP[(*traps) + 1];
		(*traps) += 1;
		for (int i = 0; i < (*traps) - 1; i++)
		{
			tmpTrap[i] = (*trap)[i];
		}
		tmpTrap[(*traps) - 1].x = hunter->x;
		tmpTrap[(*traps) - 1].y = hunter->y;
		*trap = tmpTrap;
	}
	else if (*trap == nullptr && traps == 0)
	{
		*traps = 1;
		TRAP* tmpTrap = new TRAP;
		tmpTrap->x = hunter->x;
		tmpTrap->y = hunter->y;
		field[tmpTrap->y][tmpTrap->x] = 4;
		*trap = tmpTrap;
	}
	
}

void haresMove(HUNTER* hunter, HARE** hare, int* hares, int field[30][60] , GRAVE** grave, int*graves, int points , int diffSelector ) // actions of hares
{
	
	for (int i = 0; i < *hares; i++) // rechecking distance from hunter after his move
	{
		(*hare)[i].dx = (*hare)[i].x - hunter->x;
		(*hare)[i].dy = (*hare)[i].y - hunter->y;
		(*hare)[i].dist = abs((*hare)[i].dx) + abs((*hare)[i].dy);
	}
	
	for (int i = 0; i < *hares; i++) // sorting hares depending on distance increase
	{
		for (int j = 0; j < ((*hares) - i - 1); j++)
		{
			if ((*hare)[j].dist > (*hare)[j + 1].dist)
				swap((*hare)[j], (*hare)[j + 1]);
		}
	}
	int closestHares = 15;// amount of closest hares (by default = 15)
	if (*hares < 15)
	{
		closestHares = *hares;
	}
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 0,0 };
	for (int i = 0; i < closestHares; i++)
	{
		coord.X = 10 + (*hare)[i].x;
		coord.Y = 10 + (*hare)[i].y;
		if (diffSelector == 0)//draws only on easy difficulty
		{
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 34);
			cout << char(2);
		}
		
		if (abs((*hare)[i].dx) > abs((*hare)[i].dy)) // if distance on axis x is more than on axis y
		{
				
				 if ((*hare)[i].x > hunter->x) // if hare is in right side of hunter
				{
					 if ((*hare)[i].y == hunter->y && ((*hare)[i].x - 1) == hunter->x)// if there is a hunter
					 {
						 loseGame(points);// game ends
					 }
					else if (field[(*hare)[i].y][(*hare)[i].x - 1] == 0)// if on the way is nothing
					{
						field[(*hare)[i].y][(*hare)[i].x] = 0;
						(*hare)[i].x -= 1; // decreases x coordinate
						coord.X--;
						field[(*hare)[i].y][(*hare)[i].x] = 2; // moves hare on field
					}
					else if (field[(*hare)[i].y][(*hare)[i].x - 1] == 4)// if there is a trap
					{
						field[(*hare)[i].y][(*hare)[i].x] = 0;
						(*hare)[i].x -= 1; // decreases x coordinate
						coord.X--;
						field[(*hare)[i].y][(*hare)[i].x] = 2; // moves hare on field
						dieMF(hare, grave, hares, graves, field, i, diffSelector, &points); // hare is dead and new grave adds
					}
				
				}
			else if ((*hare)[i].x < hunter->x) // if hare is in left side of hunter
			{
				if ((*hare)[i].y == hunter->y && ((*hare)[i].x + 1) == hunter->x) // if on the way is hunter
				{
					loseGame(points);
				}
				else if (field[(*hare)[i].y][(*hare)[i].x + 1] == 0)// if on the way is nothing
				{
					field[(*hare)[i].y][(*hare)[i].x] = 0;
					(*hare)[i].x += 1; //increases x coord
					coord.X++;
					field[(*hare)[i].y][(*hare)[i].x] = 2; //moves hare on field
				}
				else if (field[(*hare)[i].y][(*hare)[i].x + 1] == 4)// if there is a trap
				{
					field[(*hare)[i].y][(*hare)[i].x] = 0;
					(*hare)[i].x += 1; //increases x coord
					coord.X++;
					field[(*hare)[i].y][(*hare)[i].x] = 2; //moves hare on field
					dieMF(hare, grave, hares, graves, field, i , diffSelector, &points); // hare is dead
				}
			}
		}
		else if (abs((*hare)[i].dx) <= abs((*hare)[i].dy)) // if distance on axis x is less than on axis y
		{
			if ((*hare)[i].y > hunter->y) // if hare is in lower side of hunter
			{
				if (((*hare)[i].y - 1) == hunter->y && ((*hare)[i].x) == hunter->x) // if on the way is hunter
				{
					loseGame(points);
				}
				else if (field[(*hare)[i].y - 1][(*hare)[i].x] == 0)// if on the way is nothing
				{
					field[(*hare)[i].y][(*hare)[i].x] = 0;
					(*hare)[i].y -= 1; //decreases  y coordinate
					coord.Y--;
					field[(*hare)[i].y][(*hare)[i].x] = 2; //moves hare on field
				}
				else if (field[(*hare)[i].y - 1][(*hare)[i].x ] == 4)// if there is a trap
				{
					field[(*hare)[i].y][(*hare)[i].x] = 0;
					(*hare)[i].y -= 1; //decreases  y coordinate
					coord.Y--;
					field[(*hare)[i].y][(*hare)[i].x] = 2; //moves hare on field
					dieMF(hare, grave, hares, graves, field, i , diffSelector, &points); // hare is dead
				}
			
			}
			else if ((*hare)[i].y < hunter->y) // if hare is in upper side of hunter
			{
				if (((*hare)[i].y + 1) == hunter->y && ((*hare)[i].x) == hunter->x) // if on the way is hunter
				{
					loseGame(points);
				}
				else if (field[(*hare)[i].y + 1][(*hare)[i].x] == 0)// if on the way is nothing
				{
					field[(*hare)[i].y][(*hare)[i].x] = 0;
					(*hare)[i].y += 1; //increases y coordinate
					coord.Y++;
					field[(*hare)[i].y][(*hare)[i].x] = 2; //moves hare on field
				}
				else if (field[(*hare)[i].y + 1][(*hare)[i].x ] == 4)// if there is a trap
				{
					field[(*hare)[i].y][(*hare)[i].x] = 0;
					(*hare)[i].y += 1; //increases y coordinate
					coord.Y++;
					field[(*hare)[i].y][(*hare)[i].x] = 2; //moves hare on field
					dieMF(hare, grave, hares, graves, field, i , diffSelector, &points); // hare is dead
				}
				
			}
		}
		if (diffSelector == 0) // draws only on easy difficulty
		{
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 32);
			cout << char(2);
		}
	} // for (int i...
	
}

void pauseMenu(int field[30][60],HUNTER* hunter, HARE* hare,int  hares,GRAVE* grave,int graves,TRAP* trap,int traps, int points) // in game pause menu
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 15,15 };
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	
	for (int i = 0; i < 11; i++)
	{
		for (int j = 0; j < 22; j++)
		{
			coord.X = 15 + j;
			coord.Y = 15 + i;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 137);
			if (i == 0 && j == 0)
				cout << char(201);
			else if (i == 0 && j == 21)
				cout << char(187);
			else if (i == 10 && j == 0)
				cout << char(200);
			else if (i == 10 && j == 21)
				cout << char(188);
			else if (i == 0 || i == 10)
				cout << char(205);
			else if (j == 0 || j == 21)
				cout << char(186);
			else
				cout << char(32);
		}
	}
	string pauseMenus[] = { "New game " , "Save game" , "Load game", "Exit game"  };
	coord.X = 18;
	coord.Y = 15;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	cout << " PAUSE MENU ";
	coord.X = 18;
	coord.Y = 25;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	cout << " Esc - return ";
	coord.X = 18;
	coord.Y = 17;
	for (int i = 0; i < 4; i++)
	{
		SetConsoleCursorPosition(handle, coord);
		SetConsoleTextAttribute(handle, 128);
		cout << pauseMenus[i];
		coord.Y += 2;
	}
	coord.X = 18;
	coord.Y = 17;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 8);
	cout << pauseMenus[0];
	int key = 0;
	int selector = 0;
	int diffSelector = 0;
	do {
		if (_kbhit())
			key = _getch();
		if (key == 224)
		{
			key = _getch();
			switch (key)
			{
			case 80:
			{
				if (selector < 3)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << pauseMenus[selector];
					coord.Y += 2;
					selector++;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << pauseMenus[selector];
				}
				else if (selector == 3)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << pauseMenus[selector];
					coord.Y = 17;
					selector = 0;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << pauseMenus[selector];
				}
				break;
			}
			case 72:
			{
				if (selector > 0)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << pauseMenus[selector];
					coord.Y -= 2;
					selector--;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << pauseMenus[selector];
				}
				else if (selector == 0)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << pauseMenus[selector];
					coord.Y = 23;
					selector = 3;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << pauseMenus[selector];
				}
				break;
			}
			default:
				break;
			}//switch (key)
		}

	} while (key != 13 && key != 27);
	if (key == 27)
		gamePlay(diffSelector, field, hunter, hare, hares, grave, graves, trap, traps, points);
	else if (key == 13)
	{
		switch (selector)
		{
		case 0:
		{
			main();
			break;
		}
		case 1:
		{
			gameSave(field, points, diffSelector);
			return;
			break;
		}
		case 2:
		{
			gameLoad(field, points, diffSelector);
			return;
			break;
		}
		case 3:
		{
			system("cls");
			cout << "Thank you for playing!!!" << endl << endl;
			system("pause");
			exit(0);
			break;
		}

		default:
			break;
		}
	}
}

void drawTraps(TRAP* trap, int traps, int field[30][60], HUNTER* hunter, int points) // draws traps on field (on easy difficulty)
{
	if (trap != nullptr && traps > 0) // if there are traps
	{
		for (int i = 0; i < traps; i++) //marks traps on field array
		{
			if (field[trap[i].y][trap[i].x] == 1)
			{
				loseGame(points);
			}
			else 
				field[trap[i].y][trap[i].x] = 4;
		}
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD coord = { 0,0 };

		for (int i = 0; i < 30; i++)//draws traps on field
		{
			for (int j = 0; j < 60; j++)
			{
				if (field[i][j] == 4)
				{
					coord.X = 10 + j;
					coord.Y = 10 + i;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 32);
					cout << char(35); 

				}
			}
		}
	}
	
}

void markTraps(TRAP* trap, int traps, int field[30][60], HUNTER* hunter, int points) // marks traps on field (on medium and hard difficulty)
{
	if (trap != nullptr && traps > 0)
	{
		for (int i = 0; i < traps; i++)
		{
			if (field[trap[i].y][trap[i].x] == 1)
			{
				loseGame(points);
			}
			else
				field[trap[i].y][trap[i].x] = 4;
		}
		
	}

}

void informTablo(int points)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 10,1 };
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			coord.X = 10 + j;
			coord.Y = 1 + i;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 137);
			if (i == 0 && j == 0)
				cout << char(201);
			else if (i == 0 && j == 31)
				cout << char(187);
			else if (i == 7 && j == 0)
				cout << char(200);
			else if (i == 7 && j == 31)
				cout << char(188);
			else if (i == 0 || i == 7)
				cout << char(205);
			else if (j == 0 || j == 31)
				cout << char(186);
			else
				cout << char(32);
		}
	}
	coord.X = 13;
	coord.Y = 2;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	cout << char(24) << "Move/turn up";
	coord.Y++;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	cout << char(26) << "Move/turn right";
	coord.Y++;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	cout <<  "z - Shoot";
	coord.Y++;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	cout << "x - Set trap";
	coord.Y++;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	cout <<points << " points";
}

void showLine(HUNTER* hunter, int field[30][60]) //binocular view
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 0,0 };
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 32);
	coord.X = 10 + hunter->x;
	coord.Y = 10 + hunter->y;
	if (hunter->dir == 0) // if looks right (axis x)
	{
		do //takes from field array and draws cells
		{
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 32);
			if (field[coord.Y - 10][coord.X - 10] == 0)
				cout << char(32);
			else if (field[coord.Y - 10][coord.X - 10] == 1)
				cout << char(26);
			else if (field[coord.Y - 10][coord.X - 10] == 2)
				cout << char(2);
			else if (field[coord.Y - 10][coord.X - 10] == 3)
				cout << char(134);
			else if (field[coord.Y - 10][coord.X - 10] == 4)
				cout << char(35);
			coord.X++;
		} while (field[coord.Y - 10][coord.X - 10] == 0);// draws field line until meeting hare or grave

	}
	else if (hunter->dir == 1) // if looks up (axis y)
	{
		do
		{
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 32);
			if (field[coord.Y - 10][coord.X - 10] == 0)
				cout << char(32);
			else if (field[coord.Y - 10][coord.X - 10] == 1)
				cout << char(26);
			else if (field[coord.Y - 10][coord.X - 10] == 2)
				cout << char(2);
			else if (field[coord.Y - 10][coord.X - 10] == 3)
				cout << char(134);
			else if (field[coord.Y - 10][coord.X - 10] == 4)
				cout << char(35);
			coord.Y--;
		} while (field[coord.Y - 10][coord.X - 10] == 0);// draws field line until meeting hare or grave

	}
}

void hideLine(HUNTER* hunter, int field[30][60]) //binocular view
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 0,0 };
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 32);
	coord.X = 10 + hunter->x;
	coord.Y = 10 + hunter->y;
	do 
	{
		SetConsoleCursorPosition(handle, coord);
		SetConsoleTextAttribute(handle, 32);
		cout << char(178);
		coord.X++;
	} while (field[coord.Y - 10][coord.X - 10] == 0);// draws hide-blocks
	coord.X = 10 + hunter->x;
	coord.Y = 10 + hunter->y;
	do
	{
		SetConsoleCursorPosition(handle, coord);
		SetConsoleTextAttribute(handle, 32);
		cout << char(178);
		coord.Y--;
	} while (field[coord.Y - 10][coord.X - 10] == 0);// draws hide-blocks
}

void drawSector(HUNTER* hunter, int field[30][60]) //draws sector of hunter vision in medium difficulty
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 0,0 };
	int cornerX = 10 + hunter->x;
	int cornerY = 10 + hunter->y;
	for (int i = cornerY; i >= (cornerY - 2); i--)
	{
		for (int j = cornerX; j <= (cornerX + 2); j++)
		{
			coord.X = j;
			coord.Y = i;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 32);
			if (i == (cornerY-2) && j == (cornerX + 2))
				cout << char(178);
			else if (field[i - 10][j - 10] == 0) //nothing here
				cout << char(32);
			else if (field[i - 10][j - 10] == 1) //hunter
			{
				if (hunter->dir == 0) // hunter looks right
					cout << char(26);
				else if (hunter->dir ==1) // hunter looks up
					cout << char(24);
			}
			else if (field[i - 10][j - 10] == 2) //hare
				cout << char(2);
			else if (field[i - 10][j - 10] == 3)//grave
				cout << char(134);
			else if (field[i - 10][j - 10] == 4)//trap
				cout << char(35);
		}
	}
}

void drawCell(HUNTER* hunter, int field[30][60]) //draws cell of hunter vision in hard difficulty
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 0,0 };
	coord.X = 10 + hunter->x;
	coord.Y = 10 + hunter->y;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 32);
	if (hunter->dir == 0) // hunter looks right
	{
		cout << char(26);
		coord.X++;
		if (field[coord.X - 10][coord.Y - 10] == 0) //nothing here
			cout << char(32);
		else if (field[coord.X - 10][coord.Y - 10] == 2) //hare
			cout << char(2);
		else if (field[coord.X - 10][coord.Y - 10] == 3)//grave
			cout << char(134);
		else if (field[coord.X - 10][coord.Y - 10] == 4)//trap
			cout << char(35);
	}
		
	else if (hunter->dir == 1) // hunter looks up
	{
		cout << char(24);
		coord.Y--;
		if (field[coord.X - 10][coord.Y - 10] == 0) //nothing here
			cout << char(32);
		else if (field[coord.X - 10][coord.Y - 10] == 2) //hare
			cout << char(2);
		else if (field[coord.X - 10][coord.Y - 10] == 3)//grave
			cout << char(134);
		else if (field[coord.X - 10][coord.Y - 10] == 4)//trap
			cout << char(35);
		
	}
}

void hideSector(HUNTER* hunter, int field[30][60]) //hides sector of hunter vision in medium difficulty on moving
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 0,0 };
	int cornerX = 10 + hunter->x;
	int cornerY = 10 + hunter->y;
	for (int i = cornerY; i >= (cornerY - 2); i--)
	{
		for (int j = cornerX; j <= (cornerX + 2); j++)
		{
			coord.X = j;
			coord.Y = i;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 32);
			cout << char(178);
		}
	}
}

void hideCell(HUNTER* hunter, int field[30][60]) //hides cell of hunter vision in hard difficulty on moving
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 0,0 };
	coord.X = 10 + hunter->x;
	coord.Y = 10 + hunter->y;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 32);
	if (hunter->dir == 0) // hunter looks right
	{
		cout << char(178);
		coord.X++;
		cout << char(178);
	}

	else if (hunter->dir == 1) // hunter looks up
	{
		cout << char(178);
		coord.Y--;
		cout << char(178);

	}
}

void gameStart(int diffSelector)
{
	int field[30][60] = { 0 };
	int points = 0; // points of player
	
	int hares = 80; // amount of hares (at start - 80)
	int graves = 0; // amount of graves (at start - none)
	int traps = 0; // amount of traps (at start - none)
	
	HARE * hare = new HARE[hares]; // array of hares
	HUNTER* hunter = new HUNTER;
	GRAVE* grave = nullptr;
	TRAP* trap = nullptr;
	hunter->x = 0;
	hunter->y = 29;
	hunter->dir = 0;
	hunter->ap = 2;
	field[29][0] = 1;
	generateHares(field, hare);
	gamePlay(diffSelector, field, hunter, hare, hares, grave, graves, trap, traps, points);
}

void gameLoad(int field[30][60], int points , int diffSelector)// loading game from save file
{
	 field[30][60] = { 0 };
	 points = 0; // points of player

	int hares = 0; // amount of hares (at start - 0)
	int graves = 0; // amount of graves (at start - none)
	int traps = 0; // amount of traps (at start - none)
	HARE * hare = nullptr;
	HUNTER* hunter = nullptr;
	GRAVE* grave = nullptr;
	TRAP* trap = nullptr;
	string source = "save.txt";
	ifstream sourceFile;
	sourceFile.open(source, ios_base::in);
	if (sourceFile.is_open())// if file exists , reads data from it
	{
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				sourceFile >> field[i][j];
			}
		}
		sourceFile >> points ;
		sourceFile >> diffSelector;
		sourceFile.close();
	}
	else
	{
		system("cls");
		cout << "Sorry, no file to load..." << endl;
		system("pause");
		menu();

	}
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 60; j++)
		{
			if (field[i][j] == 1)
			{
				hunter = new HUNTER;
				hunter->x = j;
				hunter->y = i;
				hunter->dir = 0;
				hunter->ap = 2;
			}
			else if (field[i][j] == 2)
				hares++;
			else if (field[i][j] == 3)
				graves++;
			else if (field[i][j] == 4)
				traps++;
		}
	}
	hare = new HARE[hares]; // asign hares according to field data
	int currentHare = 0; //temporary counter of hares
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 60; j++)//recalculating hares properties
		{
			if (field[i][j] == 2)
			{
				hare[currentHare].x = j;
				hare[currentHare].y = i;
				hare[currentHare].dx = hare[currentHare].x - hunter->x;
				hare[currentHare].dy = hare[currentHare].y - hunter->y;
				hare[currentHare].dist =abs(hare[currentHare].dx) + abs(hare[currentHare].dy);
				currentHare++;
			}
			
		}
	}
	if (graves > 0) // if there are graves - recalculating them
	{
		grave = new GRAVE[graves]; // asign graves according to field data
		int currentGrave = 0; //temporary counter of graves
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				if (field[i][j] == 3)
				{
					grave[currentGrave].x = j;
					grave[currentGrave].y = i;
					grave[currentGrave].dx = grave[currentGrave].x - hunter->x;
					grave[currentGrave].dy = grave[currentGrave].y - hunter->y;
					currentGrave++;
				}

			}
		}
	}
	if (traps > 0) // if there are traps - recalculating them
	{
		trap = new TRAP[traps]; // asign traps according to field data
		int currentTrap = 0; //temporary counter of traps
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				if (field[i][j] == 4)
				{
					trap[currentTrap].x = j;
					trap[currentTrap].y = i;
					currentTrap++;
				}

			}
		}
	}

	gamePlay(diffSelector, field, hunter, hare, hares, grave, graves, trap, traps, points);// starting game with loaded data
}
void gameSave(int field[30][60], int points, int diffSelector) //saving game data to file
{
	string target = "save.txt";
	ofstream targetFile;
	targetFile.open(target, ios_base::out);
	
	if (targetFile.is_open()) //saving nessessary data - location of each object , only - field, points , difficulty level
	{
		for (int i = 0 ; i < 30 ; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				targetFile << field[i][j] << '\n';
			}
		}
		targetFile << points << endl;
		targetFile << diffSelector << endl;
		targetFile.close();
	}
}

void gamePlay(int diffSelector, int field[30][60],HUNTER* hunter, HARE* hare, int hares, GRAVE* grave, int graves, TRAP* trap, int traps, int points)
{
	PlaySound(TEXT("field.wav"), NULL, SND_ASYNC);
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 0,0 };
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 32);
	bool hit = true; // flag - if hunter hit the hare
	int deadHare = 0; // nimber of dead hare in array
	// drawing field depending of difficulty level
	if (diffSelector == 0)
		drawField(field);
	else if (diffSelector != 0)
		drawGray(field);
	int key = 0;
	coord.X = 10 + hunter->x;
	coord.Y = 10 + hunter->y;
	if (diffSelector == 1) // drawing hunter visible area depending on difficulty level
		drawSector(hunter, field);
	else if (diffSelector == 2)
		drawCell(hunter, field);
	while (true) // game cycle
	{
		int key = 0;
		if (diffSelector !=0)
			hideLine(hunter, field);
		informTablo(points);
		hunter->ap = 2;
		while (hunter->ap > 0)
		{
			
			if (_kbhit())
			{
				key = _getch(); //checking pressed keys
				if (key == 224)
				{
					key = _getch();
					switch (key)
					{
					case 72: //move up
					{
						if (hunter->dir == 0) // if turned right - turn up
						{
							if (diffSelector == 0)
							{
								SetConsoleCursorPosition(handle, coord);
								SetConsoleTextAttribute(handle, 34);
								cout << char(26);
							}
							else if (diffSelector == 1)
								hideSector(hunter, field);
							else if (diffSelector == 2)
								hideCell(hunter, field);
							hunter->dir = 1;// turning
							hunter->ap -= 1;// 1 action point 
							if (diffSelector == 0)
							{
								SetConsoleCursorPosition(handle, coord);
								SetConsoleTextAttribute(handle, 32);
								cout << char(24);
							}
							else if (diffSelector == 1)// drawing hunter visible area depending on difficulty level
								drawSector(hunter, field);
							else if (diffSelector == 2)
								drawCell(hunter, field);
						}
						else if (hunter->dir == 1) //if turned up - move up
						{
							if (diffSelector == 0)
							{
								SetConsoleCursorPosition(handle, coord);
								SetConsoleTextAttribute(handle, 34);
								cout << char(24);
							}
							else if (diffSelector == 1)
								hideSector(hunter, field);
							else if (diffSelector == 2)
								hideCell(hunter, field);
							if (field[hunter->y - 1][hunter->x] != 3) // checks if there is no  grave
							{
								coord.Y--; // moving
								hunter->y -= 1;
								hunter->ap = 0; // action points ended
								if (field[hunter->y - 1][hunter->x] == 2 ) // if there is a hare 
								{
									loseGame(points);//hunter is dead , game ends
								}
								else
								{
									if (diffSelector == 0)
									{
										SetConsoleCursorPosition(handle, coord);
										SetConsoleTextAttribute(handle, 32);
										cout << char(24);
									} // draw hunter on new coords
									else if (diffSelector == 1)
										drawSector(hunter, field);
									else if (diffSelector == 2)
										drawCell(hunter, field);
								}
							}
							else //if there is a grave - remains in old coords
							{
								if (diffSelector == 0)
								{
									SetConsoleCursorPosition(handle, coord);
									SetConsoleTextAttribute(handle, 32);
									cout << char(24);
								}
								else if (diffSelector == 1)
									drawSector(hunter, field);
								else if (diffSelector == 2)
									drawCell(hunter, field);
							}

						}
						break;
					}
					case 77: //move right
					{
						if (hunter->dir == 1) //if turnrd up - turn right
						{
							if (diffSelector == 0)
							{
								SetConsoleCursorPosition(handle, coord);
								SetConsoleTextAttribute(handle, 34);
								cout << char(26);
							}
							
							else if (diffSelector == 1)
								hideSector(hunter, field);
							else if (diffSelector == 2)
								hideCell(hunter, field);
							hunter->dir = 0; // turning
							hunter->ap -= 1; // 1 action point used
							if (diffSelector == 0)
							{
								SetConsoleCursorPosition(handle, coord);
								SetConsoleTextAttribute(handle, 32);
								cout << char(26);
							}
							else if (diffSelector == 1)
								drawSector(hunter, field);
							else if (diffSelector == 2)
								drawCell(hunter, field);
							
						}
						if (hunter->dir == 0) // if turned right - move right
						{
							if (diffSelector == 0)
							{
								SetConsoleCursorPosition(handle, coord);
								SetConsoleTextAttribute(handle, 34);
								cout << char(26);
							}

							else if (diffSelector == 1)
								hideSector(hunter, field);
							else if (diffSelector == 2)
								hideCell(hunter, field);
							if (field[hunter->y][hunter->x + 1] != 3) // checks if there is no  grave
							{
								coord.X++; // moving
								hunter->x += 1;
								hunter->ap = 0; // action points ended
								if (field[hunter->y][hunter->x + 1] == 2 || field[hunter->y][hunter->x + 1] == 4) // if there is a hare or a trap
								{
									loseGame(points);//hunter is dead , game ends
								}
								else
								{
									if (diffSelector == 0)
									{
										SetConsoleCursorPosition(handle, coord);
										SetConsoleTextAttribute(handle, 32);
										cout << char(26); // draw hunter on new coords
									}
									else if (diffSelector == 1)
										drawSector(hunter, field);
									else if (diffSelector == 2)
										drawCell(hunter, field);
								}
							}
							else //if there is a grave - remains in old coords
							{
								if (diffSelector == 0)
								{
									SetConsoleCursorPosition(handle, coord);
									SetConsoleTextAttribute(handle, 32);
									cout << char(26); // draw hunter 
								}
								else if (diffSelector == 1)
									drawSector(hunter, field);
								else if (diffSelector == 2)
									drawCell(hunter, field);
							}

						}
						break;
					}
					default:
						break;
					}
				}
				else if (key != 224) // if not arrows
				{
					switch (key)
					{
					case 122: //shooting
					{
						PlaySound(TEXT("shoot.wav"), NULL, SND_ASYNC);
						hit = searchTarget(hunter, hare, grave, &hares, &graves, &deadHare); // checks hit
						hunter->ap = 0;
						if (hit) // если попал
						{
							dieMF(&hare, &grave, &hares, &graves, field, deadHare , diffSelector , &points);// hare is dead and new grave appears instead of it
							points += 10; // player points 
						}
						break;
					}
					case 120: // trap setting
					{
						addTrap(hunter, &trap, &traps, field);
						break;
					}
					case 99: // binocular
					{
						showLine(hunter,field);
						break;
					}
					case 27: //ESC - ingame menu
					{
						pauseMenu(field, hunter, hare, hares, grave, graves, trap, traps, points);
						break;
					}
					default:
						break;
					} // switch(key)
				} // if (key != 224)
			} // if _kbhit()
		}// hunter moves until he has action points
		haresMove(hunter,&hare, &hares, field, &grave, &graves , points , diffSelector);// it's hares turn to move :)
		drawTraps(trap, traps, field, hunter, points); // draws traps
	}//while(true...
	
}

void diffMenu(int* diffSelector)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 23,9 };
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 128);
	string diffMenus[] = { "Phew.." , "Sh*t!.." , "F*CK!!!" };
	*diffSelector = 0;
	int diffKey = 0;

	for (int i = 0; i < 3; i++)
	{
		SetConsoleCursorPosition(handle, coord);
		SetConsoleTextAttribute(handle, 128);
		cout << diffMenus[i];
		coord.Y ++;
	}
	coord.Y = 9;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 8);
	cout << diffMenus[0];
	coord.Y = 9;
	do {
		if (_kbhit())
			diffKey = _getch();
		if (diffKey == 224)
		{
			diffKey = _getch();
			switch (diffKey)
			{
			case 80:
			{
				if (*diffSelector < 2)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << diffMenus[*diffSelector];
					coord.Y ++;
					(*diffSelector)++;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << diffMenus[*diffSelector];
				}
				else if (*diffSelector == 2)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << diffMenus[*diffSelector];
					coord.Y = 9;
					*diffSelector = 0;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << diffMenus[*diffSelector];
				}
				break;
			}
			case 72:
			{
				if (*diffSelector > 0)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << diffMenus[*diffSelector];
					coord.Y --;
					(*diffSelector)--;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << diffMenus[*diffSelector];
				}
				else if (*diffSelector == 0)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << diffMenus[*diffSelector];
					coord.Y = 11;
					*diffSelector = 2;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << diffMenus[*diffSelector];
				}
				break;
			}
			default:
				break;
			}//switch (key)
		}

	} while (diffKey != 13);
	return;
}

void printAbout()
{
	system("color 07");
	system("cls");
	string source = "about.txt";
	ifstream sourceFile;
	sourceFile.open(source, ios_base::in);
	char tmp[100];
	if (sourceFile.is_open())
	{
		while (!sourceFile.eof())
		{
			sourceFile.getline(tmp, 99);
			cout << tmp << '\n';
		}
		sourceFile.close();
	}
	cout << endl << endl;
	system("pause");
	//return;
}

void logo()
{
	system("cls");
	system("color 40");
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 7,2 };
	int view[17][60] = {
	{ 0,1,1,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,0,0 },
	{ 0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0 },
	{ 0,0,0,1,0,0,0,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0 },
	{ 0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0 },
	{ 0,0,0,1,0,0,0,1,0,0,0,1,0,1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,1,1,1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,1,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,1,1,1,0,0,1,0,0,0,1,0,1,1,1,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,0,1,0,0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,0,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,1,0,0,0,1,0,1,0,0,1,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,1,1,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,1,0,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,1,1,1,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,1,0,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,1,0,0,0,1,0,0,1,1,0,1,1,0,1,1,0,0 },
	};
	
	
		for (int i = 0; i < 17; i++) {
			for (int j = 0; j < 60; j++) {
				coord.X = 10 + j;
				coord.Y = 2 + i;
				SetConsoleCursorPosition(handle, coord);
				SetConsoleTextAttribute(handle, 64);
				if (view[i][j] == 1) {
					cout << char(178);
				}
				else if (view[i][j] == 0) {
					cout << char(32);
				}
			}
		}
		do {} while (!_kbhit());
		PlaySound(TEXT("logo2.wav"), NULL, SND_ASYNC);
		for (int i = 0; i < 40; i++)
		{
			for (int j = 0; j < 41; j++)
			{
				coord.X =(rand()%40+1)*2;
				coord.Y =rand()%42+1;
				SetConsoleCursorPosition(handle, coord);
				SetConsoleTextAttribute(handle, 0);
				cout << char(32) << char(32);
				Sleep(1);
			}
		}
	SetConsoleTextAttribute(handle, 15);
		
}

void menu()
{
	
	system("cls");
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 5,5 };
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			coord.X = 5 + j;
			coord.Y = 5 + i;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 137);
			if (i == 0 && j == 0)
				cout << char(201);
			else if (i == 0 && j == 31)
				cout << char(187);
			else if (i == 14 && j == 0)
				cout << char(200);
			else if (i == 14 && j == 31)
				cout << char(188);
			else if (i == 0 || i == 14)
				cout << char(205);
			else if (j == 0 || j == 31)
				cout << char(186);
			else
				cout << char(32);
		}
	}
	string menus[] = { "About     " , "Difficulty" , "Start     ", "Load      " , "Records   " , "Exit      " };
	string diffs[] = { "Easy " , "Medium" , "Hard  " };
	static int diffSelector = 0; // difficulty level 0 -easy, 1 - mid , 2 hard (at start = 0 - easy)
	int field[30][60] = { 0 };
	int points = 0;
	coord.X = 11;
	coord.Y = 5;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	cout << " G A M E  M E N U ";
	coord.X = 10;
	coord.Y = 7;
	for (int i = 0; i < 6; i++)
	{		
		SetConsoleCursorPosition(handle, coord);
		SetConsoleTextAttribute(handle, 128);
		cout << menus[i];
		coord.Y += 2;
	}
	coord.X = 23;
	coord.Y = 9;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 137);
	cout << diffs[diffSelector];
	coord.X = 10;
	coord.Y = 7;
	SetConsoleCursorPosition(handle, coord);
	SetConsoleTextAttribute(handle, 8);
	cout << menus[0];
	int key = 0;
	int selector = 0;
	
	do {
		if (_kbhit())
			key = _getch();
		if (key == 224)
		{
			key = _getch();
			switch (key)
			{
			case 80:
			{
				if (selector < 5)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << menus[selector];
					coord.Y+=2;
					selector++;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << menus[selector];
				}
				else if (selector == 5)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << menus[selector];
					coord.Y = 7;
					selector = 0;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << menus[selector];
				}
				break;
			}
			case 72:
			{
				if (selector > 0)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << menus[selector];
					coord.Y-=2;
					selector--;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << menus[selector];
				}
				else if (selector == 0)
				{
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 128);
					cout << menus[selector];
					coord.Y = 17;
					selector = 5;
					SetConsoleCursorPosition(handle, coord);
					SetConsoleTextAttribute(handle, 8);
					cout << menus[selector];
				}
				break;
			}
			default:
				break;
			}//switch (key)
		}

	} while (key != 13);
	switch (selector)
	{
	case 0:
	{
		printAbout();
		menu();
		break;
	}
	case 1:
	{
		diffMenu(&diffSelector);
		menu();
		break;
	}
	case 2:
	{
		gameStart(diffSelector);
		break;
	}
	case 3:
	{
		gameLoad(field, points, diffSelector);
		break;
	}
	case 4:
	{
		showRecords();
		menu();
		break;
	}
	case 5:
	{
		system("cls");
		cout << "Thank you for playing!!!" << endl << endl;
		system("pause");
		exit(0);
		break;
	}
	default:
		break;
	}
	
}

void toRecords(int points) //adds result to records table
{
	system("cls");
	const int size = 10;
	PLAYER player[size];
	string source = "records.txt";
	string target = "records.txt";
	ifstream sourceFile;
	ofstream targetFile;
	sourceFile.open(source, ios_base::binary|ios_base::in);
	
	PLAYER current = { " ", 0 };//data of current player
	cout << " Enter yor name (up to 10 symbols) : ";
	cin >> current.name;
	current.score = points;
	if (sourceFile.is_open()) //reads from file if it exists
	{
		for (int i = 0; i < size; i++)
		{
			sourceFile >> player[i].name >> player[i].score;
		}
		sourceFile.close();
		
	}
	else // if not exists - fills file with blank data
	{
		targetFile.open(target, ios_base::binary|ios_base::out);
		for (int i = 0; i < size; i++)
		{
			strcpy_s(player[i].name , " none ");
			player[i].score = -1;
		}
		for (int i = 0; i < size; i++)
		{
			targetFile << player[i].name << endl << player[i].score << endl;
		}
		targetFile.close();
	}
	if (current.score >= player[size - 1].score) // if current score bigger or equal then last element in table
	{
		strcpy_s(player[size - 1].name, current.name); //assigns current result to last element of record list
		player[size - 1].score = current.score;
		for (int i = 0; i < size - 1; i++) // sorting elements with bubble sort ;)
		{
			for (int j = 0; j < i; j++)
			{
				if (player[j].score < player[j + 1].score)
					swap(player[j], player[j + 1]);
			}
		}
		targetFile.open(target, ios_base::binary | ios_base::out); //rewrite data to file
		for (int i = 0; i < size; i++)
		{
			targetFile << player[i].name << player[i].score;
		}
		targetFile.close();
	}
	else
		cout << "Sorry, you gained too little score for record table..." << endl;
	system("pause");
	showRecords();
}

void showRecords()//shows records
{
	system("cls");
	const int size = 10;
	PLAYER player[size];
	string source = "records.txt";
	string target = "records.txt";
	ifstream sourceFile;
	
	sourceFile.open(source, ios_base::binary | ios_base::in);
	
	if (sourceFile.is_open() )//reading records from file
	{
		for (int i = 0; i < size; i++)
		{
			sourceFile >> player[i].name;
			sourceFile >> player[i].score;
		}
		sourceFile.close();

	}
	else // if file doesn't exist
	{
		for (int i = 0; i < size; i++)
		{
			strcpy_s(player[i].name ," none ");
			player[i].score = -1;
		}
		ofstream targetFile;
		targetFile.open(target, ios_base::binary | ios_base::out);
		for (int i = 0; i < size; i++)
		{
			targetFile << player[i].name << endl << player[i].score << endl;
		}
		targetFile.close();
	}
	cout << endl << endl << endl;
	cout << '\t' << "    T A B L E  O F  R E C O R D S" << endl;
	cout << '\t' << '\t' << char(201) << char(205) << char(205) << char(203) << char(205) << char(205) << char(205) << char(205) << char(205) << char(205) << char(205) << char(205) << char(205) << char(205) << char(203 ) << char(205) << char(205) << char(205) << char(205) << char(187) << endl;
	for (int i = 0; i < size; i++)
	{
		cout << '\t' << '\t' << char(186) << setw(2) << i+1 << char(186) << setw(10) << player[i].name << char(186) << setw(4) << player[i].score << char(186) << endl;
	}
	cout << '\t' << '\t' << char(200) << char(205) << char(205) << char(202) << char(205) << char(205) << char(205) << char(205) << char(205) << char(205) << char(205) << char(205) << char(205) << char(205) << char(202) << char(205) << char(205) << char(205) << char(205) << char(188) << endl;
	cout << endl << endl;
	system("pause");
	menu();
}

void loseGame(int points)
{
	system("cls");
	system("color 40");
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 7,2 };
	int view[17][60] = {
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0 },
	{ 0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0 },
	{ 0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	};


	for (int i = 0; i < 17; i++) {
		for (int j = 0; j < 60; j++) {
			coord.X = 10 + j;
			coord.Y = 2 + i;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 64);
			if (view[i][j] == 1) {
				cout << char(178);
			}
			else if (view[i][j] == 0) {
				cout << char(32);
			}
		}
	}
	do {} while (!_kbhit());
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 41; j++)
		{
			coord.X = (rand() % 40 + 1) * 2;
			coord.Y = rand() % 41 + 1;
			SetConsoleCursorPosition(handle, coord);
			SetConsoleTextAttribute(handle, 0);
			cout << char(32) << char(32);
			Sleep(1);
		}
	}
	SetConsoleTextAttribute(handle, 15);

	
	toRecords( points);
}

void main()
{
	setlocale(LC_ALL, "UTF8");
	system("mode con cols=80 lines=42");
	
	srand(time(0));
	logo();
	_getch();
	menu();
	_getch();
	
		
	
}//main











