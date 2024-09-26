#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QPointF>
#include <QMessageBox>
#include <QGraphicsLineItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentIndex(0)
    , timer(nullptr)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    drone = scene->addEllipse(0, 0, 10, 10, QPen(), QBrush(Qt::red));

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startSimulation);
    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadFile);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (timer) {
        timer->stop();
        delete timer;
    }
}

QVector<QPointF> MainWindow::loadTrajectory(const QString &filename) {
    QVector<QPointF> points;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return points;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        float x, y;
        in >> x >> y;
        points.append(QPointF(x, y));
    }
    return points;
}

void MainWindow::startSimulation() {
    if (trajectory.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Загрузите файл с координатами."));
        return;
    }

    currentIndex = 0;
    scene->clear();
    drone = scene->addEllipse(0, 0, 10, 10, QPen(), QBrush(Qt::red));
    drone->setRect(0, 0, 10, 10);

    if (timer) {
        timer->stop();
        delete timer;
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::moveObject);
    timer->start(1000);
}

void MainWindow::moveObject() {
    if (currentIndex < trajectory.size()) {
        QPointF point = trajectory[currentIndex];

        drone->setRect(point.x(), point.y() - 5, 10, 10);

        if (currentIndex > 0) {
            QPointF previousPoint = trajectory[currentIndex - 1];
            scene->addLine(previousPoint.x(), previousPoint.y() - 5, point.x(), point.y() - 5, QPen(Qt::blue, 2)); // Рисуем линию
        }

        ui->labelX->setText(QString("X: %1").arg(point.x()));
        ui->labelY->setText(QString("Y: %1").arg(point.y()));

        currentIndex++;
    } else {
        timer->stop();
    }
}

void MainWindow::loadFile() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Загрузить файл с координатами"), "", tr("Text Files (*.txt)"));
    if (!filename.isEmpty()) {
        trajectory = loadTrajectory(filename);
        currentIndex = 0;
        ui->labelX->clear();
        ui->labelY->clear();
    }
}
