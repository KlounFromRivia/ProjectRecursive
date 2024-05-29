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
static int maxProcess;

void print_menu(){
    cout<<"Выберите действие"<<endl;
    cout<<"1. Обратывая каталог, будет создаваться новый процесс"<<endl;
    cout<<"2. Без созданий новых процессов"<<endl;
    cout<<">";
}

void processDirectory(path dir, int depth, int countP) {

    if(countP >= maxProcess){//ПРоверка лимита процессов
	cout <<"Достигнуто максимальное кол-во дочерних процессов. Прерывание выполнения для каталога: "<<dir<<endl;
        return;
    }

    pid_t pid = getpid();
    cout << "Процесс ID: " << pid << ", Процесс №" << countP <<", Обрабатывающая каталог: " << dir << endl;

    //ПРоверка доступа
    if ((status(dir).permissions() & perms::owner_read) == perms::none ||
    (status(dir).permissions() & perms::owner_exec) == perms::none ||
    (status(dir).permissions() & perms::others_read) == perms::none ||
    (status(dir).permissions() & perms::others_exec) == perms::none)
    {
            cout << "Пропуск каталога " << dir << ", ограничены права доступа" << endl;
            return;
    }

    if(depth > maxDepth){//ПРоверка лимита рекурсии
	cout <<"Достигнута максимальная глубина рекурсии. Прерывание выполнения для каталога: "<<dir<<endl;
	return;
    }

    for (const auto& entry : directory_iterator(dir)) {//Обработка каталога
         if (entry.is_directory() && !entry.is_symlink()) {
    		 cout<<"Найден каталог, создание нового процесса"<<endl;
		 pid_t child_pid;
	    	 if(newfork == 1)//Многопроцессорный
		 {
			 child_pid = fork();
		 }else{
			 child_pid = -1;
		 }
		 if (child_pid == 0) {
			 countP++;
			 depth++;
			 cout<<"Счетчик рекурсий: "<<depth<<endl;
			 processDirectory(entry.path(), depth, countP);
			 depth--;
			 return;
		 } else if(child_pid > 0){
			 continue;
		 }else{ //Однопроцессорный
			depth++;
			cout<<"Счетчик рекурсий: "<<depth<<endl;
			processDirectory(entry.path(), depth, countP);
			depth--;
		 }
	 } else if(entry.is_symlink()){
	  	cout << "ПРоцесс ID: " << pid << ", Cимволическая ссылка: " << entry.path().filename() << endl;
	 } else {
           cout << "ПРоцесс ID: " << pid << ", Имя файла: " << entry.path().filename() << endl;
         }
    }
    for(int i = 0; i < countP; i++){
	    wait(NULL);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Введите: " << argv[0] << " <Путь каталога>" << "<число, лимит созданых процессов>" <<endl;
        return 1;
    }

    path startDir(argv[1]);
    if (!exists(startDir) || !is_directory(startDir)) {
        cerr << "Ошибка: неправильно указан путь к катологу" << endl;
        return 1;
    }

    maxProcess = atoi(argv[2]);
    if (!maxProcess || maxProcess <= 0){
	    cerr << "Ошибка: неправильно указано лимит" << endl;
	    return 1;
    }
    int num;
    int depth = 0;
    int currectProcess = 0;

    //Проверка ввода данных
    print_menu();
    cin >> num;
    //unsigned int start_time = clock();
    switch(num){
	    case 1:
		    newfork = 1;
		    processDirectory(startDir,depth,currectProcess);
		    break;
	    case 2:
		    newfork = 0;
		    processDirectory(startDir,depth,currectProcess);
		    break;
	    default:
		    cout<<"Выход из программы"<<endl;
		    break;
    }
    //unsigned int end_time = clock();
    //unsigned int time = end_time - start_time;
    //cout<<"Время "<<time<<endl;

    return 0;
}

