#include <iostream>
#include <random>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <sstream>
#include <stack>
#include <time.h>
using namespace std;



class game
{
private:
    int build[110][110];
    int flagged[110][110];
    int init_flag;
    int row, column;
    bool first_move;
    bool finished;
    string log_str;
    int board_created;


    pair<int, int> processInput(const string &input)
    {
        stringstream ss(input);
        char c;
        int num1, num2;
        ss >> c;
        if (!isdigit(c))
        {
            ss >> num1 >> num2;
        }
        else
        {
            ss.unget();
            ss >> num1 >> num2;
        }
        return make_pair(num1, num2);
    }


    int make_move(const int &n, const int &m, const char &ch)
    { // flagged cant reveal for flag = -4, unflagged = -5,
        log_str = "";
        if (check_valid(n, m) == -3)
        {
            log_str = " Invalid Place";
            return check_valid(n, m);
        }
        else if (check_valid(n, m) == -2 && ch == 'x')
        {
            return check_valid(n, m);
        }
        if (first_move == 0 && ch == 'f')
        {
            if (flagged[n - 1][m - 1])
            {
                flagged[n - 1][m - 1] = 0;
                log_str = " Flag Removed";
                return -5;
            }
            else if (check_valid(n, m) == -1 || check_valid(n, m) == -2)
            {
                flagged[n - 1][m - 1] = 1;
                log_str = " Flag Placed";
                return -4;
            }
            log_str = " Invalid Place for Flag";
        }
        if (first_move == 1)
        {
            int reveal, cnt = 0;
            first_move = 0;
            int best_reveal = -1;
            int best_build[110][110];
            for (int i = 0; i < 50; i++)
            {
                build_game(row, column);
                int reveal = make_reveal(n, m);
                if (best_reveal < reveal || reveal > round(row * column * 1.0 / 6))
                {
                    best_reveal = reveal;
                    for (int j = 0; j < row; j++)
                    {
                        for (int k = 0; k < column; k++)
                        {
                            best_build[j][k] = build[j][k];
                        }
                    }
                }
            }
            for (int i = 0; i < row; i++)
            {
                for (int j = 0; j < column; j++)
                {
                    build[i][j] = best_build[i][j];
                }
            }
            log_str = "Revealed";
            return best_reveal;
        }
        else if (ch == 'x')
        {
            if (flagged[n - 1][m - 1] == 1)
            {
                log_str = "Can not reveal flagged place";
                return -4;
            }
            else if (check_valid(n, m) != -1)
            {
                if (check_valid(n, m) >= 0)
                    log_str = "Can not reveal a revealed place";
                return check_valid(n, m);
            }
            else
                log_str = "Revealed";
            return make_reveal(n, m);
        }
        return 0;
    }


    int check_valid(const int &n, const int &m)
    { // flagged = -4, invalid = -3, mine = -2, unreveal = -1, counted >= 0
        if (n >= 1 && row >= n && m >= 1 && column >= m)
        {
            if (build[n - 1][m - 1] == -2)
                return build[n - 1][m - 1];
            if (flagged[n - 1][m - 1] == 1)
                return -4;
            return build[n - 1][m - 1];
        }
        else
            return -3;
    }


    int count_around(const int &n, const int &m)
    { // invalid = -3, mine = -2, count around >=0
        int checked = check_valid(n, m);

        if (checked == -1)
        {
            int fx[] = {+0, +0, +1, -1, -1, +1, -1, +1};
            int fy[] = {-1, +1, +0, +0, +1, +1, -1, -1};
            int cnt = 0;
            for (int i = 0; i < 8; i++)
                if (check_valid(n + fx[i], m + fy[i]) == -2)
                    cnt++;
            return cnt;
        }
        else
            return checked;
    }


    int make_reveal(const int n, const int m)
    { // count reveal
        if (check_valid(n, m) != -1 || flagged[n - 1][m - 1])
            return 0;
        else
        {
            int fx[] = {+0, +0, +1, -1, -1, +1, -1, +1};
            int fy[] = {-1, +1, +0, +0, +1, +1, -1, -1};
            int cnt = 0;
            for (int i = 0; i < 8; i++)
                if (check_valid(n + fx[i], m + fy[i]) == -2)
                    cnt++;
            if (cnt == 0)
            {
                build[n - 1][m - 1] = 0;
                for (int i = 0; i < 8; i++)
                    cnt += make_reveal(n + fx[i], m + fy[i]);
                return cnt + 1;
            }
            else
            {
                build[n - 1][m - 1] = cnt;
                return 1;
            }
        }
        return 0;
    }


    void build_game(const int &n, const int &m)
    {
        row = n;
        column = m;
        int temp[n * m + 2] = {0};
        for (int i = 0; i < init_flag; i++)
        {
            temp[i] = 1;
        }
        unsigned seed = rand();
        shuffle(temp, temp + n * m, default_random_engine(seed));
        // setting mine = -2, unreveal = -1
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                flagged[i][j] = 0;
                build[i][j] = -temp[i * m + j] - 1;
            }
        }
    }


    int create_board_size()
    { // exit game = -200
        while (1)
        {
            clear_text();
            string st;
        up:
            cout << "1. Small (10x10)\n2. medium (10x10)\n3. Large (40x40)\n4. Custom size\n\n\nEnter board size:";
            cin >> st;
            if (st[0] == 'e' && st[1] == 'x' && st[2] == 'i' && st[3] == 't')
            {
                return -200;
            }
            if (st[0] == 'h' && st[1] == 'e' && st[2] == 'l' && st[3] == 'p')
            {
                clear_text(1, 0, 1);
                goto up;
            }
            if (st == "1")
            {
                row = 10;
                column = 10;
                break;
            }
            else if (st == "2")
            {
                row = 20;
                column = 20;
                break;
            }
            else if (st == "3")
            {
                row = 40;
                column = 40;
                break;
            }
            else if (st == "4")
            {
                clear_text(0, 0);
                cout << "Enter 0 to go back." << endl;
                cout << "Enter number of row and column:";
                cin >> row;
                if (row == 0)
                    clear_text();
                else
                {
                    cin >> column;
                    if (5 <= row && 5 <= column && row <= 99 && column <= 99)
                    {
                        clear_text();
                        break;
                    }
                    else
                    {
                        clear_text();
                        cout << "Number of row and column must be between 5 and 99\n"
                             << endl;
                    }
                }
            }
        }

        while (1)
        {
            string st;
            clear_text();
        up2:
            cout << "Enter 0 to go back." << endl;
            cout << "\n1. easy\n2. normal\n3. hard\n\nEnter hardness of the game:";
            cin >> st;
            if (st[0] == 'e' && st[1] == 'x' && st[2] == 'i' && st[3] == 't')
            {
                return -200;
            }
            if (st[0] == 'h' && st[1] == 'e' && st[2] == 'l' && st[3] == 'p')
            {
                clear_text(1, 0, 1);
                goto up2;
            }
            if (st == "1")
            {
                init_flag = round(row * column * 10.0 / 100);
                break;
            }
            else if (st == "2")
            {
                init_flag = round(row * column * 20.0 / 100);
                break;
            }
            else if (st == "3")
            {
                init_flag = round(row * column * 35.0 / 100);
                break;
            }
            else if (st == "0")
            {
                clear_text();
                int ifexit = create_board_size();
                if (ifexit == -200)
                {
                    return -200;
                }
                break;
            }
        }

        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
                build[i][j] = -1;
        }
        return 0;
    }


    void print_board()
    {
        int reveal = 0;
        cout << "   ";
        for (int j = 0; j < column; j++)
        {
            if (j <= 9)
                cout << ' ';
            cout << j + 1 << " ";
        }
        cout << endl;
        cout << "   ";
        for (int j = 0; j < column; j++)
            cout << "---";
        cout << endl;
        for (int i = 0; i < row; i++)
        {
            if (i < 9)
                cout << ' ';
            cout << i + 1 << " ";
            cout << "|";
            for (int j = 0; j < column; j++)
            {
                if (!reveal && flagged[i][j])
                    cout << "F ";
                else if (build[i][j] == -1)
                    cout << "  ";
                else if (build[i][j] == 0)
                    cout << ". ";
                else if (build[i][j] == -2)
                {
                    if (reveal)
                        cout << "\u2588\u2588";
                    else
                        cout << "  ";
                }
                else
                    cout << build[i][j] << " ";
                cout << "|";
            }
            cout << endl
                 << "   ";
            for (int j = 0; j < column; j++)
                cout << "---";
            cout << endl;
        }
        cout << "   ";
        for (int j = 0; j < column; j++)
        {
            if (j <= 9)
                cout << ' ';
            cout << j + 1 << " ";
        }
        cout << endl
             << endl;
    }


    void print_board(int reveal)
    {
        cout << "   ";
        for (int j = 0; j < column; j++)
        {
            if (j <= 9)
                cout << ' ';
            cout << j + 1 << " ";
        }
        cout << endl;
        cout << "   ";
        for (int j = 0; j < column; j++)
            cout << "---";
        cout << endl;
        for (int i = 0; i < row; i++)
        {
            if (i < 9)
                cout << ' ';
            cout << i + 1 << " ";
            cout << "|";
            for (int j = 0; j < column; j++)
            {
                if (!reveal && flagged[i][j])
                    cout << "F ";
                else if (build[i][j] == -1)
                    cout << "  ";
                else if (build[i][j] == 0)
                    cout << ". ";
                else if (build[i][j] == -2)
                {
                    if (reveal)
                        cout << "\u2588\u2588";
                    else
                        cout << "  ";
                }
                else
                    cout << build[i][j] << " ";
                cout << "|";
            }
            cout << endl
                 << "   ";
            for (int j = 0; j < column; j++)
                cout << "---";
            cout << endl;
        }
        cout << "   ";
        for (int j = 0; j < column; j++)
        {
            if (j <= 9)
                cout << ' ';
            cout << j + 1 << " ";
        }
        cout << endl
             << endl;
    }


public:
    game()
    {
        srand(time(0));
        create_game();
    }


    void create_game()
    {
        if (board_created == 1)
            return;
        srand(time(0));
        board_created = 1;
        first_move = 1;
        finished = 0;
        for (int i = 0; i <= 100; i++)
        {
            for (int j = 0; j < 100; j++)
            {
                build[i][j] = 0;
                flagged[i][j] = 0;
            }
        }
        int ifexit = create_board_size();
        if (ifexit == -200)
        {
            board_created = 0;
            clear_text(0, 0);
            return;
        }
        clear_text(1, 0);
    }


    int if_created()
    { // 1 = created, 0 = not created
        if (board_created == 1)
            return 1;
        return 0;
    }


    int play()
    { // win = 1, lost = -1, exit = 0, invalid = -2
        if (board_created != 1)
        {
            clear_text(0, 0);
            cout << "Game is not created\n"
                 << endl
                 << "Call create_game() first\n";
            board_created = 0;
            return -2;
        }
        int total_revealed = 0;
        int total_flagged = 0;
        string last_input = "";
        int last_log;
        int cntt = 0;
        while (1)
        {
            clear_text(1, 0);
            if (last_input.size() > 1)
            {
                cout << "Last Input: " << last_input << "  " << log_str << endl
                     << endl;
            }
            print_board();
            cout << "You have left total '" << init_flag - total_flagged << "' flag" << endl;
            cout << "Enter your move: 'f <row> <column>' for flag or unflag. '<row> <column>' for dig:";
            string st;
            char ch = 'x';
            while (1)
            {
                getline(cin, st);
                if (st[0] == 'e' && st[1] == 'x' && st[2] == 'i' && st[3] == 't')
                {
                    clear_text(0, 0);
                    board_created = 0;
                    return 0;
                }
                if (st.size() < 3)
                {
                    clear_text(1, 0);
                    if (!first_move)
                        cout << "Invalid Input" << endl
                             << endl;
                    print_board();
                    cout << "You have left total '" << init_flag - total_flagged << "' flag" << endl;
                    cout << "Enter your move: 'f <row> <column>' for flag or unflag. '<row> <column>' for dig:";
                }
                else
                    break;
            }
            int n, m;
            last_input = "";
            if (st[0] == 'f' || st[0] == 'f')
            {
                ch = 'f';
                last_input += ch;
            }
            auto input = processInput(st);
            processInput("0 0");
            n = input.first;
            m = input.second;
            last_input += " " + to_string(n) + " " + to_string(m);
            last_log = make_move(n, m, ch);
            if (last_log == -2)
            {
                clear_text(0, 0);
                print_board(1);
                cout << endl
                     << "You Lost the Game :(" << endl
                     << endl
                     << "You have revealed " << round(total_revealed * 100. / (row * column)) << "% of the map." << endl;
                board_created = 0;
                return -1;
            }
            else if (last_log == -4)
            {
                total_flagged++;
                if (total_flagged > init_flag)
                {
                    make_move(n, m, ch);
                    total_flagged--;
                    cout << "Not enough flag left" << endl;
                }
            }
            else if (last_log == -5)
            {
                total_flagged--;
            }
            else if (last_log > 0)
                total_revealed += last_log;
            if (row * column - init_flag == total_revealed)
            {
                clear_text(0, 0);
                print_board(1);
                cout << "Congratulations you won the game.\n\n";
                board_created = 0;
                return 1;
            }
        }
        board_created = 0;
        return -2;
    }


    void clear_text(int no_print = 1, int ishelp = 1, int help = 0)
    {
    #ifdef linux
        system("clear");
    #endif
    #ifdef _WIN32
        system("cls");
    #endif
        if (no_print)
            cout << "Type 'exit' to exit the game\n";
        if (ishelp)
            cout << "Type 'help' to get help\n";
        if (help)
            cout << "Welcome to Minesweeper!\n\nHow to Play:\n- Use the '<x> <y>' command to reveal a cell. Syntax: '<x> <y>', where x and y are the coordinates of the cell you want to dig.\n- Use the 'f <x> <y>' command to place or remove a flag on a cell. Syntax: 'f <x> <y>', where x and y are the coordinates of the cell you want to flag or unflag.\n- Example: 'f 3 4' to place or remove a flag at position (3, 4).\n\nGame Rules:\n- The game board consists of a grid of cells. Some cells contain mines.\n- Your objective is to reveal all cells that do not contain mines without detonating any mines.\n- If you reveal a mine, the game ends.\n- Cells adjacent to mines show the number of neighboring mines. Use this information to deduce where mines are located.\n- You can place flags on cells you suspect contain mines to help keep track of them.\n\nCommands:\n- '<x> <y>': Reveals the content of the cell at position (x, y).\n- 'f <x> <y>': Places or removes a flag on the cell at position (x, y).\n- 'help': Displays this help message.\n- 'exit': Quits the game.\n\nHave fun playing Minesweeper!\n";
        cout << endl << endl;
    }


    ~game()
    {
        board_created = 0;
        for (int i = 0; i <= 100; i++)
        {
            for (int j = 0; j < 100; j++)
            {
                build[i][j] = 0;
                flagged[i][j] = 0;
            }
        }
    }
};

int main()
{

    string st;
    game p;
    p.clear_text();
    do
    {
        p.create_game();
        if (p.if_created())
            p.play();
        cout << "1.Play again?\nEnter any other key to exit:";
        cin >> st;
    } while (st == "1");
}