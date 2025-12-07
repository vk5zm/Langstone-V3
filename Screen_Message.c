#include "Graphics.h"


int screenrotate = 0;
int V2Display =0 ;


void showMessage(char * msg);
void setRotation(int rot);
int readConfig(void);

int main(int argc, char* argv[])
{
  readConfig();
  setRotation(screenrotate);
  initScreen();
  setBackColour(0,0,0);
  clearScreen();
  setForeColour(0,255,255);
  textSize = 4;  
  int y=100;
  int l = 1;
  while(argc > 1)
  {
    int p = strlen(argv[l]) * 32;
    gotoXY(400 - (p/2),y);
    displayStr(argv[l++]); 
    argc--;
    y=y+80;
  }

}


void setRotation(int rot)
{
  rotateScreen(rot);
}

int readConfig(void)
{
FILE * conffile;
char variable[50];
char value[100];
char vname[20];

conffile=fopen("$HOME/Langstone/Langstone_Pluto.conf","r");

if(conffile==NULL)
  {
    conffile=fopen("$HOME/Langstone/Langstone_Hack.conf","r");
  }
  
if(conffile==NULL)
  {
    return -1;
  }

while(fscanf(conffile,"%49s %99s [^\n]\n",variable,value) !=EOF)
  {
     
    if(strstr(variable,"RotateScreen")) sscanf(value,"%d",&screenrotate);            
  }

fclose(conffile);
return 0;

}
