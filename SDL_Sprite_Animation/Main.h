#ifndef MAIN_H
#define MAIN_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

#include <time.h>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <string>

cv::Mat matWebcam, matFrame, matGray, matGrayPrevious, matMotion, matMotionDisplay;
Uint32 start;
SDL_Event event;
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

const int FPS = 30;
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

bool running = true;

const char	 *WIN_CAMERA = "WebCam Feed";
const char	 *WIN_MOTION = "Processed Motion";
const int	 CAMERA_WIDTH = 640;
const int	 CAMERA_HEIGHT = 480;
const int	 THRESHOLD_VALUE = 45;				//  Amount of change in (gray) pixel intesity 
												//  for it to qualify as indicating motion
const double SECONDS_BETWEEN_CONTACTS = .5;		//  controls latency between contact checks
const double SECONDS_BETWEEN_SPEAKS = .5;		//	latency between repeating sound effect of sprite
const float	 DELTA_COEFFICIENT = .8;			//  controls sprite speeds (multiplier)
const int	 NUMBER_SPRITES = 6;
int SPRITES_COLLIDE = 0;						//  set to 0 (false) if sprites should not interact w/ ea other
int SPRITES_SPEAK = 0;							//  set to 0 (false) if sprites should not interact w/ ea other
char fullscreen = 0;							//  for full screen toggle
int showMatMotion = 0;

time_t timeNow, timeLast;
clock_t ticksNow;
float secondsBetweenContacts;

int tmpInt, framesPerSecond, thresholdValue;
int i,j,k, pocX, pocY;


void computeReflection(int nX, int nY);



// todo - move to .cpp??	
SDL_Texture* LoadMatImage(cv::Mat* mat) {
	SDL_Surface *surface = 
			SDL_CreateRGBSurfaceFrom(mat->data, 
									 mat->cols, 
									 mat->rows, 
									 24,				// num channels times bits/channel
									 mat->step,			// bytes/row (pitch)
									 0xff0000, 0x00ff00, 0x0000ff, 0); 
        SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, surface ); 
        SDL_FreeSurface(surface); 
		return texture;
}

SDL_Texture* LoadImage(std::string file){
    SDL_Texture *texture = nullptr;
	texture = IMG_LoadTexture(renderer, file.c_str());
    if (texture == nullptr) 
		throw std::runtime_error("Failed to load image: " + file + "  :  " + IMG_GetError());
    return texture;
}

void ApplySurface(int x, int y, SDL_Texture *tex, SDL_Renderer *rend, SDL_Rect *clip = NULL, 
		SDL_RendererFlip flip = SDL_FLIP_NONE){
    SDL_Rect pos;
    pos.x = x;
    pos.y = y;
	if (clip!=NULL) {
		pos.h = clip->h;
		pos.w = clip->w;
	} 
	else {
		SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	}
    SDL_RenderCopyEx(rend, tex, clip, &pos, 0, 0, flip);
}

class Sprite {
	public:
		int w, h, // greatest height/width of all frames to make sure sprite stays in bounds
			x, y, dX, dY, spriteCount, MAX_DELTA;
		int contactAccumulators[4];							//  clockwise from top
		int contactCounters[4];								//  clockwise from top
		int contactCountersMax;
		clock_t ticksLast, ticksLastSpoke;
		SDL_Rect* rects;
		SDL_Texture *spriteSheet; 
		SDL_RendererFlip isFlipped;
		float clipCounter;
		Mix_Chunk *chunk;

		Sprite() {
			chunk = nullptr;
			dX = dY = 0;
			ticksLast = ticksLastSpoke = clock();
			spriteSheet = nullptr;
			isFlipped = SDL_FLIP_NONE;
			clipCounter = 0;
		}
		virtual ~Sprite() {
			if (chunk!=nullptr) Mix_FreeChunk(chunk);
			SDL_DestroyTexture(spriteSheet);
		}
		virtual void render(SDL_Renderer* renderer) {};
		virtual void speak() {
			Mix_PlayChannel(-1,chunk,0);
		}
		virtual void drawBoundariesToMatMotion(cv::Mat& matMotion, cv::Mat& matMotionDisplay) {
			// Draw box around sprite to matMotion if SPRITES_COLLIDE to accout for
			// their areas in computing reflections else just to matMotionDisplay
			if (SPRITES_COLLIDE) ellipse(matMotion, 
				cv::Point(x+rects[(int)clipCounter].w/2, y+rects[(int)clipCounter].h/2),
				cv::Size(rects[(int)clipCounter].w/2, rects[(int)clipCounter].h/2),
				0,0,360,cv::Scalar(255,255,255));
			else ellipse(matMotionDisplay, 
				cv::Point(x+rects[(int)clipCounter].w/2, y+rects[(int)clipCounter].h/2),
				cv::Size(rects[(int)clipCounter].w/2, rects[(int)clipCounter].h/2),
				0,0,360,cv::Scalar(255,255,255));
		}
		virtual void update(cv::Mat& matMotion) {
			//  COMPUTE DELTA changes for CONTACT with sprite
			//		first make sure sprite is completely inside WIN_CAMERA to avoid out of bounds exceptions
			//		and that sufficient time has past since last contact to avoid double tapping/overlapping 
			if (x > rects[(int)clipCounter].w/2+1 && 
				y > rects[(int)clipCounter].h/2+1 && 
				x < WINDOW_WIDTH - (rects[(int)clipCounter].w/2+1) && 
				y < (WINDOW_HEIGHT - rects[(int)clipCounter].h/2+1) && 
				clock() - ticksLast > CLOCKS_PER_SEC * SECONDS_BETWEEN_CONTACTS) {

				// zero all counters
				for (i = 0; i < 4; i++) {	
					contactCounters[i]=0; 
					contactAccumulators[i]=0; 
				}
				// check for contact - top of box
				for (i = 1; i < rects[(int)clipCounter].w; i++) {					
					if (matMotion.at<uchar>(y-1, x+i) == 255) {
						contactAccumulators[0] += x+i;					// add all points of contact
						contactCounters[0]++;							// count number of POCs
					}
				}
				// check for contact - right side of box
				for (i = 1; i < rects[(int)clipCounter].h; i++) {					
					if (matMotion.at<uchar>(y+i, x+rects[(int)clipCounter].w+1) == 255) {
						contactAccumulators[1] += y+i;					// add all points of contact
						contactCounters[1]++;							// count number of POCs
					}
				}	
				// check for contact - left side box
				for (i = 1; i < rects[(int)clipCounter].h; i++) {					
					if (matMotion.at<uchar>(y+i, x-1) == 255) {
						contactAccumulators[2] += y+i;					// add all points of contact
						contactCounters[2]++;							// count number of POCs
					}
				}
				// check for contact - bottom of box
				for (i = 1; i < rects[(int)clipCounter].w; i++) {					
					if (matMotion.at<uchar>(y+rects[(int)clipCounter].h+1, x+i) == 255) {
						contactAccumulators[3] += x+i;					// add all points of contact
						contactCounters[3]++;							// count number of POCs
					}
				}
				contactCountersMax = 0;		// reset counter
				// determine side with greatest contact
				for (i = 1; i < 4; i++)									
					if (contactCounters[i] > contactCounters[contactCountersMax]) contactCountersMax = i;

				// first make sure contact occured
				if (contactCounters[contactCountersMax] > 0) {
					switch(contactCountersMax) {						// set pocX and pocY for new delta copmutation
					case 0:		//top
						pocX = contactAccumulators[0]/contactCounters[0]; 
						pocY = y;
						break;
					case 1:		//right
						pocY = contactAccumulators[1]/contactCounters[1]; 
						pocX = x+rects[(int)clipCounter].h;
						break;	
					case 2:		//left
						pocY = contactAccumulators[2]/contactCounters[2]; 
						pocX = x;
						break;
					case 3:		//bottom
						pocX = contactAccumulators[3]/contactCounters[3]; 
						pocY = y+rects[(int)clipCounter].w;
						break;
					}
					// todo comment here
					dX = (int)((x + (rects[(int)clipCounter].w/2) - pocX) * DELTA_COEFFICIENT);
					dY = (int)((y + (rects[(int)clipCounter].h/2) - pocY) * DELTA_COEFFICIENT);	
					ticksLast = clock();
					if (SPRITES_SPEAK && clock() - ticksLastSpoke > CLOCKS_PER_SEC * SECONDS_BETWEEN_SPEAKS) {
						speak();
						ticksLastSpoke = clock();
					}

					//  LIMIT DELTAS to MAX_DELTA
					if (dX > MAX_DELTA) dX = MAX_DELTA;  
					if (dX < -1*MAX_DELTA) dX = -1*MAX_DELTA;
					if (dY > MAX_DELTA) dY = MAX_DELTA;  
					if (dY < -1*MAX_DELTA) dY = -1*MAX_DELTA;
					
				} // end if
			} // end COMPUTE DELTA changes for CONTACT

			//  BUMPERS for contact with frame edges
			if (x + dX < 1) dX = abs(dX);
			else if (x + dX > WINDOW_WIDTH-(w+5)) dX = -1 * abs(dX);
			if (y + dY < 1) dY = abs(dY);
			else if (y + dY > WINDOW_HEIGHT-(h+5)) dY = -1 * abs(dY);

			//  UPDATE X and Y
			x += dX; y += dY; 
			if (dX<0) isFlipped = SDL_FLIP_HORIZONTAL;
			else if (dX>0) isFlipped = SDL_FLIP_NONE;
		}
};

class BubbleFish : public Sprite {
	public:
		
		
		BubbleFish() { 
			MAX_DELTA = 6;
			spriteCount = 16;
			rects = new SDL_Rect[spriteCount];
			w = 56; 
			h = 60;
			x = rand() % (640 - 2 * w) + w/2;
			y = rand() % (480 - 2 * h) + h/2;
			chunk = Mix_LoadWAV("bubbles-water-02b.wav"); 

			try {
				spriteSheet = LoadImage("BubbleFishSheetA.png"); 
			} catch (const std::runtime_error &e) {
				std::cout << e.what() << std::endl;
			}
			
			// Set rectangles to clip from spriteSheet
			for (int i=0,k=0; i < 2; i++) {
				for (int j = 0; j < 8; j++,k++) {
					rects[k].x = j*56;
					rects[k].y = i*60;
					rects[k].w = 56;
					rects[k].h = 60;
				}
			}
		}

		~BubbleFish() { }

		void render(SDL_Renderer* renderer) {
			ApplySurface(x, y, spriteSheet, renderer, &rects[(int)clipCounter], isFlipped);
			if (clipCounter > spriteCount-1) clipCounter = 0;
			else clipCounter+=.25;
		}

};

class JumpFish : public Sprite {
	public:
		
		JumpFish() { 
			MAX_DELTA = 8;
			spriteCount = 16;
			rects = new SDL_Rect[spriteCount];
			w = 64;
			h = 64;
			x = rand() % (640 - 2 * w) + w/2;
			y = rand() % (480 - 2 * h) + h/2;
			chunk = Mix_LoadWAV("water-bubble-7b.wav"); 
			try {
				spriteSheet = LoadImage("JumpFish.PNG"); 
			} catch (const std::runtime_error &e) {
				std::cout << e.what() << std::endl;
			}

			// Set rectangles to clip from spriteSheet
			for (int j = 0; j < 2; j++) {
				for (int i = 0; i < 8; i++) {
					rects[i+(8*j)].x = i*64;
					rects[i+(8*j)].y = 150;
					rects[i+(8*j)].w = 64;
					rects[i+(8*j)].h = 64;
				}
			}
		}

		~JumpFish() { }

		void render(SDL_Renderer* renderer) {
			ApplySurface(x, y, spriteSheet, renderer, &rects[(int)clipCounter], isFlipped);
			if (clipCounter > spriteCount-1) clipCounter = 0;
			else clipCounter+=.25;
		}

};

class Spongebob : public Sprite {
	public:
		Mix_Chunk** chunks;
		int numChunks;

		Spongebob() { 
			MAX_DELTA = 25;
			spriteCount = 22;
			rects = new SDL_Rect[spriteCount];
			w = 60;
			h = 60;
			x = rand() % (640 - 2 * w) + w/2;
			y = rand() % (480 - 2 * h) + h/2;

			numChunks = 9;
			chunks = new Mix_Chunk*[numChunks];
			chunks[0] = Mix_LoadWAV("spongebob_laugh.wav"); 
			chunks[1] = Mix_LoadWAV("bob_raspberry.wav"); 
			chunks[2] = Mix_LoadWAV("bob_superspectacular.wav"); 
			chunks[3] = Mix_LoadWAV("bob_quiet.wav"); 
			chunks[4] = Mix_LoadWAV("bob_extrafantastical.wav"); 
			chunks[5] = Mix_LoadWAV("bob_oops.wav"); 
			chunks[6] = Mix_LoadWAV("bob_toottoot.wav"); 
			chunks[7] = Mix_LoadWAV("bob_dootdoot.wav"); 
			chunks[8] = Mix_LoadWAV("bob_yeah.wav"); 

			Mix_VolumeChunk(chunks[0], 32); 
			try {
				spriteSheet = LoadImage("Spongebob.png"); 
			} catch (const std::runtime_error &e) {
				std::cout << e.what() << std::endl;
			}

			// Set rectangles to clip from spriteSheet
			rects[0].x = 0; rects[0].y = 10; rects[0].w = 60; rects[0].h = 60;
			rects[1].x = 60;	rects[1].y = 10;	rects[1].w = 60;	rects[1].h = 60;
			rects[2].x = 120; rects[2].y = 10; rects[2].w = 55; rects[2].h = 60;
			rects[3].x = 175; rects[3].y = 10; rects[3].w = 55; rects[3].h = 60;
			rects[4].x = 230; rects[4].y = 10; rects[4].w = 55; rects[4].h = 60;
			rects[5].x = 285; rects[5].y = 10; rects[5].w = 55; rects[5].h = 60;
			rects[6].x = 340; rects[6].y = 10; rects[6].w = 60; rects[6].h = 60;
			rects[7].x = 400; rects[7].y = 10; rects[7].w = 60; rects[7].h = 60;
			rects[8].x = 460; rects[8].y = 10; rects[8].w = 60; rects[8].h = 60;
			rects[9].x = 520; rects[9].y = 10; rects[9].w = 60; rects[9].h = 60;
			rects[10].x = 580; rects[10].y = 10; rects[10].w = 55; rects[10].h = 60;
			rects[11].x = 635; rects[11].y = 10; rects[11].w = 60; rects[11].h = 60;
			rects[12].x = 580; rects[12].y = 10; rects[12].w = 55; rects[12].h = 60;
			rects[13].x = 520; rects[13].y = 10; rects[13].w = 60; rects[13].h = 60;
			rects[14].x = 460; rects[14].y = 10; rects[14].w = 60;	rects[14].h = 60;
			rects[15].x = 400; rects[15].y = 10; rects[15].w = 60;	rects[15].h = 60;
			rects[16].x = 0;	rects[16].y = 10; rects[16].w = 60; rects[16].h = 60;
			rects[17].x = 60; rects[17].y = 10; rects[17].w = 60; rects[17].h = 60;
			rects[18].x = 120; rects[18].y = 10; rects[18].w = 55; rects[18].h = 60;
			rects[19].x = 175; rects[19].y = 10; rects[19].w = 55;	rects[19].h = 60;
			rects[20].x = 230; rects[20].y = 10; rects[20].w = 55;	rects[20].h = 60;
			rects[21].x = 285; rects[21].y = 10; rects[21].w = 55;	rects[21].h = 60;
		}

		~Spongebob() { 
			for (int i=0; i<numChunks; ++i) Mix_FreeChunk(chunks[i]);
		}

		void render(SDL_Renderer* renderer) {
			ApplySurface(x, y, spriteSheet, renderer, &rects[(int)clipCounter], isFlipped);
			if (clipCounter > spriteCount-1) clipCounter = 0;
			else clipCounter+=.25;
		}

		void speak() {
			Mix_PlayChannel(-1,chunks[rand()%numChunks],0);
		}

};

class PufferFish : public Sprite {
	public:
		
		PufferFish() { 
			MAX_DELTA = 3;
			spriteCount = 55;
			rects = new SDL_Rect[spriteCount];
			w = 135;	
			h = 125;	
			x = rand() % (640 - 2 * w) + w/2;
			y = rand() % (480 - 2 * h) + h/2;
			chunk = Mix_LoadWAV("sci-fi-bubble-popb.wav");
			try {
				spriteSheet = LoadImage("Pufferfish.png"); 
			} catch (const std::runtime_error &e) {
				std::cout << e.what() << std::endl;
			}

			// Set rectangles to clip from spriteSheet
			rects[0].x = 5;
			rects[0].y = 370;
			rects[0].w = 85;
			rects[0].h = 120;

			rects[1].x = 90;
			rects[1].y = 370;
			rects[1].w = 90;
			rects[1].h = 120;

			rects[2].x = 180;
			rects[2].y = 370;
			rects[2].w = 100;
			rects[2].h = 120;

			rects[3].x = 280;
			rects[3].y = 370;
			rects[3].w = 105;
			rects[3].h = 120;

			rects[4].x = 385;
			rects[4].y = 370;
			rects[4].w = 120;
			rects[4].h = 120;

			rects[5].x = 505;
			rects[5].y = 370;
			rects[5].w = 135;
			rects[5].h = 120;

			rects[6].x = 640;
			rects[6].y = 370;
			rects[6].w = 125;
			rects[6].h = 120;

			rects[7].x = 765;
			rects[7].y = 370;
			rects[7].w = 135;
			rects[7].h = 120;

			rects[8].x = 900;
			rects[8].y = 370;
			rects[8].w = 135;
			rects[8].h = 120;

			rects[9].x = 1035;
			rects[9].y = 370;
			rects[9].w = 135;
			rects[9].h = 120;

			rects[10].x = 1170;
			rects[10].y = 370;
			rects[10].w = 130;
			rects[10].h = 120;

			rects[11].x = 1300;
			rects[11].y = 370;
			rects[11].w = 120;
			rects[11].h = 120;

			rects[12].x = 1420;
			rects[12].y = 370;
			rects[12].w = 110;
			rects[12].h = 120;

			rects[13].x = 1530;
			rects[13].y = 370;
			rects[13].w = 100;
			rects[13].h = 120;

			rects[14].x = 1630;
			rects[14].y = 370;
			rects[14].w = 80;
			rects[14].h = 120;

			rects[15].x = 930;
			rects[15].y = 235;
			rects[15].w = 100;
			rects[15].h = 125;

			rects[16].x = 1030;
			rects[16].y = 235;
			rects[16].w = 90;
			rects[16].h = 125;

			rects[17].x = 1120;
			rects[17].y = 235;
			rects[17].w = 95;
			rects[17].h = 125;

			rects[18].x = 1215;
			rects[18].y = 235;
			rects[18].w = 95;
			rects[18].h = 125;

			rects[19].x = 1310;
			rects[19].y = 235;
			rects[19].w = 90;
			rects[19].h = 125;

			rects[20].x = 1400;
			rects[20].y = 235;
			rects[20].w = 100;
			rects[20].h = 125;

			rects[21].x = 1500;
			rects[21].y = 235;
			rects[21].w = 90;
			rects[21].h = 125;

			rects[22].x = 1590;
			rects[22].y = 235;
			rects[22].w = 90;
			rects[22].h = 125;

			rects[23].x = 930;
			rects[23].y = 235;
			rects[23].w = 100;
			rects[23].h = 125;

			rects[24].x = 1030;
			rects[24].y = 235;
			rects[24].w = 90;
			rects[24].h = 125;

			rects[25].x = 1120;
			rects[25].y = 235;
			rects[25].w = 95;
			rects[25].h = 125;

			rects[26].x = 1215;
			rects[26].y = 235;
			rects[26].w = 95;
			rects[26].h = 125;

			rects[27].x = 1310;
			rects[27].y = 235;
			rects[27].w = 90;
			rects[27].h = 125;

			rects[28].x = 1400;
			rects[28].y = 235;
			rects[28].w = 100;
			rects[28].h = 125;

			rects[29].x = 1500;
			rects[29].y = 235;
			rects[29].w = 90;
			rects[29].h = 125;

			rects[30].x = 1590;
			rects[30].y = 235;
			rects[30].w = 90;
			rects[30].h = 125;

			rects[31].x = 930;
			rects[31].y = 235;
			rects[31].w = 100;
			rects[31].h = 125;

			rects[32].x = 1030;
			rects[32].y = 235;
			rects[32].w = 90;
			rects[32].h = 125;

			rects[33].x = 1120;
			rects[33].y = 235;
			rects[33].w = 95;
			rects[33].h = 125;

			rects[34].x = 1215;
			rects[34].y = 235;
			rects[34].w = 95;
			rects[34].h = 125;

			rects[35].x = 1310;
			rects[35].y = 235;
			rects[35].w = 90;
			rects[35].h = 125;

			rects[36].x = 1400;
			rects[36].y = 235;
			rects[36].w = 100;
			rects[36].h = 125;

			rects[37].x = 1500;
			rects[37].y = 235;
			rects[37].w = 90;
			rects[37].h = 125;

			rects[38].x = 1590;
			rects[38].y = 235;
			rects[38].w = 90;
			rects[38].h = 125;

			rects[39].x = 930;
			rects[39].y = 235;
			rects[39].w = 100;
			rects[39].h = 125;

			rects[40].x = 1030;
			rects[40].y = 235;
			rects[40].w = 90;
			rects[40].h = 125;

			rects[41].x = 1120;
			rects[41].y = 235;
			rects[41].w = 95;
			rects[41].h = 125;

			rects[42].x = 1215;
			rects[42].y = 235;
			rects[42].w = 95;
			rects[42].h = 125;

			rects[43].x = 1310;
			rects[43].y = 235;
			rects[43].w = 90;
			rects[43].h = 125;

			rects[44].x = 1400;
			rects[44].y = 235;
			rects[44].w = 100;
			rects[44].h = 125;

			rects[45].x = 1500;
			rects[45].y = 235;
			rects[45].w = 90;
			rects[45].h = 125;

			rects[46].x = 1590;
			rects[46].y = 235;
			rects[46].w = 90;
			rects[46].h = 125;

			rects[47].x = 930;
			rects[47].y = 235;
			rects[47].w = 100;
			rects[47].h = 125;

			rects[48].x = 1030;
			rects[48].y = 235;
			rects[48].w = 90;
			rects[48].h = 125;

			rects[49].x = 1120;
			rects[49].y = 235;
			rects[49].w = 95;
			rects[49].h = 125;

			rects[50].x = 1215;
			rects[50].y = 235;
			rects[50].w = 95;
			rects[50].h = 125;

			rects[51].x = 1310;
			rects[51].y = 235;
			rects[51].w = 90;
			rects[51].h = 125;

			rects[52].x = 1400;
			rects[52].y = 235;
			rects[52].w = 100;
			rects[52].h = 125;

			rects[53].x = 1500;
			rects[53].y = 235;
			rects[53].w = 90;
			rects[53].h = 125;

			rects[54].x = 1590;
			rects[54].y = 235;
			rects[54].w = 90;
			rects[54].h = 125;

		}

		~PufferFish() { }

		void render(SDL_Renderer* renderer) {
			ApplySurface(x, y, spriteSheet, renderer, &rects[(int)clipCounter], isFlipped);
			if (clipCounter > 54) clipCounter = 0;
			if (clipCounter < 10) clipCounter+=.5;
			else clipCounter+=.2;
		}

		void drawBoundariesToMatMotion(cv::Mat& matMotion, cv::Mat& matMotionDisplay) {
			// Draw box around sprite to matMotion if SPRITES_COLLIDE to accout for
			// their areas in computing reflections else just to matMotionDisplay
			if (SPRITES_COLLIDE) ellipse(matMotion, 
				cv::Point(x+rects[(int)clipCounter].w/2, y+60),
				cv::Size(rects[(int)clipCounter].w/2, (rects[(int)clipCounter].h-60)/2),
				0,0,360,cv::Scalar(255,255,255));
			else ellipse(matMotionDisplay, 
				cv::Point(x+rects[(int)clipCounter].w/2, y+60),
				cv::Size(rects[(int)clipCounter].w/2, (rects[(int)clipCounter].h-60)/2),
				0,0,360,cv::Scalar(255,255,255));
		}

};

class CuttleFish : public Sprite {
	public:
		
		CuttleFish() { 
			MAX_DELTA = 15;
			spriteCount = 16;
			rects = new SDL_Rect[spriteCount];
			w = 67;
			h = 50;
			x = rand() % (640 - 2 * w) + w/2;
			y = rand() % (480 - 2 * h) + h/2;
			chunk = Mix_LoadWAV("bubbles-single2b.wav");	 
			try {
				spriteSheet = LoadImage("CuttlefishA.png"); 
			} catch (const std::runtime_error &e) {
				std::cout << e.what() << std::endl;
			}

			// Set rectangles to clip from spriteSheet
			for (int i = 0; i < 9; i++) {
			rects[i].x = i*67;
			rects[i].y = 65;
			rects[i].w = 67;
			rects[i].h = 50;
			}

			rects[9].x = 0;
			rects[9].y = 20;
			rects[9].w = 67;
			rects[9].h = 30;

			rects[10].x = 67;
			rects[10].y = 20;
			rects[10].w = 67;
			rects[10].h = 30;

			rects[11].x = 140;
			rects[11].y = 20;
			rects[11].w = 67;
			rects[11].h = 30;

			rects[12].x = 215;
			rects[12].y = 20;
			rects[12].w = 67;
			rects[12].h = 30;

			rects[13].x = 286;
			rects[13].y = 20;
			rects[13].w = 67;
			rects[13].h = 30;

			rects[14].x = 353;
			rects[14].y = 20;
			rects[14].w = 67;
			rects[14].h = 30;

			rects[15].x = 417;
			rects[15].y = 235;
			rects[15].w = 67;
			rects[15].h = 50;
		}

		~CuttleFish() { }

		void render(SDL_Renderer* renderer) {
			ApplySurface(x, y, spriteSheet, renderer, &rects[(int)clipCounter], isFlipped);
			if (clipCounter > spriteCount-1) clipCounter = 0;
			else clipCounter+=.2;
		}

};

class Patrick : public Sprite {
	public:
		Mix_Chunk** chunks;
		int numChunks;

		Patrick() { 
			MAX_DELTA  = 25;
			spriteCount = 80;
			rects = new SDL_Rect[spriteCount];
			w = 55;
			h = 60;
			x = rand() % (640 - 2 * w) + w/2;
			y = rand() % (480 - 2 * h) + h/2;

			numChunks = 9;
			chunks = new Mix_Chunk*[numChunks];
			chunks[0] = Mix_LoadWAV("PatrickSaysNO.wav");  
			chunks[1] = Mix_LoadWAV("pat_gimmethat.wav"); 
			chunks[2] = Mix_LoadWAV("pat_ouch1.wav"); 
			chunks[3] = Mix_LoadWAV("pat_ouch2.wav"); 
			chunks[4] = Mix_LoadWAV("pat_ouch3.wav"); 
			chunks[5] = Mix_LoadWAV("pat_ouch4.wav"); 
			chunks[6] = Mix_LoadWAV("pat_ouch5.wav"); 
			chunks[7] = Mix_LoadWAV("pat_dummy.wav"); 
			chunks[8] = Mix_LoadWAV("pat_pardon.wav"); 
			chunks[9] = Mix_LoadWAV("pat_aaaah.wav"); 

			chunk = Mix_LoadWAV("PatrickSaysNO.wav"); 
			Mix_VolumeChunk(chunk, 32);
			try {
				spriteSheet = LoadImage("PatrickA.png"); 
			} catch (const std::runtime_error &e) {
				std::cout << e.what() << std::endl;
			}

			// Set rectangles to clip from spriteSheet
			for (int j = 0; j < 8; j++) {
				for (int i = 0; i < 10; i++) {
					rects[i+(10*j)].x = i*55;
					rects[i+(10*j)].y = j*60;
					rects[i+(10*j)].w = 55;
					rects[i+(10*j)].h = 60;
				}
			}
		}

		~Patrick() { 
			for (int i=0; i<numChunks; ++i) Mix_FreeChunk(chunks[i]);
		}

		void render(SDL_Renderer* renderer) {
			ApplySurface(x, y, spriteSheet, renderer, &rects[(int)clipCounter], isFlipped);
			if (clipCounter > spriteCount-1) clipCounter = 0;
			else clipCounter+=.2;
		}

		void speak() {
			Mix_PlayChannel(-1,chunks[rand()%numChunks],0);
		}

};

#endif
