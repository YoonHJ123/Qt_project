#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QRandomGenerator>
#include <QVector>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void update();

    void on_START_clicked();

    void on_myright_clicked();

    void on_myleft_clicked();

    void on_Pick_clicked();

    void CardJudge(int s[], int i);

    void Print();

    void put();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
