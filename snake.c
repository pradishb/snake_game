#include <stdio.h>
#include <allegro5/allegro.h>
#include <time.h>
#include <stdlib.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

const float FPS = 60;
const int SCREEN_W = 640;
const int SCREEN_H = 480;
const int BLOCK_SIZE = 20;

enum MYKEYS {
    UP, DOWN, LEFT, RIGHT,ESCAPE,ENTER,BACKSPACE
};

enum GAME_MODES {
    MENU, GAME, GAME_OVER, CREDITS
};

struct segment
{
    int x;
    int y;
};

void ranFood(int *x, int *y)
{
    *x = rand()%(SCREEN_W/BLOCK_SIZE)*BLOCK_SIZE;
    *y = rand()%(SCREEN_H/BLOCK_SIZE)*BLOCK_SIZE;
}

int main(int argc, char **argv)
{
    //initialization
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_BITMAP *block = NULL;
    ALLEGRO_BITMAP *fblock = NULL;

    al_init();
    al_install_keyboard();
    al_init_font_addon();   // initialize the font addon
    al_init_ttf_addon();    // initialize the ttf (True Type Font) addon

    ALLEGRO_FONT *font[2];
    font[0] = al_load_ttf_font("./fonts/abscissa.ttf",50,0);
    font[1] = al_load_ttf_font("./fonts/abscissa.ttf",22,0);

    timer = al_create_timer(1.0 / FPS);
    display = al_create_display(SCREEN_W, SCREEN_H);

    block = al_create_bitmap(BLOCK_SIZE, BLOCK_SIZE);
    al_set_target_bitmap(block);
    al_clear_to_color(al_map_rgb(255, 0, 0));

    fblock = al_create_bitmap(BLOCK_SIZE, BLOCK_SIZE);
    al_set_target_bitmap(fblock);
    al_clear_to_color(al_map_rgb(255, 255, 255));

    al_set_target_bitmap(al_get_backbuffer(display));

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    //variable initialization
    srand(time(NULL));  //seeding random number
    int game_state=MENU;
    bool up_down = false;
    bool keys[6]={false,false,false,false,false,false};
    bool redraw = true;
    bool doexit = false;
    struct segment seg[(SCREEN_W*SCREEN_H)/(BLOCK_SIZE*BLOCK_SIZE)];
    struct segment food;
    struct segment temp;

    //game initialization
    float speed;
    int play;
    int score;
    int length;
    int direction;

    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);

    while(!doexit)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER) {
            if(game_state==GAME && play==0)
            {
                //check_death
                //if snake hits the wall
                if(seg[0].x>=SCREEN_W || seg[0].x<0 || seg[0].y>=SCREEN_H || seg[0].y<0)
                {
                    game_state=GAME_OVER;
                }

                //if snake eats itself
                for(int i=1;i<length;i++)
                {
                    if(seg[0].x==seg[i].x && seg[0].y==seg[i].y)
                    {
                        game_state=GAME_OVER;
                    }

                }

                //move segment
                temp.x=seg[length-1].x;
                temp.y=seg[length-1].y;
                for(int i=length-1;i>0;i--)
                {
                    seg[i].x=seg[i-1].x;
                    seg[i].y=seg[i-1].y;

                }
                if(direction == UP)
                {
                    seg[0].y-=BLOCK_SIZE;
                }
                else if(direction == DOWN)
                {
                    seg[0].y+=BLOCK_SIZE;
                }
                else if(direction == RIGHT)
                {
                    seg[0].x+=BLOCK_SIZE;
                }
                else if(direction == LEFT)
                {
                    seg[0].x-=BLOCK_SIZE;
                }

                //check if food eaten
                if(seg[0].x==food.x && seg[0].y==food.y)
                {
                    length++;
                    score+=5;
                    speed+=.05;
                    seg[length-1].x=temp.x;
                    seg[length-1].y=temp.y;
                    ranFood(&food.x,&food.y);
                }
            redraw = true;
            }
            play++;
            if(play>10.0/speed)
                play=0;
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN || ev.type == ALLEGRO_EVENT_KEY_UP ) {

            if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                up_down = 1;
            } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                up_down = 0;
            }

            switch(ev.keyboard.keycode) {

                case ALLEGRO_KEY_ESCAPE:
                    keys[ESCAPE] = up_down;
                    break;
                case ALLEGRO_KEY_ENTER:
                    keys[ENTER] = up_down;
                    break;
                case ALLEGRO_KEY_BACKSPACE:
                    keys[BACKSPACE] = up_down;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    keys[RIGHT] = up_down;
                    break;
                case ALLEGRO_KEY_LEFT:
                    keys[LEFT] = up_down;
                    break;
                case ALLEGRO_KEY_UP:
                    keys[UP] = up_down;
                    break;
                case ALLEGRO_KEY_DOWN:
                    keys[DOWN] = up_down;
                    break;
            }

        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        //GAME KEYS
        if(game_state==GAME)
        {
            if(keys[RIGHT])
                direction = RIGHT;
            else if(keys[LEFT])
                direction = LEFT;
            else if(keys[UP])
                direction = UP;
            else if(keys[DOWN])
                direction = DOWN;
        }
        //MAIN MENU
        else if(game_state==MENU)
        {
            if(keys[ENTER])
            {
                game_state=GAME;
                redraw=true;
                speed=1.0;
                play=0;
                score=0;
                length=5;
                direction = RIGHT;
                ranFood(&food.x,&food.y);

                for(int i=0;i<length;i++)
                {
                    seg[i].x=240-BLOCK_SIZE*i;
                    seg[i].y=240;
                }

            }
            else if(keys[ESCAPE])
            {
                doexit=true;
            }
            else if(keys[BACKSPACE])
            {
                game_state=CREDITS;
                redraw=true;
            }

        }
        //GAME OVER
        else if(game_state==GAME_OVER)
        {
            if(keys[ENTER])
            {
                game_state=GAME;
                redraw=true;
                speed=1.0;
                play=0;
                score=0;
                length=5;
                direction = RIGHT;
                ranFood(&food.x,&food.y);

                for(int i=0;i<length;i++)
                {
                    seg[i].x=240-BLOCK_SIZE*i;
                    seg[i].y=240;
                }
            }
            if(keys[ESCAPE])
            {
                game_state=MENU;
                redraw=true;
                keys[ESCAPE]=false;
            }
        }
        //CREDITS
        else if(game_state==CREDITS)
        {
            if(keys[ESCAPE]||keys[ENTER])
            {
                game_state=MENU;
                redraw=true;
                keys[ESCAPE]=false;
                keys[ENTER]=false;
            }
        }

        //DRAW
        if(redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            al_clear_to_color(al_map_rgb(0,0,0));

            if(game_state==GAME)
            {
                for(int i=0;i<length;i++)
                {
                    al_draw_bitmap(block, seg[i].x, seg[i].y, 0);
                }

                al_draw_bitmap(fblock, food.x, food.y, 0);
                al_draw_textf(font[1], al_map_rgb(255,255,255), 15, SCREEN_H-40,ALLEGRO_ALIGN_LEFT, "Score : %d",score);
                al_draw_textf(font[1], al_map_rgb(255,255,255), 15, SCREEN_H-70,ALLEGRO_ALIGN_LEFT, "Level: %.0f",speed);
            }
            else if(game_state==MENU)
            {
                al_draw_text(font[0], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/4,ALLEGRO_ALIGN_CENTER, "SNAKE GAME");
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2,ALLEGRO_ALIGN_CENTER, "Press Enter to Play");
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2+30,ALLEGRO_ALIGN_CENTER, "Esc to Quit");
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2+60,ALLEGRO_ALIGN_CENTER, "Backspace for Credits");
            }
            else if(game_state==GAME_OVER)
            {
                al_draw_text(font[0], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/4,ALLEGRO_ALIGN_CENTER, "GAME OVER");
                al_draw_textf(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2,ALLEGRO_ALIGN_CENTER, "Score : %d",score);
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2+30,ALLEGRO_ALIGN_CENTER, "Press Enter to Play Again");
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2+60,ALLEGRO_ALIGN_CENTER, "Esc to get back to menu");
            }
            else if(game_state==CREDITS)
            {
                al_draw_text(font[0], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/4,ALLEGRO_ALIGN_CENTER, "CREDITS");
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2,ALLEGRO_ALIGN_CENTER, "Programmed by :");
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2+30,ALLEGRO_ALIGN_CENTER, "Pradish Bijukchhe");
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2+60,ALLEGRO_ALIGN_CENTER, "Esc/Enter to get back to menu");
            }
            al_flip_display();
        }
    }

    al_destroy_bitmap(block);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}
