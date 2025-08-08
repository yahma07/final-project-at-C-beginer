// final report.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。

#include <iostream>
#include <vector>
#include <random>   // 乱数のために使用
#include <ctime>    // 乱数のシードのために使用
#include <stdexcept>

// マス一つを表すクラス
class Cell {
public:
    bool isMine;
    bool isOpen;
    bool isFlagged;
    int adjacentMines;

    // コンストラクタ (初期化)
    Cell() : isMine(false), isOpen(false), isFlagged(false), adjacentMines(0) {}
};

// ゲーム盤全体を管理するクラス
class Board {
private:
    int width;
    int height;
    int numMines;
    std::vector<std::vector<Cell>> grid;
    bool isGameOver;
    bool isWin;

    // 隣接するマスの地雷数を計算する
    void calculateAdjacentMines() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y][x].isMine) continue;

                int count = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = x + dx;
                        int ny = y + dy;
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height && grid[ny][nx].isMine) {
                            count++;
                        }
                    }
                }
                grid[y][x].adjacentMines = count;
            }
        }
    }

    // マスを開いたときに、隣接する地雷が0なら連鎖して開く (再帰関数)
    void openAdjacentCells(int x, int y) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    openCell(nx, ny);
                }
            }
        }
    }


public:
    // コンストラクタ
    Board(int w, int h, int mines) : width(w), height(h), numMines(mines), isGameOver(false), isWin(false) {
        grid.resize(height, std::vector<Cell>(width));
        placeMines();
        calculateAdjacentMines();
    }

    // 地雷をランダムに配置する
    void placeMines() {
        std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
        std::uniform_int_distribution<int> dist_w(0, width - 1);
        std::uniform_int_distribution<int> dist_h(0, height - 1);

        int mines_placed = 0;
        while (mines_placed < numMines) {
            int x = dist_w(rng);
            int y = dist_h(rng);
            if (!grid[y][x].isMine) {
                grid[y][x].isMine = true;
                mines_placed++;
            }
        }
    }

    // 盤面を表示する
    void display() const {
        std::cout << "  ";
        for (int x = 0; x < width; ++x) {
            std::cout << x << " ";
        }
        std::cout << std::endl;

        for (int y = 0; y < height; ++y) {
            std::cout << y << " ";
            for (int x = 0; x < width; ++x) {
                const auto& cell = grid[y][x];
                if (cell.isFlagged) {
                    std::cout << "P "; // Flag
                }
                else if (!cell.isOpen) {
                    std::cout << ". "; // Closed
                }
                else if (cell.isMine) {
                    std::cout << "* "; // Mine
                }
                else {
                    std::cout << cell.adjacentMines << " ";
                }
            }
            std::cout << std::endl;
        }
    }

    // マスを開く
    void openCell(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height || grid[y][x].isOpen || grid[y][x].isFlagged) {
            return;
        }

        grid[y][x].isOpen = true;

        if (grid[y][x].isMine) {
            isGameOver = true;
            isWin = false;
        }
        else if (grid[y][x].adjacentMines == 0) {
            openAdjacentCells(x, y);
        }
        checkWinCondition();
    }

    // 旗を立てる/取り除く
    void flagCell(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height || grid[y][x].isOpen) {
            return;
        }
        grid[y][x].isFlagged = !grid[y][x].isFlagged;
    }

    // 勝利条件をチェックする (ここでイテレータを使用)
    void checkWinCondition() {
        int open_count = 0;
        // イテレータを使って盤面のセルを走査
        for (auto row_it = grid.begin(); row_it != grid.end(); ++row_it) {
            for (auto cell_it = row_it->begin(); cell_it != row_it->end(); ++cell_it) {
                if (cell_it->isOpen) {
                    open_count++;
                }
            }
        }
        if (open_count == width * height - numMines) {
            isGameOver = true;
            isWin = true;
        }
    }

    bool isGameFinished() const { return isGameOver; }
    bool hasPlayerWon() const { return isWin; }
};

// ゲーム全体の流れを管理する
void playGame() {
    Board board(10, 10, 10); // 10x10の盤面、地雷10個でゲーム開始

    while (!board.isGameFinished()) {
        board.display();
        std::cout << "\n操作を選択してください (o: 開く, f: 旗を立てる): ";
        char action;
        int x, y;
        std::cin >> action >> x >> y;

        if (action == 'o') {
            board.openCell(x, y);
        }
        else if (action == 'f') {
            board.flagCell(x, y);
        }
        else {
            std::cout << "無効な操作です。" << std::endl;
        }
    }

    // ゲーム終了後の結果表示
    board.display();
    if (board.hasPlayerWon()) {
        std::cout << "\nおめでとうございます！ゲームクリアです！" << std::endl;
    }
    else {
        std::cout << "\n残念！地雷を踏んでしまいました。" << std::endl;
    }
}

int main() {
    try {
        playGame();
    }
    catch (const std::exception& e) {
        std::cerr << "エラーが発生しました: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
