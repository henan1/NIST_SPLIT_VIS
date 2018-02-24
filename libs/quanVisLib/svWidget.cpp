


#include "svWidget.h"
#include <cmath>

namespace __svl_lib {

svWidget::svWidget(svRawSliceData *data)
{
  histovalues = new svScalarArray[NUM_TOP_MAG];
  Init(data);
}

void svWidget::Init(svRawSliceData *data)
{
    //cleanup();
    layer.clear();
    myData = data;
    level = data->splitData.size()
    for(int i=0;i<data->splitData.size();i++)
    {
      layer.push_back(false);
    }

   boxside = 2;
   triangleside = 2;

   line[0][0]=0;line[0][1]=boxside/2.;
   line[1][0]= boxside * (level+1); line[1][1] = boxside/2.;

   box[0][0]= line[0][0] + 0.5 * boxside; box[0][1] = 0;
   box[1][0]= line[1][0] - 0.5 * boxside; box[1][1] = 0;
   box[2][0]= line[1][0] - 0.5 * boxside; box[2][1] = 0;

   showbox = false; //the unselected layers

   svScalar value1=sqrt(3);

   svVector3 pos;
   pos[0] = box[0][0];
   pos[1] = box[0][1] - triangleside * sqrt(3)/3;
   svVector3 t[3];
   t[0][0] = -triangleside/2;
   t[0][1] = -triangleside * value1/6.;
   t[1][0] = triangleside/2;
   t[1][1] = -triangleside * value1/6.;
   t[2][0] = 0;
   t[2][1] = triangleside * value1/3.;

   triangle[0][0][0] = t[0][0] + pos[0];
   triangle[0][0][1] = t[0][1] + pos[1];
   triangle[0][1][0] = t[1][0] + pos[0];
   triangle[0][1][1] = t[1][1] + pos[1];
   triangle[0][2][0] = t[2][0] + pos[0];
   triangle[0][2][1] = t[2][1] + pos[1];

   pos[0] = box[1][0];
   pos[1] = box[1][1] - triangleside *  sqrt(3)/3;
   triangle[1][0][0] = t[0][0] + pos[0];
   triangle[1][0][1] = t[0][1] + pos[1] ;
   triangle[1][1][0] = t[1][0] + pos[0];
   triangle[1][1][1] = t[1][1] + pos[1];
   triangle[1][2][0] = t[2][0] + pos[0];
   triangle[1][2][1] = t[2][1] + pos[1];

   pos[0] = box[2][0];
   pos[1] = box[2][1] - triangleside *  sqrt(3)/3;
   triangle[2][0][0] = t[0][0] + pos[0];
   triangle[2][0][1] = t[0][1] + pos[1] ;
   triangle[2][1][0] = t[1][0] + pos[0];
   triangle[2][1][1] = t[1][1] + pos[1];
   triangle[2][2][0] = t[2][0] + pos[0];
   triangle[2][2][1] = t[2][1] + pos[1];

   SetHistoValues();
   UpdateState();
}

void svWidget::SetHistoValues()
//0->
//min->max
{
  tophisto = -1;
  for(int i=0;i<NUM_TOP_MAG;i++)
         histovalues[i].free();
  for(int i=0;i<NUM_TOP_MAG;i++)
  {
      for(int j=0;j<myData->histovalues[i].size();j++)
      {
         histovalues[i].add(myData->histovalues[i][j]);
      }
  }
  for(int j=0;j<myData->histovalues[0].size();j++)
  {
    svScalar count = 0;
    for(int i=0;i<NUM_TOP_MAG;i++)
    {
      count += myData->histovalues[i][j];
    }
    if(tophisto< count) tophisto = count;
  }
}
/*
void svWidget::Mouse(svScalar tranx, svScalar trany,
                   svScalar scalex, svScalar scaley,
                  int x, int y)
{
   mousex = x;
   mousey = y;
   for(int i=0;i<3;i++)
   {
      tselect[i] = false;
 //     bselect[i] = false;
   }
   bselect = false;
      if(x > box[0][0]*scalex + tranx
       && x < box[2][0]*scalex + tranx
       && y >box[0][1]*scaley+trany
       && y< (box[0][1]+boxside)*scaley+trany)
      {
          bselect=true;
      }
   if(!bselect) {
   int value = triangleside * sqrt(3);
   for(int i=0;i<3;i++)
   {
      if(x > triangle[i][0][0]*scalex+tranx
      && x < (triangle[i][0][0]+triangleside)*scalex + tranx
       && y > triangle[i][0][1]*scaley+trany
       && y< (triangle[i][0][1]+value)*scaley + trany)
      {
         if((i==2&&showbox) || i!=2)
         { //if(!(showbox && i==0))
           //{
               tselect[i]=true;break;
            //}
         }
      }
     }
     }
}
void svWidget::MoveRight()
{
   svScalar movement;

    int start = (box[0][0] - line[0][0] - boxside)/boxside;
    if(start <0) start = 0;
    else if(start < (box[0][0] - line[0][0] - boxside)/boxside) start = start + 1;

   movement = (line[0][0] + 0.5 * boxside + (start-1) * boxside) - box[0][0];

  if((box[1][0] + movement)> line[0][0] + 1.*boxside
   && (box[0][0]+movement)>line[0][0]
   && (box[2][0]+movement)<line[1][0])
  {
   for(int i=0;i<3;i++)
   {
         box[i][0] = box[i][0] + movement;
         for(int j=0;j<3;j++)
              triangle[i][j][0] =triangle[i][j][0]+movement;
   }
  }
  SetVisible();
}

void svWidget::MoveLeft()
{
   svScalar movement;

    int start = (box[0][0] - line[0][0] - boxside)/boxside;
    if(start <0) start = 0;
    else if(start < (box[0][0] - line[0][0] - boxside)/boxside) start = start + 1;

   movement = line[0][0] + 0.5 * boxside + (start+1) * boxside - box[0][0];

  if((box[1][0] + movement)> line[0][0] + 1.*boxside
   && (box[0][0]+movement)>line[0][0]
   && (box[2][0]+movement)<line[1][0])
  {
   for(int i=0;i<3;i++)
   {
         box[i][0] = box[i][0] + movement;
         for(int j=0;j<3;j++)
              triangle[i][j][0] =triangle[i][j][0]+movement;
   }
  }
  SetVisible();
}
void svWidget::Move(int x, int y)
{
   svScalar movement;
  if(bselect)
  {
            movement = x - mousex;
            mousex = x; mousey=y;

  if((box[1][0] + movement)> line[0][0] + 1.*boxside
   && (box[0][0]+movement)>line[0][0]
   && (box[2][0]+movement)<line[1][0])
  {
   for(int i=0;i<3;i++)
   {
         box[i][0] = box[i][0] + movement;
         for(int j=0;j<3;j++)
              triangle[i][j][0] =triangle[i][j][0]+movement;
   }
  }
  }
  else
  {
   int index=-1;
   for(int i=0;i<3;i++)
   {
      if(tselect[i])
      {
            movement = x - mousex;
            mousex = x; mousey=y;
            index = i;break;
      }
   }
  if( index==0)
  {
       if((box[0][0]+movement)>line[0][0] && box[0][0] +movement <= box[1][0]-boxside)
       {
         box[index][0] = box[index][0] + movement;
         for(int j=0;j<3;j++)
         triangle[index][j][0] =triangle[index][j][0]+movement;
       }
  }
  else if(index == 1)
  {
      if(showbox)
      {
       if(box[1][0]+movement >= box[0][0]+boxside &&  box[1][0] +movement <= box[2][0]-boxside)
       {
         box[index][0] = box[index][0] + movement;
         for(int j=0;j<3;j++)
         triangle[index][j][0] =triangle[index][j][0]+movement;
       }
     }
     else
     {
       if(box[1][0]+movement >= box[0][0]+boxside &&  box[1][0] +movement < line[1][0])
       {
         box[index][0] = box[index][0] + movement;
         for(int j=0;j<3;j++)
         triangle[index][j][0] =triangle[index][j][0]+movement;
         if(!showbox)
         {
            box[index+1][0] = box[index+1][0] + movement;
            for(int j=0;j<3;j++)
            triangle[index+1][j][0] =triangle[index+1][j][0]+movement;
         }
       }
     }
  }
  else if(index == 2)
  {
       if((box[2][0]+movement)<line[1][0] && box[2][0]+movement>=box[1][0]+boxside)
       {
         box[index][0] = box[index][0] + movement;
         for(int j=0;j<3;j++)
         triangle[index][j][0] =triangle[index][j][0]+movement;
       }
  }

  }

  SetVisible();
}
*/

void svWidget::RenderWidgets()
{
  svScalar maglevel = myData->myQDOT->maxExp-NUM_TOP_MAG+1;

  glEnable (GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor3f(0,0,0);
  glBegin(GL_LINES);
  glVertex2f(line[0][0], line[0][1]);
  glVertex2f(line[1][0], line[1][1]);
  glEnd();

  if(bselect)
     glColor4f(253./255., 174./255., 97./255.,0.5);
  else
     glColor4f(0.5,0.5,0.5,0.5);
  glBegin(GL_QUADS);
  glVertex2f(box[0][0],box[0][1]);
  glVertex2f(box[1][0],box[1][1]);
  glVertex2f(box[1][0],box[1][1]+boxside);
  glVertex2f(box[0][0],box[0][1]+boxside);
  glEnd();

  glColor4f(0.,0.,0.,0.5);

  for(int i=0;i<3;i++)
  {
     if(i==2 && !showbox)break;

      if(tselect[i])
        glColor4f(253./255., 174./255., 97./255.,0.5);
      else
        glColor4f(0.,0.,0.,0.5);

      glBegin(GL_TRIANGLES);
      for(int j=0;j<3;j++)
      {
           glVertex2f(triangle[i][j][0], triangle[i][j][1]);
      }
      glEnd();

      glColor3f(0,0,0);
      glBegin(GL_LINE_LOOP);
      for(int j=0;j<3;j++)
      {
           glVertex2f(triangle[i][j][0], triangle[i][j][1]);
      }
      glEnd();

  }

  glColor3f(0,0,0);
  glBegin(GL_LINE_STRIP);
  glVertex2f(box[1][0],box[1][1]);
  glVertex2f(box[2][0],box[2][1]);
  glVertex2f(box[2][0],box[2][1]+boxside);
  glVertex2f(box[1][0],box[1][1]+boxside);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex2f(box[0][0],box[0][1]);
  glVertex2f(box[1][0],box[1][1]);
  glVertex2f(box[1][0],box[1][1]+boxside);
  glVertex2f(box[0][0],box[0][1]+boxside);
  glEnd();

  glBegin(GL_LINES);
  glVertex2f(line[0][0],line[0][1]-boxside*0.5);
  glVertex2f(line[0][0],line[0][1]+boxside*0.5);
  glEnd();
  glBegin(GL_LINES);
  glVertex2f(line[1][0],line[1][1]-boxside*0.5);
  glVertex2f(line[1][0],line[1][1]+boxside*0.5);
  glEnd();

  glPointSize(4);
  glBegin(GL_POINTS);

  for(int i=0;i<level;i++)
  {
    if(layer[i]) glColor3f(165./255., 0, 38./255.);
    else glColor3f(0.5,0.5,0.5);
     glVertex2f(line[0][0]+(svScalar)i * boxside + boxside, line[0][1]);
  }
  glEnd();
  glPointSize(1);

  svScalar x[3];
  bool flag[3];flag[0]=false;
  if(box[1][0]-box[0][0] > 15*boxside)
  {
         flag[1]=false;
         x[0] = box[0][0];
         x[1] = box[1][0];
  }
  else
  {
            x[0] = box[0][0];
            x[1] = x[0] + 15*boxside;
            flag[1]=true;
  }
  if(!showbox)
  {
         x[2] = box[2][0];
         flag[2]=false;
  }
  else
  {
       if(box[2][0]-x[1] < 15*boxside)
       {      x[2] = x[1] + 15 * boxside;flag[2]=true;}
       else
       {      x[2] = box[2][0];flag[2]=false;}
   }

  glColor3f(0,0,0);
  char str[20];
  for(int i=0;i<3;i++)
  {
     if( (showbox &&i==2) || i<2)
     {
     sprintf(str, "(%0.2f, %0.2f, %0.2f)", values[i][0],values[i][1],values[i][2]);
     glRasterPos2f(x[i]+boxside*0.8,box[i][1]-boxside*1.3);
     for(int j=0;j<strlen(str);j++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[j]);

    if(flag[i])
    {
     glBegin(GL_LINES);
     glVertex2f(box[i][0], box[i][1]);
     glVertex2f(x[i], box[i][1]-boxside*1.5);
     glEnd();
    }
   }
  }

  glColor3f(0,0,0);
  for(int i=0;i<level;i=i+10)
  {
     sprintf(str, "%d", i);
     glRasterPos2f(line[0][0]+boxside + boxside*(float)i,box[0][1]+boxside*1.2);
     for(int j=0;j<strlen(str);j++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[j]);
     glBegin(GL_LINES);
     glVertex2f(line[0][0]+boxside + boxside*(float)i,box[0][1]+boxside*1.2);
     glVertex2f(line[0][0]+boxside + boxside*(float)i,line[0][1]);
     glEnd();
  }

}

//void svWidget::RenderEntropy()
//{
//============================================================
/*  svScalar seg = boxside/topmax;
  svScalar shift = boxside*1.5;
  glLineWidth(2);
  glBegin(GL_LINE_STRIP);
  for(int i=0;i<level;i++)
  {
     svScalar l = shift + seg * topvalues[i];
     glVertex2f(line[0][0]+boxside + boxside*(float)i,box[0][1]+l);
  }
  glEnd();
  glLineWidth(1);

  glColor3f(0.5,0.5,0.5);

  svScalar topmaxy = (svScalar)((int)topmax) * seg;
  glPointSize(1.5);
  glBegin(GL_POINTS);
  glVertex2f(line[0][0], line[0][1]+shift + topmaxy);
  glEnd();
  glPointSize(1.);

  glBegin(GL_LINES);
  glVertex2f(line[0][0], line[0][1] + shift);
  glVertex2f(line[1][0], line[0][1] + shift);
  glEnd();
  glBegin(GL_LINES);
  glVertex2f(line[0][0], line[0][1] + shift);
  glVertex2f(line[0][0], line[0][1] + shift + boxside);
  glEnd();

  glColor3f(0,0,0);
  sprintf(str, "Entropy");
  glRasterPos2f(line[0][0]-boxside*5,line[0][1] + shift + boxside);
     for(int j=0;j<strlen(str);j++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[j]);
*/
//  sprintf(str, "%d", (int)topmax);
//  glRasterPos2f(line[0][0]+boxside*0.5,line[0][1] + shift+topmaxy);
//     for(int j=0;j<strlen(str);j++)
//            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[j]);
//}

void svWidget::RenderMagHistogram(svColors *myColor)//svScalar maglevel)
{
//=============================================================
  svVector4Array colors[NUM_TOP_MAG];
  for(int i=0;i<NUM_TOP_MAG;i++)
  {
    svVector4 white(1,1,1,1);
    colors[i].add(white);
  }
  myColor->GetDivergingColors(NUM_TOP_MAG, false, colors);
//  colors[0][0]=215.;colors[0][1]=25.; colors[0][2]=28.;
//  colors[1][0]=253.;colors[1][1]=174.;colors[1][2]=97.;
//  colors[2][0]=171.;colors[2][1]=217.;colors[2][2]=233.;
//  colors[3][0]=44.; colors[3][1]=123.;colors[3][2]=182.;
  svScalar seg = boxside*2./tophisto;
  for(int i=0;i<histovalues[0].size();i++)
  {
    svScalar height;
    svScalar preheight = 0;
    svScalar y = line[0][1] - boxside*NUM_TOP_MAG;
    for(int j=0;j<NUM_TOP_MAG-1;j++)//j>=0;j--)
    {
      int cindex = j;//NUM_TOP_MAG - j -1;
      glColor3f(colors[cindex][0], colors[cindex][1], colors[cindex][2]);
      height = preheight+seg * histovalues[j][i];
      glBegin(GL_QUADS);
      glVertex2f(line[0][0]+boxside+(svScalar)i * boxside - boxside/3., y+preheight);
      glVertex2f(line[0][0]+boxside+(svScalar)i * boxside + boxside/3., y+preheight);
      glVertex2f(line[0][0]+boxside+(svScalar)i * boxside + boxside/3., y+height);
      glVertex2f(line[0][0]+boxside+(svScalar)i * boxside - boxside/3., y+height);
      glEnd();
      preheight =  height;
    }
  }
  svScalar lx = line[1][0];
  svScalar ly = line[0][1] - boxside*NUM_TOP_MAG;
  for(int i=0;i<NUM_TOP_MAG;i++)
  {
     glColor3f(colors[i][0], colors[i][1], colors[i][2]);
     glBegin(GL_QUADS);
     glVertex2f(lx, ly);
     glVertex2f(lx+boxside/2., ly);
     glVertex2f(lx+boxside/2., ly + boxside/2.);
     glVertex2f(lx, ly + boxside/2.);
     glEnd();
     ly =ly + boxside/2.;
  }
  glColor3f(0,0,0);
  lx = line[1][0];
  ly = line[0][1] - boxside*NUM_TOP_MAG;
  for(int i=0;i<NUM_TOP_MAG;i++)
  {
    int v = maglevel+i;//hard code!!!!!!
    sprintf(str, "1e%d",v);
    glRasterPos2f(line[1][0]+boxside*0.5,ly);
     for(int j=0;j<strlen(str);j++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[j]);
     ly =ly + boxside/2.;
 }

//  sprintf(str, "%d", (int)tophisto);
//  svScalar tophistoy = (svScalar)((int)tophisto) * seg;
//  glRasterPos2f(line[0][0]-boxside*3.5,line[0][1]-boxside*4+tophistoy);
//     for(int j=0;j<strlen(str);j++)
//            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[j]);

  glColor3f(0.5,0.5,0.5);
  glBegin(GL_LINES);
  glVertex2f(line[0][0], line[0][1] - boxside*NUM_TOP_MAG);
  glVertex2f(line[1][0], line[0][1] - boxside*NUM_TOP_MAG);
  glEnd();
  glBegin(GL_LINES);
  glVertex2f(line[0][0], line[0][1] - boxside*NUM_TOP_MAG);
  glVertex2f(line[0][0], line[0][1] - boxside*NUM_TOP_MAG + boxside*2.);
  glEnd();
//=============================================================


}

void svWidget::SetValues()
{
  values[0] = myData->sliceOrigin[layerindex[0]];
  values[1] = myData->sliceOrigin[layerindex[1]];
  values[2] = myData->silceOrigin[layerindex[2]];
}

void svWidget::SetIndex( int zmin, int zmax, int notshowz)
{
     if(notshowz >= 0) showbox =true;
     else showbox= false;
//cerr<<zmin<<" "<<zmax<<endl;
   if(showbox)
   {
   box[0][0]= line[0][0] + 0.5 * boxside+zmin*boxside; box[0][1] = 0;
   box[1][0]= line[0][0] + 0.5 * boxside+(zmax+1)*boxside; box[1][1] = 0;
   svScalar b2 = line[0][0] + 0.5 * boxside+(notshowz+1)*boxside;
   if(b2<line[1][0])
        box[2][0]= b2;
   else
        box[2][0] = line[1][0]-0.5*boxside;
   box[2][1] = 0;
   }
   else
   {
   box[0][0]= line[0][0] + 0.5 * boxside+zmin*boxside; box[0][1] = 0;
   box[1][0]= line[0][0] + 0.5 * boxside+(zmax+1)*boxside; box[1][1] = 0;
   box[2][0]= box[1][0]; box[2][1] = 0;
   }

   svScalar value1=sqrt(3);

   svVector3 pos;
   pos[0] = box[0][0];
   pos[1] = box[0][1] - triangleside *  sqrt(3)/3;
   svVector3 t[3];
   t[0][0] = -triangleside/2;
   t[0][1] = -triangleside * value1/6.;
   t[1][0] = triangleside/2;
   t[1][1] = -triangleside * value1/6.;
   t[2][0] = 0;
   t[2][1] = triangleside * value1/3.;

   triangle[0][0][0] = t[0][0] + pos[0];
   triangle[0][0][1] = t[0][1] + pos[1] ;
   triangle[0][1][0] = t[1][0] + pos[0];
   triangle[0][1][1] = t[1][1] + pos[1];
   triangle[0][2][0] = t[2][0] + pos[0];
   triangle[0][2][1] = t[2][1] + pos[1];

   pos[0] = box[1][0];
   pos[1] = box[1][1] - triangleside*sqrt(3)/3;
   triangle[1][0][0] = t[0][0] + pos[0];
   triangle[1][0][1] = t[0][1] + pos[1] ;
   triangle[1][1][0] = t[1][0] + pos[0];
   triangle[1][1][1] = t[1][1] + pos[1];
   triangle[1][2][0] = t[2][0] + pos[0];
   triangle[1][2][1] = t[2][1] + pos[1];

   pos[0] = box[2][0];
   pos[1] = box[2][1] - triangleside*sqrt(3)/3;
   triangle[2][0][0] = t[0][0] + pos[0];
   triangle[2][0][1] = t[0][1] + pos[1] ;
   triangle[2][1][0] = t[1][0] + pos[0];
   triangle[2][1][1] = t[1][1] + pos[1];
   triangle[2][2][0] = t[2][0] + pos[0];
   triangle[2][2][1] = t[2][1] + pos[1];

   UpdateState();
   //SetVisible();
}
void svWidget::Repeat(bool showbox)
{
   this->showbox = showbox;

   if(showbox)
   {
   //box[0][0]= line[0][0] + 0.5 * boxside; box[0][1] = 0;
   //box[1][0]= line[0][0] + 1.5 * boxside; box[1][1] = 0;
   if((box[1][0]+boxside)<line[1][0])
        box[2][0]= box[1][0] + boxside;
   else
        box[2][0] = line[1][0]-0.5*boxside;
   box[2][1] = 0;
   }
   else
   {
   //box[0][0]= line[0][0] + 0.5 * boxside; box[0][1] = 0;
   //box[1][0]= line[1][0] - 0.5 * boxside; box[1][1] = 0;
   box[2][0]= box[1][0]; box[2][1] = 0;
   }

   svScalar value1=sqrt(3);

   svVector3 pos;
   pos[0] = box[0][0];
   pos[1] = box[0][1] - triangleside *  sqrt(3)/3;
   svVector3 t[3];
   t[0][0] = -triangleside/2;
   t[0][1] = -triangleside * value1/6.;
   t[1][0] = triangleside/2;
   t[1][1] = -triangleside * value1/6.;
   t[2][0] = 0;
   t[2][1] = triangleside * value1/3.;

   triangle[0][0][0] = t[0][0] + pos[0];
   triangle[0][0][1] = t[0][1] + pos[1] ;
   triangle[0][1][0] = t[1][0] + pos[0];
   triangle[0][1][1] = t[1][1] + pos[1];
   triangle[0][2][0] = t[2][0] + pos[0];
   triangle[0][2][1] = t[2][1] + pos[1];

   pos[0] = box[1][0];
   pos[1] = box[1][1] - triangleside*sqrt(3)/3;
   triangle[1][0][0] = t[0][0] + pos[0];
   triangle[1][0][1] = t[0][1] + pos[1] ;
   triangle[1][1][0] = t[1][0] + pos[0];
   triangle[1][1][1] = t[1][1] + pos[1];
   triangle[1][2][0] = t[2][0] + pos[0];
   triangle[1][2][1] = t[2][1] + pos[1];

   pos[0] = box[2][0];
   pos[1] = box[2][1] - triangleside*sqrt(3)/3;
   triangle[2][0][0] = t[0][0] + pos[0];
   triangle[2][0][1] = t[0][1] + pos[1] ;
   triangle[2][1][0] = t[1][0] + pos[0];
   triangle[2][1][1] = t[1][1] + pos[1];
   triangle[2][2][0] = t[2][0] + pos[0];
   triangle[2][2][1] = t[2][1] + pos[1];

   UpdateState();
}

void svWidget::UpdateState()
{
    for(int i=0;i<level;i++) layer[i]=false;

    int start = (box[0][0] - line[0][0] - boxside)/boxside;
    int end = (box[1][0] - line[0][0]-boxside)/boxside;

//    cerr<<start<<" "<<end<<" "<<(box[0][0] - line[0][0] - boxside)/boxside<<" "<<(box[1][0] - line[0][0]-boxside)/boxside<<endl;

    if(start <0) start = 0;
   // else  start = start + 1;

    else if(start < (box[0][0] - line[0][0] - boxside)/boxside) start = start + 1;

  //  cerr<<start<<" "<<end<<" "<<level<<endl;

    if(!showbox)
    {
//cerr<<"showbox no"<<endl;
          for(int i=start;i<=end;i++)layer[i] = true;
          layerindex[0] = start;
          layerindex[1] = end;
          layerindex[2] = end;
    }
    else
    {
        int end2 = (box[2][0] - line[0][0]-boxside)/boxside;

        layerindex[0]= start;
        layerindex[1] = end;
        layerindex[2] = end2;

        svIntArray l;
        for(int i=start;i<=end;i++)l.add(1);
        for(int i=end+1;i<=end2;i++)l.add(0);

        int size = end2 - start+1;
        int count = 0;
        for(int i=start-1;i>=0;i--)
        {
           int ii = size-1-count;
           layer[i] = l[ii];

           count++;
           if(count%size ==0 )count =0;
        }
        count = 0;
        for(int i=end2+1;i<level;i++)
        {
           int ii = count;
           layer[i] = l[ii];

           count++;
           if(count%size ==0 )count =0;
        }

        for(int i=start;i<=end;i++) layer[i] = 1;
        for(int i=end+1;i<=end2;i++)layer[i]=0;

        l.free();
    }

    state->UpdateVisible(layer);
}

void svWidget::cleanup()
{
  layer.clear();
  for(int i=0;i<NUM_TOP_MAG;i++)
    histovalues[i].free();
  delete [] histovalues;
}
/*
bool svWidget::isSelect()
{
    if(bselect) return true;

    bool flag = false;
    for(int i=0;i<3;i++)
      if(tselect[i]) {flag= true;break;}

   if(flag) return true;
   else return false;
}
*/
/*
void svWidget::Reset()
{
     for(int i=0;i<3;i++)tselect[i]=false;
     bselect = false;
}
*/

}
