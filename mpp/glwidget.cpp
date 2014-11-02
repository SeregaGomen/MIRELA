#include <GL/glu.h>
#include <QtGui>
#include <QtOpenGL>
#include <math.h>
#include "glwidget.h"

/*******************************************************************/
GLWidget::GLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::DoubleBuffer), parent)
{
    initParams();
    setFocusPolicy(Qt::ClickFocus);
}
/*******************************************************************/
GLWidget::~GLWidget(void)
{
    if (xList1) glDeleteLists(xList1, 1);
    if (xList2) glDeleteLists(xList2, 1);
}
/*******************************************************************/
void GLWidget::initParams(void)
{
    params.init();

    xRot =
    yRot =
    zRot =
    minX =
    maxX =
    minY =
    maxY =
    minZ =
    maxZ =
    xList1 =
    xList2 =
    radius = 0;

    diffuse = 0.8;
    ambient = 0.8;
    specular = 0.6;

    xTransl =
    yTransl =
    zTransl = 0.0;
    scale = 1.0;

    isLeftBtn =
    isRotate = true;
    isMouseDown =
    isScale =
    isTranslate = false;

}
/*******************************************************************/
void GLWidget::init(void)
{
    initParams();
    updateGL();
//    lDF.insert(0,"exp(-A*t)*sin(B*t+C)");
}
/*******************************************************************/
void GLWidget::restore(void)
{
    xRot =
    yRot =
    zRot = 0;

    diffuse = 0.8;
    ambient = 0.8;
    specular = 0.6;

    xTransl =
    yTransl =
    zTransl = 0.0;
    scale = 1.0;

    updateGL();
}
/*******************************************************************/
void GLWidget::init(ImageParams& iParams)
{
    initParams();
    params = iParams;
    updateGL();
}
/*******************************************************************/
void GLWidget::repaint(void)
{
    qglClearColor(params.bkgColor);
    if (xList1) glDeleteLists(xList1, 1);
    if (xList2) glDeleteLists(xList2, 1);
    xList1 = xList2 = 0;
    updateGL();
}
/*******************************************************************/
QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}
/*******************************************************************/
QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}
/*******************************************************************/
void GLWidget::setRotate(void)
{
    isRotate = true;
    isScale = isTranslate = false;
}
/*******************************************************************/
void GLWidget::setScale(void)
{
    isScale = true;
    isRotate = isTranslate = false;
}
/*******************************************************************/
void GLWidget::setTranslate(void)
{
    isTranslate = true;
    isScale = isRotate = false;
}
/*******************************************************************/
/*
static void qNormalizeAngle(int& angle)
{
    while (angle < 0)
        angle += 360*16;
    while (angle > 360*16)
        angle -= 360*16;
}*/
/*******************************************************************/
/*
void GLWidget::setXRotation(int angle)
{
//    qNormalizeAngle(angle);
    if (angle != xRot)
    {
        xRot = angle;
//        emit xRotationChanged(angle);
        updateGL();
    }
}*/
/*******************************************************************/
/*
void GLWidget::setYRotation(int angle)
{
//    qNormalizeAngle(angle);
    if (angle != yRot)
    {
        yRot = angle;
//        emit yRotationChanged(angle);
        updateGL();
    }
}
*/
/*******************************************************************/
/*
void GLWidget::setZRotation(int angle)
{
//    qNormalizeAngle(angle);
    if (angle != zRot)
    {
        zRot = angle;
//        emit zRotationChanged(angle);
        updateGL();
    }
}
*/
/*******************************************************************/
void GLWidget::initializeGL()
{
 //   qglClearColor(QColor::fromCmykF(0.39, 0.39, 0.0, 0.0).dark());
    qglClearColor(params.bkgColor);

    glEnable(GL_DEPTH_TEST);
    glShadeModel (GL_SMOOTH);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glEnable (GL_NORMALIZE);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
/*******************************************************************/
void GLWidget::setupLightGL(void)
{
/*    GLfloat vals[3],
            lightPosition[4] = { 0.5f*GLfloat(radius), 5.0f*GLfloat(radius), 7.0f*GLfloat(radius), 1.0f*GLfloat(radius) },
            mat_spec_col[] = { 1, 1, 1, 1 };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    vals[0] = vals[1] = vals[2] = ambient;
    glLightfv(GL_LIGHT0, GL_AMBIENT, vals);

    vals[0] = vals[1] = vals[2] = diffuse;
    glLightfv(GL_LIGHT0, GL_DIFFUSE, vals);

    vals[0] = vals[1] = vals[2] = specular;
            glLightfv(GL_LIGHT0, GL_SPECULAR, vals);

    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 64);

    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec_col);

*/

    GLfloat vals[3],
            lightdiff = 0.7,
            lightspec = 1.0,
            mat_spec_col[] = { 1, 1, 1, 1 };


    vals[0] = vals[1] = vals[2] = 0.3;
    glLightfv(GL_LIGHT0, GL_AMBIENT, vals);

    vals[0] = vals[1] = vals[2] = lightdiff;
    glLightfv(GL_LIGHT0, GL_DIFFUSE, vals);
    vals[0] = vals[1] = vals[2] = lightspec;
    glLightfv(GL_LIGHT0, GL_SPECULAR, vals);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50);
    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec_col);

    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);



}
/*******************************************************************/
void GLWidget::resizeGL(int width, int height)
{
    setupCameraGL(width, height);
    if (params.isLight)
        setupLightGL();
    else
        glEnable(GL_COLOR_MATERIAL);
}
/*******************************************************************/
//void GLWidget::mouseDoubleClickEvent(QMouseEvent *event)
//{
//    lastPos = event->pos();
//    if (event->button() & Qt::LeftButton)
//    {
////        if (event->modifiers() & Qt::ShiftModifier)
////            isShiftPressed = true;
//        isLeftBtn = true;
//        calcSelection(event->pos().x(),event->pos().y());
//    }
//    else if (event->button() & Qt::RightButton)
//    {
//        isLeftBtn = false;
//        calcSelection(event->pos().x(),event->pos().y());
//    }
//}
/*******************************************************************/
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    isMouseDown = true;
    isIdle = false;
    lastPos = event->pos();
}
/*******************************************************************/
void GLWidget::mouseReleaseEvent(QMouseEvent*)
{
    isMouseDown = false;
    isIdle = true;
//    repaint();
    update();
}
/*******************************************************************/
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();



    if ((event->buttons() & Qt::LeftButton) && (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) == false))
    {
        if (isRotate)
            setupRotate(dy,dx,0);
        else if (isScale)
            setupScale(dx, dy);
        else if (isTranslate)
            setupTranslate(dx, dy);

    }
    else if ((event->buttons() & Qt::LeftButton) && (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) == true))
        if (isRotate)
            setupRotate(dy,0,dx);
    lastPos = event->pos();
}
/*******************************************************************/
void GLWidget::setupRotate(int dx, int dy, int dz)
{
    xRot += 8*dx;
    yRot += 8*dy;
    zRot += 8*dz;
    updateGL();
}
/*******************************************************************/
void GLWidget::setupScale(int dx, int dy)
{
    if (dx > 0 || dy > 0)
        scale *= 1.05;
    else
        scale /= 1.05;
    updateGL();
}
/*******************************************************************/
void GLWidget::setupTranslate(int dx, int dy)
{
    xTransl += dx*radius*0.001;
    yTransl -= dy*radius*0.001;
    updateGL();
}
/*******************************************************************/
void GLWidget::setupCameraGL(int width, int height)
{
    double a = (height == 0) ? 1.0 : double(width)/double(height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, a, 0.01*radius, 10*radius);
    gluLookAt(0, 0, radius, 0, 0, 0, 0, 1, 0);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
}
/*******************************************************************/
void GLWidget::makeMaterial(double r,double g,double b,double a)
{
  GLfloat ambvals[] = { 0.4f, 0.4f, 0.4f, 1.0f };
  GLfloat diffvals[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat specvals[] =  { 0.7f, 0.7f, 0.7f, 1.0f };
  GLfloat val[] = { r, g, b, a };

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambvals);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffvals);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specvals);


  val[0] = r*diffuse;
  val[1] = g*diffuse;
  val[2] = b*diffuse;
  val[3] = a;
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, val);

  val[0] = r*ambient;
  val[1] = g*ambient;
  val[2] = b*ambient;
  val[3] = a;
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, val);

  val[0] = specular;
  val[1] = specular;
  val[2] = specular;
  val[3] = a;
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, val);

  val[0] = 0;
  val[1] = 0;
  val[2] = 0;
  val[3] = a;
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, val);
}
/*******************************************************************/
void GLWidget::drawCoordinateCross(void)
{
    GLint viewport[4];
    GLUquadricObj* quadObj = gluNewQuadric();


    gluQuadricDrawStyle(quadObj, GLU_FILL);

    glMatrixMode (GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode (GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glGetIntegerv (GL_VIEWPORT, viewport);

    glTranslatef (-1, -1, 0.0);
    glScalef (40.0/viewport[2], 40.0/viewport[3], 1);
    glTranslatef (1.25, 1.25, 0.0);

    glRotatef(xRot/16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot/16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot/16.0, 0.0, 0.0, 1.0);

    if (params.isLight)
    {
        glEnable (GL_COLOR_MATERIAL);
        glDisable (GL_LIGHTING);
    }

    glLineWidth (1.0f);

    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3f (0.0f, 0.0f, 0.0f);
    glVertex3f (0.9, 0.0f, 0.0f);
    glColor3f(0,1,0);
    glVertex3f (0.0f, 0.0f, 0.0f);
    glVertex3f (0.0f, 0.9, 0.0f);
    glColor3f(0,0,1);
    glVertex3f (0.0f, 0.0f, 0.0f);
    glVertex3f (0.0f, 0.0f, 0.9);
    glEnd ();

    glColor3f(1,0,0);
    glPushMatrix();
    glTranslatef(0.5,0,0);
    glRotatef(90,0,1,0);
    gluCylinder(quadObj,0.1,0,0.4,10,10);
    glPopMatrix();

    glColor3f(0,1,0);
    glPushMatrix();
    glTranslatef(0,0.5,0);
    glRotatef(-90,1,0,0);
    gluCylinder(quadObj,0.1,0,0.4,10,10);
    glPopMatrix();

    glColor3f(0,0,1);
    glPushMatrix();
    glTranslatef(0,0,0.5);
    gluCylinder(quadObj,0.1,0,0.4,10,10);
    glPopMatrix();

    glColor3f(1,0,0);
    renderText(1.1, 0.0, 0.0, (char*)"X");

    glColor3f(0,1,0);
    renderText(0.1, 1.0, 0.0, (char*)"Y");

    glColor3f(0,0,1);
    renderText(0.1, 0.0, 1.0, (char*)"Z");

    if (params.isLight)
    {
        glEnable(GL_LIGHTING);
        glDisable (GL_COLOR_MATERIAL);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    gluDeleteQuadric(quadObj);
}
/*******************************************************************/
//void GLWidget::draw2DText(double x, double y, double z, char* text)
//{
//    GLdouble winx, winy; // Место для координат проекции
//    GLint viewport[4]; // Место для данных окна просмотра

//    glGetIntegerv(GL_VIEWPORT, viewport); // Извлечь текущее окно просмотра
//    // Определим 2D позиции 3D точки
//    getScrCoord(x, y, z, winx, winy);

//    renderText(winx, viewport[3] - winy, text);
//}
/*******************************************************************/
void GLWidget::getScrCoord(double x, double y, double z, double& scrX, double& scrY)
{
    GLint viewport[4]; // Место для данных окна просмотра
    GLdouble mvmatrix[16],
             projmatrix[16]; // Место для матрицы трансформации
    GLdouble winx, winy, winz; // Место для координат проекции

    glGetIntegerv(GL_VIEWPORT, viewport); // Извлечь текущее окно просмотра
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix); // Извлечь текущую матрицу просмотра
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix); // Извлечь матрицу проекции

    // Определим 2D позиции 3D точки
    gluProject(x, y, z, mvmatrix, projmatrix, viewport, &winx, &winy, &winz);

    scrX = winx;
    scrY = winy;
}
/*******************************************************************/
void GLWidget::getWrldCoord(double x, double y, double& wrldX, double& wrldY, double& wrldZ)
{
    GLint viewport[4]; // Место для данных окна просмотра
    GLdouble mvmatrix[16],
             projmatrix[16]; // Место для матрицы трансформации
    GLdouble winx, winy, winz; // Место для координат проекции

    glGetIntegerv(GL_VIEWPORT, viewport); // Извлечь текущее окно просмотра
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix); // Извлечь текущую матрицу просмотра
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix); // Извлечь матрицу проекции

    // Определим 2D позиции 3D точки
    gluUnProject(x, y, 0, mvmatrix, projmatrix, viewport, &winx, &winy, &winz);

    wrldX = winx;
    wrldY = winy;
    wrldZ = winz;
}
/*******************************************************************/
void GLWidget::setColor(double r, double g, double b, double a)
{
    if (params.isLight)
        makeMaterial(r,g,b,a);
    else
        glColor4f(r,g,b,a);
}
/*******************************************************************/
void GLWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_F5)
        repaint();
}
/*******************************************************************/
