#include "splitToolkit.h"

using namespace __svl_lib;

void reshape(int w, int h);
void display(void);
void key(unsigned char key, int x, int y);

int window;
int mouse_button;
int mouse_state;
int movement_x;
int movement_y;
int image_lbx;
int image_lby;
int image_rbx;
int image_rby;
bool image_inside = false;
int movement_stop_x;
int movement_stop_y;
int movement_times=-1;

view3d view_info;

//svPeeling *peeling;
svQDOT *flow_field;
//svDirectArrow *directglyph;
//svDirectArrow *rawdata;
//svDirectCylinder *directcylinder;
svLTCylinder *ltcylinder;
svSplitArrow *lccylinder;
//svSummaryGlyph *summaryglyph;
svOutline *outline;
//svMesh *mesh;
//svMesh *symmetrymesh;
svWidget *widget;
//svContourImageDrawing *contour;
//svImage *image;

//svDirectArrow *directraw;
//svDirectArrow *directcontour;
//svSplitArrow *splitraw;
//svSplitArrow *splitcontour;
//svSummaryGlyph *summaryraw;
//svSummaryGlyph *summarglyph;
//svQDOTSlider *slider;
//svConnect *ccl;

char *configFile;

vector<int> symmetrytype;
svScalarArray *symmetrycount;
int symmetrytopvalue;
int symmetry_enable;

int layerVisible = 0;
int layer_repeat = 0;
//===============
float summary_arrow_scale;
float summary_tube_scale;

int data_contour;
int data_raw;

int show_data;
int enable_gr_contour = true;
float global_ratio;

svInt contour_layer;
svInt contour_index;
svScalar contour_mag;
int enable_contour_tree=0;
int contour_back_color = 0;
float imageCutoff = 10;

int enable_magwidget = 0;
int enable_velwidget = 0;
int select_map =  1;

float length_scale;
float arrow_scale;
float tube_scale;
float glui_length_scale;
float glui_arrow_scale;
float glui_tube_scale;
bool length_scale_up=false;
bool arrow_scale_up=false;
bool tube_scale_up=false;
bool updatedata = false;
bool updatesymmetry = false;
bool updatecluster = false;
bool updatevisible = false;
bool updaterender = false;
bool updatecontour = false;
int length_vis = 0;

string picture_name;
string config_name;

int symmetrymesh_enable = 0;
int mesh_enable = 0;
int mesh_type = 0;
vector<int> unique_region;
int mesh_vis = 1;

int symmetrymesh_solid_list;
int  mesh_solid_list;
int  outline_display_list;
int  summary_list;
int  render_list;

float alpha = 0.5;

svScalar mag_show;
svScalar mag_max=1e-4;
svScalar mag_min=1e-16;
float glui_mag_show=0.5;

svScalar widget_tranx;
svScalar widget_trany;
svScalar widget_scalex;
svScalar widget_scaley;
int znum;
int layer_min=0;
int layer_max=108;
int layer_show=50;
float glui_layer_show=0.5;
int samplesize = 20;

int clusterLayer = -1;
int clusterData = -1;

int encode_visible = 0;
int encode_type = LINEAR;// LLO;//LINEAR;
int colorby = ENCODE;
int color_cluster = 1;
int color_symmetry = 0;
int color_magnitude = 0;
int encode_linear = 1;
int encode_linear_tube = 0;
int encode_split = 0;
///////////////////

int frequency =1;
int summaryVisible;
svScalar scale;
svScalar directradius;
svScalar summaryradius;
int regioncount = 0;

ViewProperty viewproperty;

Paser *paser;

void InitField();
int contourindex = 0;
void ReadConfig(char *configfname);//, ConfigProperty &property);
void Update();
void ReleaseMemory();
void WriteContours();
void LocateTexture();
void LoadTexture();

GLboolean bBoundingBox=true;
GLboolean bAxis;

GLuint image_width;
GLuint image_height;

GLint nx, ny, nz;
Trackball trackball;
GLfloat m[16];

//int  zmin;
//int  zmax;
//int  nonz;

svInt volumebound[2]={0,100};
svInt volumestep = 5;
svInt vbound = 0;
svScalar magthreshold = 0;

//=========================Depth Peeling============
int g_numPasses;
int g_imageWidth;
int g_imageHeight;


nv::Model *g_model;
GLuint g_quadDisplayList;
GLuint g_vboId;
GLuint g_eboId;

bool g_useOQ;
GLuint g_queryId;

static nv::SDKPath sdkPath;

GLSLProgramObject g_shaderDualInit;
GLSLProgramObject g_shaderDualPeel;
GLSLProgramObject g_shaderDualBlend;
GLSLProgramObject g_shaderDualFinal;

vector<float> g_opacity;
bool g_showOsd;
bool g_bShowUI;
unsigned g_numGeoPasses;

int g_oldX, g_oldY;
int g_newX, g_newY;

float g_white[3];
float g_black[3];
float *g_backgroundColor;

GLuint g_dualBackBlenderFboId;
GLuint g_dualPeelingSingleFboId;
GLuint g_dualDepthTexId[2];
GLuint g_dualFrontBlenderTexId[2];
GLuint g_dualBackTempTexId[2];
GLuint g_dualBackBlenderTexId;

GLenum g_drawBuffers[7];


void MakeFullScreenQuad();
void InitDualPeelingRenderTargets();
void DeleteDualPeelingRenderTargets();
void BuildShaders(string SHADER_PATH);
void InitGL(string shader);
void RenderDualPeeling();

//===================================================

void glui_display();
void VisualMapping();


void InitLight()
{
  //setting of lighting
  GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8,1};
  GLfloat mat_specular[] = { 0.2,0.2,0.2,0.2 };
  GLfloat light_position[] = { 100,100,200, 0 };
  GLfloat white_light[] = { 0.8, 0.8, 0.8, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);

  GLfloat lmodel_ambient[] = {0.2,0.2,0.2,1.0};
  GLfloat local_view[] = {0.0};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient);
  glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

  GLfloat light1_position[] = {0,0,100,0};
  GLfloat light1_ambient[] = {0.2,0.2,0.2,1};
  GLfloat light1_diffuse[] = {0.5,0.5,0.5,1.0};
  GLfloat light1_specular[] = {0.5,0.5,0.5,1.0};
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR,light1_specular);

  ArrowLight();
}

void ArrowLight()
{
  GLfloat mat_ambient[] = {1,1,1,1};
  GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, .8};
  GLfloat mat_specular[] = { 0.2,0.2,0.2,0.2 };
  GLfloat mat_shininess[] = { 50 };
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}


/* accFrustum()
 * The first 6 arguments are identical to the glFrustum() call.
 *  
 * pixdx and pixdy are anti-alias jitter in pixels. 
 * Set both equal to 0.0 for no anti-alias jitter.
 * eyedx and eyedy are depth-of field jitter in pixels. 
 * Set both equal to 0.0 for no depth of field effects.
 *
 * focus is distance from eye to plane in focus. 
 * focus must be greater than, but not equal to 0.0.
 *
 * Note that accFrustum() calls glTranslatef().  You will 
 * probably want to insure that your ModelView matrix has been 
 * initialized to identity before calling accFrustum().
 */
void accFrustum(GLdouble left, GLdouble right, GLdouble bottom, 
   GLdouble top, GLdouble near, GLdouble far, GLdouble pixdx, 
   GLdouble pixdy, GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
   GLdouble xwsize, ywsize; 
   GLdouble dx, dy;
   GLint viewport[4];

   glGetIntegerv (GL_VIEWPORT, viewport);
	
   xwsize = right - left;
   ywsize = top - bottom;
	
   dx = -(pixdx*xwsize/(GLdouble) viewport[2] + eyedx*near/focus);
   dy = -(pixdy*ywsize/(GLdouble) viewport[3] + eyedy*near/focus);
	
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum (left + dx, right + dx, bottom + dy, top + dy, near, far);
   gluLookAt(view_info.eye[0],view_info.eye[1],view_info.eye[2],
                                view_info.coi[0],view_info.coi[1],view_info.coi[2],
                                0, 1, 0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

/* accPerspective()
 * 
 * The first 4 arguments are identical to the gluPerspective() call.
 * pixdx and pixdy are anti-alias jitter in pixels. 
 * Set both equal to 0.0 for no anti-alias jitter.
 * eyedx and eyedy are depth-of field jitter in pixels. 
 * Set both equal to 0.0 for no depth of field effects.
 *
 * focus is distance from eye to plane in focus. 
 * focus must be greater than, but not equal to 0.0.
 *
 * Note that accPerspective() calls accFrustum().
 */
void accPerspective(GLdouble fovy, GLdouble aspect, 
   GLdouble near, GLdouble far, GLdouble pixdx, GLdouble pixdy, 
   GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
   GLdouble fov2,left,right,bottom,top;

   fov2 = ((fovy*M_PI) / 180.0) / 2.0;

   top = near / (cos(fov2) / sin(fov2));
   bottom = -top;

   right = top * aspect;
   left = -right;

   accFrustum (left, right, bottom, top, near, far,
               pixdx, pixdy, eyedx, eyedy, focus);
}

/*  Initialize lighting and other values.
 */
void Init(void)
{
   InitLight();

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);

   //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

   glEnable(GL_DEPTH_TEST);
  // glShadeModel (GL_FLAT);

   glEnable(GL_NORMALIZE);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);  
   glEnable(GL_COLOR_MATERIAL);
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); 

}

void accDisplay(void)
{
   //InitLight();
//   glEnable(GL_LIGHTING);
//   glEnable(GL_LIGHT0);
//   glEnable(GL_LIGHT1);

   //glEnable(GL_MULTISAMPLE);

   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   float aspect = view_info.aspect_ratio * float(image_width)/float(image_height);
   float GLfloat_fov;
   if ( aspect < 1 ){
                  // fovy is a misnomer.. we GLfloatly mean the fov applies to the
                  // smaller dimension
                  float fovx, fovy;
                  fovx = fovy = view_info.view_angle;
                  GLfloat_fov = svToDegree(2*atan(tan(svToRadian(fovx/2))/aspect));
   }
   else{
                GLfloat_fov = view_info.view_angle;
    }

     //  GLfloat m[16];

      if(mouse_state == GLUT_DOWN)
      {
          glEnable(GL_CULL_FACE);
      }

       trackball.getMatrix().getValue(m);

       int jitter;
       glClear(GL_ACCUM_BUFFER_BIT);
      // glClearAccum(0.5, 0.5, 0.5, 1.0);
       for (jitter = 0; jitter < ACSIZE; jitter++) {
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // glClearColor(0.5, 0.5, 0.5, 1.0);
       accPerspective (GLfloat_fov,
                       aspect,
                      view_info.hither,
                      view_info.yon, 
                     j8[jitter].x, j8[jitter].y, view_info.eye[0],view_info.eye[1],view_info.eye[2]);

        glPushMatrix();
      //  glMultMatrixf(m);
        RenderDualPeeling();
       // splitglyph->RenderVBO();
        //summaryglyph->Render();
        //directglyph->Render();
        //peeling->RenderDualPeeling();
        //glEnable(GL_DEPTH_TEST);
        //summaryglyph->RenderFont();
        glPopMatrix();
 
        glAccum(GL_ACCUM, 1.0/ACSIZE);
      }
      glAccum (GL_RETURN, 1.0);


//================2D Visualization======================================================
        glDisable(GL_CULL_FACE);

        //viewproperty.tm = m;

        for(int i=0;i<16;i++)viewproperty.tm[i]=m[i];

        glGetDoublev (GL_MODELVIEW_MATRIX, viewproperty.mvmatrix);
        glGetDoublev (GL_PROJECTION_MATRIX, viewproperty.projmatrix);
        glGetIntegerv( GL_VIEWPORT, viewproperty.viewport );

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0,(GLfloat) viewproperty.viewport[2], 0.0, (GLfloat) viewproperty.viewport[3]);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_LIGHTING);

        glColor3f(1,1,1);

       if(layerVisible)
       {
        glPushMatrix();
        glTranslatef(widget_tranx, widget_trany,0);
        glScalef(widget_scalex,widget_scaley,1);
        widget->Render(topmaglevel);
        glPopMatrix();
       }
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(viewproperty.projmatrix);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(viewproperty.mvmatrix);

      glFlush();
      glutSwapBuffers();
}

void Reshape(int w, int h)
{
   g_imageWidth=w;g_imageHeight=h;

   glClearColor(0., 0., 0., 1);
   glClearAccum(0, 0., 0., 1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glClear(GL_ACCUM_BUFFER_BIT);

   glViewport(0, 0, (GLsizei) w, (GLsizei) h);

   image_width = w;
   image_height = h;

  widget_tranx=325;
  widget_trany=image_height-75;
  widget_scalex=4;
  widget_scaley=10.;

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   float aspect = view_info.aspect_ratio * float(w)/float(h);
   float GLfloat_fov;
   if ( aspect < 1 ){
                  // fovy is a misnomer.. we GLfloatly mean the fov applies to the
                  // smaller dimension
                  float fovx, fovy;
                  fovx = fovy = view_info.view_angle;
                  GLfloat_fov = svToDegree(2*atan(tan(svToRadian(fovx/2))/aspect));
   }
   else{
                GLfloat_fov = view_info.view_angle;
    }
    gluPerspective(GLfloat_fov,
                                           aspect,
                                           view_info.hither,
                                           view_info.yon);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(view_info.eye[0],view_info.eye[1],view_info.eye[2],
                                view_info.coi[0],view_info.coi[1],view_info.coi[2],
                                0, 1, 0);

   DeleteDualPeelingRenderTargets();
   InitDualPeelingRenderTargets();
}

void UpdateColor()
{
  ltcylinder->SetSolidColor();
  lccylinder->SetColorByPower();
}

void UpdateCluster()
{
 if(1)//!configproperty.isContour)
 {
   rawdata->ResetCluster();
   rawdata->SetROI(paser->GetMagRange00(),paser->GetMagRange01());
   rawdata->GenerateClustersBySymmetry(paser->GetStep1Property(),
                      paser->GetSymmetryProperty(),
                      symmetrytype);
   rawdata->SetROI(paser->GetMagRange10(),paser->GetMagRange11());
   rawdata->GenerateClustersBySymmetry(paser->GetStep2Property(),
                      paser->GetSymmetryProperty(),
                      symmetrytype);


   directglyph->ResetCluster();//cerr<<"done1"<<endl;
   directglyph->SetROI(paser->GetMagRange00(),paser->GetMagRange01());
//cerr<<paser->GetMagRange00()<<" "<<paser->GetMagRange01()<<endl;
//configproperty.magrange[0][0], configproperty.magrange[0][1]);
   directglyph->GenerateClustersBySymmetry(paser->GetStep1Property(),
                      paser->GetSymmetryProperty(),
//configproperty.step1_kmeansproperty,
//                      configproperty.symmetryproperty,
                      symmetrytype);
   directglyph->SetROI(paser->GetMagRange10(),paser->GetMagRange11());
//configproperty.magrange[1][0], configproperty.magrange[1][1]);
   directglyph->GenerateClustersBySymmetry(paser->GetStep2Property(),
                      paser->GetSymmetryProperty(),
                      symmetrytype);

   splitglyph->ResetCluster();
   splitglyph->GenerateClusters(directglyph->GetClusterLabels());

   summaryglyph->ResetCluster();
   summaryglyph->GenerateClusters(directglyph->GetClusterLabels());
 }
// else
// {
//   directglyph->ResetCluster();//cerr<<"done1"<<endl;
//   directglyph->SetROI(configproperty.magrange[0][0], configproperty.magrange[0][1]);
//   directglyph->GenerateClusters(configproperty.step1_kmeansproperty);
//   directglyph->SetROI(configproperty.magrange[1][0], configproperty.magrange[1][1]);
//   directglyph->GenerateClusters(configproperty.step2_kmeansproperty);

//   splitglyph->ResetCluster();
//   splitglyph->GenerateClusters(directglyph->GetClusterLabels());
//   summaryglyph->ResetCluster();
//   summaryglyph->GenerateClusters(directglyph->GetClusterLabels());
// }
}
void UpdateRender()
{
cerr<<"test"<<endl;
    length_scale = (glui_length_scale * LINEAR_SCALE)*5;
    directglyph->SetScale(length_scale);
    length_scale = (glui_length_scale * SPLITVECTORS_SCALE)*5.;
    splitglyph->SetScale(length_scale);
    length_scale = (glui_length_scale * LINEAR_SCALE)*2;

    arrow_scale = (glui_arrow_scale * ARROW_SCALE)*2.;
    directglyph->SetRadius(arrow_scale);
    splitglyph->SetRadius(arrow_scale);

    tube_scale = (glui_tube_scale * TUBE_SCALE)*2.;
    directglyph->SetTubeRadius(tube_scale);
    splitglyph->SetTubeRadius(tube_scale);

    directglyph->GenerateArrows();
    splitglyph->GenerateArrows();

    directglyph->GenerateTubes();
    splitglyph->GenerateTubes();

    if(encode_type == LINEAR)
    {
         length_scale = (glui_length_scale * LINEAR_SCALE)*5;
         //cerr<<length_scale<<endl;
         directglyph->UpdateArrowVBO();
         directglyph->UpdateTubeVBO();
     }
/*     else if(encode_type == LLO)
     {
        length_scale = (glui_length_scale * SPLITVECTORS_SCALE)*5.;
     }
     else if(encode_type == LT)
     {
        length_scale = (glui_length_scale * SPLITVECTORS_SCALE)*5.;
     }
     else if(encode_type == LINEAR_TUBE)
    {
         length_scale = (glui_length_scale * LINEAR_SCALE)*5;
     }
*/     else
     {
        length_scale = (glui_length_scale * SPLITVECTORS_SCALE)*5.;
        splitglyph->UpdateArrowVBO();
        splitglyph->UpdateTubeVBO();
        splitglyph->GenerateLegend();
     }
cerr<<"done"<<endl;
    summaryglyph->SetScale(length_scale);
    summaryglyph->SetRadius(summary_arrow_scale);
    summaryglyph->SetTubeRadius(summary_tube_scale);
    summaryglyph->Generate(alpha);
//cerr<<"done"<<endl;
}
void UpdateContour()
{
   contour->SetSideX(50.);
cerr<<"start"<<endl;
   contour->ResetContourTree(flow_field->GetPlaneNum());
   char *contour_fname = new char[400];
   contour->New(flow_field->GetPlaneNum());
   for(int i=0;i<flow_field->GetPlaneNum();i++)
   {
     //  cerr<<contour_fname<<endl;
          sprintf(contour_fname,"%s/%s/layer%s%d.txt",
                  paser->GetStoreDir(),
                  paser->GetRawFile(),
                  paser->GetPlaneName(),i);
//          cerr<<contour_fname<<endl;
//configproperty.storeDir, configproperty.rawFile, configproperty.planename, i);
          contour->SetData(contour_fname, i);
   }
   delete [] contour_fname;cerr<<"init contour"<<endl;
   contour->GeneratePixels();cerr<<"pixel"<<endl;
cerr<<"Reset"<<endl; 
   contour->SetBound(volumebound[0], volumebound[1], volumestep);
cerr<<"bound"<<endl;
   contour->GenerateContourTree(paser->GetContourTreeProperty());
//configproperty.contourtreeproperty);
cerr<<"contourtree"<<endl;
   contour->GenerateContours(paser->GetContourTreeDir(),
                             paser->GetPlaneName());
//configproperty.contourtreeproperty.dir,
//                             configproperty.planename);
cerr<<"contours"<<endl;
   contour->GenerateSelectedContours(paser->GetContourProperty());
//configproperty.contourproperty);
cerr<<"select contours"<<endl;
   contour->GenerateVisibility(vbound, magthreshold);
cerr<<"contour visibility"<<endl;
   //contour->SetPosition();
   Reshape2DContour();
   contour->SetContourPosition();

  char *str = new char[400];
  sprintf(str, "%s/%s/2dcontour.txt", paser->GetStoreDir(),paser->GetRawFile());
  contour->SaveContourtoFile(str);
  delete [] str;

  Reshape2DContour();
}
void UpdateImageBack()
{
cerr<<"start"<<endl;
   char *contour_fname = new char[400];
   image->New(flow_field->GetPlaneNum());
   for(int i=0;i<flow_field->GetPlaneNum();i++)
   {
          sprintf(contour_fname,"%s/%s/layer%s%d.txt",
                  paser->GetStoreDir(),
                  paser->GetRawFile(),
                  paser->GetPlaneName(),i);
          image->SetData(contour_fname, i);
   }
   delete [] contour_fname;cerr<<"init contour"<<endl;
   image->GeneratePixels();cerr<<"pixel"<<endl;
   image->SetPosition();
}
void UpdateImage()
{
  char *str = new char[400];
  sprintf(str, "%s/%s/2dimage.txt", paser->GetStoreDir(),paser->GetRawFile());

  char *str1 = new char[400];
  sprintf(str1, "%s/%s/2dcolor.txt", paser->GetStoreDir(),paser->GetRawFile());
  char *str2 = new char[400];
  sprintf(str2, "%s/%s/2dcontour.txt", paser->GetStoreDir(),paser->GetRawFile());

  image->GenerateColors(str1, str2, imageCutoff, str);
//  image->SetColors(str);
  contour->SetColors(str);

  delete [] str1;
  delete [] str2;
  delete [] str;
}
void UpdateVBOData()
{
   splitglyph->UpdateData();
   directglyph->UpdateData();
}
void UpdateSymmetry()
{
        rawdata->GenerateSymmetry(paser->GetSymmetryProperty());
        directglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
        splitglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
        summaryglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
        symmetrycount = directglyph->GetSymmetryMagProb(symmetrytopvalue);
}
void UpdateSymmetryMesh()
{
   if(1)//!configproperty.isContour)
   {
      vector<int> region;
      for(int i=0;i<symmetrytype.size();i++)
      {
       if(texture_status[symmetrytype[i]] == SYMMETRY_DOWN && symmetrytype[i]>=0)
       {
         region.push_back(symmetrytype[i]);
         region.push_back(symmetrytype[i]);
       }
      }
      symmetrymesh->New(region);
      symmetrymesh->SetDisplayList(symmetrymesh_solid_list);
cerr<<symmetrytype.size()<<endl;
     for(int i=0;i<symmetrytype.size();i++)
     {
       if(symmetrytype[i]<0) continue;
//cerr<<i<<endl;

      svVector3 p = paser->GetSymmetryPos();
      svVector3 ddd = paser->GetSymmetryDir();
      char *symmetrystr = new char[200];
      for(int j=0;j<200;j++) symmetrystr[j] = '\0';
      sprintf(symmetrystr, "%s%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f", symmetrystr,
//                  configproperty.symmetryproperty.pos[0],
//                  configproperty.symmetryproperty.pos[1],
//                  configproperty.symmetryproperty.pos[2],
//                  configproperty.symmetryproperty.dir[0],
//                  configproperty.symmetryproperty.dir[1],
 //                 configproperty.symmetryproperty.dir[2]);
                    p[0],p[1],p[2],ddd[0],ddd[1],ddd[2]);
      char *str1 = new char[400];
      char *str2 = new char[400];
      sprintf(str1, "%s/%s/symmetryregion%s%d1.txt", 
//configproperty.storeDir,  
//                    configproperty.rawFile,
                     paser->GetStoreDir(), paser->GetRawFile(),
                     symmetrystr, symmetrytype[i]);
      sprintf(str2, "%s/%s/symmetryregion%s%d2.txt", 
//configproperty.storeDir,  
//                    configproperty.rawFile,
                     paser->GetStoreDir(), paser->GetRawFile(),
                    symmetrystr, symmetrytype[i]);
      directglyph->SaveSymmetry(symmetrytype[i], str1, str2,
                              paser->GetSymmetryProperty());
                             // configproperty.symmetryproperty);
      splitglyph->SaveSymmetry(symmetrytype[i], str1, str2,
                              paser->GetSymmetryProperty());
                             // configproperty.symmetryproperty);
      char *outstr1 = new char[400];
      char *outstr2 = new char[400];
      sprintf(outstr1, "%s/%s/symmetrymesh%s%d1.txt", 
//configproperty.storeDir,
//                     configproperty.rawFile,
                     paser->GetStoreDir(), paser->GetRawFile(),
                     symmetrystr, symmetrytype[i]);
      sprintf(outstr2, "%s/%s/symmetrymesh%s%d2.txt", 
//configproperty.storeDir,  
//                     configproperty.rawFile,
                     paser->GetStoreDir(), paser->GetRawFile(),
                     symmetrystr, symmetrytype[i]);

      flow_field->NewMesh(str1, outstr1);
      flow_field->NewMesh(str2, outstr2);
      
      symmetrymesh->SetData(outstr1, i*2);
      symmetrymesh->SetData(outstr2, i*2+1);
     // symmetrymesh->GenerateSurfaces(0);//unique_region[1]);

      delete [] outstr1;
      delete [] outstr2;
      delete [] str1;
      delete [] str2;
   }

   region.clear();
   symmetrymesh->SetColors();
   symmetrymesh->GenerateSurfacesByColor();
  }
}
void UpdateVisible()
{
//    for(int i=0;i<VISUAL_TYPE-1;i++)
//             if(texture_visual_status[i] == SYMMETRY_DOWN)
//                                { encode_type = i;}
cerr<<"start"<<endl;
    if(encode_linear) encode_type = LINEAR;
    else encode_type = 1-LINEAR;//SPLITVECTORS;
//if(encode_linear_tube) encode_type = LINEAR_TUBE;
//    else encode_type = LC;
cerr<<encode_type<<endl;
    symmetrytype.clear();
    bool flag = false;
    for(int i=0;i<SYMMETRY_TYPE+1;i++)
    {
         if(i<SYMMETRY_TYPE)//&&texture_status[i] == SYMMETRY_DOWN)
         { 
            if(texture_status[i] == SYMMETRY_DOWN)
            {
              symmetrytype.push_back(i);
              flag = true;
            }
         }
         else if(texture_status[i] == SYMMETRY_DOWN)
         {
              symmetrytype.push_back(-1);
              flag = true;
         }
    } 
//      symmetrytype.clear();

//      bool flag = false;
//      for(int i=0;i<SYMMETRY_TYPE_NUM;i++)
//      {
//              if(texture_status[i] == SYMMETRY_DOWN)
//              {
//                  symmetrytype.push_back(i);
//                  flag = true;
//              }
//      }
   char *str = new char[400];
   sprintf(str, "%s/%s/", paser->GetStoreDir(), paser->GetRawFile());
cerr<<str<<endl;
//todo
   directglyph->ResetVisible();
cerr<<"directglyph->ResetVisible();"<<endl;
//   if(configproperty.isContour)
//   {
//     directglyph->SetSampling(frequency);
//   }
cerr<<"directglyph->SetSampling(frequency)"<<endl;
   directglyph->SetSymmetryVisible(symmetrytype);
cerr<<"SetSymmetryVisible(symmetrytype);"<<endl;
   directglyph->SetVisible(contourindex);
//   directglyph->SetVisible(zmin, zmax);
   directglyph->SetVisible(widget->GetVisible());
   directglyph->SetClusterVisible(clusterLayer, clusterData);
   directglyph->UpdateIndex();
cerr<<"   directglyph->UpdateIndex();"<<endl;
   directglyph->GenerateIndex();
   directglyph->SetSampleData(samplesize);
cerr<<"directglyph->SetSampleData(samplesize);"<<endl;

   splitglyph->ResetVisible();
   if(paser->GetIsContour())//configproperty.isContour)
   {
      splitglyph->SetContourSampling(frequency);
   }
   else
   {
       splitglyph->SetSampling(frequency);
   }
   delete [] str;
   splitglyph->SetSymmetryVisible(symmetrytype);
   splitglyph->SetVisible(contourindex);
   splitglyph->SetVisible(widget->GetVisible());
   splitglyph->SetVisible(clusterLayer, clusterData);
   splitglyph->UpdateIndex();
   splitglyph->GenerateIndex();
   splitglyph->SetSampleData(samplesize);
    if(encode_type == LINEAR)
    {
         directglyph->UpdateIndexVBO();
     }
     else if(encode_type == LLO)
     {
     }
     else if(encode_type == LT)
     {
     }
     else
     {
        splitglyph->UpdateIndexVBO();
     }


   summaryglyph->ResetVisible();
   if(paser->GetIsContour())//configproperty.isContour)
     summaryglyph->SetContourSampling(frequency);
   else
      summaryglyph->SetSampling(frequency);
cerr<<"summaryglyph"<<endl;
   summaryglyph->SetSymmetryVisible(symmetrytype);
cerr<<"summaryglyph"<<endl;
   summaryglyph->SetVisible(contourindex);
cerr<<"summaryglyph"<<endl;
   summaryglyph->SetVisible(widget->GetVisible());
cerr<<"summaryglyph"<<endl;
   summaryglyph->SetClusterVisible(clusterLayer, clusterData);
   summaryglyph->Generate(alpha);
cerr<<"summaryglyph"<<endl;

   contour->SetROI(directglyph->getVisibleLabel());
}

void UpdateSingleContour()
{
    contour->GenerateContour(paser->GetContourTreeDir(),
                             paser->GetPlaneName());
    
    directglyph->GenerateContour(paser->GetContourTreeDir(),
                             paser->GetPlaneName(),
                             contour_layer,
                             contour_mag);
    splitglyph->GenerateContour(paser->GetContourTreeDir(),
                             paser->GetPlaneName(),
                             contour_layer,
                             contour_mag);
    summaryglyph->GenerateContour(paser->GetContourTreeDir(),
                             paser->GetPlaneName(),
                             contour_layer,
                             contour_mag);
    directglyph->SaveContour(paser->GetContourProperty());

//    directglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
//    splitglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
//    summaryglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
//    symmetrycount = directglyph->GetSymmetryMagProb(symmetrytopvalue);

    contour->SetContourPosition();

    UpdateSymmetry();

    contour->SetContourPosition();

    UpdateVBOData();cerr<<"UpdateVBOData()"<<endl;
    UpdateVisible();cerr<<"UpdateVisible()"<<endl;
    UpdateCluster();cerr<<"UpdateCluster()"<<endl;
    UpdateColor();cerr<<"UpdateColor()"<<endl;
    UpdateRender();cerr<<"UpdateRender()"<<endl;
    UpdateSymmetryMesh();

    paser->SetContours(contour_layer, contour_mag);
    paser->SavetoFile(configFile);
}

void control_cb(int control)
{
      glutSetWindow(window);
     if(control == DATA_ID)
     {
         updatedata = true;
         paser->SetData(show_data);
         paser->SavetoFile(configFile);

         if(!paser->GetIsContour())//!configproperty.isContour)
         {
            contour_box->set_int_val(0);
            contour_box->disable();
            enable_contour_tree=0;
         }
         else
         {
            contour_box->enable();
         }
     }
     if(control == SYMMETRY_ID)
     {
           if(symmetry_enable == true)
           {
                   //encode_visible = 0;
                   layerVisible = 0;
                   box_layer->set_int_val(layerVisible);
                   enable_contour_tree = 0;
                   contour_box->set_int_val(enable_contour_tree);
           }  
     }
     else if(control == OVERVIEW_ID || control == MESH_ID)
     {
           
     }
     else if(control == CONTOUR_TREE_ID)
     {//cerr<<"CONTOUR_TREE_ID "<<enable_contour_tree<<endl;
         if(enable_contour_tree)
         {
               Reshape2DContour();

       //        encode_visible = 0;
               layerVisible = 0;
               box_layer->set_int_val(layerVisible);
               symmetry_enable = 0;
               symmetrybox->set_int_val(symmetry_enable);
         }
     }
     else if(control == ENCODE_LINEAR_ID)
     {
     //   if(encode_visible)
     //   {
     //         layerVisible = 0;
     //         box_layer->set_int_val(layerVisible);
     //        symmetry_enable = 0; symmetrybox->set_int_val(symmetry_enable);
     //    }
         if(encode_linear) 
         {
            if(color_cluster + color_symmetry == 0)
            {
                colorby = ENCODE;
                color_cluster = 1;
                color_box1->set_int_val(1);
            }
            color_magnitude =0;
            color_box1->enable();
            color_box2->enable();
            color_box3->set_int_val(0);
            color_box3->disable();
            encode_split = 0;
            encode_box2->set_int_val(0);
            encode_linear_tube = 0;
            //encode_box3->set_int_val(0);
         }
          updatevisible = true;
          updaterender = true;
     }
     else if(control == ENCODE_LINEAR_TUBE_ID)
     {
        if(encode_linear_tube)
        {
            colorby = SOLID_COLOR;
            color_cluster = 0;
            color_symmetry = 0;
            color_box1->set_int_val(0);
            color_magnitude =0;
            color_box2->set_int_val(0);
            color_box3->set_int_val(0);
            color_box3->disable();
            encode_split = 0;
            encode_box2->set_int_val(0);
            encode_linear = 0;
            encode_box1->set_int_val(0);
        }
          updatevisible = true;
          updaterender = true;
     }
     else if(control == ENCODE_SPLIT_ID)
     {
         if(encode_split)
         {
             color_cluster =0;
             color_symmetry = 0;
             color_magnitude = 1;
             color_box1->set_int_val(0);
             color_box2->set_int_val(0);
             color_box1->disable();//cerr<<"1"<<endl;
             color_box2->disable();//cerr<<"1"<<endl;
             color_box3->disable();//cerr<<"1"<<endl;
             color_box3->set_int_val(1);//cerr<<"1"<<endl;
             encode_linear = 0;//cerr<<"1"<<endl;
             encode_box1->set_int_val(0);//cerr<<"1"<<endl;
             encode_linear_tube = 0;
             //encode_box3->set_int_val(0);
         }
          updatevisible = true;
          updaterender = true;
     }
     else if(control == LAYER_ID)
     {
        if(layerVisible)
        {
                 symmetry_enable = 0; 
                 symmetrybox->set_int_val(symmetry_enable);
                 enable_contour_tree = 0;
                 contour_box->set_int_val(enable_contour_tree);
        }
     }
     else if(control == LAYER_REPEAT_ID)
     {
        widget->Repeat(layer_repeat);
        paser->SetLayer(widget->GetZmin(),
                        widget->GetZmax(),
                        widget->GetNonz(),
                        layer_repeat);
        paser->SavetoFile(configFile);
        if(layerVisible)
        {     svVector3 value[3];
             svInt *index=new svInt[3];  widget->GetIndex(index);
             for(int i=0;i<3;i++)
             {
               value[i]  = flow_field->GetPlanePosition(index[i]);
             }
             widget->SetValues(value);

             UpdateVisible();
        }
     }
     else if(control == LENGTH_VIS_ID)
     {//cerr<<length_vis<<endl;
           widget->SetHistoValues(splitglyph->GetMagProb(topmaglevel));
           widget->SetValues(splitglyph->GetEntropyValues(), true);
     }
     else if(control == LENGTH_ID || control == ARROW_ID || control == TUBE_ID)
     {
          if(encode_type == LINEAR)
          {
               length_scale = (glui_length_scale * LINEAR_SCALE)*5;   
               directglyph->SetScale(length_scale);
               arrow_scale = (glui_arrow_scale * ARROW_SCALE)*2.;
               directglyph->SetRadius(arrow_scale);
               tube_scale = (glui_tube_scale * TUBE_SCALE)*2.;
               directglyph->SetTubeRadius(tube_scale);
          }
          else if(encode_type == LLO)
          {
               length_scale = (glui_length_scale * SPLITVECTORS_SCALE)*5.;
          } 
          else if(encode_type == LT)
          {
               length_scale = (glui_length_scale * SPLITVECTORS_SCALE)*5.;
          }
          else if(encode_type == LINEAR_TUBE)
          {
               tube_scale = (glui_tube_scale * TUBE_SCALE)*2.;
               length_scale = (glui_length_scale * LINEAR_SCALE)*2;
          }
          else
          {
               length_scale = (glui_length_scale * SPLITVECTORS_SCALE)*5.;
               splitglyph->SetScale(length_scale);  
               arrow_scale = (glui_arrow_scale * ARROW_SCALE)*2.;
               splitglyph->SetRadius(arrow_scale);
               tube_scale = (glui_tube_scale * TUBE_SCALE)*2.;
               splitglyph->SetTubeRadius(tube_scale);
          }
          summaryglyph->SetScale(length_scale);
        
          updaterender = true; 
          arrow_scale_up = true;
     } 
     else if(control == FREQ_ID)
     {
          updatevisible = true;
     }
     else if(control == COLORBY_CLUSTER_ID)
     {
          if(color_cluster)
          {
              colorby = ENCODE;
              color_symmetry = 0;
              color_box2->set_int_val(color_symmetry);
          }
          else
          {
              if(color_symmetry == 0)
                     colorby = SOLID_COLOR;
          }
          updatevisible = true;
          updaterender = true;
     }
     else if(control == COLORBY_SYMMETRY_ID)
     {
          if(color_symmetry)
          {
             colorby = SYMMETRY_COLOR;
             color_cluster = 0;
             color_box1->set_int_val(color_cluster);
          }
          else
          {
             if(color_cluster == 0)
                    colorby = SOLID_COLOR;
          }
          updatevisible = true;
          updaterender = true;
     }
/*     else if(control == CONFIG_ID)
     {
        ReleaseMemory();
        ReadConfig(configFile,configproperty);
        Update();
     }
*/
     else if(control == UPDATE_ID)
     {
          cout<<"Please wait ...."<<endl;

          paser->ResetBool();

          paser->ReadFiles(configFile);
          if(paser->GetUpdateData())
          {
             updatedata = true;
          }
          if(paser->GetUpdateCluster())
          {
             updatecluster = true;
             updaterender = true;
          }
          if(paser->GetUpdateContour())
          {
             updatecontour = true;
             updatecluster = true;
             updaterender = true;
             updatesymmetry = true;
             updatevisible = true;
          }
          if(paser->GetUpdateRender())
          {
             updatevisible = true;
          }
          if(paser->GetUpdateSymmetry())
          {
             updatesymmetry = true;
             updatecluster = true;
             updaterender = true;
             updatevisible = true;
          }


          cerr<<"update "<<endl;

          if(updatedata)
          {
             cerr<<"update data"<<endl;

             InitField();

             paser->SetSeedNum(flow_field->GetPlaneNum());
             paser->SetPlane(flow_field->GetPlaneDistance(),flow_field->GetMinPlane());
             paser->ProcessContour();
             char *str = new char[400];
             sprintf(str, "%s/%s/density.txt", paser->GetStoreDir(), paser->GetRawFile());
             paser->SetContourValues(flow_field->GetMaxDensity(str));
             paser->ProcessClusterLayers();
             paser->ResetBool();
             delete [] str;

             Update();
          }
          else
          { 

             paser->ProcessContour();
             char *str = new char[400];
             sprintf(str, "%s/%s/density.txt", paser->GetStoreDir(), paser->GetRawFile());
             paser->SetContourValues(flow_field->GetMaxDensity(str));
             paser->ProcessClusterLayers();
             paser->ResetBool();
             delete [] str;

          if(updatecontour)
          {
             cerr<<"update contour"<<endl;
             if(paser->GetIsContour())
             {
               UpdateContour();
             }
/*
              if(configproperty.contourproperty.type[0] == 1)
              {
                   char *str = new char[400];
                   sprintf(str, "%s/%s/density.txt", configproperty.storeDir, 
                                                     configproperty.rawFile);
                   svScalar vv = flow_field->GetMaxDensitybyLayer(str, contour_layer);
                   contour_mag = contour_mag/vv;
                   delete [] str;
              }
             WriteContours();
             ReleaseMemory();
             ReadConfig(configFile,configproperty);
             Update();
*/
          }
          if(updatevisible)
          {
               cerr<<"update visible"<<endl;
               UpdateVisible();
          }
          if(updatesymmetry)
          {
              cerr<<"update symmetry"<<endl;
              UpdateSymmetry();
          }
          if(updatecluster)
          {
               cerr<<"update cluster"<<endl;
               UpdateCluster();
          }
          if(updatevisible)
          {
               cerr<<"update visible"<<endl;
               UpdateVisible();
               UpdateSymmetryMesh();
          }
          if(updaterender)
          {
               cerr<<"update render"<<endl;
               UpdateColor(); 
               UpdateRender(); 
          }
          }

          updatesymmetry = false;
          updatedata = false;
          updatecontour =false;
          updatecluster = false;
          updatevisible = false;
          updaterender = false;

          arrow_scale_up = false;
          cout<<"Update done ..."<<endl;
     }
     else if(control == MESH_TYPE_ID || control == MESH_VIS_ID)
     {
         // mesh_type=list_mesh_type->get_int_val();
          mesh_vis=list_mesh_vis->get_int_val();
          if(mesh_vis == 0)
          {
                mesh->GenerateWireframe();//unique_region[1]);
          }
          else
          {
                mesh->GenerateSurfaces(1);//unique_region[1]);
          }
     }
     else if(control == ALPHA_ID)
     {
     }
     else if(control == PICTURE_ID)
     {
          int x, y, w, h;
          char *str = new char[200];
          const char *picture = text_picture->get_text(); 
          if(strlen(picture)>0)
          {
            sprintf(str,"%s/screenshots/%s.ppm", SRC_DIR, picture);
          }
          else
          {
             char buffer[80];
             getTime(buffer);
             sprintf(str,"%s/screenshots/%s.ppm", SRC_DIR,buffer);   
          }//cerr<<str<<endl;
          GLUI_Master.get_viewport_area(&x, &y, &w, &h);
          CaptureScreen(str, x, y, w,h);

          delete [] str;
     }
     else if(control == CONFIG_ID)
     {
          char *str = new char[200];
          const char *con = text_config->get_text();
          if(strlen(con)>0)
          {
            sprintf(str,"%s/%s", SRC_DIR, con);
          }
          else
          {
             char buffer[80];
             getTime(buffer);
             sprintf(str,"%s/config%s", SRC_DIR,buffer);
          }
          paser->SavetoFile(str);
          delete [] str;
     }
     else if(control == CONFIG_TEXT_ID)
     {
     }
     else if(control == PICTURE_TEXT_ID)
     {
     }

  //   cerr<<sb_arrow->ifmouseup()<<endl;
  //   glutSetWindow(window);
/*
         if(sb_arrow->ifmouseup())
         {
             cerr<<"scrolldown"<<endl;
             scrolldown = false;
         }
 */
     glutPostRedisplay();
}

void key(unsigned char key, int x, int y)
{
        switch (key) {
        case '\033':
        case 'q':
        case 'Q':
                exit(0);
                break;
        case 'h':
        case 'H':
               clusterLayer=-1;
               clusterData=-1;
               UpdateVisible();
               trackball.reset();
               break;
      /*  case  'l':
               {cout<<"Please input the index of the min layer"<<endl;
               cin>>zmin;
               cout<<"Please input the index of the  max layer"<<endl;
               cin>>zmax;
               UpdateVisible();
               svVector3 p = flow_field->GetPlanePosition(zmin);
               cout<<"min plane: "<<p[0]<<" "<<p[1]<<" "<<p[2]<<endl;
               p = flow_field->GetPlanePosition(zmax);
               cout<<"max plane: "<<p[0]<<" "<<p[1]<<" "<<p[2]<<endl;
                }break;
*/  
        case 't':
                {
                   svVector3 value[3];
                   widget->MoveRight();
                   svInt *index=new svInt[3];  widget->GetIndex(index);
                   for(int i=0;i<3;i++)
                   {
                     value[i]  = flow_field->GetPlanePosition(index[i]);
                   }
                   widget->SetValues(value);                   
                   UpdateVisible();
                } break;
         case 'T':
              {
                  svVector3 value[3];
                  widget->MoveLeft();
                  svInt *index=new svInt[3];  widget->GetIndex(index);
                  for(int i=0;i<3;i++)
                  {
                      value[i]  = flow_field->GetPlanePosition(index[i]);
                  }
                  widget->SetValues(value);
                  UpdateVisible();
               } break;
        }
 
        glutPostRedisplay();
}
void mouse(int button, int state, int x, int y)
{
    mouse_button = button;
    mouse_state = state;
  
//cerr<<mouse_button<<" "<<state<<endl;
        long s=0x00000000;
    s |= (button == GLUT_LEFT_BUTTON)   ? ((state == GLUT_DOWN) ? Trackball::LBUTTON_DOWN : Trackball::LBUTTON_UP) : 0;
    s |= (button == GLUT_MIDDLE_BUTTON) ? ((state == GLUT_DOWN) ? Trackball::MBUTTON_DOWN : Trackball::MBUTTON_UP) : 0;
    s |= (button == GLUT_RIGHT_BUTTON)  ? ((state == GLUT_DOWN) ? Trackball::RBUTTON_DOWN : Trackball::RBUTTON_UP) : 0;

    int key_state = glutGetModifiers();
    s |= (key_state & GLUT_ACTIVE_CTRL)  ? Trackball::CTRL_DOWN  : 0;
//    s |= (key_state & GLUT_ACTIVE_ALT)   ? Trackball::ALT_DOWN   : 0;
    s |= (key_state & GLUT_ACTIVE_SHIFT) ? Trackball::SHIFT_DOWN : 0;

     int tmplayer, tmpindex;

     slider->Mouse(x, image_height-y);
     slider->widgetMouse(x, image_height - y);

     if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
     {
         svVector3 eye;
         eye[0] = view_info.eye[0];
         eye[1] = view_info.eye[1];
         eye[2] = view_info.eye[2];
         Matrix4f sc = trackball.getZoomMatrix();
         directglyph->Mouse(x,y,eye,view_info.hither,
                            viewproperty.tm,sc[0][0],clusterLayer, clusterData);
         UpdateVisible();
         //directglyph->Mouse(x,y,viewproperty.tm,9e+9);
     }

     if(enable_contour_tree && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
     {
        if(image_inside)
        {
          updatecontour = true;
          contour->Mouse(x,image_height-y,contour_layer, contour_index, contour_mag);
          if(contour_layer>=0 && contour_index>=0)
          {
               UpdateSingleContour();
          }
         }
     }
       
       if(symmetry_enable && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
       {
        bool flag = false;
        for(int i=0;i<SYMMETRY_TYPE+1;i++)
        {
             if(x > texture_pos[i][0] && x < texture_pos[i][0]+texture_size *texture_ratio 
              && image_height-y>texture_pos[i][1] 
              && image_height-y<texture_pos[i][1]+texture_size)
            {
            //    if(i%2==0 && texture_status[i+1] == SYMMETRY_DOWN)
            //           continue;
            //    if(i%2==1 && texture_status[i-1] == SYMMETRY_DOWN)
            //           continue;
                 if(texture_status[i] == SYMMETRY_DOWN_SELECT)
                       texture_status[i] = SYMMETRY_UP;
                 else if (texture_status[i] == SYMMETRY_UP_SELECT)
                      texture_status[i] = SYMMETRY_DOWN;
                 else if(texture_status[i] == SYMMETRY_UP)
                         texture_status[i] = SYMMETRY_DOWN_SELECT;
                 else if(texture_status[i] == SYMMETRY_DOWN)
                         texture_status[i] = SYMMETRY_UP_SELECT;
            //     flag = true;
               // cerr<<texture_status[i]<<endl;
               flag = true;
//               if(texture_status[i] == SYMMETRY_DOWN)
//                     texture_status[i] = SYMMETRY_UP;
//               else
//                     texture_status[i] = SYMMETRY_DOWN;
            }     
        }
        if(flag){
                  //UpdateVisible();  
                  updatevisible = true; 
                  updatecluster=true;
            //      updaterender = true;
        }
      }
        else if(layerVisible && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {//cerr<<x<<" "<<y<<endl;
         //   int x1 = (x-widget_tranx)/widget_scalex;
         //   int y1 = (image_height-y-widget_trany)/widget_scaley;
            widget->Mouse(widget_tranx, widget_trany,
                          widget_scalex, widget_scaley,
                          x,image_height-y);
            updatevisible = true; 
        }

        if(button == GLUT_LEFT_BUTTON && state == GLUT_UP && widget->isSelect())
        {
           UpdateVisible();
           
           widget->Reset();
        }

        if (s & Trackball::BUTTON_DOWN){
          trackball.mouseDown(s, x, y);
          glEnable(GL_CULL_FACE);
          ACSIZE = 1;g_numPasses=2;
        }

        if (s & Trackball::BUTTON_UP){
          trackball.mouseUp(s, x, y);
          glDisable(GL_CULL_FACE);
          ACSIZE=4;g_numPasses=4;
        }
        glutPostRedisplay();

}

void motion(int x, int y)
{
        int x1 = (x-widget_tranx)/widget_scalex;
        int y1 = (image_height-y-widget_trany)/widget_scaley;

       slider->widgetMotion(x, image_height-y);
       if(layerVisible)
       {
             svVector3 value[3];
             widget->Move(x1, y1);
             svInt *index=new svInt[3];  widget->GetIndex(index);
             for(int i=0;i<3;i++)
             {
               value[i]  = flow_field->GetPlanePosition(index[i]);
             }
             widget->SetValues(value);
       }

       if(!widget->isSelect())
        trackball.mouseMotion(x, y);
        glutPostRedisplay();
}

void movement(int x, int y)
{
     movement_x = x;
     movement_y = y;

     if(mouse_button == GLUT_LEFT_BUTTON )
     {
       slider->widgetMovement(x, image_height-y);
       if(enable_contour_tree)
       {
//          cerr<<image_lbx<<" "<<image_rbx<<" "<<image_lby<<" "<<image_rby<<endl;
          if(x >= image_lbx 
            && x <= image_rbx 
            && image_height-y >= image_lby 
            && image_height - y <=image_rby )
          {
             image_inside = true;
             contour->Move(x, image_height -y);
          }
          else
          {
             image_inside = false;
             contour->Move();
          }
           //contour->SetContourPosition();
       }
       if(symmetry_enable)
       {
           for(int i=0;i<SYMMETRY_TYPE+1;i++)
           {
//                if(i%2==0 && texture_status[i+1] == SYMMETRY_DOWN)
//                       continue;
//                if(i%2==1 && texture_status[i-1] == SYMMETRY_DOWN)
//                       continue;

             if(x > texture_pos[i][0] && x < texture_pos[i][0]+texture_size *texture_ratio
              && image_height-y>texture_pos[i][1]
              && image_height-y<texture_pos[i][1]+texture_size)
            {
                 if(texture_status[i] == SYMMETRY_UP)
                         texture_status[i] = SYMMETRY_UP_SELECT;
                 else if(texture_status[i] == SYMMETRY_DOWN)
                         texture_status[i] = SYMMETRY_DOWN_SELECT;
           //      cerr<<i<<" "<<texture_status[i]<<endl;
            } 
            else
            {
                 if(texture_status[i] == SYMMETRY_UP_SELECT)
                         texture_status[i] = SYMMETRY_UP;
                 else if(texture_status[i] == SYMMETRY_DOWN_SELECT)
                         texture_status[i] = SYMMETRY_DOWN;
            }
           }
       }
    }
    glutPostRedisplay();
}
void InitField()
{
	char *qdot_format = new char[400];
	sprintf(qdot_format,"%s/%s/format.txt", 
                    paser->GetStoreDir(),
                    paser->GetRawFile());
        flow_field->SetVTK(
          paser->GetRawDir(), paser->GetRawFile(),
          paser->GetStoreDir(), 
          "sort.txt", "format.txt", "density.txt",
          paser->GetPlaneCenter(),
          paser->GetPlaneVector(),
          paser->GetPlaneDistance(),
          paser->GetFormat()
        );
        flow_field->New(qdot_format);	
        char *str = new char[200];

        sprintf(str, "%s/%s/", 
         paser->GetStoreDir(),paser->GetRawFile());
        flow_field->NewMesh(str);

      delete [] str;
      delete [] qdot_format;

}
void ReadConfig(char *inputfile)
{
    paser->ReadFiles(inputfile);

    InitField();

    paser->SetSeedNum(flow_field->GetPlaneNum());    
    paser->SetPlane(flow_field->GetPlaneDistance(),flow_field->GetMinPlane());

    char *str = new char[400];

    sprintf(str, "%s/%s/density.txt", paser->GetStoreDir(), paser->GetRawFile());
    paser->ProcessContour();
    paser->SetContourValues(flow_field->GetMaxDensity(str));
    paser->ProcessClusterLayers();
    paser->ResetBool();
    delete [] str;

}
void LoadTexture()
{
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_DEPTH_TEST);

    texture_pos = new svVector3[SYMMETRY_TYPE+1];
    texture_status = new int[SYMMETRY_TYPE+1];
    texture_visual_pos = new svVector3[VISUAL_TYPE];
    texture_visual_status = new int[VISUAL_TYPE];

    for(int i=0;i<SYMMETRY_TYPE;i++)
        texture_status[i] = SYMMETRY_DOWN;
    texture_status[SYMMETRY_TYPE] = SYMMETRY_UP;

    texture_visual_status[0] = SYMMETRY_DOWN;
    texture_visual_status[1] = SYMMETRY_UP;
    texture_visual_status[2] = SYMMETRY_UP;

    LocateTexture();
}

void LocateTexture()
{
    //texture_visual_size = 90;//72
    texture_size = 30.;//72*2.25;//(svScalar)(image_height/15.);
    texture_ratio = 1.8;
    texture_visual_ratio = 2.2;
    svScalar size = texture_size;//cerr<<size<<endl;
    for(int i=0;i<SYMMETRY_TYPE+1;i++)
    {
       texture_pos[i][0] = widget_tranx+100+texture_size*texture_ratio*1.25*(float)(i);
       texture_pos[i][1] = widget_trany+40;
//image_height - texture_size*1.5*(svScalar)(i+1);//2.125 
//       cerr<<texture_pos[i][0]<<" "<<texture_pos[i][1]<<endl;
    }
    int size2 = texture_size;
//    texture_size = 10.;
    texture_visual_size = 40;//texture_size;

    size = texture_visual_size;
    texture_visual_pos[0][0]= widget_tranx;
//325;          
    texture_visual_pos[0][1]= widget_trany;
//image_height-size*1.5;///2.125;//*1.2;
    texture_visual_pos[1][0]= widget_tranx+texture_visual_size*texture_visual_ratio*1.25; 
    texture_visual_pos[1][1]=widget_trany;//image_height-size*1.5*2;///2.125 * 2;
    texture_visual_pos[2][0]=325+size*3.4; texture_visual_pos[2][1]=image_height-size*1.2;
}
/*
void ComputeGLUIFactor()
{
    if(encode_type == LINEAR || encode_type == LINEAR_TUBE)
    {
        glui_length_scale = (length_scale / LINEAR_SCALE)/5.;
    }
    else
    {
        glui_length_scale = (length_scale / SPLITVECTORS_SCALE)/5.;
    } 
    glui_arrow_scale = (arrow_scale / ARROW_SCALE)/2.;
    glui_tube_scale = (tube_scale / TUBE_SCALE)/2.;

}
*/
void InitDualPeelingRenderTargets()
{
        glGenTextures(2, g_dualDepthTexId);
        glGenTextures(2, g_dualFrontBlenderTexId);
        glGenTextures(2, g_dualBackTempTexId);

        glewExperimental = GL_TRUE;
        glGenFramebuffersEXT(1, &g_dualPeelingSingleFboId);

        for (int i = 0; i < 2; i++)
        {
                glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualDepthTexId[i]);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                 glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RG32F, g_imageWidth, g_imageHeight,
                                         0, GL_RGB, GL_FLOAT, 0);

                glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualFrontBlenderTexId[i]);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, g_imageWidth, g_imageHeight,
                                         0, GL_RGBA, GL_FLOAT, 0);

                glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualBackTempTexId[i]);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, g_imageWidth, g_imageHeight,
                                         0, GL_RGBA, GL_FLOAT, 0);
        }
        glGenTextures(1, &g_dualBackBlenderTexId);
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualBackBlenderTexId);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, g_imageWidth, g_imageHeight,
                                 0, GL_RGB, GL_FLOAT, 0);

        glGenFramebuffersEXT(1, &g_dualBackBlenderFboId);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_dualBackBlenderFboId);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                                          GL_TEXTURE_RECTANGLE_ARB, g_dualBackBlenderTexId, 0);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_dualPeelingSingleFboId);
        int j = 0;
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                                                  GL_TEXTURE_RECTANGLE_ARB, g_dualDepthTexId[j], 0);
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,
                                                                  GL_TEXTURE_RECTANGLE_ARB, g_dualFrontBlenderTexId[j], 0);
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT,
                                                                  GL_TEXTURE_RECTANGLE_ARB, g_dualBackTempTexId[j], 0);

        j = 1;
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT,
                                                                  GL_TEXTURE_RECTANGLE_ARB, g_dualDepthTexId[j], 0);
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT4_EXT,
                                                                  GL_TEXTURE_RECTANGLE_ARB, g_dualFrontBlenderTexId[j], 0);
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT5_EXT,
                                                                  GL_TEXTURE_RECTANGLE_ARB, g_dualBackTempTexId[j], 0);

        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT6_EXT,
                                                          GL_TEXTURE_RECTANGLE_ARB, g_dualBackBlenderTexId, 0);
        CHECK_GL_ERRORS;
}
void DeleteDualPeelingRenderTargets()
{
        glDeleteFramebuffersEXT(1, &g_dualBackBlenderFboId);
        glDeleteFramebuffersEXT(1, &g_dualPeelingSingleFboId);
        glDeleteTextures(2, g_dualDepthTexId);
        glDeleteTextures(2, g_dualFrontBlenderTexId);
        glDeleteTextures(2, g_dualBackTempTexId);
        glDeleteTextures(1, &g_dualBackBlenderTexId);
}

void BuildShaders(string SHADER_PATH)
{
        printf("\nloading shaders...\n");

        g_shaderDualInit.attachVertexShader(SHADER_PATH+"dual_peeling_init_vertex.glsl");
        g_shaderDualInit.attachFragmentShader(SHADER_PATH+"dual_peeling_init_fragment.glsl");
        g_shaderDualInit.link();

        g_shaderDualPeel.attachVertexShader(SHADER_PATH+"shade_vertex.glsl");
        g_shaderDualPeel.attachVertexShader(SHADER_PATH+"dual_peeling_peel_vertex.glsl");
        g_shaderDualPeel.attachFragmentShader(SHADER_PATH+"shade_fragment.glsl");
        g_shaderDualPeel.attachFragmentShader(SHADER_PATH+"dual_peeling_peel_fragment.glsl");
        g_shaderDualPeel.link();

        g_shaderDualBlend.attachVertexShader(SHADER_PATH+"dual_peeling_blend_vertex.glsl");
        g_shaderDualBlend.attachFragmentShader(SHADER_PATH+"dual_peeling_blend_fragment.glsl");
        g_shaderDualBlend.link();

        g_shaderDualFinal.attachVertexShader(SHADER_PATH+"dual_peeling_final_vertex.glsl");
        g_shaderDualFinal.attachFragmentShader(SHADER_PATH+"dual_peeling_final_fragment.glsl");
        g_shaderDualFinal.link();

}

void MakeFullScreenQuad()
{
        g_quadDisplayList = glGenLists(1);
        glNewList(g_quadDisplayList, GL_COMPILE);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, 1.0, 0.0, 1.0);
        glBegin(GL_QUADS);
        {
                glVertex2f(0.0, 0.0);
                glVertex2f(1.0, 0.0);
                glVertex2f(1.0, 1.0);
                glVertex2f(0.0, 1.0);
        }
        glEnd();
        glPopMatrix();
        glEndList();
}

void RenderDualPeeling()
{
/*   glEnable(GL_NORMALIZE);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);
   glEnable(GL_COLOR_MATERIAL);
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
   glDisable(GL_LIGHTING);
*/
 
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
 //===================================
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_dualPeelingSingleFboId);

        glDrawBuffers(2, &g_drawBuffers[1]);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawBuffer(g_drawBuffers[0]);
        glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glBlendEquationEXT(GL_MAX_EXT);

        g_shaderDualInit.bind();
       // splitglyph->Render();
    
        glPushMatrix();
        glMultMatrixf(m);
        glEnable(GL_LIGHTING);
        if(encode_type == LINEAR)
        {
            if(arrow_scale_up) directglyph->RenderSample();
            else directglyph->RenderVBO();
        }
        else if(encode_type == LINEAR_TUBE)
        {
        }
        else if(encode_type == LLO)
        {
        }
        else if(encode_type == LT)
        {
        }
        else
        {
            if(arrow_scale_up) splitglyph->RenderSample();
            else
            {
                   splitglyph->RenderVBO();
                   splitglyph->RenderLegend();
            }
        }
        if(summaryVisible)summaryglyph->Render();
        if(mesh_enable) mesh->Render();
        if(symmetrymesh_enable) symmetrymesh->Render();
        glDisable(GL_LIGHTING);
        outline->DrawXYZ(splitglyph->GetLb(), splitglyph->GetRb(), viewproperty);
        if(symmetry_enable)
        outline->DrawXYZPrime(paser->GetSymmetryPos(),
                              paser->GetSymmetryDir(),
                              paser->GetSymmetryX(),
                              paser->GetSymmetryY(),
//configproperty.symmetryproperty.pos,
//                              configproperty.symmetryproperty.dir,
//                              configproperty.symmetryproperty.x,
//                              configproperty.symmetryproperty.y,
                              splitglyph->GetRb());
        outline->DrawAxis(splitglyph->GetLb(), splitglyph->GetRb());
        outline->DrawAxisSticks(splitglyph->GetLb(), splitglyph->GetRb(), viewproperty);
        glPopMatrix();
        outline->DrawXYZFont(splitglyph->GetLb(), splitglyph->GetRb(), viewproperty);
        if(symmetry_enable)
        outline->DrawXYZPrimeFont(paser->GetSymmetryPos(),
                              paser->GetSymmetryDir(),
                              paser->GetSymmetryX(),
                              paser->GetSymmetryY(),
                              splitglyph->GetRb(),viewproperty);
        outline->DrawAxisFont(splitglyph->GetLb(), splitglyph->GetRb(), viewproperty);
       // display();

        g_numGeoPasses++;
        g_shaderDualInit.unbind();

        glEnable(GL_LIGHTING);

        CHECK_GL_ERRORS;

//=======================================

        glDrawBuffer(g_drawBuffers[6]);
        glClearColor(g_backgroundColor[0], g_backgroundColor[1], g_backgroundColor[2], 0);
        glClear(GL_COLOR_BUFFER_BIT);

        int currId = 0;

        for (int pass = 1; g_useOQ || pass < g_numPasses; pass++) {
                currId = pass % 2;
                int prevId = 1 - currId;
                int bufId = currId * 3;

                glDrawBuffers(2, &g_drawBuffers[bufId+1]);
                glClearColor(0, 0, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT);

                glDrawBuffer(g_drawBuffers[bufId+0]);
                glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT);
                glDrawBuffers(3, &g_drawBuffers[bufId+0]);
                glBlendEquationEXT(GL_MAX_EXT);

                g_shaderDualPeel.bind();
                g_shaderDualPeel.bindTextureRECT("DepthBlenderTex", g_dualDepthTexId[prevId], 0);
                g_shaderDualPeel.bindTextureRECT("FrontBlenderTex", g_dualFrontBlenderTexId[prevId], 1);
              /* for(int j=0;j<display_list.size();j++)
               {
                        g_shaderDualPeel.setUniform("Alpha", (float*)&(g_opacity[j]), 1);
                        glCallList(display_list[j]);
               }
*/
                //splitglyph->Render();
                glPushMatrix();
                glMultMatrixf(m);
                 glEnable(GL_LIGHTING);
                g_shaderDualPeel.setUniform("Alpha", (float*)&(alpha),1);
                if(encode_type == LINEAR)
                {
                       if(arrow_scale_up) directglyph->RenderSample();
                       else directglyph->RenderVBO();
                }
                else if(encode_type == LINEAR_TUBE)
                {
                }
                else if(encode_type == LLO)
                {
                }
                else if(encode_type == LT)
               {
                }
                 else
                 {
                     //  splitglyph->RenderVBO();
                       if(arrow_scale_up) splitglyph->RenderSample();
                       else 
                       {
                                splitglyph->RenderVBO();
                                splitglyph->RenderLegend();
                       }
                 }
                 if(summaryVisible)summaryglyph->Render();
                 if(mesh_enable)mesh->Render();
                 if(symmetrymesh_enable)symmetrymesh->Render();
                 glDisable(GL_LIGHTING);
                 float solid = 1;
                g_shaderDualPeel.setUniform("Alpha", (float*)&(solid),1);
                 outline->DrawXYZ(splitglyph->GetLb(), splitglyph->GetRb(),viewproperty);
        if(symmetry_enable)
                 outline->DrawXYZPrime(paser->GetSymmetryPos(),
                              paser->GetSymmetryDir(),
                              paser->GetSymmetryX(),
                              paser->GetSymmetryY(),
                              splitglyph->GetRb());
                 outline->DrawAxis(splitglyph->GetLb(), splitglyph->GetRb());
                 outline->DrawAxisSticks(splitglyph->GetLb(), splitglyph->GetRb(), viewproperty);
                 glPopMatrix();
                 outline->DrawXYZFont(splitglyph->GetLb(), splitglyph->GetRb(), viewproperty);
        if(symmetry_enable)
                 outline->DrawXYZPrimeFont(paser->GetSymmetryPos(),
                              paser->GetSymmetryDir(),
                              paser->GetSymmetryX(),
                              paser->GetSymmetryY(),
                              splitglyph->GetRb(),viewproperty);
                 outline->DrawAxisFont(splitglyph->GetLb(), splitglyph->GetRb(), viewproperty);
                 glEnable(GL_LIGHTING);
            //    glPopMatrix();
        //        display();
                g_numGeoPasses++;
                g_shaderDualPeel.unbind();
                CHECK_GL_ERRORS;
                glDrawBuffer(g_drawBuffers[6]);

                glBlendEquationEXT(GL_FUNC_ADD);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                if (g_useOQ) {
                        glBeginQuery(GL_SAMPLES_PASSED_ARB, g_queryId);
                }

                g_shaderDualBlend.bind();
                g_shaderDualBlend.bindTextureRECT("TempTex", g_dualBackTempTexId[currId], 0);
                glCallList(g_quadDisplayList);
                g_shaderDualBlend.unbind();

                CHECK_GL_ERRORS;

                if (g_useOQ) {
                        glEndQuery(GL_SAMPLES_PASSED_ARB);
                        GLuint sample_count;
                        glGetQueryObjectuiv(g_queryId, GL_QUERY_RESULT_ARB, &sample_count);
                        if (sample_count == 0) {
                                break;
                        }
                }
        }

        glDisable(GL_BLEND);

//==============================================
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDrawBuffer(GL_BACK);
        g_shaderDualFinal.bind();
        g_shaderDualFinal.bindTextureRECT("DepthBlenderTex", g_dualDepthTexId[currId], 0);
        g_shaderDualFinal.bindTextureRECT("FrontBlenderTex", g_dualFrontBlenderTexId[currId], 1);
        g_shaderDualFinal.bindTextureRECT("BackBlenderTex", g_dualBackBlenderTexId, 2);
        glCallList(g_quadDisplayList);
        g_shaderDualFinal.unbind();


        CHECK_GL_ERRORS;
}


void InitGL(string shader)
{
     g_numPasses = 4;
     g_useOQ = false;
     g_showOsd = true;
     g_bShowUI = true;
     g_numGeoPasses = 0;

     g_white[0]=1;g_white[1]=1;g_white[2]=1;
     g_black[0]=0;g_black[1]=0;g_black[2]=1;

     g_drawBuffers[0] = GL_COLOR_ATTACHMENT0_EXT;
     g_drawBuffers[1] = GL_COLOR_ATTACHMENT1_EXT;
     g_drawBuffers[2] = GL_COLOR_ATTACHMENT2_EXT;
     g_drawBuffers[3] = GL_COLOR_ATTACHMENT3_EXT;
     g_drawBuffers[4] = GL_COLOR_ATTACHMENT4_EXT;
     g_drawBuffers[5] = GL_COLOR_ATTACHMENT5_EXT;
     g_drawBuffers[6] = GL_COLOR_ATTACHMENT6_EXT;
     g_backgroundColor = g_white;

        InitDualPeelingRenderTargets();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        BuildShaders(shader);
        MakeFullScreenQuad();

        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_NORMALIZE);

        glGenQueries(1, &g_queryId);
}

void initContours()
{

}

void initParameters(){
  symmetrycount = new svScalarArray[4];
  if(encode_type == LINEAR)
    length_scale = LINEAR_SCALE;
  else
    length_scale = SPLITVECTORS_SCALE;

  arrow_scale = ARROW_SCALE;
  tube_scale = TUBE_SCALE;
  summary_arrow_scale = SUMMARY_ARROW_SCALE;
  summary_tube_scale = SUMMARY_TUBE_SCALE;

  summaryVisible = 0;
  symmetry_enable = 0;
  textureinit = 0;

  widget_tranx=275;
  widget_trany=image_height-125;
  widget_scalex=4;
  widget_scaley=10.;

  svColors *color_object = new svColors();
  for(int i=0;i<SYMMETRY_TYPE;i++)
  {
      buttoncolors[i] = color_object->Get8Colors(i);
      buttoncolors[i][3] = 0.5;
  }

}

void initDisplayList(){
  symmetrymesh_solid_list = 50;
  mesh_solid_list =30;
  render_list = 10;
  summary_list = 20;
  outline_display_list = 40;
}

void initDualPeeling(){
  char *str = new char[200];
  sprintf(str,"%s/libs/dual_depth_peeling/shaders/",SRC_DIR);
  InitGL(str);
  delete [] str;
}

void initVariables(){
  mesh = new svMesh();
  symmetrymesh = new svMesh();
  outline = new svOutline();
  flow_field = new svQDOT();
  widget = new svWidget();
  contour = new svContourImageDrawing();
  image = new svImage();
  slider = new svQDOTSlider();
  slider->initRec(180, 300);
}

void initConfig(char *configfname){
  configFile = strdup(configfname);
  paser = new Paser();
  ReadConfig(configFile);

  char *str = new char[200];
  char buffer[80];
  getTime(buffer);
  sprintf(str,"%s/config%s", SRC_DIR,buffer);
  paser->SavetoFile(str);
  delete [] str;
}

void initRender(){
  rawdata = new svDirectArrow(flow_field);
  directglyph = new svDirectArrow(flow_field);
  summaryglyph = new svSummaryGlyph(flow_field);
  splitglyph = new svSplitArrow(flow_field);
}

void init(char *configfname)//rbfname, char *cpname)
{
//======== init parameters=======
  initParameters();
//===========display list=========
  initDisplayList();
//===========dual peeling=========
  initDualPeeling();
//==========variable==============
  initVariables();
//==========config================
  initConfig(configfname);
//===========operation============
  show_data = paser->GetIsContour();
//===========GUI==================
  glui_display();
//===========Tex==================
  LoadTexture();
//===========functions===========
  Update();
//===========done================
}

void Update()
{
//=======================================
//  Config(configfname, configproperty);
  char *str = new char[200];
  unique_region.clear();
  unique_region = flow_field->GetUniqueRegion();
  sort(unique_region.begin(), unique_region.end());
//  if(!paser->GetIsContour())//!configproperty.isContour)
//  { 
//      contour_box->set_int_val(0);
//      contour_box->disable();
//      enable_contour_tree=0;
//  }
//  else
  {
      contour_box->enable();
  }
  widget->SetLevel(flow_field->GetPlaneNum());
  widget->Init();
  widget->SetIndex(paser->GetZmin(), paser->GetZmax(), paser->GetNonz());//zmin, zmax, nonz);
  widget->Repeat(paser->GetRepeated()); 
  svVector3 value[3];
  svInt *index=new svInt[3];  widget->GetIndex(index);
  for(int i=0;i<3;i++)
  {
         value[i]  = flow_field->GetPlanePosition(index[i]);
  }
   widget->SetValues(value);
//   widget->SetValues(splitglyph->GetEntropyValues());

cerr<<"===="<<endl;
  summaryglyph->New(flow_field, flow_field->GetPlaneNum());
  directglyph->New(flow_field, flow_field->GetPlaneNum());
  rawdata->New(flow_field, flow_field->GetPlaneNum());
  splitglyph->New(flow_field, flow_field->GetPlaneNum());

//  UpdateImage();

cerr<<"=============="<<endl;
  if(paser->GetIsContour())
  {
//cerr<<"============="<<endl;
       splitglyph->GenerateContours(paser->GetContourProperty());
//configproperty.contourproperty);
       directglyph->GenerateContours(paser->GetContourProperty());//cerr<<"done"<<endl;
       summaryglyph->GenerateContours(paser->GetContourProperty()); 

       splitglyph->GenerateEntropy();
       directglyph->GenerateEntropy();
       widget->SetHistoValues(splitglyph->GetMagProb(topmaglevel));
       widget->SetValues(splitglyph->GetEntropyValues(),true);
//       UpdateContour();
//       UpdateImageBack();
        directglyph->GenerateSymmetry(paser->GetSymmetryProperty());
//configproperty.symmetryproperty);//, frequency);
        splitglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
        summaryglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
        symmetrycount = directglyph->GetSymmetryMagProb(symmetrytopvalue);
  }
  else
   {
         for(int i=0;i<flow_field->GetPlaneNum();i++)
         {
               sprintf(str, "%s/%s/%s%d.txt", paser->GetStoreDir(),
                       paser->GetRawFile(), paser->GetPlaneName(),i);
//configproperty.storeDir, configproperty.rawFile, configproperty.planename, i);
               //cerr<<str<<endl;
	           directglyph->SetData(str, i);
                   summaryglyph->SetData(str, i);
                   splitglyph->SetData(str, i);
          //     sprintf(str, "%s/%s/format%d.txt",configproperty.storeDir, configproperty.rawFile, i);
         }

         //UpdateTempContour();
               char *format_fname = new char[400];
               sprintf(format_fname,"%s/%s/zformat.txt",paser->GetStoreDir(),
                       paser->GetRawFile());
               char *index_fname = new char[400];
               sprintf(index_fname,"%s/%s/reverseindex.txt",
                       paser->GetStoreDir(),
                       paser->GetRawFile());
//configproperty.storeDir, configproperty.rawFile);
         cerr<<format_fname<<endl;
                   directglyph->SetFormat(format_fname, index_fname);
                   summaryglyph->SetFormat(format_fname, index_fname);
                   splitglyph->SetFormat(format_fname, index_fname);
//SetFormat(char *format_fname, char *index_fname);
           //        summaryglyph->SetFormat(str,i);
             //      splitglyph->SetFormat(str,i);
         //}
               delete [] format_fname;            
               delete [] index_fname;

        cerr<<"format"<<endl;
 
        splitglyph->GenerateEntropy();
        directglyph->GenerateEntropy();
        cerr<<"entropy"<< " "<<topmaglevel<<endl;

        widget->SetHistoValues(splitglyph->GetMagProb(topmaglevel));
        widget->SetValues(splitglyph->GetEntropyValues(),true);

        cerr<<"entropy"<<endl;
        directglyph->SetContourLabel();
        summaryglyph->SetContourLabel();
        splitglyph->SetContourLabel();
        cerr<<"setcontourlable"<<endl;

        directglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
        splitglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
        summaryglyph->GenerateSymmetry(paser->GetSymmetryProperty());//, frequency);
        symmetrycount = directglyph->GetSymmetryMagProb(symmetrytopvalue);
  }
  for(int i=0;i<flow_field->GetPlaneNum();i++)
  {
      sprintf(str, "%s/%s/%s%d.txt", paser->GetStoreDir(),
                       paser->GetRawFile(), paser->GetPlaneName(),i);
      rawdata->SetData(str,i);
  }
  UpdateContour();
  UpdateImageBack();

  char *format_fname = new char[400];
  sprintf(format_fname,"%s/%s/zformat.txt",paser->GetStoreDir(),
              paser->GetRawFile());
  char *index_fname = new char[400];
  sprintf(index_fname,"%s/%s/reverseindex.txt",
                       paser->GetStoreDir(),
                       paser->GetRawFile());
  rawdata->SetFormat(format_fname, index_fname);
  delete [] format_fname;
  delete [] index_fname;
  rawdata->GenerateSymmetry(paser->GetSymmetryProperty());
  cerr<<"done1"<<endl;

   rawdata->ResetCluster();
   rawdata->SetXdistance(flow_field->GetXdistance());//cerr<<"done1"<<endl;
   rawdata->SetYdistance(flow_field->GetYdistance());//cerr<<"done1"<<endl;
   rawdata->SetZdistance(flow_field->GetZdistance());//cerr<<"done1"<<endl;
   rawdata->SetROI(paser->GetMagRange00(),paser->GetMagRange01());
   rawdata->GenerateClustersBySymmetry(paser->GetStep1Property(),
                         paser->GetSymmetryProperty(),
                      symmetrytype);
   rawdata->SetROI(paser->GetMagRange10(),paser->GetMagRange11());
   rawdata->GenerateClustersBySymmetry(paser->GetStep2Property(),
                         paser->GetSymmetryProperty(),
                      symmetrytype);
   rawdata->SetNumPower(flow_field->GetNumPower());
   rawdata->SetScaling(flow_field->GetScaling());

   directglyph->ResetCluster();//cerr<<"done1"<<endl;
   directglyph->SetXdistance(flow_field->GetXdistance());//cerr<<"done1"<<endl;
   directglyph->SetYdistance(flow_field->GetYdistance());//cerr<<"done1"<<endl;
   directglyph->SetZdistance(flow_field->GetZdistance());//cerr<<"done1"<<endl;
   directglyph->SetROI(paser->GetMagRange00(),paser->GetMagRange01());
//configproperty.magrange[0][0], configproperty.magrange[0][1]);
   directglyph->GenerateClustersBySymmetry(paser->GetStep1Property(),
                         paser->GetSymmetryProperty(),
//configproperty.step1_kmeansproperty,
//                      configproperty.symmetryproperty,
                      symmetrytype);
   directglyph->SetROI(paser->GetMagRange10(),paser->GetMagRange11());
//configproperty.magrange[1][0], configproperty.magrange[1][1]);
   directglyph->GenerateClustersBySymmetry(paser->GetStep2Property(),
                         paser->GetSymmetryProperty(),
//configproperty.step2_kmeansproperty,
//                      configproperty.symmetryproperty,
                      symmetrytype);
   directglyph->SetNumPower(flow_field->GetNumPower());
   directglyph->SetScaling(flow_field->GetScaling());


   splitglyph->ResetCluster();
   splitglyph->SetXdistance(flow_field->GetXdistance());
   splitglyph->SetYdistance(flow_field->GetYdistance());
   splitglyph->SetZdistance(flow_field->GetZdistance());
   splitglyph->GenerateClusters(directglyph->GetClusterLabels());
   splitglyph->SetES(flow_field->GetScaling());
   splitglyph->SetNumPower(flow_field->GetNumPower());
   splitglyph->SetScaling(flow_field->GetScaling());

   summaryglyph->ResetCluster();
   summaryglyph->SetXdistance(flow_field->GetXdistance());
   summaryglyph->SetYdistance(flow_field->GetYdistance());
   summaryglyph->SetZdistance(flow_field->GetZdistance());
   summaryglyph->GenerateClusters(directglyph->GetClusterLabels());
   summaryglyph->SetNumPower(flow_field->GetNumPower());
   summaryglyph->SetScaling(flow_field->GetScaling());
   summaryglyph->SetDisplayList(summary_list);

   slider->setBoundary(directglyph->GetLb(), directglyph->GetRb());
   slider->setMag(flow_field->GetMinValue(), flow_field->GetMaxValue());
   svVector3 origin; origin[0]=0;origin[1]=0;origin[2]=0;
   slider->setImage(image_width, image_height, 350, 0);
   slider->setRec(origin);

   UpdateVBOData();cerr<<"UpdateVBOData()"<<endl;
   UpdateVisible();cerr<<"UpdateVisible()"<<endl;
   UpdateCluster();cerr<<"UpdateCluster()"<<endl;
   UpdateColor();cerr<<"UpdateColor()===="<<endl;
  // if(paser->GetIsContour())
 //  {
       Update2DColor();cerr<<"Update2DColor"<<endl;
       UpdateImage();cerr<<"UpdateImage"<<endl;
  // }
   UpdateRender();cerr<<"UpdateRender()"<<endl;
   UpdateSymmetryMesh();cerr<<"UpdateSymmetryMesh"<<endl;

   mesh->SetDisplayList(mesh_solid_list);
   mesh->New(unique_region);
   for(int i=0;i<unique_region.size();i++)
   {
        sprintf(str, "%s/%s/mesh%d.txt", 
                paser->GetStoreDir(),
                paser->GetRawFile(),
               unique_region[i]);
        mesh->SetData(str, i); 
   }
   mesh->GenerateSurfaces(1);//unique_region[1]);
   delete [] str;
  cerr<<"mesh generation"<<endl;
  svVector3 center = flow_field->GetCenter();//cerr<<"done"<<endl;
  center.getValue(view_info.coi);

  GLfloat x, y, z;
  flow_field->GetPhysicalDimension(&x,&y, &z);

  view_info.eye[0] = 0;//x/2.0;
  view_info.eye[1] = 0;// y/2.0;
  view_info.eye[2] = z*2.0;

  viewproperty.eye[0] = view_info.eye[0];
  viewproperty.eye[1] = view_info.eye[1];
  viewproperty.eye[2] = view_info.eye[2];

  trackball.setEye(view_info.eye);
  trackball.setFocus(center);
  trackball.setWindowSize(image_width, image_height);
  trackball.reset();
cerr<<"trackball"<<endl;
}

//**********************
//// GLUI
////**********************

void glui_data(){
 GLUI_Panel *panel_data = glui->add_panel("Data");
 panel_data->set_alignment(GLUI_ALIGN_LEFT);
 GLUI_Checkbox *box_raw = glui->add_checkbox_to_panel(panel_data, "Raw", &enable_raw, RAW_DATA_ID, control_cb);
 GLUI_Checkbxo *box_contour = glui->add_checkbox_to_paenl(panel_data, "Contour", &enable_contour, CONTOUR_DATA_ID, control_cb);
}

void glui_roi(){
 GLUI_Panel *panel_roi =  glui->add_panel("ROI selection");
 panel_roi->set_alignment(GLUI_ALIGN_LEFT);

 GLUI_Panel *panel_symmetry = glui->add_panel_to_panel(panel_roi,"");
 panel_symmetry->set_alignment(GLUI_ALIGN_LEFT);
 GLUI_Checkbox *box_symmetry = glui->add_checkbox_to_panel(panel_symmetry,"By symmetry", &enable_symmetry, SYMMETRY_ROI_ID, control_cb);
 box_symmetry_type = glui->add_checkbox_to_panel(panel_symmetry, "Types", &enable_symmetry_type, SYMMETRY_TYPE_ID, control_cb);
 GLUI_Checkbox *symmetrysurface =  glui->add_checkbox_to_panel(panel_symmetry, "Surface", &enable_symmetry_mesh);

  GLUI_Panel *panel_layer= glui->add_panel_to_panel(panel_roi,"");
  panel_layer->set_alignment(GLUI_ALIGN_LEFT);
  GLUI_Checkbox *box_layer = glui->add_checkbox_to_panel(panel_layer,"By layer", &enable_layer, LAYER_ROI_ID, control_cb);
  GLUI_Panel *panel_layer_slider = glui->add_panel_to_panel(panel_layer,"");
  panel_layer_slider->set_alignment(GLUI_ALIGN_LEFT);
  box_layer_slider = glui->add_checkbox_to_panel(panel_layer_slider,"Slider", &enable_layer_slider, LAYER_SLIDER_ID, control_cb);
  glui->add_column_to_panel(panel_layer_slider, true);
  new GLUI_Checkbox(panel_layer_slider, "Repeat", &enable_layer_repeat, LAYER_REPEAT_ID, control_cb);

  GLUI_Panel *panel_attribute = glui->add_panel_to_panel(panel_roi,"");
  panel_attribute->set_alignment(GLUI_ALIGN_LEFT);
  GLUI_Checkbox *box_attribute = glui->add_checkbox_to_panel(panel_attribute,"By attribute", &enable_attribute, ATTRIBUTE_ROI_ID, control_cb);
  GLUI_Checkbox *box_attribute_panel = glui->add_checkbox_to_panel(panel_attribute,"Panel", &enable_attribute_panel, ATTRIBUTE_PANEL_ID, control_cb);

  GLUI_Panel *panel_cluster = glui->add_panel_to_panel(panel_roi,"");
  panel_cluster->set_alignment(GLUI_ALIGN_LEFT);
  GLUI_Checkbox *box_cluster = glui->add_checkbox_to_panel(panel_cluster,"By cluster", &enable_cluster, CLUSTER_ROI_ID, control_cb);
  GLUI_Checkbox *box_cluster_surface = glui->add_checkbox_to_panel(panel_surface,"Surface", &enable_cluster_surface, CLUSTER_SURFACE_ID, control_cb);

 GLUI_Panel *panel_map = glui->add_panel_to_panel(panel_roi, "By 2D map");
 panel_map->set_alignment(GLUI_ALIGN_LEFT);
 GLUI_Checkbox *box_map = glui->add_checkbox_to_panel(panel_map,"Show 2D map", &enable_map, MAP_ID, control_cb);
 GLUI_Panel *panel_map_type = glui->add_panel_to_panel("Types");
 panel_map_type->set_alignment(GLUI_ALIGN_LEFT);
 GLUI_RadioGroup *group_map_type = new GLUI_RadioGroup(panel_map_type, &map_type, MAP_TYPE_ID, control_cb);
 glui->add_radiobutton_to_group(group_map_type, "Magnitude");
 glui->add_radiobutton_to_group(group_map_type, "Entropy");
 glui->add_radiobutton_to_group(group_map_type, "Distance");
 glui->add_radiobutton_to_group(group_map_type, "Cluster");

}

void glui_visual(){
 char str[20];
 
 GLUI_Panel *panel_visual = glui->add_panel("Display");

 panel_visual->set_alignment(GLUI_ALIGN_LEFT);
 GLUI_Panel *panel_visual_mapping = glui->add_panel_to_panel("Visual mapping");
 panel_visual_mapping->set_alignment(GLUI_ALIGN_LEFT);
 GLUI_RadioGroup *group_visual_mapping = new GLUI_RadioGroup(panel_visual_mapping, &encode_type, ENCODE_ID, control_cb);
 glui->add_radiobutton_to_group(group_visual_mapping, "Direct");
 glui->add_radiobutton_to_group(group_visual_mapping, "SplitVectors");

 new GLUI_Checkbox(panel_visual, "OverviewGlyph", &enable_overview, OVERVIEW_ID, control_cb);

 GLUI_Panel *panel_mesh = glui->add_panel_to_panel(panel_visual,"");
 panel_mesh->set_alignment(GLUI_ALIGN_LEFT);
 new GLUI_Checkbox(panel_mesh, "QDOT", &enable_mesh, MESH_ID,control_cb);
 glui->add_column_to_panel(panel_mesh, true);
 GLUI_RadioGroup *group_mesh_vis = new GLUI_RadioGroup(panel_mesh, &mesh_vis, MESH_VIS_ID, control_cb);
 glui->add_radiobutton_to_group(group_mesh_vis, "Wireframe");
 glui->add_radiobutton_to_group(group_mesh_vis, "Surface");

  sprintf(str, "Length");
  GLUI_Panel *panel_length = glui->add_panel_to_panel(panel_visual,"");
  panel_length->set_alignment(GLUI_ALIGN_LEFT);
  GLUI_StaticText *text_line = glui->add_statictext_to_panel(panel_length,str);
  text_line->set_w(50);
  glui->add_column_to_panel(panel_length,true);
  sb_line = new GLUI_Scrollbar(panel_length, "Length", GLUI_SCROLL_HORIZONTAL, &length_scale, LENGTH_ID, control_cb);
  sb_line->set_float_limits(0.,1.);

  sprintf(str, "Arrow");
  GLUI_Panel *panel_arrow = glui->add_panel_to_panel(panel_visual,"");
  panel_arrow->set_alignment(GLUI_ALIGN_LEFT);
  GLUI_StaticText *text_arrow = glui->add_statictext_to_panel(panel_arrow,str);
  text_arrow->set_w(50);
  glui->add_column_to_panel(panel_arrow,true);
  sb_arrow = new GLUI_Scrollbar(panel_arrow, "Arrow", GLUI_SCROLL_HORIZONTAL, &arrow_scale, ARROW_ID, control_cb);
  sb_arrow->set_float_limits(0.01,1.);

  sprintf(str, "Tube");
  GLUI_Panel *panel_tube = glui->add_panel_to_panel(panel_visual,"");
  panel_tube->set_alignment(GLUI_ALIGN_LEFT);
  GLUI_StaticText *text_tube = glui->add_statictext_to_panel(panel_tube,str);
  text_tube->set_w(50);
  glui->add_column_to_panel(panel_tube,true);
  sb_tube = new GLUI_Scrollbar(panel_tube, "Tube", GLUI_SCROLL_HORIZONTAL, &tube_scale, TUBE_ID, control_cb);
  sb_tube->set_float_limits(0.01,1.);

  sprintf(str, "Transparency");
  GLUI_Panel *panel_alpha = glui->add_panel_to_panel(panel_visual,"");
  panel_alpha->set_alignment(GLUI_ALIGN_LEFT);
  GLUI_StaticText *text_alpha = glui->add_statictext_to_panel(panel_alpha,str);
  text_alpha->set_w(50);
  glui->add_column_to_panel(panel_alpha,true);
  sb_alpha = new GLUI_Scrollbar(panel_alpha, "Transparency", GLUI_SCROLL_HORIZONTAL, &alpha, ALPHA_ID, control_cb);
  sb_alpha->set_float_limits(0.,1.);

  GLUI_EditText *text_freq = new GLUI_EditText(panel_visual, "Spin sampling", &frequency, FREQ_ID, control_cb);

 GLUI_Panel *panel_color = glui->add_panel_to_panel(panel_visual, "Color");
 panel_color->set_alignment(GLUI_ALIGN_LEFT);
 GLUI_RadioGroup *group_color = new GLUI_RadioGroup(panel_color, &colorby, COLOR_ID, control_cb);
 glui->add_radiobutton_to_group(group_color, "By cluster");
 glui->add_radiobutton_to_group(group_color, "By symmetry");
 glui->add_radiobutton_to_group(group_color, "By magnitude (power)");

}

void glui_operation(){
  GLUI_Panel *panel_picture = glui->add_panel("");
  panel_picture->set_alignment(GLUI_ALIGN_LEFT);
  text_picture = glui->add_edittext_to_panel(panel_picture, "Name", picture_name, PICTURE_TEXT_ID, control_cb);
  text_picture->set_alignment(GLUI_ALIGN_LEFT);
  glui->add_column_to_panel(panel_picture,true);
  new GLUI_Button(panel_picture, "Screenshot", PICTURE_ID, control_cb);

  GLUI_Panel *panel_config = glui->add_panel("");
  panel_config->set_alignment(GLUI_ALIGN_LEFT);
  text_config = glui->add_edittext_to_panel(panel_config, "Name", config_name, CONFIG_TEXT_ID, control_cb);
  text_config->set_alignment(GLUI_ALIGN_LEFT);
  glui->add_column_to_panel(panel_config,true);
  new GLUI_Button(panel_config, "Save config", CONFIG_ID, control_cb);
}

void glui_display()
{
  glui = GLUI_Master.create_glui_subwindow( window,
                                            GLUI_SUBWINDOW_LEFT );
  new GLUI_StaticText(glui, "Show me");
//===================Data========================================
  glui_data();
//====================ROI========================================
  glui_roi();
//============Visual=====================
  glui_visual();
//==========others====================================
  glui_operations();
//=====================================================
  new GLUI_Button(glui, "Update", UPDATE_ID, control_cb);
//=======================done====================
}
//**********************
// GLUI DONE 
//**********************


//**********************
// program entry
//**********************

int main(int argc, char** argv)
{
   image_width  = IMAGE_WIDTH;
   image_height = IMAGE_HEIGHT;
//cerr<<"start"<<endl;
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA
                          | GLUT_DEPTH | GLUT_ACCUM | GLUT_MULTISAMPLE);
//cerr<<"start"<<endl;
        glutInitWindowSize(image_width, image_height);
           glutInitWindowPosition(0, 0);
        window = glutCreateWindow("QDOT");
//cerr<<"start"<<endl; 
        if (glewInit() != GLEW_OK)
        {
                printf("glewInit failed. Exiting...\n");
                exit(1);
        }

//cerr<<"start"<<endl;
   GLUI_Master.set_glutReshapeFunc(Reshape);
   init(argv[1]);
   Init();
   glutDisplayFunc(accDisplay);

   GLUI_Master.set_glutKeyboardFunc(key);
   GLUI_Master.set_glutSpecialFunc(NULL);
   GLUI_Master.set_glutIdleFunc(idle);
   GLUI_Master.set_glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutPassiveMotionFunc(movement);  

 //  glui_display();

   glutMainLoop();
   return 0;


}
