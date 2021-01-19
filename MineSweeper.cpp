#include<iostream>
#include <windows.h>
#include<unistd.h>
#include<ctime>
#include<thread>

using namespace std;

struct Mine
{
	int **box;
	int **helperBox;
	int row;
	int column;
	int bomb;
	int **bombCoord;
	int flagCount;
	int sec = 0;
	int min = 0;
	int hour = 0;
};

int timer(struct Mine*);
int construction(struct Mine*);
int plantBomb(struct Mine*);
int display(struct Mine*);
int play(struct Mine*);
int reveal(struct Mine*, int, int, int);
void explore(struct Mine*, int, int);
boolean isCompleted(struct Mine*);
int displayMines(struct Mine*);

// Used to display welcome screen at the center of console

void gotoxy (int x, int y)
{
    COORD coordinates;
    coordinates.X = x;
    coordinates.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coordinates);
}

int main()
{
	struct Mine mine;
	
	gotoxy(40, 13);
		
	string w = "Hey there! Can you complete the minesweeper :)";
	
	for(int iter = 0; iter<46; iter++)
	{
		cout<<w[iter];
		usleep(100000);
	}
	
	usleep(999999);
	
	system("cls");
	
	int difficulty;
	
	cout<<"\n1. Easy (Size: 8x10, Mines: 10)";
	cout<<"\n2. Medium (Size: 14x18, Mines: 40)";
	cout<<"\n3. Hard (Size: 20x24, Mines: 99)";
	cout<<"\nEnter your option: ";
	cin>>difficulty;
	
	switch(difficulty)
	{
		case 1:
			mine.row = 8;
			mine.column = 10;
			mine.bomb = 10;
			mine.flagCount = 10;
			construction(&mine);
			break;
			
		case 2:
			mine.row = 14;
			mine.column = 18;
			mine.bomb = 40;
			mine.flagCount = 40;
			construction(&mine);
			break;
			
		case 3:
			mine.row = 20;
			mine.column = 24;
			mine.bomb = 99;
			mine.flagCount = 99;
			construction(&mine);
			break;
			
		default:
			cout<<"\nIllegal Value";
			cout<<"\nRestarting: ";
			usleep(999999);
			system("cls");
			main();
			break;
	}
	
	plantBomb(&mine);
	
	std::thread t(timer, &mine);
	t.detach();
	
	play(&mine);
	
	return 0;
}

int timer(struct Mine *mine)
{
	while(true)
	{
		mine->sec++;
		sleep(1);
		if(mine->sec == 60)
		{
			mine->min++;
			mine->sec = 0;
			
			if(mine->min == 60)
			{
				mine->hour++;
				mine->min = 0;
			}
		}
	}
	
	return 0;
}

int construction(struct Mine *mine)
{
	mine->box = new int*[mine->row];
	mine->helperBox = new int*[mine->row];
		
	for(int row=0; row<mine->row; row++)
	{
		mine->box[row] = new int[mine->column];
		mine->helperBox[row] = new int[mine->column];
		for(int col=0; col<mine->column; col++)
		{
			mine->box[row][col] = 0;
			mine->helperBox[row][col] = 0;
		}
	}
	
	return 0;
}

int plantBomb(struct Mine *mine)
{
	int prevX = -1, prevY = -1;
	mine->bombCoord = new int*[mine->bomb];
	
	srand(time(0));
	for(int count=0; count<mine->bomb; )
	{
		mine->bombCoord[count] = new int[2];
		int x = rand()%mine->row;
		int y = rand()%mine->column;
		if(x == prevX && y == prevY)
		{
			int limitX, limitY;
			if(x>0)
			{
				x--;
				limitX = x+3;
			}
			else
			{
				limitX = x+2;
			}
			if(y>0)
			{
				y--;
				limitY = y+3;
			}
			else
			{
				limitY = y+2;
			}
			for(int row=x; row<limitX && row<mine->row; row++)
			{
				for(int col=y; col<limitY && col<mine->column; col++)
				{
					if(mine->box[row][col] != -1)
					{
						x = row;
						y = col;
						row = mine->row;
						col = mine->column;
					}	
				}
			}
		}
		if(mine->box[x][y] != -1)
		{
			mine->box[x][y] = -1;
			mine->bombCoord[count][0] = x;
			mine->bombCoord[count][1] = y;
			prevX = x;
			prevY = y;
			count++;
			int limitX, limitY;
			if(x>0)
			{
				x--;
				limitX = x+3;
			}
			else
			{
				limitX = x+2;
			}
			if(y>0)
			{
				y--;
				limitY = y+3;
			}
			else
			{
				limitY = y+2;
			}
			for(int row=x; row<limitX && row<mine->row; row++)
			{
				for(int col=y; col<limitY && col<mine->column; col++)
				{
					if(mine->box[row][col] != -1)
					{
						mine->box[row][col]++;
					}	
				}
			}
		}
	}
	
	return 0;
}

int display(struct Mine *mine)
{	
	system("cls");
	usleep(999999);
	
	cout<<"\n\nFlag: "<<mine->flagCount<<"\n\n";
	
	for(int border=0; border<mine->column; border++)
	{
		cout<<border<<"\t";	
	}
	
	cout<<"\n";
	
	for(int border=0; border<mine->column; border++)
	{
		cout<<"|\t";	
	}
	
	cout<<"\n";
		
	for(int border=0; border<mine->column; border++)
	{
		cout<<"********";	
	}
	
	cout<<"*";
	
	cout<<"\n";
	
	for(int row=0; row<mine->row; row++)
	{
		for(int col=0; col<mine->column; col++)
		{
			if(mine->helperBox[row][col] == 0)
			{
				if(col == 0)
				{
					cout<<"*";
				}
				cout<<"   -   ";
				if(col != mine->column-1)
				{
					cout<<"|";
				}
				if(col == mine->column-1)
				{
					cout<<"*--"<<row;
				}
			}
			else if(mine->helperBox[row][col] == 2)
			{
				if(col == 0)
				{
					cout<<"*";
				}
				cout<<"   F   ";
				if(col != mine->column-1)
				{
					cout<<"|";
				}
				if(col == mine->column-1)
				{
					cout<<"*--"<<row;
				}
			}
			else
			{
				if(mine->box[row][col] == -1)
				{
					if(col == 0)
					{
						cout<<"*";
					}
					cout<<"   *   ";
					if(col != mine->column-1)
					{
						cout<<"|";
					}
					if(col == mine->column-1)
					{
						cout<<"*--"<<row;
					}
				}
				else
				{
					if(col == 0)
					{
						cout<<"*";
					}
					if(mine->box[row][col] == 0)
					{
						cout<<" "<<"\t";	
					}
					else
					{
						cout<<"   "<<mine->box[row][col]<<"   ";
					}
					if(col != mine->column-1)
					{
						cout<<"|";
					}
					else
					{
						cout<<"*--"<<row;
					}
				}
			}
		}
		
		cout<<"\n";
	}
	
	for(int border=0; border<mine->column; border++)
	{
		cout<<"********";	
	}
	
	cout<<"*";
	
	cout<<"\n";
	
	for(int border=0; border<mine->column; border++)
	{
		cout<<"|\t";	
	}
	
	cout<<"\n";
	
	for(int border=0; border<mine->column; border++)
	{
		cout<<border<<"\t";	
	}
	
	cout<<"\n\n";
	
	return 0;
}

int play(struct Mine *mine)
{
	while(true)
	{	
		if(isCompleted(mine))
		{
			cout<<"\n\nWoah! You have won... :)";
			cout<<"\nYou took exactly "<<mine->hour<<" hours "<<mine->min<<" minutes "<<mine->sec<<" seconds ";
			return 0;	
		}
		
		int x, y, flag;
		
		cout<<"\nEnter the X coordinate: ";
		cin>>x;
		cout<<"Enter the Y coordinate: ";
		cin>>y;
		
		if(x<0 || y<0 || x>=mine->row || y>=mine->column)
		{
			cout<<"\nInvalid Coordinates...";
			usleep(999999);
			continue;
		}
		
		cout<<"\n0. Open";
		cout<<"\n1. Flag/Unflag";
		cout<<"\nEnter your option: ";
		cin>>flag;
		
		if(flag != 0 && flag != 1)
		{
			cout<<"\nInvalid Option...";
			usleep(999999);
			continue;
		}
		
		int result = reveal(mine, x, y, flag);
		
		if(result == 0)
		{
			displayMines(mine);
			display(mine);
			cout<<"\nGame Over :(";
			return 0;
		}
		
		display(mine);
	}
}

int reveal(struct Mine *mine, int x, int y, int flag)
{
	if(flag == 1)
	{
		if(mine->helperBox[x][y] != 2)
		{
			if(mine->flagCount == 0)
			{
				cout<<"\nYou don't have flags anymore...";
				usleep(999999);
			}
			else
			{
				if(mine->helperBox[x][y] == 0)
				{
					mine->flagCount--;
					mine->helperBox[x][y] = 2;
				}
				else
				{
					cout<<"You cannot flag a openned tile...";
					usleep(999999);
				}
			}
		}
		else
		{
			mine->flagCount++;
			mine->helperBox[x][y] = 0;
		}
		
		return 1;
	}
	else
	{
		if(mine->helperBox[x][y] > 0)
		{
			cout<<"\nRequested cell has already been openned or flagged...";
			usleep(999999);
			
			return 1;	
		}
	}
	
	if(mine->box[x][y] == -1 && flag == 0)
	{
		return 0;
	}
	
	if(flag == 0)
	{
		explore(mine, x, y);
	}
	
	return 1;
}

void explore(struct Mine *mine, int x, int y)
{
	if(x<0 || y<0 || x>=mine->row || y>=mine->column)
	{
		return;
	}
	
	if(mine->box[x][y] == -1 || mine->helperBox[x][y] > 0)
	{
		return;
	}
	
	mine->helperBox[x][y] = 1;
	
	if(mine->box[x][y] > 0)
	{
		return;
	}
	
	explore(mine, x-1, y-1);	// Top left
	explore(mine, x-1, y);      // Top
	explore(mine, x-1, y+1);	// Top Right
	explore(mine, x, y-1);      // Left
	explore(mine, x, y+1);	    // Right
	explore(mine, x+1, y-1);	// Bottom Left
	explore(mine, x+1, y);	    // Bottom
	explore(mine, x+1, y+1);	// Bottom Right
	
	return;
}

boolean isCompleted(struct Mine *mine)
{
	for(int row=0; row<mine->row; row++)
	{
		for(int col=0; col<mine->column; col++)
		{
			if(mine->helperBox[row][col] == 0)
			{
				return 0;
			}
		}
	}
	
	return 1;
}

int displayMines(struct Mine *mine)
{
	for(int count=0; count<mine->bomb; count++)
	{
		mine->helperBox[mine->bombCoord[count][0]][mine->bombCoord[count][1]] = 1;
	}
	
	return 0;
}
