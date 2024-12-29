#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <QRandomGenerator>
#include <QVector>
#include <QPixmap>
#include <iostream>
using namespace std;

QLCDNumber *lcdNumbers[14]; //각 위젯을 저장하기 위한 포인터 배열 선언
QPushButton *Buttons[14];
QVector<int> my; //나의 카드덱 배열 선언
QVector<int> your; //상대방의 카드덱 배열 선언
int card[54]; //카드 저장 배열
char shape[15]; //카드 모양
int num[15]; //카드 숫자
int Decknum = 0; //카드덱 부여 횟수
int CardinButton[14]; //각 버튼이 어떤 카드를 가지고 있는지를 저장하는 변수
int flag1 = 0; //내 덱에 있는 카드 볼 때 필요한 플래그 (오른쪽 버튼)
int laidCard; //놓여있는 카드가 무엇인지 저장하는 변수
char laidCardShape; //놓여있는 카드의 모양
int laidCardNum; //놓여있는 카드의 숫자
int is_myturn;  //플레이어 순서 정하는 변수
int turn = 0; //뽑거나 내거나 둘 중하나 체크하는 변수
int is_attack = 0; //공격 상황 변수
int damage = 0; //뽑아야 할 장 수

//공격 인터페이스
class Attack
{
public:
    virtual void getAttack() = 0;

};

//공격 (5점) 블랙조커 클래스
class BJoker : public Attack
{
public:
    void getAttack() override
    {
        damage += 5;
    }
};

//공격 (10점) 컬러조커 클래스
class CJoker : public Attack
{
public:
    void getAttack() override
    {
       damage +=10;
    }
};

//공격 (3점) A 클래스
class A : public Attack
{
public:
    void getAttack() override
    {
        damage +=3;
    }
};

//공격 (2점) TWO 클래스
class Two : public Attack
{
public:
    void getAttack() override
    {
        damage +=2;
    }
};

//공격 (2점) TWO 클래스
class Three : public Attack
{
public:
    void getAttack() override
    {
        damage =0;
    }
};

class My
{
public:
    My() : m_attack(0) {}
    ~My() {
        if(m_attack)
            delete m_attack;
    }
public:
    void eat(Attack* attack)
    {
        if(m_attack)
            delete m_attack;
        m_attack = attack;
    }
    void getAttack()
    {
        m_attack ->getAttack();
    }

private:
    Attack* m_attack;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    my_IP = QHostAddress("192.168.0.132");
//    your_IP = QHostAddress("192.168.0.73");
//    SPORT = 5000;
//    RPORT = 4999;
//    sendsocket = new QUdpSocket;
//    receivesocket = new QUdpSocket;

//    if(receivesocket->bind(my_IP, RPORT)){
//        connect(receivesocket, SIGNAL(readyRead()), this, SLOT(sub_udp()));
//    }
    QTimer *timer=new QTimer;
    timer->start();
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    for(int i = 0; i < 14; i++){
        QString lcdName = QString("l%1").arg(i + 1); //각 위젯이름 생성
        lcdNumbers[i] = findChild<QLCDNumber *>(lcdName); //해당 위젯을 찾아 배열에 저장 -> 포인터+이름을 통해

        QString buttonName = QString("mycard_%1").arg(i + 1);
        Buttons[i] = findChild<QPushButton *>(buttonName);
    }

    ui->Pick->setIcon(QIcon("/home/yoonhyeongjun/card_deck/B/0.jpg")); //카드 뒷면 이미지
    ui->Pick->setIconSize(QSize(350, 350));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update()
{
    int width = 235, height = 350;  // 이미지 너비, 높이

    int index;
    for(int i = 0; i < my.size(); i++){ //카드 7번째 UI에 있는 카드번호가 내 카드덱 배열에 몇 번째 인덱스인지 찾기
        if (my[i] == CardinButton[6]) {
            index = i;
            break;
        }
    }

    if(my.size() <= 7){ //카드 수에 따라 가시성 효과
        ui->myright->setVisible(false);
        ui->myleft->setVisible(false);
    }
    else{
        if(CardinButton[0] == my[0]) //내 카드덱 첫 줄에서는 왼쪽 버튼 가시성 X
            ui->myleft->setVisible(false);
        else
            ui->myleft->setVisible(true);
        if(CardinButton[6] < 0 | CardinButton[6] > 53)
            ui->myright->setVisible(false);
        else{
            if(index == my.size()-1)
                ui->myright->setVisible(false);
            else
                ui->myright->setVisible(true);
        }
    }

    put();
    ui->lcdNumber->display(laidCard);
    ui->lcdNumber_2->display(damage);
    ui->lcdNumber_3->display(is_attack);

    if (is_myturn == 1){  //턴 출력
        ui->label_2->setText("Myturn");
    }
    else
        ui->label_2->setText("Yourturn");

}

void MainWindow::on_START_clicked()
{
    for(int i = 0; i < 54; i++){ //카드 배열에 순서대로 수 저장
        card[i] = i;
    }
    for(int i = 0; i < 10000; i++){ //카드 배열 속 숫자들 랜덤으로 섞기(shuffle 알고리즘)
        int tmp, index1, index2;
        index1 = QRandomGenerator::global()->bounded(0, 53);
        index2 = QRandomGenerator::global()->bounded(0, 53);

        tmp = card[index1];
        card[index1] = card[index2];
        card[index2] = tmp;
    }

    if (card[0] <= 27)  //경기 순서 부여
        is_myturn = 1;
    else
        is_myturn = 0;

    for(Decknum = 0; Decknum < 14; Decknum++){ //카드 부여
        if(Decknum < 7)
            my.push_back(card[Decknum]);
        else
            your.push_back(card[Decknum]);
    }

    for(int i = 0; i < 14; i++){ //0~53까지의 숫자들을 이용해 트럼프 카드 모양, 숫자 표현
        CardJudge(card, i);

        CardinButton[i] = card[i]; //각 버튼이 어떤 카드를 가지고 있는지 저장

        lcdNumbers[i]->display(card[i]); //해당 lcd에 해당 카드번호 출력
        Buttons[i]->setIcon(QIcon(QString("/home/yoonhyeongjun/card_deck/%1/%2.png").arg(shape[i]).arg(num[i]))); //해당 버튼에 받은 카드의 사진 출력 -> 경로를 통해
        Buttons[i]->setIconSize(QSize(350, 350)); //사진(Icon)의 크기 설정
    }

    //시작 중앙 카드 뽑기
    laidCard = card[Decknum];
    CardJudge(card, Decknum);

    laidCardShape = shape[Decknum];
    laidCardNum = num[Decknum];

    ui->lcdNumber->display(laidCard);
    ui->lcdNumber_2->display(damage);
    ui->lcdNumber_3->display(is_attack);
    QPixmap pixmap(QString("/home/yoonhyeongjun/card_deck/%1/%2.png").arg(laidCardShape).arg(laidCardNum)); //해당 버튼에 받은 카드의 사진 출력 -> 경로를 통해
    int width = 235, height = 350;  // 이미지 너비, 높이
    QPixmap scaledPixmap = pixmap.scaled(width, height); //이미지 크기 설정
    ui->label->setPixmap(scaledPixmap);

    ui->START->setVisible(false); //시작 후 못 누르게 하기 위함
}

void MainWindow::on_Pick_clicked()
{
    My* myeat = new My();

    if(is_myturn == 1 && turn == 0) //myturn일때
    {
        if(Decknum >= 53)  //뽑을 카드가 없을 때
            ui->label->setText("NoCard");
        else{
            if(is_attack == 0){ //공격상황이 아닐때 뽑음
                Decknum++;
                my.push_back(card[Decknum]);
            }
            else  //공격상황일때 뽑음
            {
                if(laidCard == 53)
                {
                    myeat -> eat(new BJoker());
                    myeat ->getAttack();
                }
                else if(laidCard == 54)
                {
                    myeat -> eat(new CJoker());
                    myeat ->getAttack();
                }
                else if(laidCardNum == 1)
                {
                    myeat -> eat(new A());
                    myeat ->getAttack();
                }
                else if(laidCardNum == 2)
                {
                    myeat -> eat(new Two());
                    myeat ->getAttack();
                }
                else if(laidCardNum == 3)
                {
                    myeat -> eat(new Three());
                    myeat ->getAttack();
                }
            }
            is_attack = 0;
        }

        Print();
    }
    is_myturn = 0;
    turn = 1;
}

void MainWindow::put()
{
    if(is_myturn == 1 && turn == 0) //myturn 일때
    {
        int width = 235, height = 350;  // 이미지 너비, 높이
        int index;
        for(int i = 0; i < my.size(); i++){ //카드 7번째 UI에 있는 카드번호가 내 카드덱 배열에 몇 번째 인덱스인지 찾기
            if (my[i] == CardinButton[6]) {
                index = i;
                break;
            }
        }

        for(int i = 0; i < 14; i++){
            QObject::connect(Buttons[i], &QPushButton::clicked, [=]() { //i인덱스 버튼이 눌림을 감지하면때
                if(is_attack == 0) //공격 상황이 아니라면
                {
                    if(laidCardShape == shape[i] | laidCardNum == num[i] | CardinButton[i] == 53 | CardinButton[i] == 54){
                        laidCard = CardinButton[i];
                        ui->lcdNumber->display(laidCard); //해당 lcd에 해당 카드번호 출력

                        QPixmap pixmap(QString("/home/yoonhyeongjun/card_deck/%1/%2.png").arg(shape[i]).arg(num[i]));
                        QPixmap scaledPixmap = pixmap.scaled(width, height);
                        ui->label->setPixmap(scaledPixmap);

                        laidCardShape = shape[i];
                        laidCardNum = num[i];
                        ui->state->setText("");

                        int index = -1; // 초기값 설정
                        for (int k = 0; k < my.size(); k++) {
                            if (my[k] == CardinButton[i]) {
                                index = k; // 해당 값의 인덱스 저장
                                break;
                            }
                        }

                        if (index != -1) { // 해당 값이 배열에서 찾아진 경우에만 수행
                            for (int j = index; j < my.size() - 1; j++) {
                                my[j] = my[j + 1]; // 현재 인덱스에 다음 값을 복사
                            }
                            my.pop_back(); // 배열의 마지막 요소 제거
                        }

                        if(laidCard == 53 || laidCard == 54 || laidCardNum == 1 || laidCardNum == 2)
                            is_attack = 1;
                        else if(laidCardNum == 3)
                            is_attack = 0;
                    }
                    else{
                        ui->state->setText("Impossible");
                    }
                }
                else if(is_attack == 1) //공격 상황이라면
                {
                    if(CardinButton[i] == 53 || CardinButton[i] == 54 || laidCardNum == 1 || laidCardNum == 2 || laidCardNum == 3){
                        laidCard = CardinButton[i];
                        ui->lcdNumber->display(laidCard); //해당 lcd에 해당 카드번호 출력

                        QPixmap pixmap(QString("/home/yoonhyeongjun/card_deck/%1/%2.png").arg(shape[i]).arg(num[i]));
                        QPixmap scaledPixmap = pixmap.scaled(width, height);
                        ui->label->setPixmap(scaledPixmap);

                        laidCardShape = shape[i];
                        laidCardNum = num[i];
                        ui->state->setText("");

                        int index = -1; // 초기값 설정
                        for (int k = 0; k < my.size(); k++) {
                            if (my[k] == CardinButton[i]) {
                                index = k; // 해당 값의 인덱스 저장
                                break;
                            }
                        }

                        if (index != -1) { // 해당 값이 배열에서 찾아진 경우에만 수행
                            for (int j = index; j < my.size() - 1; j++) {
                                my[j] = my[j + 1]; // 현재 인덱스에 다음 값을 복사
                            }
                            my.pop_back(); // 배열의 마지막 요소 제거
                        }

                        if(laidCard == 53 || laidCard == 54 || laidCardNum == 1 || laidCardNum == 2)
                            is_attack = 1;
                        else if(laidCardNum == 3)
                            is_attack = 0;

                    }
                    else{
                        ui->state->setText("Impossible");
                    }
                }
                is_myturn = 0;
                turn = 1;
            });
        }
    }
}

void MainWindow::on_myright_clicked()
{
    Print();
}

void MainWindow::on_myleft_clicked()
{
    flag1 = 0;
    int index;
    for(int i = 0; i < my.size(); i++){ //카드 1번째 UI에 있는 카드번호가 내 카드덱 배열에 몇 번째 인덱스인지 찾기
        if (my[i] == CardinButton[0]) { //문제
            index = i;
            break;
        }
    }

    for(int i = 0; i < 7; i++){ //0~53까지의 숫자들을 이용해 트럼프 카드 모양, 숫자 표현
        Buttons[i]->setVisible(true);
        CardinButton[i] = my[index-7+i];

        CardJudge(CardinButton, i);

        lcdNumbers[i]->display(CardinButton[i]); //해당 lcd에 해당 카드번호 출력
        Buttons[i]->setIcon(QIcon(QString("/home/yoonhyeongjun/card_deck/%1/%2.png").arg(shape[i]).arg(num[i]))); //해당 버튼에 받은 카드의 사진 출력 -> 경로를 통해
        Buttons[i]->setIconSize(QSize(350, 350)); //사진(Icon)의 크기 설정
    }
}

void MainWindow::CardJudge(int s[], int i) //카드 종류 판단 함수
{
    if(s[i] >= 0 && s[i] <= 12){
        shape[i] = 'S';
        num[i] = s[i]+1;
    }
    else if(s[i] >= 13 && s[i] <= 25){
        shape[i] = 'D';
        num[i] = s[i] - 12;
    }
    else if(s[i] >= 26 && s[i] <= 38){
        shape[i] = 'C';
        num[i] = s[i] - 25;
    }
    else if(s[i] >= 39 && s[i] <= 51){
        shape[i] = 'H';
        num[i] = s[i] - 38;
    }
    else{
        shape[i] = 'J';
        num[i] = s[i] - 51;
    }
}

void MainWindow::Print()
{
    int index, mult, itelse;
    for(int i = 0; i < my.size(); i++){ //카드 7번째 UI에 있는 카드번호가 내 카드덱 배열에 몇 번째 인덱스인지 찾기
        if (my[i] == CardinButton[6]) {
            index = i;
            break;
        }
    }

    if(0 <= index+7 && index+7 < my.size()){ //현재 보여지는 카드덱의 마지막 카드로부터 7장 뒤 카드가 내 덱에 있는지 확인
        itelse = 7;
    }
    else{
        itelse = my.size()%7;
    }

    index += 1; //인덱스가 아닌 배열 자체 숫자
    mult = index/7; //index변수를 7로 나눈 몫

    ui->lcdNumber->display(itelse);

    for(int i = 0; i < itelse; i++){ //0~53까지의 숫자들을 이용해 트럼프 카드 모양, 숫자 표현
        Buttons[i]->setVisible(true);
        CardinButton[i] = my[i+7*mult];

        CardJudge(CardinButton, i);

        lcdNumbers[i]->display(CardinButton[i]); //해당 lcd에 해당 카드번호 출력
        Buttons[i]->setIcon(QIcon(QString("/home/yoonhyeongjun/card_deck/%1/%2.png").arg(shape[i]).arg(num[i]))); //해당 버튼에 받은 카드의 사진 출력 -> 경로를 통해
        Buttons[i]->setIconSize(QSize(350, 350)); //사진(Icon)의 크기 설정
    }
    for(int i = itelse; i < 7; i++){
        Buttons[i]->setVisible(false);
    }
}

