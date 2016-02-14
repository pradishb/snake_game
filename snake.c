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
    UP, DOWN, LEFT, RIGHT,ESCAPE,ENTER,BACKSPACE,H
};

enum GAME_MODES {
    MENU, GAME, GAME_OVER, CREDITS, HIGH_SCORES, NEW_RECORD
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

void read_scores(char *players,int *scores)
{
    FILE *fp;
    fp = fopen("./.highscores", "r");
    fseek(fp, 0L, 0); // seek to the first
    for(int i=0;i<3;i++)
    {
        fscanf(fp, "%s", players+i*12);
        fscanf(fp, "%d", scores+i);
    }
    fclose(fp);
}

void write_scores(char *players,int *scores)
{
    FILE *fp;
    fp = fopen("./.highscores", "w+");
    for(int i=0;i<3;i++)
    {
        fprintf(fp, "%s\t%d\n", players+i*12,*(scores+i));
    }
    fclose(fp);
}

void create_scores()
{
    FILE *fp;
    int pos;
    fp = fopen("./.highscores", "a+");
    fseek(fp, 0L, SEEK_END); // seek to the end
    pos = ftell(fp);    //tell position
    if(pos==0)          //check if empty, if empty create
    {
        fprintf(fp,"%s\t%d\n","chiku", 250);
        fprintf(fp,"%s\t%d\n","vale", 150);
        fprintf(fp,"%s\t%d\n","kale", 100);
    }
    fclose(fp);

}
int main(int argc, char **argv)
{
    //initialization
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_BITMAP *block = NULL;
    ALLEGRO_BITMAP *fblock = NULL;
    ALLEGRO_USTR *input = al_ustr_new("");

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
    bool keys[8]={false,false,false,false,false,false,false,false};
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
    int s_direction;
    char buffer[12];

    //highscores initialization
    char top_players[3][12];
    int top_scores[3];

    create_scores();

    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);

    while(!doexit)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER) {
            if(game_state==GAME)
            {
                if(play==0)
                {
                    //change snake direction
                    if(!(s_direction==LEFT && direction==RIGHT) && !(s_direction==RIGHT && direction==LEFT) && !(s_direction==UP && direction==DOWN) && !(s_direction==DOWN && direction==UP))
                        s_direction=direction;

                    //check_death
                    //if snake hits the wall
                    if(seg[0].x>=SCREEN_W || seg[0].x<0 || seg[0].y>=SCREEN_H || seg[0].y<0)
                    {
                        if(score>top_scores[2])
                            game_state=NEW_RECORD;
                        else
                            game_state=GAME_OVER;
                    }

                    //if snake eats itself
                    for(int i=1;i<length;i++)
                    {
                        if(seg[0].x==seg[i].x && seg[0].y==seg[i].y)
                        {
                            if(score>top_scores[2])
                                game_state=NEW_RECORD;
                            else
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
                    if(s_direction == UP)
                    {
                        seg[0].y-=BLOCK_SIZE;
                    }
                    else if(s_direction == DOWN)
                    {
                        seg[0].y+=BLOCK_SIZE;
                    }
                    else if(s_direction == RIGHT)
                    {
                        seg[0].x+=BLOCK_SIZE;
                    }
                    else if(s_direction == LEFT)
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
                //Control Speed
                play++;
                if(play>10.0/speed)
                    play=0;
            }
            else if(game_state==NEW_RECORD)
            {
                al_ustr_to_buffer(input,buffer,13);
                redraw = true;
            }
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
                case ALLEGRO_KEY_H:
                    keys[H] = up_down;
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
                s_direction = RIGHT;
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
            else if(keys[H])
            {
                game_state=HIGH_SCORES;
                read_scores(top_players[0],top_scores);
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
                s_direction = RIGHT;
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
        //HIGH_SCORES
        else if(game_state==HIGH_SCORES)
        {
            if(keys[ESCAPE]||keys[ENTER])
            {
                game_state=MENU;
                redraw=true;
                keys[ESCAPE]=false;
                keys[ENTER]=false;
            }
        }
        //NEW_RECORD
        else if(game_state==NEW_RECORD)
        {
            if(keys[ENTER])
            {
                game_state=HIGH_SCORES;
                if(score>top_scores[0])
                {
                    top_scores[2]=top_scores[1];
                    top_scores[1]=top_scores[0];
                    top_scores[0]=score;
                    strcpy(top_players[2],top_players[1]);
                    strcpy(top_players[1],top_players[0]);
                    strcpy(top_players[0],buffer);
                }
                else if(score>top_scores[1])
                {
                    top_scores[2]=top_scores[1];
                    top_scores[1]=score;
                    strcpy(top_players[2],top_players[1]);
                    strcpy(top_players[1],buffer);
                }
                else
                {
                    top_scores[2]=score;
                    strcpy(top_players[2],buffer);
                }
                write_scores(top_players[0],top_scores);
                read_scores(top_players[0],top_scores);
                keys[ENTER]=false;
                redraw=true;
            }
            if(ev.type == ALLEGRO_EVENT_KEY_CHAR) {
                int unichar = ev.keyboard.unichar;
                int buffer_length=al_ustr_length(input);
                if (unichar >= 32 && buffer_length<12)
                    al_ustr_append_chr(input, unichar);
                if(keys[BACKSPACE])
                {
                    al_ustr_remove_chr(input,buffer_length-1);
                }
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
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2+90,ALLEGRO_ALIGN_CENTER, "H for Highscores");
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
            else if(game_state==HIGH_SCORES)
            {
                al_draw_text(font[0], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/4,ALLEGRO_ALIGN_CENTER, "HIGH SCORES");
                for(int i=0;i<3;i++)
                {
                    al_draw_textf(font[1], al_map_rgb(255,255,255), SCREEN_W/4, SCREEN_H/2+30*i,ALLEGRO_ALIGN_LEFT, "%s", top_players[i]);
                    al_draw_textf(font[1], al_map_rgb(255,255,255), SCREEN_W/1.35, SCREEN_H/2+30*i,ALLEGRO_ALIGN_RIGHT, "%d", top_scores[i]);
                }
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2+120,ALLEGRO_ALIGN_CENTER, "Esc/Enter to get back to menu");
            }
            else if(game_state==NEW_RECORD)
            {
                al_draw_text(font[0], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/4,ALLEGRO_ALIGN_CENTER, "NEW RECORD");
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2,ALLEGRO_ALIGN_CENTER, "Enter you name");
                al_draw_textf(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2+30,ALLEGRO_ALIGN_CENTER, "%s_", buffer);
                al_draw_text(font[1], al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2+90,ALLEGRO_ALIGN_CENTER, "Enter to continue");
            }
            al_flip_display();
        }
    }

    al_ustr_free(input);
    al_destroy_bitmap(block);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}
