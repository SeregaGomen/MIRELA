#include <QAbstractScrollArea>
#include <QInputDialog>
#include <QToolTip>
#include <QMouseEvent>
#include <math.h>
#include "glfun.h"
#include "analyse.h"

extern double sqr(double x);

/*******************************************************************/
GLFunWidget::GLFunWidget(vector<double>* px,vector<double>* py,vector<double>* pz,matrix<int>* ps,TResultList* r, unsigned i,QWidget *parent) : GLWidget(parent)
{
    x = px;
    y = py;
    z = pz;
    surface = ps;
    results = r;
    funIndex = i;

    isIdle =
    isRotate = true;
    isScale =
    isTranslate = false;

    xRot =
    yRot =
    zRot = 0;

    minX = *min_element((*x).begin(),(*x).end());
    maxX = *max_element((*x).begin(),(*x).end());
    minY = *min_element((*y).begin(),(*y).end());
    maxY = *max_element((*y).begin(),(*y).end());
    minZ = *min_element((*z).begin(),(*z).end());
    maxZ = *max_element((*z).begin(),(*z).end());
    radius = sqrt((maxX - minX)*(maxX - minX) + (maxY - minY)*(maxY - minY)  + (maxZ - minZ)*(maxZ - minZ));

    initColorTable();
    if (z->size())
        createNormal();

    for (unsigned i = 0; i < 4; i++)
        ind[i] = i;

    setContextMenuPolicy(Qt::CustomContextMenu);
    action1 = new QAction(QString::fromUtf8("Условие отбора узлов"), this);
    menu.addAction(action1);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(showContextMenu(QPoint)));
}
/*******************************************************************/
void GLFunWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (params.isQuality)
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0,1.0);
    }
    else
        glDisable(GL_POLYGON_OFFSET_FILL);
    if (params.isLight)
    {
        glEnable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
    }
    else
    {
        glDisable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
    }

    glScalef(scale, scale, scale);
    glTranslatef(xTransl, yTransl, zTransl);
    glRotatef(xRot/16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot/16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot/16.0, 0.0, 0.0, 1.0);

    if (!x)
        return;
    if (isIdle)
        displayObject();
    else
        displaySceleton();
    if (params.isCoord)
        drawCoordinateCross();
}
/*******************************************************************/
void GLFunWidget::displayObject(void)
{
    if (!xList1)
        createFunObject();
    glCallList(xList1);
    if (params.isValueScale)
        paintScale();
    if (params.isCoord)
        drawCoordinateCross();
}
/*******************************************************************/
void GLFunWidget::displaySceleton(void)
{
    if (!xList2)
        createSceleton();
    glCallList(xList2);
}
/*******************************************************************/
void GLFunWidget::createFunObject(void)
{
    initColorTable();

    xList1 = glGenLists(1);
    setupCameraGL(width(),height());
    glNewList (xList1, GL_COMPILE);

    drawFun_3D();
    if (predicate.length()/* && params.isVertex*/)
        drawPredicate();
    glEndList();
}
/*******************************************************************/
void GLFunWidget::drawFun_3D(void)
{
    TVertex3D* p = new TVertex3D[surface->size2()];
    double x0 = (maxX + minX)*0.5,
           y0 = (maxY + minY)*0.5,
           z0 = (maxZ + minZ)*0.5;

    for (unsigned i = 0; i < surface->size1(); i++)
    {
        for (unsigned j = 0; j < surface->size2(); j++)
        {
            p[j].x = cX((*surface)[i][j]) - x0;
            p[j].y = cY((*surface)[i][j]) - y0;
            p[j].z = cZ((*surface)[i][j]) - z0;
            p[j].u = (*results)[funIndex].getResults((*surface)[i][j]);
            p[j].nx = Normal[i][0];
            p[j].ny = Normal[i][1];
            p[j].nz = Normal[i][2];
        }
        drawFacet23D(p,surface->size2());
    }
    delete [] p;
}
/*******************************************************************/
void GLFunWidget::drawFacet23D(TVertex3D* vertex,int size)
{
    TVertex3D vtx[3];

    if (params.isFace)
    {
            // Рисование четырехугольника
            vtx[0] = vertex[ind[0]];
            vtx[1] = vertex[ind[1]];
            vtx[2] = vertex[ind[3]];
            drawTriangle3D(vtx);
            vtx[0] = vertex[ind[0]];
            vtx[1] = vertex[ind[2]];
            vtx[2] = vertex[ind[3]];
            drawTriangle3D(vtx);
    }

    if (params.isMesh)
    {
        setColor(0,0,0,params.alpha);
        glBegin(GL_LINE_LOOP);
            glVertex3d(vertex[0].x, vertex[0].y, vertex[0].z);
            glVertex3d(vertex[1].x, vertex[1].y, vertex[1].z);
            glVertex3d(vertex[2].x, vertex[2].y, vertex[2].z);
            glVertex3d(vertex[3].x, vertex[3].y, vertex[3].z);
        glEnd();

    }
    if (params.isVertex)
    {
        glPointSize(3);
        glBegin(GL_POINTS);
        setColor(0,0,0,params.alpha);
        for (int i = 0; i < size; i++)
            glVertex3d(vertex[i].x, vertex[i].y, vertex[i].z);
        glEnd();
    }
}
/*******************************************************************/
void GLFunWidget::calcSelection(int /*x*/, int /*y*/)
{

}
/*******************************************************************/
void GLFunWidget::initColorTable(void)
{
    double h,
          red   = 0,
          green = 0,
          blue  = 1,
          u,
          value;


    uMin = *min_element((*results)[funIndex].getResults().begin(),(*results)[funIndex].getResults().end());
    uMax = *max_element((*results)[funIndex].getResults().begin(),(*results)[funIndex].getResults().end());
    u    = max(fabs(uMin),fabs(uMax));
    h    = 1.0/double(params.numColor);

    if (params.isRGB)
    {
        uMin = -u;
        uMax =  u;

        if (uMin == uMax)
        {
            uMin -= 1;
            uMax += 1;
        }
        red = 0.0;
        green = 0.0;
        blue = 1.0;
        value = uMin;
        for (int i = 0; i < params.numColor; i++)
        {
            colorTable[i].setRGB(red,green,blue);
            colorTable[i].setValue(value);
            if (i < params.numColor*0.5)
            {
                blue -= 2.0*h;
                green += 2.0*h;
            }
            else
            {
                red += 2.0*h;
                green -= 2.0*h;
                blue = 0;
            }
            value += (uMax - uMin)*h;
        }
    }
    if (params.isBW)
    {
        if (uMax < 0)
            uMax = 0;
        else if (uMin > 0)
            uMin = 0;

        // От черного к белому
        red   = 0.0;
        green = 0.0;
        blue  = 0.0;
        value = 0.0;
        for (int i = 0; i < params.numColor - 1; i++)
        {
            colorTable[i].setRGB(red,green,blue);
            colorTable[i].setValue(value);
            red += h;
            green += h;
            blue += h;
            value += (uMax - uMin)*h;
        }
        colorTable[params.numColor - 1].setRGB(1,1,1);
    }
    if (params.isNegative)
        for (int i = 0; i < params.numColor; i++)
            colorTable[i].setNegative();
}
/*******************************************************************/
void GLFunWidget::paintScale(void)
{
    QRect rc = rect();
    QFont glFont("Courier", 12, QFont::Normal);
    QFontMetrics fm(glFont);
    QString val;
    int num = 8,
        fontW1 = fm.width("12"),
        fontW2 = fm.width("1234567890"),
        fontH = fm.height(),
        i;
    double minU = 0,
           maxU = 0,
           v_start,
           v_stop,
           v_step,
           v,
           cy;

     minU = *min_element((*results)[funIndex].getResults().begin(),(*results)[funIndex].getResults().end());
     maxU = *max_element((*results)[funIndex].getResults().begin(),(*results)[funIndex].getResults().end());

    if (rc.height() < 9*fontH) return;

    v_start = maxU;
    v_stop  = minU;
    v_step  = (maxU - minU)/(num - 1);
    cy      = rc.top() + 20;

    v = v_start;
    if (minU == maxU)
    {
        i = getColorIndex(v);
        glColor3f(colorTable[i].red(),colorTable[i].green(),colorTable[i].blue());
        glFont.setStyleStrategy(QFont::OpenGLCompatible);
        renderText(rc.width() - fontW1 - fontW2 - 10,cy,"█",glFont);
        glColor3f(0,0,0);
//        renderText(rc.width() - fontW2 - 10,cy,QString("%1").arg(v,5,'e',3),glFont);
        renderText(rc.width() - fontW2 - 10,cy,val.sprintf("%+5.3E",v),glFont);


        return;
    }

    for (int k = 0; k < 8; k++)
     {
            if (k == 7) v = v_stop;

            i = getColorIndex(v);
            glColor3f(colorTable[i].red(),colorTable[i].green(),colorTable[i].blue());
            glFont.setStyleStrategy(QFont::OpenGLCompatible);
            renderText(rc.width() - fontW1 - fontW2 - 10,cy,"█",glFont);
            glColor3f(0,0,0);
            renderText(rc.width() - fontW2 - 10,cy,val.sprintf("%+5.3E",v),glFont);

            cy += fontH;
            v -= v_step;
     }
}
/*******************************************************************/
void swap(TVertex3D& v1,TVertex3D& v2)
{
    TVertex3D t = v1;

    v1 = v2;
    v2 = t;
}
/*******************************************************************/
void sort(TVertex3D* vtx)
{
    int minI = 0;
    double minU = vtx[0].u;

    // Определение позиции минимального элемента
    for (int i = 1; i < 3; i++)
        if (vtx[i].u < minU)
        {
            minI = i;
            minU = vtx[i].u;
        }
    // Циклически помещаем минимальный элемент в начало массива
    for (int i = 0; i < minI; i++)
        std::rotate(vtx,vtx + 1,vtx + 3);

}
/*******************************************************************/
void createVertex(TVertex3D& p, TVertex3D& p0, TVertex3D& p1, double work)
{
    double C = p1.u - p0.u,
           A = (work - p0.u)/C,
           B = (p1.u - work)/C;

    p.x = A*p1.x + B*p0.x;
    p.y = A*p1.y + B*p0.y;
    p.z = A*p1.z + B*p0.z;
    p.u = work;
    p.nx = p1.nx + p0.nx;
    p.ny = p1.ny + p0.ny;
    p.nz = p1.nz + p0.nz;
}
/*******************************************************************/
void GLFunWidget::drawTriangle3D(TVertex3D* vertex)
{
    TVertex3D tmp;

    if (predicate.length())
    {
        if (fabs(vertex[0].u) < predicate.toFloat())
          vertex[0].u = 0;
        if (fabs(vertex[1].u) < predicate.toFloat())
          vertex[1].u = 0;
        if (fabs(vertex[2].u) < predicate.toFloat())
          vertex[2].u = 0;
    }
    vertex[0].c = getColorIndex(vertex[0].u);
    vertex[1].c = getColorIndex(vertex[1].u);
    vertex[2].c = getColorIndex(vertex[2].u);
    if ((vertex[0].c == vertex[1].c) && (vertex[1].c == vertex[2].c))
    {
        glBegin(GL_TRIANGLES);
            setColor(colorTable[vertex[0].c].red(),colorTable[vertex[0].c].green(),colorTable[vertex[0].c].blue(),params.alpha);
            vertex[0].setVertex();
            vertex[1].setVertex();
            vertex[2].setVertex();
        glEnd();
        return;
    }
    sort(vertex);
    if (vertex[2].u < vertex[1].u)
    {
        swap(vertex[2],vertex[1]);
        vertex[0].invertNormal();
        vertex[1].invertNormal();
        vertex[2].invertNormal();
    }
    if (vertex[0].c != vertex[1].c)
    {
        if (vertex[1].c != vertex[2].c)
        {
            createVertex(tmp,vertex[2],vertex[0],vertex[1].u);
            triangle1(vertex[0],tmp, vertex[1]);
            triangle2(tmp,vertex[1],vertex[2]);
        }
        else
            triangle1(vertex[0], vertex[2], vertex[1]);
    }
    else if (vertex[1].c != vertex[2].c)
        triangle2(vertex[0], vertex[1], vertex[2]);
}
/*******************************************************************/
void GLFunWidget::triangle1(TVertex3D& p0,TVertex3D& p1,TVertex3D& p2)
{
    int color = getColorIndex(p0.u),
        lastColor = getColorIndex(p2.u);
    double a,
           b,
           c01,
           c02,
           tmp,
           cx01,
           cy01,
           cz01,
           cx02,
           cy02,
           cz02,
           step = (uMax - uMin)/params.numColor,
           work = (uMin + (step)*(color + 1));
    TVertex3D t1,
              t2;

    c01 = p0.u - p1.u;
    a   = (work - p1.u)/c01;
    b   = (p0.u - work)/c01;

    t1.x = a*p0.x + b*p1.x;
    t1.y = a*p0.y + b*p1.y;
    t1.z = a*p0.z + b*p1.z;
    t1.nx = p0.nx + p1.nx;
    t1.ny = p0.ny + p1.ny;
    t1.nz = p0.nz + p1.nz;

    c02 = p0.u - p2.u;
    a   = (work - p2.u)/c02;
    b   = (p0.u - work)/c02;

    t2.x = a*p0.x + b*p2.x;
    t2.y = a*p0.y + b*p2.y;
    t2.z = a*p0.z + b*p2.z;
    t2.nx = p0.nx + p2.nx;
    t2.ny = p0.ny + p2.ny;
    t2.nz = p0.nz + p2.nz;

    glBegin(GL_TRIANGLES);
        setColor(colorTable[color].red(),colorTable[color].green(),colorTable[color].blue(),params.alpha);
        p0.setVertex();
        t2.setVertex();
        t1.setVertex();
    glEnd();
    color++;

    glBegin(GL_QUADS);
        if (color < lastColor)
        {
            tmp  = step/c01;
            cx01 = (p0.x - p1.x)*tmp;
            cy01 = (p0.y - p1.y)*tmp;
            cz01 = (p0.z - p1.z)*tmp;

            tmp  = step/c02;
            cx02 = (p0.x - p2.x)*tmp;
            cy02 = (p0.y - p2.y)*tmp;
            cz02 = (p0.z - p2.z)*tmp;
            do
            {
                setColor(colorTable[color].red(),colorTable[color].green(),colorTable[color].blue(),params.alpha);
                t1.setVertex();
                t2.setVertex();

                t1.x += cx01;
                t1.y += cy01;
                t1.z += cz01;
                t2.x += cx02;
                t2.y += cy02;
                t2.z += cz02;

                t2.setVertex();
                t1.setVertex();
                color++;
            }
            while (color < lastColor);
        }
        setColor(colorTable[color].red(),colorTable[color].green(),colorTable[color].blue(),params.alpha);
        t1.setVertex();
        t2.setVertex();
        p2.setVertex();
        p1.setVertex();
    glEnd();
}
/*******************************************************************/
void GLFunWidget::triangle2(TVertex3D& p0,TVertex3D& p1,TVertex3D& p2)
{
    int color = getColorIndex(p0.u),
        lastColor = getColorIndex(p2.u);
    double a,
          b,
          c20,
          c21,
          tmp,
          cx20,
          cy20,
          cz20,
          cx21,
          cy21,
          cz21,
          step = (uMax - uMin)/params.numColor,
          work = (uMin + (step)*(color + 1));
    TVertex3D t0,
              t1;

    c20 = p2.u - p0.u;
    a   = (work - p0.u)/c20;
    b   = (p2.u - work)/c20;

    t0.x = a*p2.x + b*p0.x;
    t0.y = a*p2.y + b*p0.y;
    t0.z = a*p2.z + b*p0.z;
    t0.nx = p0.nx + p2.nx;
    t0.ny = p0.ny + p2.ny;
    t0.nz = p0.nz + p2.nz;


    c21 = p2.u - p1.u;
    a   = (work - p1.u)/c21;
    b   = (p2.u - work)/c21;

    t1.x = a*p2.x + b*p1.x;
    t1.y = a*p2.y + b*p1.y;
    t1.z = a*p2.z + b*p1.z;
    t1.nx = p2.nx + p1.nx;
    t1.ny = p2.ny + p1.ny;
    t1.nz = p2.nz + p1.nz;


    glBegin(GL_QUADS);
        setColor(colorTable[color].red(),colorTable[color].green(),colorTable[color].blue(),params.alpha);
        p0.setVertex();
        p1.setVertex();
        t1.setVertex();
        t0.setVertex();
        color++;
        if (color < lastColor)
        {
            tmp  = step/c20;
            cx20 = (p2.x - p0.x)*tmp;
            cy20 = (p2.y - p0.y)*tmp;
            cz20 = (p2.z - p0.z)*tmp;

            tmp  = step/c21;
            cx21 = (p2.x - p1.x)*tmp;
            cy21 = (p2.y - p1.y)*tmp;
            cz21 = (p2.z - p1.z)*tmp;
            do
            {
                setColor(colorTable[color].red(),colorTable[color].green(),colorTable[color].blue(),params.alpha);
                t0.setVertex();
                t1.setVertex();

                t0.x += cx20;
                t0.y += cy20;
                t0.z += cz20;
                t1.x += cx21;
                t1.y += cy21;
                t1.z += cz21;

                t1.setVertex();
                t0.setVertex();
                color++;
             }
             while (color < lastColor);
         }
     glEnd();
     glBegin(GL_TRIANGLES);
         setColor(colorTable[color].red(),colorTable[color].green(),colorTable[color].blue(),params.alpha);
         t0.setVertex();
         t1.setVertex();
         p2.setVertex();
     glEnd();
}
/*******************************************************************/
int GLFunWidget::getColorIndex(double u)
{
  int ret = int(floor((u - uMin)/((uMax - uMin)/double(params.numColor))));

  if (ret > params.numColor - 1)
      ret = params.numColor - 1;
  else if (ret < 0)
      ret = 0;
  return ret;
}
/*******************************************************************/
void GLFunWidget::createNormal(void)
{
    vector<double> &X = *x,
                   &Y = *y,
                   &Z = *z;
    matrix<int>& Faces = *surface;
    unsigned NumFaces = Faces.size1();
    double x[3],
           y[3],
           z[3],
           A,
           B,
           C,
           D,
           maxLen;

    Normal.resize(NumFaces,3);
    for (unsigned i = 0; i < NumFaces; i++)
    {
        for (unsigned j = 0; j < 3; j++)
        {
            x[j] = X[Faces[i][j]];
            y[j] = Y[Faces[i][j]];
            z[j] = Z[Faces[i][j]];
        }
        A  = (y[1] - y[0])*(z[2] - z[0]) - (y[2] - y[0])*(z[1] - z[0]);
        B  = (x[2] - x[0])*(z[1] - z[0]) - (x[1] - x[0])*(z[2] - z[0]);
        C  = (x[1] - x[0])*(y[2] - y[0]) - (x[2] - x[0])*(y[1] - y[0]);
        D  = sqrt(A*A + B*B + C*C);

        if (D == 0) D = 1;

        Normal[i][0] += A/D;
        Normal[i][1] += B/D;
        Normal[i][2] += C/D;
    }
    maxLen =*max_element(Normal.asVector().begin(),Normal.asVector().end());
    for (unsigned i = 0; i < Normal.size1(); i++)
        for (unsigned j = 0; j < Normal.size2(); j++)
            Normal[i][j] /= maxLen;
}
/*******************************************************************/
void GLFunWidget::createSceleton(void)
{
    matrix<int>& Faces = *surface;
    double x0 = (maxX + minX)*0.5,
           y0 = (maxY + minY)*0.5,
           z0 = (maxZ + minZ)*0.5;

    xList2 = glGenLists(1);
    glNewList (xList2, GL_COMPILE);

    if (params.isLight)
    {
        glEnable (GL_COLOR_MATERIAL);
        glDisable (GL_LIGHTING);
    }
    glColor3d(0,0,0);
    glPointSize(1);
    glBegin(GL_POINTS);
    for (unsigned i = 0; i < Faces.size1(); i++)
        for (unsigned j = 0; j < Faces.size2(); j++)
            glVertex3d(cX(Faces[i][j]) - x0, cY(Faces[i][j]) - y0, cZ(Faces[i][j]) - z0);
    glEnd();
    if (params.isLight)
    {
        glEnable(GL_LIGHTING);
        glDisable (GL_COLOR_MATERIAL);
    }
    glEndList();
}
/*******************************************************************/
void GLFunWidget::showContextMenu(const QPoint &pos)
{
    QPoint globalPos;
    QAction* selectedItem;


    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(pos);
    else
        globalPos = ((QWidget*)sender())->mapToGlobal(pos);

    if ((selectedItem = menu.exec(globalPos)))
    {
        if (selectedItem == action1)
        {
            bool isOk;
            QString exp = QInputDialog::getText(this, tr("Selection condition nodes"),tr("Predicate:"), QLineEdit::Normal, predicate,&isOk);

            if (isOk && exp.length())
            {
                predicate = exp;
                processPredicate();
            }
        }
    }
}
/*******************************************************************/
void GLFunWidget::processPredicate(void)
{
    repaint();
}
/*******************************************************************/
void GLFunWidget::drawPredicate(void)
{
    double x0 = (maxX + minX)*0.5,
          y0 = (maxY + minY)*0.5,
          z0 = (maxZ + minZ)*0.5;

    glPointSize(4);
    glBegin(GL_POINTS);
    for (unsigned i = 0; i < x->size(); i++)
        if (fabsl((*results)[funIndex].getResults(i) > predicate.toFloat()))
        {
            if ((*results)[funIndex].getResults(i) > 0)
                glColor3b(1,0,0);
            else
                glColor3b(0,0,1);
            glVertex3d(cX(i) - x0, cY(i) - y0, cZ(i) - z0);
        }
    glEnd();

}
/*******************************************************************/
void GLFunWidget::set(vector<double>* px,vector<double>* py,vector<double>* pz,matrix<int>* ps,TResultList* r,unsigned i)
{
    x = px;
    y = py;
    z = pz;
    surface = ps;
    results = r;
    funIndex = i;

    minX = *min_element((*x).begin(),(*x).end());
    maxX = *max_element((*x).begin(),(*x).end());
    minY = *min_element((*y).begin(),(*y).end());
    maxY = *max_element((*y).begin(),(*y).end());
    minZ = *min_element((*z).begin(),(*z).end());
    maxZ = *max_element((*z).begin(),(*z).end());
    radius = sqrt((maxX - minX)*(maxX - minX) + (maxY - minY)*(maxY - minY)  + (maxZ - minZ)*(maxZ - minZ));
    initColorTable();
    repaint();
}
/*******************************************************************/
double GLFunWidget::cX(unsigned i)
{
    return (*x)[i] + params.koff*(*results)[0].getResults(i);
}
/*******************************************************************/
double GLFunWidget::cY(unsigned i)
{
    return (*y)[i] + params.koff*(*results)[1].getResults(i);
}
/*******************************************************************/
double GLFunWidget::cZ(unsigned i)
{
    return (*z)[i] + params.koff*(*results)[2].getResults(i);
}
/*******************************************************************/
void GLFunWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
    double colors[3];
    double val;
    int index = -1;

    if (params.isLight) return;
    glReadPixels(e->x(), e->y(), 1, 1, GL_RGB, GL_FLOAT, colors);
    for (int i = 0; i < params.numColor; i++)
        if (fabs(colorTable[i].red() - colors[0]) < 1.0e-1 && fabs(colorTable[i].green() - colors[1]) < 1.0e-1 && fabs(colorTable[i].blue() - colors[2]) < 1.0e-1)
        {
            index = i;
            val = colorTable[i].value();
            break;
        }
    if (index == -1)
        return;
    QToolTip::showText(QPoint(e->x(),e->y()),QString::number(val,'E'));
}
/*******************************************************************/

// Si=((Sxx-Syy)^2+(Sxx-Szz)^2+(Syy-Szz)^2+6*(Sxy^2+Sxz^2+Syz^2))^0.5/1.41
