#ifndef GLFUN_H
#define GLFUN_H

#include <GL/glu.h>
#include <QMenu>
#include "glwidget.h"
#include "matrix.h"

class TResultList;

// Описание цвета
class GLColor
{
private:
    double r;
    double g;
    double b;
    double v;
public:
     GLColor(void)
     {
         r = g = b = v = 0;
     }
     GLColor(double pr, double pg, double pb, double pv)
     {
         r = pr;
         g = pg;
         b = pb;
         v = pv;
     }
    ~GLColor(void) {}
     void operator = (GLColor& p)
     {
         r = p.r;
         g = p.g;
         b = p.b;
         v = p.v;
     }
     double red(void)
     {
         return r;
     }
     double green(void)
     {
         return g;
     }
     double blue(void)
     {
         return b;
     }
     double value(void)
     {
        return v;
     }
     void setRed(double pr)
     {
         r = pr;
     }
     void setGreen(double pg)
     {
         g = pg;
     }
     void setBlue(double pb)
     {
         b = pb;
     }
     void setValue(double pv)
     {
         v = pv;
     }
     void setRGB(double pr, double pg, double pb)
     {
         r = pr;
         g = pg;
         b = pb;
     }
     void setNegative(void)
     {
         r = 1.0 - r;
         g = 1.0 - g;
         b = 1.0 - b;
     }
};
//---------------------------------------------------------------------
class GLFunWidget : public GLWidget
{
    Q_OBJECT
private:
    matrix<double> Normal;
    vector<double>* x; // Геометрия расчета
    vector<double>* y;
    vector<double>* z;
    matrix<int>* surface;
    TResultList* results; // Результаты расчета
    unsigned funIndex; // Номер функции в списке
    GLColor colorTable[512];
    QString predicate;
    QMenu menu; // Контекстное меню
    QAction* action1;
    double uMin;
    double uMax;
    unsigned ind[4]; // Порядок обхода вершин четырехугольника

    void displayObject(void);
    void displaySceleton(void);
    void createFunObject(void);
    void createSceleton(void);
    void drawFun_3D(void);
    void createNormal(void);
    void calcSelection(int,int);
    void drawFacet23D(TVertex3D*,int);
    void drawTriangle3D(TVertex3D*);
    void triangle1(TVertex3D&,TVertex3D&,TVertex3D&);
    void triangle2(TVertex3D&,TVertex3D&,TVertex3D&);
    void initColorTable(void);
    void paintScale(void);
    void processPredicate(void);
    void drawPredicate(void);
    int getColorIndex(double);
    double cX(unsigned);
    double cY(unsigned);
    double cZ(unsigned);
protected:
    void paintGL(void);

public slots:
    void showContextMenu(const QPoint&);
    void mouseDoubleClickEvent(QMouseEvent*);

public:
    GLFunWidget(vector<double>*,vector<double>*,vector<double>*,matrix<int>*,TResultList*,unsigned,QWidget* parent = 0);
    ~GLFunWidget(void)
    {
        delete action1;
    }
    void set(vector<double>*,vector<double>*,vector<double>*,matrix<int>*,TResultList*,unsigned);
    void clearObject(void)
    {
        x = y = z = NULL;
        surface = NULL;
        results = NULL;
    }
};
//---------------------------------------------------------------------


#endif // GLFUN_H
