//
//  volatileMap.c
//  volatileMap
//
// Copyright (C) 2020 Luca Giacometti [samelinux@gmail.com]
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <inttypes.h>
#include <sys/time.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>

#define MAP_W (80)
#define MAP_H (22)

uint32_t mapx=0;
uint32_t mapy=0;

char getch()
{
 char buf=0;
 bool didSetTermios=false;
 struct termios old;
 if(tcgetattr(0,&old)>=0)
 {
  old.c_lflag&=~ICANON;
  old.c_lflag&=~ECHO;
  old.c_cc[VMIN]=1;
  old.c_cc[VTIME]=0;
  tcsetattr(0,TCSANOW,&old);
  didSetTermios=true;
 }
 read(0,&buf,1);
 if(didSetTermios)
 {
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  tcsetattr(0,TCSADRAIN,&old);
 }
 return buf;
}

uint64_t lehmerSeed=0;

void srandomLehmer(uint64_t seed)
{
 lehmerSeed=seed;
}

int randomLehmer(int min,int max)
{
 uint64_t tmp=0;
 uint32_t multiplier1=0;
 if (max-min+1<=0)
 {
  return 0;
 }
 lehmerSeed+=0xe120fc15;
 tmp=(uint64_t)lehmerSeed*0x4a39b70d;
 multiplier1=(tmp>>32)^tmp;
 tmp=(uint64_t)multiplier1*0x12fad5c9;
 return ((tmp>>32)^tmp)%(max-min+1)+min;
}

long long getTime()
{
 struct timeval te;
 gettimeofday(&te,NULL);
 return te.tv_sec*1000LL+te.tv_usec/1000;
}

void move(int dx,int dy)
{
 uint32_t newx=mapx+dx;
 uint32_t newy=mapy+dy;
 if(newx>=0 && newx<0xFFFFFFFF-MAP_W)
 {
  mapx+=dx;
 }
 if(newy>=0 && newy<0xFFFFFFFF-MAP_H)
 {
  mapy+=dy;
 }
}

int main(int argc,char** argv)
{
 long long t1;
 char command=0;
 while(1)
 {
  t1=getTime();
  printf("\0331;1H\033[2J");
  printf("\033[H");
  for(uint64_t y=mapy;y<mapy+MAP_H;y++)
  {
   for(uint64_t x=mapx;x<mapx+MAP_W;x++)
   {
    srandomLehmer(((y/50)<<32)|(x/50));
    if(randomLehmer(0,100)<85)
    {
     srandomLehmer(((y)<<32)|(x));
     printf("%c",randomLehmer(0,100)>90?'T':' ');
     continue;
    }
    char newTile=' ';
    char tile[9];
    int size=0;
    int start=0;
    int houseCount=0;
    int i=0;
    for(int dy=-1;dy<=1;dy++)
    {
     for(int dx=-1;dx<=1;dx++)
     {
      srandomLehmer(((y/10+dy)<<32)|(x/10+dx));
      newTile=randomLehmer(0,100)>60?'c':' ';
      if(newTile=='c' && i!=4) houseCount++;
      tile[i++]=newTile;
      size=randomLehmer(2,7);
      start=randomLehmer(1,2);
     }
    }
    if(tile[3]!='c' && tile[4]=='c')
    {
     if(((y%10==start || y%10==start+size) && (x%10>=start && x%10<=start+size))||
       ((x%10==start || x%10==start+size) && (y%10>=start && y%10<=start+size)))
     {
      if(x%10==start && y%10==start+size/2 && tile[4]=='c')
      {
       tile[4]='+';
      }
      else
      {
       tile[4]='#';
      }
     }
     else
     {
      tile[4]=' ';
     }
    }
    else
    {
     srandomLehmer(((y/5)<<32)|(x/5));
     bool test=x/5<=1 || x/5>=22 || y/5<=1 || y/5>=8;
     if(randomLehmer(0,100)>80 || test)
     {
      srandomLehmer((y<<32)|x);
      tile[4]=randomLehmer(0,100)>95?'T':' ';
     }
     else
     {
      tile[4]=' ';
     }
    }
    printf("%c",tile[4]);
   }
   printf("\n");
  }
  printf("Generating map at %u,%u took: %lld ms\n",mapx,mapy,getTime()-t1);
  printf("Use yubnhjkl to move around, q to quit\n");
  command=getch();
  switch(command)
  {
   case 'Y': move(-MAP_W,-MAP_H); break;
   case 'U': move(+MAP_W,-MAP_H); break;
   case 'H': move(-MAP_W,+0); break;
   case 'J': move(+0,+MAP_H); break;
   case 'K': move(+0,-MAP_H); break;
   case 'L': move(+MAP_W,+0); break;
   case 'B': move(-MAP_W,+MAP_H); break;
   case 'N': move(+MAP_W,+MAP_H); break;
   case 'y': move(-1,-1); break;
   case 'u': move(+1,-1); break;
   case 'h': move(-1,+0); break;
   case 'j': move(+0,+1); break;
   case 'k': move(+0,-1); break;
   case 'l': move(+1,+0); break;
   case 'b': move(-1,+1); break;
   case 'n': move(+1,+1); break;
   case 'q':
    return 0;
    break;
  }
 }
 return 0;
}

