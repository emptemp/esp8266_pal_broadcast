
int WIDTH = 512;
int HEIGHT = 312;

int timer;
boolean start=false;

int y_pos1 = 156;
int y_pos2 = 156;

int vx=15;
int vy=0;
int x=30;
int y=151;

void setup()
{
  size(512, 312);
  background(0);
  noStroke();
}

void getDotPos()
{
  if(y > HEIGHT-10 || y < 10)
  {
    vy=vy*-1;
  }
  if(x > WIDTH-10)
  {
    vx=vx*-1;
  }
  if(x <= 20 && y_pos1-40 < y && y < y_pos1+40)
  {
    vy=(y-y_pos1)/2;
    vx=vx*-1;
  }
  if(x >= WIDTH-20 && y_pos2-40 < y && y < y_pos2+40)
  {
    vy=(y-y_pos2)/2;
    vx=vx*-1;
  }
  else
  {
    textSize(32);
    text("GAME OVER", 20,20);
  }
  x = x + vx;
  y = y + vy;
}

void draw() {
//  if ((millis() - timer >= 5) && start==true) 
//  {
    getDotPos();
//    timer = millis();
//  }
  clear();
  rect(0, y_pos1-40, 20, 80);
  rect(WIDTH-20, y_pos2-40, WIDTH, 80);
  rect(x,y,10,10);
  delay(100);
}

void keyPressed() {
  if (key == 'w') 
  {
    y_pos1-=5;
  } 
  else if (key == 's') 
  {
    y_pos1+=5;
  }
  else if (key == 'o') 
  {
    y_pos2-=5;
  } 
  else if (key == 'l') 
  {
    y_pos2+=5;
  }
  else
  {
    start=true;
  }
}
