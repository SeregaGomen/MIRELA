#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "imageparams.h"


class SetupTaskDialog;


//---------------------------------------------------------------------
// Описание вершины для визуализации
class TVertex3D
{
public:
    double x;  // Координаты вершины
    double y;
    double z;
    double nx; // Компоненты вектора нормали
    double ny;
    double nz;
    double u;  // Значение функции в вершине
    int c;     // Индекс цвета, соответствующий вершине
    TVertex3D(void)
    {
        x = y = z = nx = ny = nz = u = 0.0;
        c = 0;
    }
    TVertex3D(const TVertex3D& r)
    {
        x = r.x;
        y = r.y;
        z = r.z;
        nx = r.nx;
        ny = r.ny;
        nz = r.nz;
        u = r.u;
        c = r.c;
    }
   ~TVertex3D(void) {}
    TVertex3D& operator = (const TVertex3D& r)
    {
        x = r.x;
        y = r.y;
        z = r.z;
        nx = r.nx;
        ny = r.ny;
        nz = r.nz;
        u = r.u;
        c = r.c;
        return *this;
    }
    void setVertex(void)
    {
        glNormal3d(nx,ny,nz);
        glVertex3d(x,y,z);
    }
    void invertNormal(void)
    {
        nx *= -1;
        ny *= -1;
        nz *= -1;
    }
};
//---------------------------------------------------------------------
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget* parent = 0);
    ~GLWidget(void);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void init(void);
    void init(ImageParams&);
    void restore(void);
    void repaint(void);
    void setImageParams(ImageParams& r)
    {
        params = r;
    }
    void setColor(double r,double g,double b,double a = 1);
    bool getRotate(void)
    {
        return isRotate;
    }
    bool getScale(void)
    {
        return isScale;
    }
    bool getTranslate(void)
    {
        return isTranslate;
    }
    ImageParams& getImageParams(void)
    {
        return params;
    }

public slots:
//    void setXRotation(int angle);
//    void setYRotation(int angle);
//    void setZRotation(int angle);
    void setRotate(void);
    void setScale(void);
    void setTranslate(void);

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

protected:
    ImageParams params;

    int xList1;
    int xList2;
    int xRot;
    int yRot;
    int zRot;
    bool isRotate;
    bool isScale;
    bool isTranslate;
    bool isLeftBtn;
    bool isIdle;
    bool isMouseDown;
    double scale;
    double xTransl;
    double yTransl;
    double zTransl;
    double maxX;
    double minX;
    double maxY;
    double minY;
    double maxZ;
    double minZ;
    double radius;

    void initializeGL(void);
    virtual void paintGL(void) = 0;
    void resizeGL(int,int);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);
//    void mousedoubleClickEvent(QMouseEvent*);
    void setupCameraGL(int,int);
    void setupLightGL(void);
    void drawCoordinateCross(void);
    void makeMaterial(double,double,double,double);

private:
    QPoint lastPos;

    double diffuse;
    double ambient;
    double specular;

//    void draw2DText(double,double,double,char*);
    void setupScale(int,int);
    void setupTranslate(int,int);
    void setupRotate(int,int,int);
    void getScrCoord(double,double,double,double&,double&);
    void getWrldCoord(double,double,double&,double&,double&);
    void initParams(void);
    virtual void calcSelection(int,int) = 0;
};

#endif
