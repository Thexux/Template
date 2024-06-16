#ifndef MYVIEW_H
#define MYVIEW_H

#include <QGraphicsView>

class OneBox;
class BoxGroup;

class MyView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyView(QWidget *parent = 0);

public slots:
    void startGame();
    void clearFullRows();
    void moveBox();
    void gameOver();
    void restartGame();    
    void finishGame();
    void pauseGame();
    void returnGame();

private:
    BoxGroup *boxGroup;
    BoxGroup *nextBoxGroup;
    QGraphicsLineItem *topLine;
    QGraphicsLineItem *bottomLine;
    QGraphicsLineItem *leftLine;
    QGraphicsLineItem *rightLine;
    qreal gameSpeed;
    QList<int> rows;
    
    QGraphicsTextItem *gameScoreText;
    QGraphicsTextItem *gameLevelText;

    QGraphicsWidget *maskWidget;

    QGraphicsWidget *startButton;
    QGraphicsWidget *finishButton;
    QGraphicsWidget *restartButton;
    QGraphicsWidget *pauseButton;
    QGraphicsWidget *optionButton;
    QGraphicsWidget *returnButton;
    QGraphicsWidget *helpButton;
    QGraphicsWidget *exitButton;
    QGraphicsWidget *showMenuButton;

    QGraphicsTextItem *gameWelcomeText;
    QGraphicsTextItem *gamePausedText;
    QGraphicsTextItem *gameOverText;

    void initView();
    void initGame();
    void updateScore(const int fullRowNum = 0);
    void clearAnimation(OneBox* box);
};

#endif // MYVIEW_H
