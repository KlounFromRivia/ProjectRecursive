#include <iostream>
#include <filesystem>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
using namespace filesystem;

void processDirectory(path dir, int depth) {
    pid_t pid = getpid();
    cout << "Процесс ID: " << pid << ", Обрабатывающая каталог: " << dir << endl;
    for (const auto& entry : directory_iterator(dir)) {
        if (entry.is_directory()) {
            cout<<"Найден каталог, создание нового процесса"<<endl;
            pid_t child_pid = fork();
            if (child_pid == 0) {
                depth++;
                cout<<"Счетчик рекурсий: "<<depth<<endl;
                processDirectory(entry.path(), depth);
                exit(0);
            }else{
                wait(NULL);
            }
        } else {
            cout << "ПРоцесс ID: " << pid << ", Имя файла: " << entry.path().filename() << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Введите: " << argv[0] << " <Путь каталога>" << endl;
        return 1;
    }

    path startDir(argv[1]);
    int depth = 0;
    if (!exists(startDir) || !is_directory(startDir)) {
        cerr << "Ошибка: неправильно указан путь к катологу" << endl;
        return 1;
    }

    processDirectory(startDir, depth);

    return 0;
}


