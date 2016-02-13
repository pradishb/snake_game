#include <stdio.h>
#include <allegro5/allegro.h>
#include <time.h>
#include <stdlib.h>

const float FPS = 8;
const int SCREEN_W = 640;
const int SCREEN_H = 480;
const int BLOCK_SIZE = 20;

enum MYKEYS {
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
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

    al_init();
    al_install_keyboard();

    timer = al_create_timer(1.0 / FPS);
    display = al_create_display(SCREEN_W, SCREEN_H);
    block = al_create_bitmap(BLOCK_SIZE, BLOCK_SIZE);

    al_set_target_bitmap(block);
    al_clear_to_color(al_map_rgb(255, 0, 0));
    al_set_target_bitmap(al_get_backbuffer(display));

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    //variable initialization
    srand(time(NULL));  //seeding random number
    int length=5;
    int direction = KEY_UP;
    bool redraw = true;
    bool doexit = false;
    struct segment seg[(SCREEN_W*SCREEN_H)/(BLOCK_SIZE*BLOCK_SIZE)];
    struct segment food;
    struct segment temp;

    ranFood(&food.x,&food.y);

    for(int i=0;i<length;i++)
    {
        seg[i].x=240-BLOCK_SIZE*i;
        seg[i].y=240;
    }

    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);

    while(!doexit)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER) {
            //check_death
            //if snake hits the wall
            if(seg[0].x>SCREEN_W || seg[0].x<0 || seg[0].y>SCREEN_H || seg[0].y<0)
            {
                doexit = true;
            }

            //if snake eats itself
            for(int i=1;i<length;i++)
            {
                if(seg[0].x==seg[i].x && seg[0].y==seg[i].y)
                {
                    doexit = true;
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
            if(direction == KEY_UP)
            {
                seg[0].y-=BLOCK_SIZE;
            }
            else if(direction == KEY_DOWN)
            {
                seg[0].y+=BLOCK_SIZE;
            }
            else if(direction == KEY_RIGHT)
            {
                seg[0].x+=BLOCK_SIZE;
            }
            else if(direction == KEY_LEFT)
            {
                seg[0].x-=BLOCK_SIZE;
            }

            //check if food eaten
            if(seg[0].x==food.x && seg[0].y==food.y)
            {
                length++;
                seg[length-1].x=temp.x;
                seg[length-1].y=temp.y;
                ranFood(&food.x,&food.y);
            }
            redraw = true;
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch(ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    direction = KEY_UP;
                    break;

                case ALLEGRO_KEY_DOWN:
                    direction = KEY_DOWN;
                    break;

                case ALLEGRO_KEY_LEFT:
                    direction = KEY_LEFT;
                    break;

                case ALLEGRO_KEY_RIGHT:
                    direction = KEY_RIGHT;
                    break;
            }
        }
        if(redraw && al_is_event_queue_empty(event_queue) && !doexit) {
            redraw = false;

            al_clear_to_color(al_map_rgb(0,0,0));

            for(int i=0;i<length;i++)
            {
                al_draw_bitmap(block, seg[i].x, seg[i].y, 0);
            }

            al_draw_bitmap(block, food.x, food.y, 0);

            al_flip_display();
        }
    }
    printf("GAME OVER\n");
    printf("SCORE : %d\n", length*5-25);

    al_destroy_bitmap(block);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}
