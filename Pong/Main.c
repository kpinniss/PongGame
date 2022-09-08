#include <stdio.h>
#include <SDL.h>
#include "Constants.h"
#include <stdbool.h>

SDL_Window* _window = NULL;
SDL_Renderer* _rendr = NULL;
bool _gameIsRunning = false;
int last_frame_time = 0;

struct gameObject {
	float xpos;
	float ypos;
	float width;
	float height;
	float vel_x;
	float vel_y;
} _ball,_paddle;

//Initialize SDL and Window
bool initWindow(void)
{
	//init SDL
	printf("Initializing SDL...\n");
	if (SDL_Init(SDL_INIT_EVERYTHING) == FAILED_INIT)
	{
		fprintf(stderr,"Error initializing SDL.\n");
		return false;
	}

	//create window
	printf("Creating window...\n");
	_window = SDL_CreateWindow(
		WINDOW_TITLE,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_ALWAYS_ON_TOP
	);

	//create renderer
	printf("Creating renderer...\n");
	_rendr = SDL_CreateRenderer(
		_window,
		DEFAULT_DISPLAY_DRIVERS, 
		0
	);

	//check if window was created successfully
	if (!_window)
	{
		fprintf(stderr, "Error creating SDL window SDL.\n");
		return false;
	}
	
	//check renderer
	if (!_rendr)
	{
		fprintf(stderr, "Error creating SDL renderer SDL.\n");
		return false;
	}

	printf("window initilized!\n");
	return true;
}

processInput()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	SDL_KeyCode key;
	key = event.key.keysym.sym;
	switch (event.type)
	{
	case SDL_QUIT:
		stopGame();//triggered by (x) close button on window
		break;
	case SDL_KEYDOWN:
		if (key == SDLK_RIGHT)
		{
			_paddle.vel_x = 400;
		}
		if (key == SDLK_LEFT)
		{
			_paddle.vel_x = -400;
		}
		if (key == SDLK_ESCAPE)
		{
			stopGame();
		}
		break;
	case SDL_KEYUP:
		if (key == SDLK_RIGHT || key == SDLK_LEFT)
		{
			_paddle.vel_x = 0;
		}
		break;
	}
}

update()
{
	//wait until target frame time id reached
	//*** removed to save CPU usage (while loop causes high cpu usage) replaced with delay function ***
	//while (SDL_TICKS_PASSED(!SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME)); //returns true if A has passed b
	int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
	//Only call delay if we are too fast to process this frame
	if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME)
	{
		SDL_Delay(timeToWait);
	}

	//deltaTime
	float deltaTime = ((SDL_GetTicks() - last_frame_time) / 1000.0f);

	//logic to keep fixed time step (deltaTime)
	last_frame_time = SDL_GetTicks();

	_ball.xpos += _ball.vel_x * deltaTime;
	_ball.ypos += _ball.vel_y * deltaTime;

	//TODO
	//udpate paddle based on velocity DONE**
	_paddle.xpos += _paddle.vel_x * deltaTime;
	//check for ball collisions (paddle and walls)
	printf("XPOS: ");
	printf("%.6f",_ball.xpos);
	printf("\n");
	//_ball - wall collisions
	if (_ball.xpos >= WINDOW_WIDTH - _ball.width)
	{
		flip(&_ball.vel_x);
	}
	if (_ball.xpos <= (0 - _ball.width))
	{
		flip(&_ball.vel_x);
	}
	if (_ball.ypos <= (0 - _ball.height))
	{
		flip(&_ball.vel_y);
	}

	//ball - paddle collisions
	if ((_ball.ypos) >= (_paddle.ypos - _paddle.height)
		&& (_ball.xpos >= _paddle.xpos 
			&& _ball.xpos <= (_paddle.xpos + _paddle.width)))
	{
		/*flip(&_ball.vel_x);*/
		flip(&_ball.vel_y);
	}

	//check for paddle collisions on walls
	if (_paddle.xpos >= (WINDOW_WIDTH - _paddle.width))
	{
		_paddle.vel_x = 0;
		_paddle.xpos = (WINDOW_WIDTH - _paddle.width);
	}
	if (_paddle.xpos <= 0)
	{
		_paddle.vel_x = 0;
		_paddle.xpos = 0;
	}
	//check for game over when ball leaves screen (bottom)
	if (_ball.ypos > WINDOW_HEIGHT + _ball.height)
	{
		stopGame();
	}
}

flip(float *num)
{
	*num = *num * (-1);
}

render()
{
	//render window color black
	SDL_SetRenderDrawColor(_rendr, 0,0,0,255);
	//clear renderer to render other things
	SDL_RenderClear(_rendr);

	//Draw
	//Draw ball
	SDL_Rect ball = { 
		(int)_ball.xpos,
		(int)_ball.ypos,
		(int)_ball.width,
		(int)_ball.height
	};
	//set new color for renderer
	SDL_SetRenderDrawColor(_rendr, 255, 0, 0, 255);
	//fill ball with color
	SDL_RenderFillRect(_rendr, &ball);

	//Draw paddle
	SDL_Rect paddle = {
		_paddle.xpos,
		_paddle.ypos,
		_paddle.width,
		_paddle.height
	};
	//set new color for renderer
	SDL_SetRenderDrawColor(_rendr, 255, 0, 0, 255);
	//fill ball with color
	SDL_RenderFillRect(_rendr, &paddle);

	//present renders
	SDL_RenderPresent(_rendr); //buffer swap
}

setup()
{
	//set ball props
	_ball.xpos = 20;
	_ball.ypos = 20;
	_ball.width = 15;
	_ball.height = 15;
	_ball.vel_x = BASE_SPEED;
	_ball.vel_y = BASE_SPEED;

	//set paddle props
	_paddle.width = 150;
	_paddle.height = 20;
	_paddle.xpos = (WINDOW_WIDTH / 2) - (_paddle.width / 2);
	_paddle.ypos = (WINDOW_HEIGHT - 20);
	_paddle.vel_x = 0;
	_paddle.vel_y = 0;
}

destroyWindow()
{
	SDL_DestroyRenderer(_rendr);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

stopGame()
{
	printf("Game Over!");
	_gameIsRunning = false;
}

int main(int arg, char* args[])
{
	_gameIsRunning = initWindow();
	setup();
	while (_gameIsRunning)
	{
		processInput();
		update();
		render();
	}
	return 0;
}