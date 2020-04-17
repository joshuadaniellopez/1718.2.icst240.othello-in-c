/*
	Filename        : 	Othelloclient.c
  	File Modified by: 	CAGAMPANG, Rashiel James
						LOPEZ, Joshua Daniel
						STA. ANA, Levi John
						VELASCO, Dave
  	Last Modified   :	3.9.2018
  	Description     :	Othello in C
	Honor Code      :	Some codes are of our own, except Sockets(It is based on Sir Glenn's code) 
						We have not received any        
						unauthorized help in completing this work. I have not  
						copied from my classmate, friend, nor any unauthorized 
						resource. I am well aware of the policies stipulated   
						in the handbook regarding academic dishonesty.          
						If proven guilty, I won't be credited any points for   
						this exercise. 
*/

//Libraries

//printf()
#include <stdio.h>
//malloc(), free()
#include <stdlib.h>
//client server libraries
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

//Constants

#define true 1
#define false 0

#define blank ' '
#define black 'X'
#define white 'O'

#define boardSizeX 8
#define boardSizeY 8

//Structures and Custom Type Definitions

typedef char bool;

struct OthelloGame
{
	int turn, boardSize;
	char** board; // Visible Game Board
};

struct Coordinates
{
	int x, y, axis;
};

//Debugger Variables

bool printLoopTraces = false;
bool printCoordinateTraces = false;

//Function Prototypes

struct OthelloGame NewGame();

int PlacePiece(struct OthelloGame _currentGame, int _x, int _y);

struct OthelloGame PlayTurn(struct OthelloGame _currentGame, int _x, int _y);

void CheckPoint(struct OthelloGame _currentGame, struct Coordinates _origin);

void CheckAxis(struct OthelloGame _currentGame, struct Coordinates _origin);

void DisplayGame(struct OthelloGame _currentGame);

void ScoreGame(struct OthelloGame _currentGame);

void EndGame(struct OthelloGame _currentGame);

void ExitGame(struct OthelloGame _currentGame);

//Main Driver

int main(int argc, char* argv[])
{
	struct OthelloGame currentGame = NewGame();
	bool gameOver = false;
	int moveX, skips = 0;
	char endGame, previousPiece, moveY, * endPointer;
	/*
	DisplayGame(currentGame);
	ScoreGame(currentGame);
	currentGame = PlayTurn(currentGame, 2, 4);
	DisplayGame(currentGame);
	ScoreGame(currentGame);
	currentGame = PlayTurn(currentGame, 2, 5);
	DisplayGame(currentGame);
	ScoreGame(currentGame);
	currentGame = PlayTurn(currentGame, 3, 5);
	DisplayGame(currentGame);
	ScoreGame(currentGame);
	currentGame = PlayTurn(currentGame, 4, 5);
	DisplayGame(currentGame);
	ScoreGame(currentGame);
	*/
	int client_sock,  port_no,  n;
    struct sockaddr_in server_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
        //printf("Usage: %s hostname port_no",  argv[0]);
        exit(1);
    }

    //printf("Client starting ...\n");
    // Create a socket using TCP
    client_sock = socket(AF_INET,  SOCK_STREAM,  0);
    if (client_sock < 0) 
        die_with_error("Error: socket() Failed.");

    //printf("Looking for host '%s'...\n", argv[1]);
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        die_with_error("Error: No such host.");
    }
    //printf("Host found!\n");

    // Establish a connection to server
    port_no = atoi(argv[2]);
    bzero((char *) &server_addr,  sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,  
         (char *)&server_addr.sin_addr.s_addr, 
         server->h_length);
         
    server_addr.sin_port = htons(port_no);

    //printf("Connecting to server at port %d...\n", port_no);
    if (connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
        die_with_error("Error: connect() Failed.");

    //printf("Connection successful!\n");
	while (!gameOver)
	{
		DisplayGame(currentGame);
		ScoreGame(currentGame);
		
		
		//server first
		if(currentGame.turn % 2 == 0)
		{
			
			//printf("Message sent! Awaiting reply ...\n");
			bzero(buffer, 256);
			
			n = recv(client_sock, buffer, 255, 0);
			if (n < 0) 
				 die_with_error("Error: recv() Failed.");
			//printf("Server says : %s\n", buffer);
			//printf("[server] > %s\n", buffer);
			//printf("[server] > %s", buffer);
		}
		else
		{

			
			//printf("Please type a message : ");
			//printf("< ");
			printf("WhitePlayer(X) Input: ");
			fgets(buffer, 255, stdin);
			//printf("Sending message to server ...\n");
				
			n = send(client_sock, buffer, strlen(buffer), 0);
			if (n < 0) 
				 die_with_error("Error: send() Failed.");
		}
		
		moveX = (int)buffer[2] - 48;
		moveY = buffer[0];
		//printf("%d %d", moveX, moveY);
		//scanf(" %c %d", &moveY, &moveX);
		
		
		//Need to ignore newline character.
		int convertedX, convertedY;
		convertedX = moveX - 1;
		convertedY = (int) moveY;
		if (moveY < 97)
		{
			//Capital Letter Input (65=A,72=H)
			convertedY -= 65;
		}
		else
		{
			//Lower Case Letter Input (97=a,104=h)
			convertedY -= 97;
		}
		if (printCoordinateTraces)
			printf("Input Y X (%c, %d) -> Result X Y (%d, %d)\n", moveY, moveX, convertedX, convertedY);
		previousPiece = currentGame.board[convertedY][convertedX];
		currentGame = PlayTurn(currentGame, convertedX, convertedY);
		if (currentGame.board[convertedY][convertedX] == previousPiece){
			skips++;
			printf("Illegal Move!\n");
		}
		else
			skips = 0;
		if (skips > 1)
			gameOver = true;
		//scanf(" %c", &moveY);
		
	}
	ScoreGame(currentGame);
	ExitGame(currentGame);
	return 0;
}

//Function Definitions

struct OthelloGame NewGame()
{
	struct OthelloGame newGame;
	//Set Turns
	newGame.turn = 0;
	//Set Board
	newGame.boardSize = boardSizeX * boardSizeY;
	newGame.board = (char**) malloc(sizeof(char*) * boardSizeY);
	int centerX = boardSizeX / 2, centerY = boardSizeY / 2, i, j;
	for (i = 0; i < boardSizeY; ++i)
	{
		newGame.board[i] = (char*) malloc(sizeof(char) * boardSizeX);
		for (j = 0; j < boardSizeX; ++j)
		{
			newGame.board[i][j] = blank;
		}
	}
	newGame.board[centerY - 1][centerX - 1] = black;
	newGame.board[centerY - 1][centerX] = white;
	newGame.board[centerY][centerX - 1] = white;
	newGame.board[centerY][centerX] = black;
	return newGame;
}

struct OthelloGame PlayTurn(struct OthelloGame _currentGame, int _x, int _y)
{
	int result = PlacePiece(_currentGame, _x, _y);
	_currentGame.turn++;
	return _currentGame;
}

int PlacePiece(struct OthelloGame _currentGame, int _x, int _y)
{
	char currentPiece = _currentGame.board[_y][_x];
	if (currentPiece != blank)
		return 1; //Error: Occupied Square
	else if (!WithinBounds(_x, _y))
		return 2; //Error: Out of Bounds
	else
	{
		struct Coordinates origin;
		origin.x = _x;
		origin.y = _y;
		if (printLoopTraces)
			printf("PlacePiece at (%d, %d)\n", origin.x, origin.y);
		CheckPoint(_currentGame, origin);
		return 0;
	}
}

void CheckPoint(struct OthelloGame _currentGame, struct Coordinates _origin)
{
	int i = 0;
	for (i = 0; i < 4; ++i)
	{
		_origin.axis = i;
		CheckAxis(_currentGame, _origin);
	}
	char origin = _currentGame.board[_origin.y][_origin.x];
	if (origin == blank)
	{
		//No flanks.
	}
	else
	{
		//Flanks available.
	}
}

void CheckAxis(struct OthelloGame _currentGame, struct Coordinates _origin)
{
	int xIncrement, yIncrement;
	int xCurrent = _origin.x, yCurrent = _origin.y;
	char originPiece = (_currentGame.turn % 2 == 0) ? black : white;
	char opponentPiece = (_currentGame.turn % 2 == 0) ? white : black;
	char currentCharacter;
	if (printLoopTraces)
	{
		printf("Checking Axis %d\n", _origin.axis);
		printf("Origin Piece: '%c'\n", originPiece);
		printf("Opponent Piece: '%c'\n", opponentPiece);
		printf("Read from Board: %c\n", _currentGame.board[yCurrent][xCurrent]);
		printf("Current Coordinates: (%d, %d)\n", xCurrent, yCurrent);
	}
	//Get Axis Increments
	if (_origin.axis == 0) //North
	{
		xIncrement = 0;
		yIncrement = -1;
	}
	else if (_origin.axis == 1) //North-East
	{
		xIncrement = 1;
		yIncrement = -1;
	}
	else if (_origin.axis == 2) //East
	{
		xIncrement = 1;
		yIncrement = 0;
	}
	else //South-East
	{
		xIncrement = 1;
		yIncrement = 1;
	}
	//Check half of the current axis.
	//While the currently checked piece is an opponent piece...
	int flankCount = 0;
	do
	{
		xCurrent += xIncrement;
		yCurrent += yIncrement;
		if (WithinBounds(xCurrent, yCurrent))
		//If the next piece's coordinates are within the playable board...
		{
			currentCharacter = _currentGame.board[yCurrent][xCurrent];
			++flankCount;
			if (printLoopTraces)
			{
				printf("Iterating Flank Counter 1: Current Character '%c'\n", currentCharacter);
				printf("Read from Board: %c\n", _currentGame.board[yCurrent][xCurrent]);
				printf("Current Coordinates: (%d, %d)\n", xCurrent, yCurrent);
			}
		}
		else
			break;
	}
	while (currentCharacter == opponentPiece);
	if (currentCharacter == originPiece && WithinBounds(xCurrent, yCurrent))
	//Flanking commences here...
	{
		int i;
		bool legal = false;
		for (i = 0; i < flankCount; i++)
		{
			yCurrent -= yIncrement;
			xCurrent -= xIncrement;
			if (printLoopTraces)
				printf("Flipping Piece...\n");
			if (_currentGame.board[yCurrent][xCurrent] == opponentPiece) //This condition needs fixing.
			{
				_currentGame.board[yCurrent][xCurrent] = originPiece;
				legal = true;
			}
		};
		if (legal == true)
			_currentGame.board[_origin.y][_origin.x] = originPiece;
	}
	//Check other half of the current axis.
	//While the currently checked piece is an opponent piece...
	flankCount = 0;
	xCurrent = _origin.x;
	yCurrent = _origin.y;
	do
	{
		xCurrent -= xIncrement;
		yCurrent -= yIncrement;
		if (WithinBounds(xCurrent, yCurrent))
		//If the next piece's coordinates are within the playable board...
		{
			currentCharacter = _currentGame.board[yCurrent][xCurrent];
			++flankCount;
			if (printLoopTraces)
			{
				printf("Iterating Flank Counter 2: Current Character '%c'\n", currentCharacter);
				printf("Read from Board: %c\n", _currentGame.board[yCurrent][xCurrent]);
				printf("Current Coordinates: (%d, %d)\n", xCurrent, yCurrent);
			}
		}
		else
			break;
	}
	while (currentCharacter == opponentPiece);
	if (currentCharacter == originPiece && WithinBounds(xCurrent, yCurrent))
	//Flanking commences here...
	{
		int i;
		bool legal = false;
		for (i = 0; i++ < flankCount; _currentGame.board[yCurrent][xCurrent] = originPiece)
		{
			xCurrent += xIncrement;
			yCurrent += yIncrement;
			if (printLoopTraces)
				printf("Flipping Piece...\n");
			if (_currentGame.board[yCurrent][xCurrent] == opponentPiece)
			{
				_currentGame.board[yCurrent][xCurrent] = originPiece;
				legal = true;
			}
		}
		if (legal == true)
			_currentGame.board[_origin.y][_origin.x] = originPiece;
	}
}

int WithinBounds(int _x, int _y)
{
	if (_x >= 0 && _x < boardSizeX && _y >= 0 && _y < boardSizeY)
		return true;
	return false;
}

void DisplayGame(struct OthelloGame _currentGame)
{
	int leftMargin = 2, currentTurn = _currentGame.turn + 1, i, j;
	printf("---------------\n");
	printf("Turn Number %d\n", currentTurn);
	if (_currentGame.turn % 2 == 0)
		printf("Black's Turn\n");
	else
		printf("White's Turn\n");
	printf("---------------\n");
	printf("Current State:\n");
	printf("---------------\n");
	for (i = 0; i <= boardSizeY; ++i)
	{
		if (i != boardSizeY)
		{
			for (j = -leftMargin - 1; j < boardSizeX; ++j)
			{
				if (j < -1)
					printf(" ");
				else if (j > -1)
					printf("%c|", _currentGame.board[i][j]);
				else
					printf("%c|", (char) (i + 65));
			}
			printf("\n");
			printf("    - - - - - - - - \n");
		}
		else
		{
			for (j = -leftMargin - 1; j < boardSizeX; ++j)
			{
				if (j < -1)
					printf(" ");
				else if (j > -1)
					printf("%c ", (char) (j + 49));
				else
					printf("%c ", blank);
			}
			printf("\n");
		}
	}
	printf("---------------\n");
}

void ScoreGame(struct OthelloGame _currentGame)
{
	int blackScore = 0, whiteScore = 0, i, j;
	char currentPiece;
	for (i = 0; i < boardSizeY; ++i)
		for (j = 0; j < boardSizeX; ++j)
		{
			currentPiece = _currentGame.board[i][j];
			if (currentPiece == black)
				++blackScore;
			else if (currentPiece == white)
				++whiteScore;
		}
	printf("---------------\n");
	printf("  Black : %d\n", blackScore);
	printf("  White : %d\n", whiteScore);
	printf("  Turn  : %d\n", _currentGame.turn);
	printf("---------------\n");
}

void EndGame(struct OthelloGame _currentGame)
{
	int blackScore = 0, whiteScore = 0, i, j;
	char currentPiece;
	for (i = 0; i < boardSizeY; ++i)
		for (j = 0; j < boardSizeX; ++j)
		{
			currentPiece = _currentGame.board[i][j];
			if (currentPiece == black)
				++blackScore;
			else if (currentPiece == white)
				++whiteScore;
		}
	if (blackScore > whiteScore)
	{
		//Black Victory
	}
	else if (blackScore < whiteScore)
	{
		//White Victory
	}
	else
	{
		//Draw
	}
}

void ExitGame(struct OthelloGame _currentGame)
{
	int i;
	for (i = 0; i < boardSizeY; ++i)
		free(_currentGame.board[i]);
	free(_currentGame.board);
}


void die_with_error(char *error_msg){
    printf("%s", error_msg);
    exit(-1);
}

