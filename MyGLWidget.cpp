#include "MyGLWidget.h"
#include <glm/gtx/color_space.hpp>
#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  DoingInteractive = NONE;
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  carregaShaders();
  iniEscena ();
  iniCamera ();

}


void MyGLWidget::iniEscena ()
{
  creaBuffersUFO();
  creaBuffersBackground();

  UFOPos = glm::vec3(0.0, 0.0, 0.0);
  centreEsc = glm::vec3 (0, 0, 0);
  radiEsc = 12;

}

void MyGLWidget::iniCamera ()
{
  ra = 1.0;
  angleY = 0.0;
  angleX = 0.2;

  projectTransform ();
  viewTransform ();
  lucesTransform();
}

void MyGLWidget::paintGL () 
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#endif
    //--------------------------------------------------------
    // En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
    // useu els paràmetres que considereu (els que hi ha són els de per defecte)
    //  glViewport (0, 0, ample, alt);

    //--------------------------------------------------------
    // Esborrem el frame-buffer i el depth-buffer
    glClearColor(0.4f, 0.5f, 0.6f, 1.f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //--------------------------------------------------------
    // Enviem matrius
    projectTransform ();
    viewTransform ();
    lucesTransform();

    //--------------------------------------------------------


    //--------------------------------------------------------
    // Activem el VAO per a pintar l'UFO
    glBlendFunc(GL_ONE,GL_ZERO);
    glBindVertexArray (VAO_UFO);
    modelTransformUFO();
    normalMatrixUniform();
    currentAlpha(1);
    glDrawArrays(GL_TRIANGLES, 0, UFOModel.faces().size()*3);
    //------------------------------------------------------------
    // Activem el VAO per a pintar la base
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray (VAO_background);
    modelTransformBackground ();
    normalMatrixUniform();
    currentAlpha(0.75);
    glDrawArrays(GL_TRIANGLES, 0, backgroundModel.faces().size()*3);
    //-----

    glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
  ample = w;
  alt = h;
  projectTransform ();
}



void MyGLWidget::modelTransformBackground ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);

  newTG = TG;
}

void MyGLWidget::modelTransformUFO ()
{
  // Codi per a la TG necessària
  float alcada = 3;
  UFO_TG =glm::mat4(1.0f);
  UFO_TG = glm::translate(UFO_TG, UFOPos);
  UFO_TG = glm::rotate(UFO_TG,glm::radians(rotUFO),glm::vec3(0.0,1.0,0.0));
  UFO_TG = glm::scale(UFO_TG, alcada*glm::vec3(escalaUFO, escalaUFO, escalaUFO));
  UFO_TG = glm::translate(UFO_TG, -centreBaseUFO);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &UFO_TG[0][0]);
  newTG = UFO_TG;
}

void MyGLWidget::lucesTransform()
{
  // Matriu de transformació de LUCES

  glm::vec4 totalLuces[8];

  for(int i = 0; i<8 ;i++){
      glm::mat4 TG(1.0f);
      totalLuces[i] = glm::vec4(0.0,0.0,0.0,1.0);
      TG = glm::translate(TG,UFOPos);
      TG = glm::translate(TG, glm::vec3(0.0,3.1,0.0));
      TG = glm::rotate(TG,glm::radians(rotUFO),glm::vec3(0.0,1.0,0.0));
      TG = glm::rotate(TG,glm::radians(45.0f*i),glm::vec3(0.0,1.0,0.0));
      TG = glm::scale(TG, glm::vec3(3*escalaUFO));
      TG = glm::translate(TG, glm::vec3(-1.74,0.0,0.0));

      totalLuces[i] = TG * totalLuces[i];

  }
  glUniform4fv(LUZLoc, 8, &totalLuces[0].x);
}



void MyGLWidget::projectTransform ()
{
  float fov, zn, zf;
  glm::mat4 Proj;  // Matriu de projecció

  fov = float(M_PI/3.0);
  zn = radiEsc*0.25;
  zf = 3*radiEsc;

  float rav =  width() /  (float)height() ;
  if(rav<ra){
      fov= 2*atan(tan(0.5*fov)/rav);
  }

  Proj = glm::perspective(fov, rav, zn, zf);

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}


void MyGLWidget::viewTransform ()
{
  // Matriu de posició i orientació
  View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -2*radiEsc));
  View = glm::rotate(View, angleX/*(float) M_PI/10.0f*/, glm::vec3(1, 0, 0));
  View = glm::rotate(View, -angleY, glm::vec3(0, 1, 0));
  View = glm::translate(View, -centreEsc);

  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}


void MyGLWidget::keyPressEvent(QKeyEvent* event)  // Cal modificar aquesta funció...
{
    makeCurrent();
    switch (event->key()) {       
        case Qt::Key_S: {
            UFOPos -= glm::vec3(0.f,0.2f,0.0f);break;
        }
        case Qt::Key_W: {
            UFOPos += glm::vec3(0.f,0.2f,0.0f);break;
        }
        case Qt::Key_A: {
            UFOPos -= glm::vec3(0.2f,0.0f,0.0f);break;
        }
        case Qt::Key_D: {
            UFOPos += glm::vec3(0.2f,0.0f,0.0f);break;
        }
        case Qt::Key_R: {
            rotUFO += 1.0;break;
        }
        case Qt::Key_H:{
            if(h < 360) h++;
            else if(h == 360) h = 0;
            emit HSVchanged(h);
        }

        default: event->ignore(); break;
    }
    update();
}

void MyGLWidget::cambiarHSV(int newH){
    makeCurrent();
    h = newH;
    std::cout<<"HSV slot h: "<<h<<std::endl;
    update();
}
void MyGLWidget::cambiarHSV_v(int newV){
    makeCurrent();
    v = float(newV)/100;
    std::cout<<"HSV slot v: "<<v<<std::endl;
    update();

}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
    angleY += (e->x() - xClick) * M_PI / 180.0;    
    angleX += (e->y() - yClick) * M_PI / 180.0;
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}

void MyGLWidget::calculaCapsaModel (Model &m, float &escala, glm::vec3 &centreBase)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = m.vertices()[0];
  miny = maxy = m.vertices()[1];
  minz = maxz = m.vertices()[2];
  for (unsigned int i = 3; i < m.vertices().size(); i+=3)
  {
    if (m.vertices()[i+0] < minx)
      minx = m.vertices()[i+0];
    if (m.vertices()[i+0] > maxx)
      maxx = m.vertices()[i+0];
    if (m.vertices()[i+1] < miny)
      miny = m.vertices()[i+1];
    if (m.vertices()[i+1] > maxy)
      maxy = m.vertices()[i+1];
    if (m.vertices()[i+2] < minz)
      minz = m.vertices()[i+2];
    if (m.vertices()[i+2] > maxz)
      maxz = m.vertices()[i+2];
  }
  escala = 1.0/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::creaBuffersBackground(){
    creaBuffersModel(backgroundModel,"./models/background.obj", &VAO_background, escalaBackground, centreBaseBackground);
}


void MyGLWidget::creaBuffersUFO(){
  creaBuffersModel(UFOModel,"./models/ufo.obj", &VAO_UFO, escalaUFO, centreBaseUFO);
}

void MyGLWidget::creaBuffersModel(Model &model, const char *fileName, GLuint *VAO,  float &escala, glm::vec3 &centreBase )
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  model.load(fileName);

  // Calculem la capsa contenidora del model
  calculaCapsaModel(model, escala, centreBase);
  
  // Creació del Vertex Array Object del model
  glGenVertexArrays(1, VAO);
  glBindVertexArray(*VAO);

  // Creació dels buffers del model
  GLuint VBO_Model[6];
  // Buffer de posicions
  glGenBuffers(6, VBO_Model);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3, model.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);

}

void MyGLWidget::normalMatrixUniform(){
    glm::mat3 NormalMatrix = glm::inverseTranspose(glm::mat3(View*newTG));
    glm::vec3 HSVColor = glm::rgbColor(glm::vec3(h,1.0,v));
    glUniformMatrix3fv(NMLoc, 1, GL_FALSE, &NormalMatrix[0][0]);
    glUniform3fv(HSVLoc,1,&HSVColor[0]);
}

void MyGLWidget::currentAlpha(float newAlpha){
    glUniform1f(ALPHALoc,newAlpha);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("./shaders/basicLlumShader.frag");
  vs.compileSourceFile("./shaders/basicLlumShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // ===================================================================
  // Vèrtexs, normals i coeficients dels materials
  // ===================================================================
  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “normal” del vertex shader
  normalLoc = glGetAttribLocation (program->programId(), "normal");
  // Obtenim identificador per a l'atribut “matamb” del vertex shader
  matambLoc = glGetAttribLocation (program->programId(), "matamb");
  // Obtenim identificador per a l'atribut “matdiff” del vertex shader
  matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
  // Obtenim identificador per a l'atribut “matspec” del vertex shader
  matspecLoc = glGetAttribLocation (program->programId(), "matspec");
  // Obtenim identificador per a l'atribut “matshin” del vertex shader
  matshinLoc = glGetAttribLocation (program->programId(), "matshin");

  // ===================================================================
  // Matrius
  // ===================================================================
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");
  //nuevas uniforms
  NMLoc = glGetUniformLocation (program->programId(), "NormalMatrix");
  HSVLoc = glGetUniformLocation (program->programId(), "HSVColor");
  ALPHALoc = glGetUniformLocation (program->programId(), "newAlpha");
  LUZLoc = glGetUniformLocation (program->programId(), "luces");

}

