#include "mainwindow.h"
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <vector>
#include <set>
#include <chrono>
#include <random>

using namespace std;

constexpr int N = 16; // N X N sudoku grid
constexpr int sqrt_N = 4;
constexpr int WINDOW_L = 1920;
constexpr int WINDOW_W = 1080;

vector<vector<QSpinBox*>> sudokuGrid;

void initializeSudokuGrid(QGridLayout* layout) {
    sudokuGrid.resize(N);
    for (int i = 0; i < N; ++i) {
        sudokuGrid[i].resize(N);
        for (int j = 0; j < N; ++j) {
            QSpinBox* box = new QSpinBox;
            box->setMinimum(0);
            box->setMaximum(N);
            box->setSpecialValueText(" ");
            box->setButtonSymbols(QAbstractSpinBox::NoButtons);
            sudokuGrid[i][j] = box;
            layout->addWidget(box, i, j);
        }
    }
}

vector<vector<int>> getSudokuGrid() {
    vector<vector<int>> res;
    res.resize(N);
    for (int i = 0; i < N; ++i) {
        res[i].resize(N);
        for (int j = 0; j < N; ++j) {
            res[i][j] = sudokuGrid[i][j]->value();
        }
    }
    return res;
}

void setSudokuGrid(const vector<vector<int>>& grid) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            sudokuGrid[i][j]->setValue(grid[i][j]);
        }
    }
}

void clearSudokuGrid() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            sudokuGrid[i][j]->setValue(0);
        }
    }
}

bool isPartialSudokuGridValid(const vector<vector<int>> &grid) {
    // for each row
    for (int i = 0; i < N; ++i) {
        set<int> cur;
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == 0)
                continue;
            if (cur.count(grid[i][j]))
                return false;
            cur.insert(grid[i][j]);
        }
    }

    // for each column
    for (int i = 0; i < N; ++i) {
        set<int> cur;
        for (int j = 0; j < N; j++) {
            if (grid[j][i] == 0)
                continue;
            if (cur.count(grid[j][i]))
                return false;
            cur.insert(grid[j][i]);
        }
    }

    // for each box
    for (int i = 0; i < N; i += sqrt_N) {
        for (int j = 0; j < N; j += sqrt_N) {
            set<int> cur;
            for (int k = j; k < sqrt_N + j; ++k) {
                for (int l = i; l < sqrt_N + i; ++l) {
                    if (grid[k][l] == 0)
                        continue;
                    if (cur.count(grid[k][l]))
                        return false;
                    cur.insert(grid[k][l]);
                }
            }
        }
    }
    return true;
}

bool isSudokuGridValid(const vector<vector<int>> &grid) {
    set<int> target;
    for (int i = 1; i <= N; ++i) {
        target.insert(i);
    }

    // for each row
    for (int i = 0; i < N; ++i) {
        set<int> cur;
        for (int j = 0; j < N; j++) {
            cur.insert(grid[i][j]);
        }
        if (cur != target)
            return false;
    }

    // for each column
    for (int i = 0; i < N; ++i) {
        set<int> cur;
        for (int j = 0; j < N; j++) {
            cur.insert(grid[j][i]);
        }
        if (cur != target)
            return false;
    }

    // for each box
    for (int i = 0; i < N; i += sqrt_N) {
        for (int j = 0; j < N; j += sqrt_N) {
            set<int> cur;
            for (int k = j; k < sqrt_N + j; ++k) {
                for (int l = i; l < sqrt_N + i; ++l) {
                    cur.insert(grid[k][l]);
                }
            }
            if (cur != target)
                return false;
        }
    }

    return true;
}

bool solveSudokuGrid(vector<vector<int>> &grid, int x = 0, int y = 0) {
    if (x == N)
        return isSudokuGridValid(grid);

    if (y == N)
        return solveSudokuGrid(grid, x + 1, 0);

    if (grid[x][y] != 0)
        return solveSudokuGrid(grid, x, y + 1);

    for (int i = 1; i <= N; ++i) {
        grid[x][y] = i;
        if (isPartialSudokuGridValid(grid)) {
            bool res = solveSudokuGrid(grid, x, y + 1);
            if (res)
                return res;
        }
        grid[x][y] = 0;
    }

    return false;
}

void generateRandomSudoku() {
    clearSudokuGrid();
    vector<vector<int>> grid, gridCheck;
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

    do {
        grid = vector<vector<int>>(N, vector<int>(N));
        const int fill_count = rng() % N + N;

        for (int i = 0; i < fill_count; ++i) {

            int coords = uniform_int_distribution<int>(0, N * N - 1)(rng);
            int x_coord = coords / N;
            int y_coord = coords % N;
            if (grid[x_coord][y_coord] == 0) {
                for (int value = 1; value <= N; ++value) {
                    grid[x_coord][y_coord] = value;

                    if (!isPartialSudokuGridValid(grid)) {
                        grid[x_coord][y_coord] = 0;
                    }
                    else {
                        goto AC;
                    }
                }
            }
            else i--;
            AC:;
        }
        gridCheck = grid;

    } while (!solveSudokuGrid(gridCheck));

    setSudokuGrid(grid);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setFixedSize(WINDOW_L*0.75, WINDOW_W*0.75);
    this->setWindowTitle(QString::fromStdString("A " + to_string(N) + " X " + to_string(N) + " Sudoku Solver"));

    QWidget *container = new QWidget(this);
    this->setCentralWidget(container);

    QGridLayout *layout = new QGridLayout;

    initializeSudokuGrid(layout);

    QPushButton *verifySudoku = new QPushButton("Verify Sudoku");
    layout->addWidget(verifySudoku, N+1, 0, 1, N, Qt::AlignHCenter);
    QObject::connect(verifySudoku, &QPushButton::clicked, [&](){
        vector<vector<int>> grid = getSudokuGrid();
        bool valid = isPartialSudokuGridValid(grid);
        QMessageBox *Msgbox = new QMessageBox;
        if(valid) {
            Msgbox->setText("The Sudoku is valid!");
        }
        else {
            Msgbox->setText("The Sudoku is invalid!");
        }
        Msgbox->exec();
    });

    QPushButton *solveSudoku = new QPushButton("Solve Sudoku");
    layout->addWidget(solveSudoku, N+2, 0, 1, N, Qt::AlignHCenter);
    QObject::connect(solveSudoku, &QPushButton::clicked, [&](){

        vector<vector<int>> grid = getSudokuGrid();

        if (solveSudokuGrid(grid))
           setSudokuGrid(grid);
        else
            qDebug() << QString("LMAO DED");

    });

    QPushButton *generateSudoku = new QPushButton("Generate Sudoku");
    layout->addWidget(generateSudoku, N+3, 0, 1, N, Qt::AlignHCenter);
    QObject::connect(generateSudoku, &QPushButton::clicked, [&](){
        generateRandomSudoku();
    });

    QPushButton *clearSudoku = new QPushButton("Clear Sudoku");
    layout->addWidget(clearSudoku, N+4, 0, 1, N, Qt::AlignHCenter);
    QObject::connect(clearSudoku, &QPushButton::clicked, [&](){
        clearSudokuGrid();
    });

    container->setLayout(layout);
}

MainWindow::~MainWindow()
{

}

