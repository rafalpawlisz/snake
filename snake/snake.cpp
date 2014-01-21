#include<allegro.h>
#include<string>
#include<cmath>
#include<time.h>
#define unit 16 //diameter of the particular element
#define boardx 1280+2*unit //max width (with the buffer zone)
#define boardy 720+2*unit //max height (with the buffer zone)
using std::string;
void introduction(), preparation(int board[][boardy], float&, float&, int&, float&), counting(float&, float&, int, float&), fruit(int board [][boardy], float, float, int&), movement(int board[][boardy], float&, float&, int), displaying(BITMAP*, BITMAP*, int board[][boardy], int), score(BITMAP*, BITMAP*, int, int board[][boardy]), sound(SAMPLE*, int&), termination(BITMAP*, BITMAP*, SAMPLE*);
bool menu(BITMAP*, BITMAP*), obstacle(int board[][boardy], float, float);
int color(string), round(float), mute=1;

int main(){
	int board[boardx][boardy], length; //actual lenght is 9 fewer
	float headx, heady, angle; //predefined location of the snake's head
	srand(time(NULL)); //setting a seed on time dependence
	introduction(); //preparing the program
	BITMAP* buffer=create_bitmap(boardx-2*unit,boardy-2*unit); //creating a buffer
	BITMAP* graphics=load_bmp("graphics.bmp",default_palette); //creating a bitmap containing all the graphics
	SAMPLE* background=load_sample("background.wav"); //creating a backround music sample
	play_sample(background,255,127,1000,1); //plays the sample
	while(menu(buffer, graphics)){ //displaying a menu till exit the game
		preparation(board, headx, heady, length, angle);
		while(obstacle(board, headx, heady)){ //a game loop; checking if snake bumped
			counting(headx, heady, length, angle);
			fruit(board, headx, heady, length); //checking if snake ate fruit
			movement(board, headx, heady,length);
			displaying(buffer, graphics, board, length);
			if(key[KEY_ESC] && !menu(buffer,graphics)) //escape key action
				break;
			sound(background, mute); //m key action
		}
	}
	termination(buffer, graphics, background); //finishing the program
	return 0;
}
END_OF_MAIN();

void introduction(){ //preparing the program
	allegro_init();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,boardx-2*unit,boardy-2*unit,0,0);
	install_keyboard();
	install_mouse();
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "");
	set_volume(255,255);
}

void preparation(int board[boardx][boardy], float &headx, float &heady, int &length, float &angle){ //preparing the game
	//cleaning the board; the whole board is setting as a background
	for(int y=0;y<boardy;++y)
		for(int x=0;x<boardx;++x)
			board[x][y]=0;

	//fixing the snake's head
	headx=(boardx-2*unit)/2;
	heady=(boardy-2*unit)/2;
	length=10;
	angle=0;
	board[int(headx)][int(heady)]=10;

	//fixing the fruit
	board[rand()%(boardx-int(0.75*2*unit))+int(0.75*2*unit)][rand()%(boardy-int(0.75*2*unit))+int(0.75*2*unit)]=1;

	//fixing the obstacle
	for(int y=120;y<124;++y){
		for(int x=120;x<360;++x){
			board[x][y]=2;
			board[x+810][y]=2;
			board[x][y+480]=2;
			board[x+810][y+480]=2;
		}
	}
	for(int y=120;y<240;++y){
		for(int x=120;x<124;++x){
			board[x][y]=2;
			board[x+1050][y]=2;
			board[x][y+360]=2;
			board[x+1050][y+360]=2;
		}
	}
}

bool menu(BITMAP* buffer, BITMAP* graphics){ //handling a menu
	show_mouse(screen);

	//copying the background
	blit(graphics,buffer,0,0,0,0,600,600);
	blit(graphics,buffer,0,0,600,0,600,600);
	blit(graphics,buffer,0,0,900,0,600,600);
	blit(graphics,buffer,0,0,0,600,600,600);
	blit(graphics,buffer,0,0,600,600,600,600);
	blit(graphics,buffer,0,0,900,600,600,600);

	//copying the text
	masked_blit(graphics,buffer,0,600,340,50,600,220); //snake
	masked_blit(graphics,buffer,600,0,515,300,250,100); //start
	masked_blit(graphics,buffer,600,100,540,400,200,100); //exit

	//displaying everything
	blit(buffer,screen,0,0,0,0,1280,720);

	//reading mouse actions
	while(true){
		if(mouse_b==1){
			if(mouse_x>=515 && mouse_x<=765 && mouse_y>=300 && mouse_y<=400){
				show_mouse(NULL);
				return true;
			}
			if(mouse_x>=540 && mouse_x<=740 && mouse_y>=400 && mouse_y<=500){
				show_mouse(NULL);
				return false;
			}
		}
	}
	return false;
}

void counting(float &headx, float &heady, int length, float &angle){
	//rotation / speeding; counting new head position
	float speed=1;
	if(key[KEY_LEFT]) angle-=1./128;
	if(key[KEY_RIGHT]) angle+=1./128;
	if(key[KEY_UP]) speed=2;
	//if(key[KEY_DOWN]) speed=0.5; //the admin's feature ;)
	headx+=cos(angle*3.1415926535*2)*speed;
	heady+=sin(angle*3.1415926535*2)*speed;

	//teleportation between edges of the board
	if(headx<unit+1) headx=boardx-unit-1;
	if(headx>boardx-unit-1) headx=unit+1;
	if(heady<unit+1) heady=boardy-unit-1;
	if(heady>boardy-unit-1) heady=unit+1;
}

void fruit(int board[boardx][boardy], float headx, float heady, int &length){
	//eating the fruit
	for(int y=heady-unit;y<=heady+unit;++y){
		for(int x=headx-unit;x<headx+unit;++x){
			if(board[x][y]==1){
				board[x][y]=0;
				length+=5;

				//generating a new fruit
				do{
					x=rand()%(boardx-2*unit)+unit;
					y=rand()%(boardy-2*unit)+unit;
				}while(board[x][y]!=0);
				board[x][y]=1;
			//	board[rand()%(boardx-int(frame/2))+int(frame/2)][rand()%(boardy-int(frame/2))+int(frame/2)]=1; //old function
			}
		}
	}
}

void movement(int board[boardx][boardy], float &headx, float &heady, int length){
	//fixing the new position on the board
	board[round(headx)][round(heady)]=length+1;
	for(int y=unit/2;y<boardy-unit/2;++y)
		for(int x=unit/2;x<boardx-unit/2;++x)
			if(board[x][y]>=10)
				board[x][y]-=1;

	rest(0); //delay between steps
}

int round(float x){
	return floor(x+0.5);
}

void displaying(BITMAP* buffer, BITMAP* graphics, int board[boardx][boardy], int length){
	//drawing the background
	blit(graphics,buffer,0,0,0,0,600,600);
	blit(graphics,buffer,0,0,600,0,600,600);
	blit(graphics,buffer,0,0,900,0,600,600);
	blit(graphics,buffer,0,0,0,600,600,600);
	blit(graphics,buffer,0,0,600,600,600,600);
	blit(graphics,buffer,0,0,900,600,600,600);

	for(int y=unit;y<boardy-unit;++y){
		for(int x=unit;x<boardx-unit;++x){
			if(board[x][y]>10) //drawing the snake's body
				masked_blit(graphics,buffer,640,200,x-unit/2,y-unit/2,16,16);
			if(board[x][y]==1) //drawing the fruit
				masked_blit(graphics,buffer,620,200,x-unit/2,y-unit/2,16,16);
			if(board[x][y]==2) //drawing the obstacles
				masked_blit(graphics,buffer,600,200,x-unit/2,y-unit/2,16,16);
		}
	}
	for(int y=unit;y<boardy-unit;++y) //drawing the snake's head
		for(int x=unit;x<boardx-unit;++x)
			if(board[x][y]==length)
				masked_blit(graphics,buffer,660,200,x-unit/2,y-unit/2,16,16);

	score(graphics, buffer, length, board);

	int x,y; //don't know
	blit(buffer,screen,0,0,0,0,boardx-2*unit,boardy-2*unit); //copying the buffer to the screen; actual displaying
}

void score(BITMAP* graphics, BITMAP* buffer, int length, int board[boardx][boardy]){ //not ready yet
	//int place=1; //like in the coordinate system

	//for(int y=unit;y<50;++y)
	//	for(int x=boardx-unit-360;x<boardx-unit;++x)
	//		if(board[x][y]!=0)
	//			place=0;

	//int x,y;
	//if(place==1){
	//	x=unit;
	//	y=boardy-unit-360;
	//	masked_blit(graphics,buffer,0,830,904,0,110,50);
	//}

}

//old functio; to be removed soon
int color(string x){ //making default colors
	if(x=="bg") return makecol(150,200,0); //backround color
	if(x=="snake") return makecol(50,70,10);
	if(x=="fruit") return makecol(255,0,0);
	if(x=="obstacle") return makecol(0,0,0);
	return 0;
}

bool obstacle(int board[boardx][boardy], float headx, float heady){
	//hitting the wall
	for(int y=heady-unit;y<=heady+unit;++y){
		for(int x=headx-unit;x<headx+unit;++x){
			if(board[x][y]==2){
				board[x][y]=0;
				return false;
			}
		}
	}
	//for(int y=4;y<boardy-4;++y){
	//	for(int x=4;x<boardx-4;++x){
	//		if(board[x][y]==20){
	//			headx=x;
	//			heady=y;
	//		}
	//	}
	//}
	//for(int y=heady-4;y<=heady+4;++y){
	//	for(int x=headx-4;x<headx+4;++x){
	//		if(board[x][y]>20){
	//			board[x][y]=0;
	//			return false;
	//		}
	//	}
	//}
	return true;
}

void sound(SAMPLE* background, int &mute){ //muting
	if(key[KEY_M])
		mute*=-1;

	if(mute==1){
		adjust_sample(background,255,127,1000,1);
	}

	if(mute==-1){
		adjust_sample(background,0,127,1000,1);
	}
}

void termination(BITMAP* buffer, BITMAP* graphics, SAMPLE* background){ //finishes the program
	destroy_bitmap(buffer);
	destroy_bitmap(graphics);
	destroy_sample(background);
	allegro_exit();
}