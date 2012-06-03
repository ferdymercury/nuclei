#ifndef SPECTRATOR_H
#define SPECTRATOR_H

#include <QMainWindow>

namespace Ui {
class Spectrator;
}
class ENSDF;

class Spectrator : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Spectrator(QWidget *parent = 0);
    ~Spectrator();

private slots:
    void selectedA(const QString &a);
    void selectedNuclide(const QString &nuclide);
    
private:
    Ui::Spectrator *ui;

    ENSDF *currentMassChain;
};

#endif // SPECTRATOR_H
