#include "myview.h"
#include "mybox.h"
#include <QIcon>
#include <QDebug>
#include <QPoint>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QLabel>
#include <QGraphicsProxyWidget>
#include <QPushButton>
#include <QApplication>

const double INITSPEED = 500;

MyView::MyView(QWidget *parent) :
    QGraphicsView(parent)
{
    initView();
}

void MyView::startGame()
{
    gameWelcomeText->hide();
    startButton->hide();
    optionButton->hide();
    helpButton->hide();
    exitButton->hide();
    maskWidget->hide();

    initGame();
}

void MyView::clearFullRows()
{
    for (int y = 429; y > 50; y -= 20)
    {
        QList < QGraphicsItem *> itemList =
                scene()->items(199, y, 202, 22,
                               Qt::ContainsItemShape,
                               Qt::AscendingOrder);

        if (itemList.size() == 0) break;
        if (itemList.size() >= 5) 
        {
            for (auto u : itemList) 
            {
                OneBox *box = (OneBox*)u;
                clearAnimation(box);
            }
            rows << y;
        }
     }

    if (rows.size()) QTimer::singleShot(300, this, SLOT(moveBox()));
    else 
    {
        boxGroup->createBox(QPointF(300, 70), nextBoxGroup->getCurrentShape());
        nextBoxGroup->clearBoxGroup(true);
        nextBoxGroup->createBox(QPointF(500, 70));
    }
}

void MyView::clearAnimation(OneBox* box)
{
    QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
    box->setGraphicsEffect(blurEffect);

    QPropertyAnimation *animation = new QPropertyAnimation(box, "scale");
    animation->setEasingCurve(QEasingCurve::OutCurve);
    animation->setDuration(250);
    animation->setStartValue(4);
    animation->setEndValue(0.25);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation, SIGNAL(finished()), box, SLOT(deleteLater()));
}

void MyView::moveBox()
{
    for (int i = rows.size() - 1; i >= 0; i--)
    {  
        for (auto u : scene()->items(
            199, 49, 202, rows[i] - 47,
            Qt::ContainsItemShape,
            Qt::AscendingOrder)) u->moveBy(0, 20);
            
    }
    
    updateScore(rows.size());
    rows.clear();

    boxGroup->createBox(QPointF(300, 70), nextBoxGroup->getCurrentShape());
    nextBoxGroup->clearBoxGroup(true);
    nextBoxGroup->createBox(QPointF(500, 70));
}

void MyView::gameOver()
{
    pauseButton->hide();
    showMenuButton->hide();
    maskWidget->show();
    gameOverText->show();
    restartButton->setPos(370, 200);
    finishButton->show();
    clearFocus();
}

void MyView::initView()
{
    setRenderHint(QPainter::Antialiasing);

    setCacheMode(CacheBackground);
    setWindowTitle(u8"方块游戏");
    setWindowIcon(QIcon(":/icon.png"));
    setMinimumSize(810, 510);
    setMaximumSize(810, 510);

    QGraphicsScene *scene = new QGraphicsScene;
    scene->setSceneRect(5, 5, 800, 500);
    scene->setBackgroundBrush(QPixmap(":/background.png"));
    setScene(scene);

    topLine = scene->addLine(197, 47, 403, 47);
    bottomLine = scene->addLine(197,453,403,453);
    leftLine = scene->addLine(197,47,197,453);
    rightLine = scene->addLine(403,47,403,453);

    boxGroup = new BoxGroup;
    connect(boxGroup, SIGNAL(needNewBox()), this, SLOT(clearFullRows()));
    connect(boxGroup, SIGNAL(gameFinished()), this, SLOT(gameOver()));
    scene->addItem(boxGroup);
    nextBoxGroup = new BoxGroup;
    scene->addItem(nextBoxGroup);

    gameScoreText = new QGraphicsTextItem();
    gameScoreText->setFont(QFont("Times", 20, QFont::Bold));
    gameScoreText->setPos(650, 350);
    scene->addItem(gameScoreText);

    gameLevelText = new QGraphicsTextItem();
    gameLevelText->setFont(QFont("Times", 30, QFont::Bold));
    gameLevelText->setPos(20, 150);
    scene->addItem(gameLevelText);

    // 设置初始为隐藏状态
    topLine->hide();
    bottomLine->hide();
    leftLine->hide();
    rightLine->hide();
    gameScoreText->hide();
    gameLevelText->hide();

    // 黑色遮罩
    QWidget *mask = new QWidget;
    mask->setAutoFillBackground(true);
    mask->setPalette(QPalette(QColor(0, 0, 0, 80)));
    mask->resize(900, 600);
    maskWidget = scene->addWidget(mask);
    maskWidget->setPos(-50, -50);
    // 设置其Z值为1，这样可以处于Z值为0的图形项上面
    maskWidget->setZValue(1);

    // 选项面板
    QWidget *option = new QWidget;
    QPushButton *optionCloseButton = new QPushButton(tr(u8"关   闭"), option);
    QPalette palette;
    palette.setColor(QPalette::ButtonText, Qt::black);
    optionCloseButton->setPalette(palette);
    optionCloseButton->move(120, 300);
    connect(optionCloseButton, SIGNAL(clicked()), option, SLOT(hide()));
    option->setAutoFillBackground(true);
    option->setPalette(QPalette(QColor(0, 0, 0, 130)));
    option->resize(300, 400);
    QGraphicsWidget *optionWidget = scene->addWidget(option);
    optionWidget->setPos(250, 50);
    optionWidget->setZValue(3);
    optionWidget->hide();

    // 帮助面板
    QWidget *help = new QWidget;
    QPushButton *helpCloseButton = new QPushButton(tr(u8"关   闭"), help);
    helpCloseButton->setPalette(palette);
    helpCloseButton->move(120, 300);
    connect(helpCloseButton, SIGNAL(clicked()), help, SLOT(hide()));
    help->setAutoFillBackground(true);
    help->setPalette(QPalette(QColor(0, 0, 0, 180)));
    help->resize(300, 400);
    QGraphicsWidget *helpWidget = scene->addWidget(help);
    helpWidget->setPos(250, 50);
    helpWidget->setZValue(3);
    helpWidget->hide();

    // 游戏欢迎文本
    gameWelcomeText = new QGraphicsTextItem();
    gameWelcomeText->setHtml(tr(u8"<font color=white>方块游戏</font>"));
    gameWelcomeText->setFont(QFont("Times", 30, QFont::Bold));
    gameWelcomeText->setPos(250, 100);
    gameWelcomeText->setZValue(2);
    scene->addItem(gameWelcomeText);

    // 游戏暂停文本
    gamePausedText = new QGraphicsTextItem();
    gamePausedText->setHtml(tr(u8"<font color=white>游戏暂停中！</font>"));
    gamePausedText->setFont(QFont("Times", 30, QFont::Bold));
    gamePausedText->setPos(300, 100);
    gamePausedText->setZValue(2);
    scene->addItem(gamePausedText);
    gamePausedText->hide();

    // 游戏结束文本
    gameOverText = new QGraphicsTextItem();
    gameOverText->setHtml(tr(u8"<font color=white>游戏结束！</font>"));
    gameOverText->setFont(QFont("Times", 30, QFont::Bold));
    gameOverText->setPos(320, 100);
    gameOverText->setZValue(2);
    scene->addItem(gameOverText);
    gameOverText->hide();

    QPushButton *button1 = new QPushButton(tr(u8"开    始"));
    QPushButton *button2 = new QPushButton(tr(u8"选    项"));
    QPushButton *button3 = new QPushButton(tr(u8"帮    助"));
    QPushButton *button4 = new QPushButton(tr(u8"退    出"));
    QPushButton *button5 = new QPushButton(tr(u8"重新开始"));
    QPushButton *button6 = new QPushButton(tr(u8"暂    停"));
    QPushButton *button7 = new QPushButton(tr(u8"主 菜 单"));
    QPushButton *button8 = new QPushButton(tr(u8"返回游戏"));
    QPushButton *button9 = new QPushButton(tr(u8"结束游戏"));

    connect(button1, SIGNAL(clicked()), this, SLOT(startGame()));
    connect(button2, SIGNAL(clicked()), option, SLOT(show()));
    connect(button3, SIGNAL(clicked()), help, SLOT(show()));
    connect(button4, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(button5, SIGNAL(clicked()), this, SLOT(restartGame()));
    connect(button6, SIGNAL(clicked()), this, SLOT(pauseGame()));
    connect(button7, SIGNAL(clicked()), this, SLOT(finishGame()));
    connect(button8, SIGNAL(clicked()), this, SLOT(returnGame()));
    connect(button9, SIGNAL(clicked()), this, SLOT(finishGame()));

    startButton = scene->addWidget(button1);
    optionButton = scene->addWidget(button2);
    helpButton = scene->addWidget(button3);
    exitButton = scene->addWidget(button4);
    restartButton = scene->addWidget(button5);
    pauseButton = scene->addWidget(button6);
    showMenuButton = scene->addWidget(button7);
    returnButton = scene->addWidget(button8);
    finishButton = scene->addWidget(button9);

    startButton->setPos(370, 200);
    optionButton->setPos(370, 250);
    helpButton->setPos(370, 300);
    exitButton->setPos(370, 350);
    restartButton->setPos(600, 150);
    pauseButton->setPos(600, 200);
    showMenuButton->setPos(600, 250);
    returnButton->setPos(370, 200);
    finishButton->setPos(370, 250);

    startButton->setZValue(2);
    optionButton->setZValue(2);
    helpButton->setZValue(2);
    exitButton->setZValue(2);
    restartButton->setZValue(2);
    returnButton->setZValue(2);
    finishButton->setZValue(2);

    restartButton->hide();
    finishButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    returnButton->hide();
}

void MyView::initGame()
{
    boxGroup->createBox(QPointF(300, 70));
    boxGroup->setFocus();
    boxGroup->startTimer(INITSPEED);
    gameSpeed = INITSPEED;
    nextBoxGroup->createBox(QPointF(500, 70));

    scene()->setBackgroundBrush(QPixmap(":/background01.png"));
    gameScoreText->setHtml(tr("<font color=red>0</font>"));
    gameLevelText->setHtml(tr(u8"<font color=white>第<br>一<br>幕</font>"));

    restartButton->show();
    pauseButton->show();
    showMenuButton->show();
    gameScoreText->show();
    gameLevelText->show();
    topLine->show();
    bottomLine->show();
    leftLine->show();
    rightLine->show();
}

void MyView::updateScore(const int fullRowNum)
{
    int score = fullRowNum * 100;
    int currentScore = gameScoreText->toPlainText().toInt();
    currentScore += score;

    gameScoreText->setHtml(tr("<font color=red>%1</font>").arg(currentScore));

    if (currentScore > 500)
    { 
        gameLevelText->setHtml(tr(u8"<font color=white>第<br>二<br>幕</font>"));
        scene()->setBackgroundBrush(QPixmap(":/background02.png"));
        gameSpeed = 300;
        boxGroup->stopTimer();
        boxGroup->startTimer(gameSpeed);
    }

}

void MyView::restartGame()
{

}

void MyView::finishGame()
{

}

void MyView::pauseGame()
{

}

void MyView::returnGame()
{
    
}
