#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsEllipseItem>
#include <QVector>
#include <QPointF>
#include <QTimer>
#include <QGraphicsScene>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startSimulation();
    void moveObject();
    void loadFile();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsEllipseItem *drone;
    QTimer *timer;
    QVector<QPointF> trajectory;
    int currentIndex;

    QVector<QPointF> loadTrajectory(const QString &filename);
};

#endif // MAINWINDOW_H
