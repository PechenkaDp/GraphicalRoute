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
    : QMainWindow(parent) // Инициализация базового класса QMainWindow с родительским виджетом
    , ui(new Ui::MainWindow) // Создание нового объекта интерфейса (ui) для MainWindow
    , currentIndex(0) // Инициализация текущего индекса (для отслеживания положения в траектории) со значением 0
    , timer(nullptr) // Инициализация указателя на таймер (пока он равен nullptr)
{
    ui->setupUi(this); // Настройка пользовательского интерфейса, созданного в Qt Designer

    scene = new QGraphicsScene(this); // Создание новой графической сцены для отображения графики
    ui->graphicsView->setScene(scene); // Установка созданной сцены в элемент QGraphicsView на форме
    // Добавление красного эллипса (представляющего дрон) на сцену, с размерами 10x10
    drone = scene->addEllipse(0, 0, 10, 10, QPen(), QBrush(Qt::red));

    // Связывание сигнала нажатия кнопки "Запуск" с методом startSimulation
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startSimulation);
    // Связывание сигнала нажатия кнопки "Загрузить файл" с методом loadFile
    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadFile);
}

// Деструктор класса MainWindow
MainWindow::~MainWindow()
{
    delete ui; // Освобождение ресурсов, выделенных для объекта интерфейса (ui)
    if (timer) { // Проверка, если таймер был инициализирован
        timer->stop(); // Остановка таймера
        delete timer; // Освобождение ресурсов, выделенных для таймера
    }
}

// Функция для загрузки траектории из файла
QVector<QPointF> MainWindow::loadTrajectory(const QString &filename) {
    QVector<QPointF> points; // Вектор для хранения загруженных точек
    QFile file(filename); // Создание объекта QFile для работы с файлом
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { // Открытие файла для чтения
        return points; // Возврат пустого вектора, если файл не удалось открыть
    }

    QTextStream in(&file); // Создание текстового потока для чтения данных из файла
    while (!in.atEnd()) { // Цикл до конца файла
        float x, y; // Переменные для хранения координат
        in >> x >> y; // Чтение координат из файла
        points.append(QPointF(x, y)); // Добавление координат в вектор
    }
    return points; // Возврат вектора с загруженными точками
}

// Функция для запуска симуляции
void MainWindow::startSimulation() {
    if (trajectory.isEmpty()) { // Проверка, если траектория пуста
        // Показ предупреждающего сообщения
        QMessageBox::warning(this, tr("Ошибка"), tr("Загрузите файл с координатами."));
        return; // Выход из функции
    }

    currentIndex = 0; // Сброс текущего индекса до 0
    scene->clear(); // Очистка графической сцены
    // Добавление дрона на начальную позицию (0, 0)
    drone = scene->addEllipse(0, 0, 10, 10, QPen(), QBrush(Qt::red));
    drone->setRect(0, 0, 10, 10); // Установка положения дрона

    if (timer) { // Проверка, если таймер был инициализирован
        timer->stop(); // Остановка таймера
        delete timer; // Освобождение ресурсов, выделенных для таймера
    }

    timer = new QTimer(this); // Создание нового таймера
    // Связывание сигнала таймера с функцией moveObject
    connect(timer, &QTimer::timeout, this, &MainWindow::moveObject);
    timer->start(1000); // Запуск таймера с интервалом 1000 миллисекунд (1 секунда)
}

// Функция для перемещения объекта (дрона)
void MainWindow::moveObject() {
    if (currentIndex < trajectory.size()) { // Проверка, если текущий индекс меньше размера траектории
        QPointF point = trajectory[currentIndex]; // Получение текущей точки из траектории

        // Перемещение дрона на текущие координаты, с учётом смещения по Y
        drone->setRect(point.x(), point.y() - 5, 10, 10);

        if (currentIndex > 0) { // Проверка, если текущий индекс больше 0
            QPointF previousPoint = trajectory[currentIndex - 1]; // Получение предыдущей точки
            // Рисование линии между предыдущей и текущей точками
            scene->addLine(previousPoint.x(), previousPoint.y() - 5, point.x(), point.y() - 5, QPen(Qt::blue, 2));
        }

        // Обновление текстовых меток с текущими координатами
        ui->labelX->setText(QString("X: %1").arg(point.x()));
        ui->labelY->setText(QString("Y: %1").arg(point.y()));

        currentIndex++; // Увеличение текущего индекса для перехода к следующей точке
    } else {
        timer->stop(); // Остановка таймера, если все точки пройдены
    }
}

// Функция для загрузки файла с координатами
void MainWindow::loadFile() {
    // Открытие диалогового окна для выбора файла
    QString filename = QFileDialog::getOpenFileName(this, tr("Загрузить файл с координатами"), "", tr("Text Files (*.txt)"));
    if (!filename.isEmpty()) { // Проверка, если файл выбран
        trajectory = loadTrajectory(filename); // Загрузка координат из выбранного файла
        currentIndex = 0; // Сброс текущего индекса до 0
        ui->labelX->clear(); // Очистка метки X
        ui->labelY->clear(); // Очистка метки Y
    }
}
