//#include "Main.h"
//
//
//int main(int argc, char* argv[]) {
//	
//	Uint32 start;
//	SDL_Event event;
//
//	SDL_RendererFlip bubblefishIsFlipped = SDL_FLIP_NONE;
//	SDL_Rect bubblefishRects[16];
//	setBubblefishRects(bubblefishRects);
//	float bubblefishClipCtr = 0;
//	int bubblefishX = 1; int bubblefishY = 1;
//	
//	SDL_RendererFlip jumpfishIsFlipped = SDL_FLIP_NONE;
//	SDL_Rect jumpfishRects[16];
//	setJumpfishRects(jumpfishRects);
//	float jumpfishClipCtr = 0;
//	int jumpfishX = 40; int jumpfishY = 40;
//	
//	SDL_RendererFlip spongebobIsFlipped = SDL_FLIP_NONE;
//	SDL_Rect spongebobRects[22];
//	setSpongebobRects(spongebobRects);
//	float spongebobClipCtr = 0;
//	int spongebobX = 120; int spongebobY = 120;
//	
//	SDL_RendererFlip pufferfishIsFlipped = SDL_FLIP_NONE;
//	SDL_Rect pufferfishRects[55];
//	setPufferfishRects(pufferfishRects);
//	float pufferfishClipCtr = 0;
//	int pufferfishX = 220; int pufferfishY = 220;
//	
//	SDL_RendererFlip cuttlefishIsFlipped = SDL_FLIP_NONE;
//	SDL_Rect cuttlefishRects[16];
//	setCuttlefishRects(cuttlefishRects);
//	float cuttlefishClipCtr = 0;
//	int cuttlefishX = 280; int cuttlefishY = 340;
//	
//	SDL_RendererFlip patrickIsFlipped = SDL_FLIP_NONE;
//	SDL_Rect patrickRects[80];
//	setPatrickRects(patrickRects);
//	float patrickClipCtr = 0;
//	int patrickX = 80; int patrickY = 340;
//	
//	//  Mixer INITs
//	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
//		std::cout << "Init Error: \n" << SDL_GetError() << std::endl;
//		return 1;
//	}
//	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
//	Mix_Music *music;
//	Mix_Chunk *fishChunk1, *fishChunk2, *fishChunk3, *fishChunk4, *fishChunk5, *fishChunk6;
//	music = Mix_LoadMUS("MUSIC_underwater-bubbles.wav"); Mix_VolumeMusic(28);
//	fishChunk1 = Mix_LoadWAV("bubbles-water-02b.wav"); 
//	fishChunk2 = Mix_LoadWAV("water-bubble-7b.wav");
//	fishChunk3 = Mix_LoadWAV("sci-fi-bubble-popb.wav");
//	fishChunk4 = Mix_LoadWAV("PatrickSaysNO.wav"); Mix_VolumeChunk(fishChunk4, 32);
//	fishChunk5 = Mix_LoadWAV("bubbles-single2b.wav");	
//	fishChunk6 = Mix_LoadWAV("spongebob_laugh.wav"); Mix_VolumeChunk(fishChunk6, 32);
//	Mix_PlayMusic(music, -1);
//
//	//  SDL and IMAGE INITs
//	window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
//		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
//	if (window == nullptr) {
//		std::cout << "ERROR - failure at SDL_CreateWindow() \n" << SDL_GetError() << std::endl;
//		return 2;
//	}	
//	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
//	if (renderer == nullptr) {
//		std::cout << "ERROR - failure at SDL_CreateRenderer() \n" << SDL_GetError() << std::endl;
//		return 2;
//	}
//
//	SDL_Texture *background = nullptr, *foreground = nullptr, *bubblefish = nullptr, *jumpfish = nullptr,
//		*spongebob = nullptr, *pufferfish = nullptr, *cuttlefish = nullptr, *patrick = nullptr;
//	try {
//	background = LoadImage("The_Crab.bmp"); 
//	foreground = LoadImage("Underwater-World-2144372A.png");
//	bubblefish = LoadImage("BubbleFishSheetA.png"); 
//	jumpfish = LoadImage("JumpFish.PNG");
//	spongebob = LoadImage("Spongebob.png");
//	pufferfish = LoadImage("Pufferfish.png");
//	cuttlefish = LoadImage("CuttlefishA.png");
//	patrick = LoadImage("PatrickA.png");
//	} catch (const std::runtime_error &e) {
//		std::cout << e.what() << std::endl;
//	}
//	
//	bool running = true;
//	while (running) {
//		start = SDL_GetTicks();
//		
//		//  Event handling
//		while (SDL_PollEvent(&event)) {
//			switch (event.type) {
//			case SDL_QUIT:
//				running = false;
//				break;
//			case SDL_KEYDOWN:
//				switch (event.key.keysym.sym) {
//				case SDLK_ESCAPE:
//					running = false;
//					break;
//				case SDLK_KP_8: case SDLK_UP:
//					if (bubblefishY > 5) bubblefishY-=4;
//					break;
//				case SDLK_KP_4: case SDLK_LEFT:
//					bubblefishIsFlipped = SDL_FLIP_HORIZONTAL;
//					if (bubblefishX > 5) bubblefishX-=4;
//					break;
//				case SDLK_KP_2: case SDLK_DOWN:
//					if (bubblefishY < WINDOW_HEIGHT - 6) bubblefishY+=4;
//					break;
//				case SDLK_KP_6: case SDLK_RIGHT:
//					bubblefishIsFlipped = SDL_FLIP_NONE;
//					if (bubblefishX < WINDOW_WIDTH - 60) bubblefishX+=4;
//					break;
//				case SDLK_1:
//					Mix_PlayChannel(-1,fishChunk1,0);
//					break;
//				case SDLK_2:
//					Mix_PlayChannel(-1,fishChunk2,0);
//					break;
//				case SDLK_3:
//					Mix_PlayChannel(-1,fishChunk3,0);
//					break;
//				case SDLK_4:
//					Mix_PlayChannel(-1,fishChunk4,0);
//					break;
//				case SDLK_5:
//					Mix_PlayChannel(-1,fishChunk5,0);
//					break;
//				case SDLK_6:
//					Mix_PlayChannel(-1,fishChunk6,0);
//					break;
//				}
//			}
//		} // end while (PollEvent)
//
//		//  Logic
//		
//
//
//		//  Render
//		ApplySurface(0,0,background,renderer, nullptr);
//		ApplySurface(0,50,foreground,renderer, nullptr);
//		ApplySurface(bubblefishX,bubblefishY,bubblefish,renderer, &bubblefishRects[(int)bubblefishClipCtr], bubblefishIsFlipped);
//		if (bubblefishClipCtr > 15) bubblefishClipCtr = 0;
//		else bubblefishClipCtr+=.25;
//		ApplySurface(jumpfishX,jumpfishY,jumpfish,renderer, &jumpfishRects[(int)jumpfishClipCtr], jumpfishIsFlipped);
//		if (jumpfishClipCtr > 15) jumpfishClipCtr = 0;
//		else jumpfishClipCtr+=.25;
//		ApplySurface(spongebobX,spongebobY,spongebob,renderer, &spongebobRects[(int)spongebobClipCtr], spongebobIsFlipped);
//		if (spongebobClipCtr > 21) spongebobClipCtr = 0;
//		else spongebobClipCtr+=.25;
//		ApplySurface(pufferfishX,pufferfishY,pufferfish,renderer, &pufferfishRects[(int)pufferfishClipCtr], pufferfishIsFlipped);
//		if (pufferfishClipCtr > 54) pufferfishClipCtr = 0;
//		if (pufferfishClipCtr < 10) pufferfishClipCtr+=.5;
//		else pufferfishClipCtr+=.2;
//		ApplySurface(cuttlefishX,cuttlefishY,cuttlefish,renderer, &cuttlefishRects[(int)cuttlefishClipCtr], cuttlefishIsFlipped);
//		if (cuttlefishClipCtr > 15) cuttlefishClipCtr = 0;
//		else cuttlefishClipCtr+=.2;
//		ApplySurface(patrickX,patrickY,patrick,renderer, &patrickRects[(int)patrickClipCtr], patrickIsFlipped);
//		if (patrickClipCtr > 79) patrickClipCtr = 0;
//		else patrickClipCtr+=.2;
//
//		SDL_RenderPresent(renderer);
//
//		// FPS control
//		if (1000/FPS > SDL_GetTicks() - start)
//			SDL_Delay(1000/FPS - (SDL_GetTicks()-start));
//	} // end while (running)
//	
//	//  Cleanup 
//	Mix_FreeChunk(fishChunk1);
//	Mix_FreeChunk(fishChunk2);
//	Mix_FreeChunk(fishChunk3);
//	Mix_FreeChunk(fishChunk4);
//	Mix_FreeChunk(fishChunk5);
//	Mix_FreeChunk(fishChunk6);
//	Mix_FreeMusic(music);
//	Mix_CloseAudio();
//
//	SDL_DestroyTexture(background);
//	SDL_DestroyTexture(foreground);
//	SDL_DestroyTexture(bubblefish);
//	SDL_DestroyTexture(jumpfish);
//	SDL_DestroyTexture(spongebob);
//	SDL_DestroyTexture(pufferfish);
//	SDL_DestroyTexture(cuttlefish);
//	SDL_DestroyTexture(patrick);
//	SDL_DestroyRenderer(renderer);
//	SDL_DestroyWindow(window);
//	SDL_Quit();
//	return 0;
//}