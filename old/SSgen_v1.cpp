#include<iostream>
#include<filesystem>
#include<fstream>
#include<ctime>
#include <sstream>
#include<cmath>

// #include <windows.h>

namespace fs = std::filesystem;
std::string dirForCopy (std::string);               // Запрашиваем директорию для генерирования
bool validNum (std::string);                        // Проверка числа
std::string doubleToString(double &, int &);              // Преобразование числа в нужный вид
std::string textEdit (std::string);                 // Функция редактирования содержимого


// Глобальные переменные
long long freqStart = 0, freqStop = 0, freqSpan = 0;
int valueRBW = 0, valueVBW = 0;
int valuePoint = 0, numPoint = 0, numTrace = 0;
long long firstWLong = 0;
std::string sep = "";
fs::path fileNew;


int main (int argc, char* argv[]) {
    setlocale(LC_ALL, "RUS");
    fs::path pathStart, dirGen, dirLv1, dirLv2;
    std::ifstream fileSourse;
    std::ofstream fileResult;    

    try {        
        pathStart = argv[0];
        pathStart = pathStart.parent_path();        
        std::string tmp = dirForCopy("Specify the directory: ");
        dirGen = pathStart;
        pathStart.append(tmp);
        dirGen.append(tmp + "-generat"); 

    } catch(const std::exception& e) {
        std::cerr << "Cyrillic is not allowed in the names of folders and files!" << '\n';
        std::cerr << "The program will be closed!" << std::endl;
        return -1;
    }       

    try {
        fs::create_directory(dirGen);
        
        for (const auto & entry : fs::directory_iterator(pathStart)) {

            if (entry.is_directory()) {                
                dirLv1 = dirGen;
                dirLv1 /= entry.path().filename();          // Извлекаем название вложенной папки 1 уровень
                fs::create_directory(dirLv1);               // Создаем копию папки в генерируемой папке

                for (const auto & entry_2 : fs::directory_iterator(entry)) {
                    dirLv2 = dirLv1;
                    dirLv2 /= entry_2.path().filename();    // Извлекаем название вложенной папки 2 уровень
                    fs::create_directory(dirLv2);           // Создаем копию папки в генерируемой папке
                    
                    for (const auto & fileList : fs::directory_iterator(entry_2)) {
                        fileNew = dirLv2;
                        fileNew /= fileList.path().filename();
                        // Обнуляем глобальные переменные перед открытием очередного файла
                        freqStart = 0; freqStop = 0; freqSpan = 0;
                        valueRBW = 0; valueVBW = 0;

                        fileSourse.open(fileList.path(), std::ios::in | std::ios::binary);
                        fileResult.open(fileNew, std::ios::app | std::ios::binary);
                        
                        std::string text = "";

                        while (!fileSourse.eof()) {
                            
                            std::getline(fileSourse, text);                            

                            text = textEdit(text);

                            // Если возникнет ошибка в обработке файла, выводим текст, например "error:Файл _____ неверно указана частота." и завершаем программу.
                            if (text.substr(0, 5) == "error") {
                                std::cout << text.substr(6, text.size() - 7) << std::endl;
                                system("pause");
                                return -1;
                            }                            
                            fileResult << text;          // Обработка строки
                        }                        
                        fileSourse.close();
                        fileResult.close();                     
                    }                    
                }
            }             
        }

    } catch(const std::exception& e) {
        std::cerr << "An unexpected error has occurred!" << e.what() << std::endl;
        std::cerr << "The program will be closed!" << std::endl;
        return -1;
    }
    std::cout << "The program has done its job." << std::endl;
    system("pause");
    return 0;
}

// ********************

// Запрашиваем директорию для генерирования
std::string dirForCopy (std::string inTxt) {
    std::string resTxt = "";

    do {
        std::cout << std::endl << inTxt;
        std::getline(std::cin, resTxt);

        if(resTxt.empty()) {
            std::cerr << std::endl << "You forgot to specify the directory!" <<  std::endl;
        }
    } while(resTxt.empty());

    return resTxt;
}

// Преобразование числа в нужный вид
std::string doubleToString(double &number, int &precision) {
    std::ostringstream stream;
    stream.precision(precision);
    stream << std::fixed << number;
    return stream.str();
}

// Проверка числа
bool validNum (std::string inTxt) {
    bool res = (inTxt.empty()) ? false: true;  
    int point = 0;  
    
    for(int i = 0; i < inTxt.length(); i++){  
        if(i == 0 && inTxt[i] == '-'){
            res &= true;
            continue;
        } else if(point == 0 && inTxt[i] == '.'){
            res &= true;
            point++;
            continue;
        } 
        res &= inTxt[i] >= '0' && inTxt[i] <= '9';
    }

    return res;
}

// Функция редактирования содержимого
std::string textEdit (std::string inTxt) {
    std::string firstWord = "", secondWord = "", remains = "";  
    double tempNum = 0;
    int numAftePoint = 0;

    if (inTxt.find("sep") != std::string::npos) {
        sep = inTxt.substr(4, 1);  
        return inTxt;
    } 

    // Разбор по словам
    int indF = inTxt.find(sep);
    int indS = inTxt.find_last_of(sep);
    firstWord = inTxt.substr(0, indF);
    indF++;
    secondWord = inTxt.substr(indF, indS - indF);
    indS++;
    remains = inTxt.substr(indS, inTxt.find("\0") - indS);

    // Обработка даты
    if(firstWord == "Date") {       
        std::time_t curTime = time(0);
        std::string tt = ctime(&curTime);
        secondWord = tt.substr(8, 2) + "." + tt.substr(4, 3) + " " + tt.substr(20, 4);        
    }

    // Заполняем частоты
    if (firstWord == "Start" || firstWord == "Stop" || firstWord == "Span") {        
        freqStart = (firstWord == "Start" && validNum(secondWord)) ? std::stoll(secondWord) : freqStart;
        freqStop = (firstWord == "Stop" && validNum(secondWord)) ? std::stoll(secondWord) : freqStop;
        freqSpan = (firstWord == "Span" && validNum(secondWord)) ? std::stoll(secondWord) : freqSpan;
    }

    // Значение RBW, VBW
    if (firstWord == "RBW" || firstWord == "VBW") {
        valueRBW = (firstWord == "RBW" && validNum(secondWord)) ? std::stoi(secondWord) : valueRBW;
        valueVBW = (firstWord == "VBW" && validNum(secondWord)) ? std::stoi(secondWord) : valueVBW;        
    }
    
    // Проверим начальные установки
    if (firstWord == "y-Unit") {
        bool valid = true;

        if (freqStart == 9100) {
            valid &= (freqStop == 149900);            
            valid &= (freqSpan == freqStop - freqStart);
            valid &= (valueRBW == 200);
            valid &= (valueVBW == 2000);            
            
        } else if (freqStart == 154500) {
            valid &= (freqStop == 29998500);            
            valid &= (freqSpan == freqStop - freqStart);
            valid &= (valueRBW == 9000);
            valid &= (valueVBW == 100000);            
            
        } else if (freqStart == 30060000) {
            valid &= (freqStop == 999900000);            
            valid &= (freqSpan == freqStop - freqStart);
            valid &= (valueRBW == 120000);
            valid &= (valueVBW == 1000000);

        } else if (freqStart == 1000200000) {
            valid &= (freqStop == 5999400000);            
            valid &= (freqSpan == freqStop - freqStart);
            valid &= (valueRBW == 1000000);
            valid &= (valueVBW == 10000000);
            
        } else if (freqStart == 1000500000) {
            valid &= (freqStop == 5999500000);            
            valid &= (freqSpan == freqStop - freqStart);
            valid &= (valueRBW == 1000000);
            valid &= (valueVBW == 10000000);
        } else {
            valid = false;
        }
        // Выводим сообщение если данные не верны
        if (!valid) {
            return "error:Check the initial settings!";
        }            
    }

    if (firstWord == "Trace" && validNum(secondWord)) {
        valuePoint = 0;
        numPoint = 0;
        numTrace = std::stoi(secondWord);
    }

    if (firstWord == "Values") {
        valuePoint = (validNum(secondWord)) ? std::stoi(secondWord) : valuePoint;
    }

    if (validNum(firstWord)) {        

        // Обработка значений вида 10e-06 
        if (secondWord.find("e-") != std::string::npos) {// != std::string::npos) {
            int indF = secondWord.find("e-"), i = 0;
            std::string swOne = secondWord.substr(0, indF), tmpTxt = "";
            int swPow = std::stoi(secondWord.substr(indF + 2, 2)) - 1;

            if (swOne[0] == '-') {
                    tmpTxt.push_back('-');
                    i++;
            }
            tmpTxt.append("0.");

            for(int k = 0; k < swPow; k++) {
                tmpTxt.push_back('0');
            }
            
            for(i; i < swOne.size(); i++) {
                if (swOne[i] != '.') {
                    tmpTxt.push_back(swOne[i]);
                }            
            }                        
            secondWord = tmpTxt;            
        }
        
        numAftePoint = secondWord.size() - secondWord.find(".") - 1;        
                                     
        // Подсчитываем количество записей в Trace
        if (validNum(secondWord)) {
            
            std::istringstream doublef(secondWord);
            doublef >> tempNum;            
            doublef.str("");                                   // Обнуление потока             

            tempNum *= (rand() % 5 + 98) * 0.01;              // Изменяем значение (rand() % (102 - 98 + 1) + 98) * 0.01;

            secondWord = doubleToString(tempNum, numAftePoint);

        } else {
            return "error:Error in reading amplitude values at frequencies " + firstWord + "! \nFile: " + fileNew.string();
        }
        numPoint++;
        // Проверяем правильность заполнения трейсов
        if (numPoint == 0 && firstWord != std::to_string(freqStart)) {                   // Проверяем точку начала
            return "error:Check the starting frequency value! Trace " + std::to_string(numTrace) + "! \nFile: " + fileNew.string();

        } else if (numPoint == valuePoint && firstWord != std::to_string(freqStop)) {     // Проверяем точку останова
            return "error:Check the value of the final frequency! Trace " + std::to_string(numTrace) + "!\n" + fileNew.string();
            
        } else if (numPoint != valuePoint && firstWord == std::to_string(freqStop)) {     // Проверяем количество строк в трейсе
            return "error:The number of calculation points differs from the number of lines in Trace " + std::to_string(numTrace) + "!\n" + fileNew.string();
        }
    }
    
    if (inTxt == "") {
        return inTxt;
    } 

    return (remains == "") ? firstWord + "," + secondWord + "," : firstWord + "," + secondWord + "," + remains;
}
