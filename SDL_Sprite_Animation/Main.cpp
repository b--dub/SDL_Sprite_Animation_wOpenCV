//  Under The Sea Game with OpenCV and SDL2.0
//  author: Brad Walsh


#include "Main.h"


int main(int argc, char* argv[]) {
	
	srand(time(0));
	
	// Video cap init
	cv::VideoCapture cap(0);
	if( !cap.isOpened() ) {
		std::cout << "ERROR  -  VideoCapture was not opened!!" << std::endl;
		return -1;
	}
	
	
	
	//  Mixer INITs
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "Init Error: \n" << SDL_GetError() << std::endl;
		return 1;
	}
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 0);
	Mix_Music *music;
	music = Mix_LoadMUS("MUSIC_underwater-bubbles.wav"); Mix_VolumeMusic(28);
	Mix_PlayMusic(music, -1);

	//  SDL and Image INITs
	window = SDL_CreateWindow("Under the Sea!!    C:Collide  S:Speak  F:FullScreen  M:ShowMotion", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cout << "ERROR - failure at SDL_CreateWindow() \n" << SDL_GetError() << std::endl;
		return 2;
	}	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		std::cout << "ERROR - failure at SDL_CreateRenderer() \n" << SDL_GetError() << std::endl;
		return 3;
	}
	SDL_Texture *background = nullptr, *foreground = nullptr;

	try {
	foreground = LoadImage("Underwater-World-2144372A.png");
	} catch (const std::runtime_error &e) {
		std::cout << e.what() << std::endl;
	}
	
	// SPRITE INITs
	BubbleFish bubblefish;  // todo - do I need these names??
	JumpFish jumpfish;
	Spongebob spongebob;
	PufferFish pufferfish;
	CuttleFish cuttlefish;
	Patrick patrick;
	Sprite* sprites[] = {&pufferfish, &bubblefish, &jumpfish, &cuttlefish, &spongebob, &patrick};
	
	//  Initial LOAD, FLIP, 2GRAY, and CLONE
	cap >> matWebcam;
	flip(matWebcam, matWebcam, 1);
	background = LoadMatImage(&matWebcam);  // format mat to SDL_Texture & assign to background
	cvtColor(matWebcam, matGray, CV_BGR2GRAY);	// copy live frame to grayscale
	matGrayPrevious = matGray.clone();			// make copy for compare
	
	// running loop
	while (running) {
		start = SDL_GetTicks();					// counter for FPS
		

		//  Event handling - mostly for debugging
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_f:
					SDL_SetWindowFullscreen(window, fullscreen ^= 1);
					break;
				case SDLK_m:
					showMatMotion ^= 1;
					if (showMatMotion) {
						cv::namedWindow(WIN_MOTION, CV_WINDOW_AUTOSIZE);
						cv::moveWindow(WIN_MOTION, 645, 440);
					}
					else cv::destroyWindow(WIN_MOTION);
					break;
					
				case SDLK_c:
					SPRITES_COLLIDE ^= 1;
					break;
				case SDLK_s:
					SPRITES_SPEAK ^= 1;
					break;
				case SDLK_KP_8: case SDLK_UP:
					if (sprites[0]->y > 5) sprites[0]->y-=4;
					break;
				case SDLK_KP_4: case SDLK_LEFT:
					sprites[0]->isFlipped = SDL_FLIP_HORIZONTAL;
					if (sprites[0]->x > 5) sprites[0]->x-=4;
					break;
				case SDLK_KP_2: case SDLK_DOWN:
					if (sprites[0]->y < WINDOW_HEIGHT - 6) sprites[0]->y+=4;
					break;
				case SDLK_KP_6: case SDLK_RIGHT:
					bubblefish.isFlipped = SDL_FLIP_NONE;
					if (sprites[0]->x < WINDOW_WIDTH - 60) sprites[0]->x+=4;
					break;
				case SDLK_1:
					sprites[0]->speak();
					break;
				case SDLK_2:
					sprites[1]->speak();
					break;
				case SDLK_3:
					sprites[2]->speak();
					break;
				case SDLK_4:
					sprites[3]->speak();
					break;
				case SDLK_5:
					sprites[4]->speak();
					break;
				case SDLK_6:
					sprites[5]->speak();
					break;
				}
			}
		} // end while (PollEvent)

		//  IDENTIFY MOTION
		absdiff(matGray, matGrayPrevious, matMotion);
		threshold(matMotion, matMotion, THRESHOLD_VALUE, 255, cv::THRESH_BINARY);

		//  ERODE and DILATE motion to reduce noise
		erode(matMotion, matMotion, cv::Mat(cv::getStructuringElement(cv::MORPH_RECT,cv::Size(4,4))));
		dilate(matMotion, matMotion, cv::Mat(cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2,2))));
		
		//  Render
		matMotionDisplay = matMotion.clone();
		ApplySurface(0,0,background,renderer, nullptr);
		SDL_DestroyTexture(background);						// else major memory leak
		ApplySurface(0,50,foreground,renderer, nullptr);
		for each (Sprite* sprite in sprites) sprite->drawBoundariesToMatMotion(matMotion, matMotionDisplay);
		for each (Sprite* sprite in sprites) sprite->update(matMotion);
		for each (Sprite* sprite in sprites) sprite->render(renderer);
		
		//  Draw windows to screen
		SDL_RenderPresent(renderer);
		if (showMatMotion) {
			if (SPRITES_COLLIDE) imshow(WIN_MOTION, matMotion);
			else imshow(WIN_MOTION, matMotionDisplay);
		}

		//  Subsequent LOAD, FLIP, CLONE and 2GRAY
		cap >> matWebcam;								// live webcam frame
		flip(matWebcam, matWebcam, 1);
		background = LoadMatImage(&matWebcam);  // format mat to SDL_Texture & assign to background
		matGrayPrevious = matGray.clone();				// create copy for compare
		cvtColor(matWebcam, matGray, CV_BGR2GRAY);		// copy new frame to grayscale
		
		// FPS control
		if (1000/FPS > SDL_GetTicks() - start)
			SDL_Delay(1000/FPS - (SDL_GetTicks()-start));
			
	} // end while (running)
	
	//  Cleanup 
	Mix_FreeMusic(music);
	Mix_CloseAudio();

	SDL_DestroyTexture(background);
	SDL_DestroyTexture(foreground);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}