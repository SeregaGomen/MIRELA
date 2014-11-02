#ifndef IMAGEPARAMS_H
#define IMAGEPARAMS_H

#include <QColor>
#include <QDataStream>
#include <fstream>

using namespace std;

class ImageParams
{
public:
    bool isMesh;       // Отображение сетки
    bool isVertex;     // ... узлов
    bool isFace;       // ... поверхности
    bool isCoord;      // ... осей координат
    bool isLight;      // ... освещения
    bool isValueScale; // ... шкалы значений
    bool isLimit;      // ... граничных условий
    bool isForce;      // ... нагрузок
    bool isQuality;    // Качественная прорисовка линий сетки на поверхности объекта (иногда сильно тормозит отображение)
    bool isRGB;        // Цветная (RGB) визуализация
    bool isSpectral;   // Цветная (спектральная) визуализация
    bool isNegative;   // Негативное отображение
    bool isBW;         // Черно-белое отображение
    int numColor;      // Кол-во градаций цвета (цветов) при отображении
    double koff;        // Коэффициент усиления трансформации
    double alpha;       // Прозрачность материала
    QColor bkgColor;   // Цвет фона изображения

    ImageParams(void)
    {
        init();
    }
    ~ImageParams(void) {}
    ImageParams& operator = (ImageParams& right)
    {
        koff = right.koff;
        alpha = right.alpha;
        numColor = right.numColor;
        isRGB = right.isRGB;
        isCoord = right.isCoord;
        isLight = right.isLight;
        isQuality = right.isQuality;
        isMesh = right.isMesh;
        isFace = right.isFace;
        isVertex = right.isVertex;
        isSpectral = right.isSpectral;
        isNegative = right.isNegative;
        isBW = right.isBW;
        isValueScale = right.isValueScale;
        isLimit = right.isLimit;
        isForce = right.isForce;
        bkgColor = right.bkgColor;
        return *this;
    }
    void write(QDataStream& file)
    {
        file << koff;
        file << alpha;
        file << numColor;
        file << isRGB;
        file << isCoord;
        file << isLight;
        file << isQuality;
        file << isMesh;
        file << isFace;
        file << isVertex;
        file << isSpectral;
        file << isNegative;
        file << isBW;
        file << isValueScale;
        file << isLimit;
        file << isForce;
        file << bkgColor;
    }
    void write(ofstream& file)
    {
        file << koff;
        file << alpha;
        file << numColor;
        file << isRGB;
        file << isCoord;
        file << isLight;
        file << isQuality;
        file << isMesh;
        file << isFace;
        file << isVertex;
        file << isSpectral;
        file << isNegative;
        file << isBW;
        file << isValueScale;
        file << isLimit;
        file << isForce;
        file << bkgColor.red();
        file << bkgColor.green();
        file << bkgColor.blue();
    }
    void read(QDataStream& file)
    {
        file >> koff;
        file >> alpha;
        file >> numColor;
        file >> isRGB;
        file >> isCoord;
        file >> isLight;
        file >> isQuality;
        file >> isMesh;
        file >> isFace;
        file >> isVertex;
        file >> isSpectral;
        file >> isNegative;
        file >> isBW;
        file >> isValueScale;
        file >> isLimit;
        file >> isForce;
        file >> bkgColor;
    }
    void read(ifstream& file)
    {
        int r,
            g,
            b;

        file >> koff;
        file >> alpha;
        file >> numColor;
        file >> isRGB;
        file >> isCoord;
        file >> isLight;
        file >> isQuality;
        file >> isMesh;
        file >> isFace;
        file >> isVertex;
        file >> isSpectral;
        file >> isNegative;
        file >> isBW;
        file >> isValueScale;
        file >> isLimit;
        file >> isForce;
        file >> r;
        file >> g;
        file >> b;

        bkgColor.setRed(r);
        bkgColor.setGreen(g);
        bkgColor.setBlue(b);
    }
    void init(void)
    {
        koff = 0.0;
        alpha = 1.0;
        numColor = 16;
        isRGB =
        isCoord =
        isLight =
        isQuality =
        isFace =
        isValueScale = true;
        isMesh =
        isVertex =
        isSpectral =
        isNegative =
        isLimit =
        isForce =
        isBW = false;
        bkgColor = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0).dark();
    }
};

#endif // IMAGEPARAMS_H
