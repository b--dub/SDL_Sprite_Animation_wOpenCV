//#include <SDL.h>
//#include <SDL_image.h>
//#include <SDL_mixer.h>
//
//#include <iostream>
//#include <stdexcept>
//#include <string>
//
//SDL_Window *window = nullptr;
//SDL_Renderer *renderer = nullptr;
//
//int FPS = 30;
//int WINDOW_WIDTH = 640;
//int WINDOW_HEIGHT = 480;
//
//
//SDL_Texture* LoadImage(std::string file){
//    SDL_Texture *texture = nullptr;
//	texture = IMG_LoadTexture(renderer, file.c_str());
//    if (texture == nullptr) 
//		throw std::runtime_error("Failed to load image: " + file + IMG_GetError());
//    return texture;
//}
//
//void ApplySurface(int x, int y, SDL_Texture *tex, SDL_Renderer *rend, SDL_Rect *clip = NULL, 
//		SDL_RendererFlip flip = SDL_FLIP_NONE){
//    SDL_Rect pos;
//    pos.x = x;
//    pos.y = y;
//	if (clip!=NULL) {
//		pos.h = clip->h;
//		pos.w = clip->w;
//	} 
//	else {
//		SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
//	}
//    SDL_RenderCopyEx(rend, tex, clip, &pos, 0, 0, flip);
//}
//
//void setBubblefishRects(SDL_Rect* clip) {
//	int k = 0;
//	for (int i = 0; i < 2; i++) {
//		for (int j = 0; j < 8; j++) {
//			clip[k].x = j*56;
//			clip[k].y = i*60;
//			clip[k].w = 56;
//			clip[k].h = 60;
//			k++;
//		}
//	}
//}
//
//void setJumpfishRects(SDL_Rect* clip) {
//	for (int j = 0; j < 2; j++)
//		for (int i = 0; i < 8; i++) {
//				clip[i+(8*j)].x = i*64;
//				clip[i+(8*j)].y = 150;
//				clip[i+(8*j)].w = 64;
//				clip[i+(8*j)].h = 64;
//		}
//}
//
//void setSpongebobRects(SDL_Rect* clip) {
//	
//	clip[0].x = 0;
//	clip[0].y = 10;
//	clip[0].w = 60;
//	clip[0].h = 60;
//
//	clip[1].x = 60;
//	clip[1].y = 10;
//	clip[1].w = 60;
//	clip[1].h = 60;
//
//	clip[2].x = 120;
//	clip[2].y = 10;
//	clip[2].w = 55;
//	clip[2].h = 60;
//
//	clip[3].x = 175;
//	clip[3].y = 10;
//	clip[3].w = 55;
//	clip[3].h = 60;
//
//	clip[4].x = 230;
//	clip[4].y = 10;
//	clip[4].w = 55;
//	clip[4].h = 60;
//
//	clip[5].x = 285;
//	clip[5].y = 10;
//	clip[5].w = 55;
//	clip[5].h = 60;
//
//	clip[6].x = 340;
//	clip[6].y = 10;
//	clip[6].w = 60;
//	clip[6].h = 60;
//
//	clip[7].x = 400;
//	clip[7].y = 10;
//	clip[7].w = 60;
//	clip[7].h = 60;
//
//	clip[8].x = 460;
//	clip[8].y = 10;
//	clip[8].w = 60;
//	clip[8].h = 60;
//
//	clip[9].x = 520;
//	clip[9].y = 10;
//	clip[9].w = 60;
//	clip[9].h = 60;
//
//	clip[10].x = 580;
//	clip[10].y = 10;
//	clip[10].w = 55;
//	clip[10].h = 60;
//
//	clip[11].x = 635;
//	clip[11].y = 10;
//	clip[11].w = 60;
//	clip[11].h = 60;
//
//	clip[12].x = 580;
//	clip[12].y = 10;
//	clip[12].w = 55;
//	clip[12].h = 60;
//
//	clip[13].x = 520;
//	clip[13].y = 10;
//	clip[13].w = 60;
//	clip[13].h = 60;
//
//	clip[14].x = 460;
//	clip[14].y = 10;
//	clip[14].w = 60;
//	clip[14].h = 60;
//
//	clip[15].x = 400;
//	clip[15].y = 10;
//	clip[15].w = 60;
//	clip[15].h = 60;
//
//	clip[16].x = 0;
//	clip[16].y = 10;
//	clip[16].w = 60;
//	clip[16].h = 60;
//
//	clip[17].x = 60;
//	clip[17].y = 10;
//	clip[17].w = 60;
//	clip[17].h = 60;
//
//	clip[18].x = 120;
//	clip[18].y = 10;
//	clip[18].w = 55;
//	clip[18].h = 60;
//
//	clip[19].x = 175;
//	clip[19].y = 10;
//	clip[19].w = 55;
//	clip[19].h = 60;
//
//	clip[20].x = 230;
//	clip[20].y = 10;
//	clip[20].w = 55;
//	clip[20].h = 60;
//
//	clip[21].x = 285;
//	clip[21].y = 10;
//	clip[21].w = 55;
//	clip[21].h = 60;
//
//}
//
//void setPufferfishRects(SDL_Rect* clip) {
//	
//	clip[0].x = 5;
//	clip[0].y = 370;
//	clip[0].w = 85;
//	clip[0].h = 120;
//
//	clip[1].x = 90;
//	clip[1].y = 370;
//	clip[1].w = 90;
//	clip[1].h = 120;
//
//	clip[2].x = 180;
//	clip[2].y = 370;
//	clip[2].w = 100;
//	clip[2].h = 120;
//
//	clip[3].x = 280;
//	clip[3].y = 370;
//	clip[3].w = 105;
//	clip[3].h = 120;
//
//	clip[4].x = 385;
//	clip[4].y = 370;
//	clip[4].w = 120;
//	clip[4].h = 120;
//
//	clip[5].x = 505;
//	clip[5].y = 370;
//	clip[5].w = 135;
//	clip[5].h = 120;
//
//	clip[6].x = 640;
//	clip[6].y = 370;
//	clip[6].w = 125;
//	clip[6].h = 120;
//
//	clip[7].x = 765;
//	clip[7].y = 370;
//	clip[7].w = 135;
//	clip[7].h = 120;
//
//	clip[8].x = 900;
//	clip[8].y = 370;
//	clip[8].w = 135;
//	clip[8].h = 120;
//
//	clip[9].x = 1035;
//	clip[9].y = 370;
//	clip[9].w = 135;
//	clip[9].h = 120;
//
//	clip[10].x = 1170;
//	clip[10].y = 370;
//	clip[10].w = 130;
//	clip[10].h = 120;
//
//	clip[11].x = 1300;
//	clip[11].y = 370;
//	clip[11].w = 120;
//	clip[11].h = 120;
//
//	clip[12].x = 1420;
//	clip[12].y = 370;
//	clip[12].w = 110;
//	clip[12].h = 120;
//
//	clip[13].x = 1530;
//	clip[13].y = 370;
//	clip[13].w = 100;
//	clip[13].h = 120;
//
//	clip[14].x = 1630;
//	clip[14].y = 370;
//	clip[14].w = 80;
//	clip[14].h = 120;
//
//	clip[15].x = 930;
//	clip[15].y = 235;
//	clip[15].w = 100;
//	clip[15].h = 125;
//
//	clip[16].x = 1030;
//	clip[16].y = 235;
//	clip[16].w = 90;
//	clip[16].h = 125;
//
//	clip[17].x = 1120;
//	clip[17].y = 235;
//	clip[17].w = 95;
//	clip[17].h = 125;
//
//	clip[18].x = 1215;
//	clip[18].y = 235;
//	clip[18].w = 95;
//	clip[18].h = 125;
//
//	clip[19].x = 1310;
//	clip[19].y = 235;
//	clip[19].w = 90;
//	clip[19].h = 125;
//
//	clip[20].x = 1400;
//	clip[20].y = 235;
//	clip[20].w = 100;
//	clip[20].h = 125;
//
//	clip[21].x = 1500;
//	clip[21].y = 235;
//	clip[21].w = 90;
//	clip[21].h = 125;
//
//	clip[22].x = 1590;
//	clip[22].y = 235;
//	clip[22].w = 90;
//	clip[22].h = 125;
//
//	clip[23].x = 930;
//	clip[23].y = 235;
//	clip[23].w = 100;
//	clip[23].h = 125;
//
//	clip[24].x = 1030;
//	clip[24].y = 235;
//	clip[24].w = 90;
//	clip[24].h = 125;
//
//	clip[25].x = 1120;
//	clip[25].y = 235;
//	clip[25].w = 95;
//	clip[25].h = 125;
//
//	clip[26].x = 1215;
//	clip[26].y = 235;
//	clip[26].w = 95;
//	clip[26].h = 125;
//
//	clip[27].x = 1310;
//	clip[27].y = 235;
//	clip[27].w = 90;
//	clip[27].h = 125;
//
//	clip[28].x = 1400;
//	clip[28].y = 235;
//	clip[28].w = 100;
//	clip[28].h = 125;
//
//	clip[29].x = 1500;
//	clip[29].y = 235;
//	clip[29].w = 90;
//	clip[29].h = 125;
//
//	clip[30].x = 1590;
//	clip[30].y = 235;
//	clip[30].w = 90;
//	clip[30].h = 125;
//
//	clip[31].x = 930;
//	clip[31].y = 235;
//	clip[31].w = 100;
//	clip[31].h = 125;
//
//	clip[32].x = 1030;
//	clip[32].y = 235;
//	clip[32].w = 90;
//	clip[32].h = 125;
//
//	clip[33].x = 1120;
//	clip[33].y = 235;
//	clip[33].w = 95;
//	clip[33].h = 125;
//
//	clip[34].x = 1215;
//	clip[34].y = 235;
//	clip[34].w = 95;
//	clip[34].h = 125;
//
//	clip[35].x = 1310;
//	clip[35].y = 235;
//	clip[35].w = 90;
//	clip[35].h = 125;
//
//	clip[36].x = 1400;
//	clip[36].y = 235;
//	clip[36].w = 100;
//	clip[36].h = 125;
//
//	clip[37].x = 1500;
//	clip[37].y = 235;
//	clip[37].w = 90;
//	clip[37].h = 125;
//
//	clip[38].x = 1590;
//	clip[38].y = 235;
//	clip[38].w = 90;
//	clip[38].h = 125;
//
//	clip[39].x = 930;
//	clip[39].y = 235;
//	clip[39].w = 100;
//	clip[39].h = 125;
//
//	clip[40].x = 1030;
//	clip[40].y = 235;
//	clip[40].w = 90;
//	clip[40].h = 125;
//
//	clip[41].x = 1120;
//	clip[41].y = 235;
//	clip[41].w = 95;
//	clip[41].h = 125;
//
//	clip[42].x = 1215;
//	clip[42].y = 235;
//	clip[42].w = 95;
//	clip[42].h = 125;
//
//	clip[43].x = 1310;
//	clip[43].y = 235;
//	clip[43].w = 90;
//	clip[43].h = 125;
//
//	clip[44].x = 1400;
//	clip[44].y = 235;
//	clip[44].w = 100;
//	clip[44].h = 125;
//
//	clip[45].x = 1500;
//	clip[45].y = 235;
//	clip[45].w = 90;
//	clip[45].h = 125;
//
//	clip[46].x = 1590;
//	clip[46].y = 235;
//	clip[46].w = 90;
//	clip[46].h = 125;
//
//	clip[47].x = 930;
//	clip[47].y = 235;
//	clip[47].w = 100;
//	clip[47].h = 125;
//
//	clip[48].x = 1030;
//	clip[48].y = 235;
//	clip[48].w = 90;
//	clip[48].h = 125;
//
//	clip[49].x = 1120;
//	clip[49].y = 235;
//	clip[49].w = 95;
//	clip[49].h = 125;
//
//	clip[50].x = 1215;
//	clip[50].y = 235;
//	clip[50].w = 95;
//	clip[50].h = 125;
//
//	clip[51].x = 1310;
//	clip[51].y = 235;
//	clip[51].w = 90;
//	clip[51].h = 125;
//
//	clip[52].x = 1400;
//	clip[52].y = 235;
//	clip[52].w = 100;
//	clip[52].h = 125;
//
//	clip[53].x = 1500;
//	clip[53].y = 235;
//	clip[53].w = 90;
//	clip[53].h = 125;
//
//	clip[54].x = 1590;
//	clip[54].y = 235;
//	clip[54].w = 90;
//	clip[54].h = 125;
//
//}
//
//void setCuttlefishRects(SDL_Rect* clip) {
//	for (int i = 0; i < 9; i++) {
//			clip[i].x = i*67;
//			clip[i].y = 65;
//			clip[i].w = 67;
//			clip[i].h = 50;
//	}
//
//	clip[9].x = 0;
//	clip[9].y = 20;
//	clip[9].w = 67;
//	clip[9].h = 30;
//
//	clip[10].x = 67;
//	clip[10].y = 20;
//	clip[10].w = 67;
//	clip[10].h = 30;
//
//	clip[11].x = 140;
//	clip[11].y = 20;
//	clip[11].w = 67;
//	clip[11].h = 30;
//
//	clip[12].x = 215;
//	clip[12].y = 20;
//	clip[12].w = 67;
//	clip[12].h = 30;
//
//	clip[13].x = 286;
//	clip[13].y = 20;
//	clip[13].w = 67;
//	clip[13].h = 30;
//
//	clip[14].x = 353;
//	clip[14].y = 20;
//	clip[14].w = 67;
//	clip[14].h = 30;
//
//	clip[15].x = 417;
//	clip[15].y = 235;
//	clip[15].w = 67;
//	clip[15].h = 125;
//
//}
//
//void setPatrickRects(SDL_Rect* clip) {
//	for (int j = 0; j < 8; j++)
//		for (int i = 0; i < 10; i++) {
//				clip[i+(10*j)].x = i*55;
//				clip[i+(10*j)].y = j*60;
//				clip[i+(10*j)].w = 55;
//				clip[i+(10*j)].h = 60;
//		}
//}
//
