#include <iostream>
#include <filesystem>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctime>

using namespace std;
using namespace filesystem;

static int newfork;
const int maxDepth = 10;

void print_menu(){
    cout<<"Выберите действие"<<endl;
    cout<<"1. Обратывая каталог, будет создаваться новый процесс"<<endl;
    cout<<"2. Без созданий новых процессов"<<endl;
    cout<<">";
}

void processDirectory(path dir, int depth) {
    if(depth > maxDepth){
	cout <<"Достигнута максимальная глубина рекурсии. Прерывание выполнения для каталога: "<<dir<<endl;
	return;
    }
    pid_t pid = getpid();
    cout << "Процесс ID: " << pid << ", Обрабатывающая каталог: " << dir << endl;
    if (status(dir).permissions() == perms::none) {
	    cout << "Пропуск каталога " << dir << ", ограничены права доступа" << endl;
	    return;
    }
    for (const auto& entry : directory_iterator(dir)) {
         if (entry.is_directory()) {
    		 cout<<"Найден каталог, создание нового процесса"<<endl;
		 pid_t child_pid;
	    	 if(newfork == 1)//Многопроцессорный
		 {
			 pid_t child_pid;
			 child_pid = fork();
			 if (child_pid == 0) {
				 depth++;
				 cout<<"Счетчик рекурсий: "<<depth<<endl;
				 processDirectory(entry.path(), depth);
				 return;
			 } else {
				 wait(NULL);
			 }
		 }else{ //Однопроцессорный
			depth++;
			cout<<"Счетчик рекурсий: "<<depth<<endl;
			processDirectory(entry.path(), depth);
		 }
	 } else if(entry.is_symlink()){
	  	cout << "ПРоцесс ID: " << pid << ", Файл является символической ссылкой: " << entry.path().filename() << endl;
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
    if (!exists(startDir) || !is_directory(startDir)) {
        cerr << "Ошибка: неправильно указан путь к катологу" << endl;
        return 1;
    }
    int num;
    int depth = 0;
    unsigned int start_time;
    unsigned int end_time;

    //Проверка ввода данных
    print_menu();
    cin >> num;
    switch(num){
	    case 1:
		    newfork = 1;
		    start_time = clock();
		    processDirectory(startDir,depth);
		    end_time = clock();
		    break;
	    case 2:
		    newfork = 0;
		    start_time = clock();
		    processDirectory(startDir,depth);
		    end_time = clock();
		    break;
	    default:
		    cout<<"Выход из программы"<<endl;
		    break;
    }


    unsigned int time;
    time = end_time - start_time;
    cout<<"Время "<<time<<endl;
    return 0;
}

