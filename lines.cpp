//
// Created by v.bakayev on 25.01.2017.
//
#include <stdio.h>
#include <cstdlib>
#include "lines.h"
#include "main.h"

#define isSelected()  (selected !=-1)
#define MIN_LINE_LEN 4
int NODES [SIZE*SIZE]= {
                1,1,1,0,2,2,2,2,
                1,0,0,1,2,0,0,0,
                1,0,0,1,2,0,0,0,
                1,1,1,0,2,2,2,0,
                1,0,0,1,2,0,0,2,
                1,0,0,1,2,0,0,2,
                1,1,1,0,2,2,2,0,
                0,0,0,0,0,0,0,0 };
int NEXT[3];
int num_next;
int selected = -1;
int try_move(int from, int to);
int find_path(int from, int to);
int check_line (int point);
int put_next( int next );
void gen_next(void);
void game_over(void);
// ADDITION
int score;
//
int num_free();
void new_game(void) {
    score=0;
    for (int i=0; i<SIZE*SIZE; i++) NODES[i]=0;
    gen_next();
    for(int i=0; i<num_next; i++) put_next(NEXT[i]);
    gen_next();
}
int select (int num) {
    if (isSelected()) {
        int iret = try_move(selected, num);
        if (iret == 0)
            return ALARM_NO_PATH;
        int good = check_line(num);
        score+=good;
        if (good == 0)
        {
            //put next cells to field
            for (int i = 0; i < num_next; i++) {
                int pos = put_next(NEXT[i]); printf("putnext %d\n",pos);
                check_line(pos);
            }
            if (num_free()==0)
                return GAME_OVER;

            gen_next();
            //end put next cells to field
        }
    }
    else
    {
        if (NODES[num]==0)
            return ALARM_ERROR_SELECT;
        selected = num;
    }
    return 0;
}

int try_move(int from, int to) {
    selected = -1;
    if (from==to)return 0;
    if (find_path(from, to)==0)return 0;

    NODES[to]=NODES[from];
    NODES[from]=0;
    return 1;
}

//// ПРОВЕРКА НАЛИЧИЯ ПУТИ
int CELLS[SIZE*SIZE];
int count=0;

void check (int from);

int find_path(int from, int to) {
    CELLS[0]=from; count=1;
    for(int i =0; i< count; i++ )
    {
        check (CELLS[i]);
    }
    //
    for(int i =0; i< count; i++ )
         if(CELLS[i] == to )
             return 1;
    return 0;
}

void AddCell(int n) {
    for(int i =0; i< count; i++ )
    {
        if(CELLS[i]==n) return;
    }
    CELLS[count]=n; count++;
}

void check (int from) {
    int x=from%SIZE;
    int y=from/SIZE;
    if (x>0)
        if(NODES[y*SIZE+x-1]==0) AddCell(y*SIZE+x-1);
    if (x<SIZE-1)
        if(NODES[y*SIZE+x+1]==0) AddCell(y*SIZE+x+1);
    if (y>0)
        if(NODES[(y-1)*SIZE+x]==0) AddCell((y-1)*SIZE+x);
    if (y<SIZE-1)
        if(NODES[(y+1)*SIZE+x]==0) AddCell((y+1)*SIZE+x);
}
//// ПРОВЕРКА НАЛИЧИЯ ПУТИ END

int POINTS[SIZE * 4];
int points_count;

int calc_line( int val, int x0, int y0, int dx, int dy ) {
    int count = 0;
    int x = x0+dx;     int y = y0+dy;
    while ( ( y >= 0 ) && ( y < SIZE ) && ( x >= 0 ) && ( x < SIZE) ) {
        int i = y * SIZE + x;
        if (NODES[i] != val) break;
        POINTS[points_count++] = i;
        count++;
        x+=dx; y+=dy;
    }
    x = x0-dx; y = y0-dy;
    while ( ( y >= 0 ) && ( y < SIZE ) && ( x >= 0 ) && ( x < SIZE) ) {
        int i = y * SIZE + x ;
        if (NODES[i] != val) break;
        POINTS[points_count++] = i ;
        count++;
        x-=dx; y-=dy;
    }
    return count;
}

int check_line (int point) {

    int x = point % SIZE;
    int y = point / SIZE;
    int val = NODES[point];
    int count;

    points_count = 0;

    count = calc_line ( val, x, y, 1, 0); //HORIZ LINE
    if (count < MIN_LINE_LEN) points_count -= count;

    count = calc_line ( val, x, y, 0, 1); //VERT LINE
    if (count < MIN_LINE_LEN) points_count -= count;

    count = calc_line ( val, x, y, 1, 1); //DIAG LINE SE-NW
    if (count < MIN_LINE_LEN) points_count -= count;

    count = calc_line ( val, x, y, 1, -1); //DIAG LINE NE-SW
    if (count < MIN_LINE_LEN) points_count -= count;

    if (points_count == 0) return 0;

    // clear LINES
    for (int i = 0; i<points_count;i++) {
        int n = POINTS[i];
        NODES[n] = 0;
    }
    NODES[point] = 0;
    return points_count+1;
}

int num_free(){
    int n = 0;

    for ( int i=0; i<SIZE*SIZE; i++ )
    {
        if (NODES[i]==0) n++;
    }
    printf("num_free %d\n",n);
    return n;
}

void gen_next(void){
    for(int i=0; i<3; i++) NEXT[i]=0;
    int free=num_free();
    num_next = free < 3 ? free : 3;
    for(int i=0; i< num_next;i++)
        NEXT[i]=1+((NUM_CELL_TYPE-1) * rand())/RAND_MAX; //0
    }

int put_next( int next ) {
        int free = num_free();
        int index = ((free-1) * rand()) / RAND_MAX;
        printf ("index = %d\n", index);
        int count = 0;
        for (int i = 0; i < SIZE * SIZE; i++)
        {
            if (NODES[i] == 0) {
                if (count == index) {
                    NODES[i] = next;
                    return i;
                }
                count++;
            }
        }
}
