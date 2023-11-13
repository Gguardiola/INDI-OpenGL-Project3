#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"

#include "model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  public slots:
    void cambiarHSV(int newH);
    void cambiarHSV_v(int newV);
  signals:
    void HSVchanged(int newH);

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    // mouse{Press/Release/Move}Event - Són cridades quan es realitza l'event 
    // corresponent de ratolí
    virtual void mousePressEvent (QMouseEvent *event);
    virtual void mouseReleaseEvent (QMouseEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *event);

  private:

    void creaBuffersModel(Model &model, const char *fileName, GLuint *VAO, float &escala, glm::vec3 &centreBase);
    void creaBuffersUFO();
    void creaBuffersBackground ();
    void carregaShaders ();
    void projectTransform ();
    void viewTransform ();
    void modelTransformBackground ();
    void modelTransformUFO ();
    void calculaCapsaModel (Model &m, float &escala, glm::vec3 &centreBase);
    void iniEscena ();
    void iniCamera ();
    void normalMatrixUniform();
    void currentAlpha(float newAlpha);
    void lucesTransform();


    // VAO names
    GLuint VAO_UFO, VAO_base, VAO_background ;
    // Program
    QOpenGLShaderProgram *program;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // attribute locations
    GLuint vertexLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc, NMLoc,HSVLoc,ALPHALoc,LUZLoc;
    GLint ample, alt;

    // model
    Model UFOModel,  backgroundModel;
    // paràmetres calculats a partir de la capsa contenidora del model
    glm::vec3 centreBaseUFO, centreBaseBackground;
    float escalaUFO, escalaBackground,rotUFO;

    glm::vec3 centreEsc;
    float radiEsc, ra;

    glm::vec3 UFOPos;

    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
    float angleX, angleY;

    glm::mat4 View, UFO_TG, newTG;
    //variable para HSV
    int h;
    float v = 1.0;

};

