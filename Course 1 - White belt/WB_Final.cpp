#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <exception>
#include <map>
#include <set>

using namespace std;

//=================================================================================
class Date {
public:    
    // Конструктор даты
    Date(const string& s) {
        stringstream stream(s);        
        char c1, c2;   
        // формат ввода => целое число/символ/целое число/символ/целое число
        // только, если данные будут введены в таком порядке и такого типа поток вернет бит 1 и будет хорошим
        stream >> year >> c1 >> month >> c2 >> day;
        // проверяем: символы, состояние потока, конец потока (на случай мусора после числа)
        if ((c1 != '-') or (c2 != '-') or (!stream) or (!stream.eof())) {
            throw runtime_error("Wrong date format: " + s);
        }        
        if ((month < 1) or (month > 12)) {
            throw runtime_error("Month value is invalid: " + to_string(month));
        }        
        if ((day < 1) or (day > 31)) {
            throw runtime_error("Day value is invalid: " + to_string(day));
        }                
    }
    int GetYear() const {
        return year;
    }
    int GetMonth() const {
        return month;
    }
    int GetDay() const {
        return day;
    }
    // Возвращает значение даты в виде отформатированной строки "YYYY-MM-DD"
    string GetDate() const {        
        stringstream stream;        
        stream << setw(4) << setfill('0') << year << "-" 
            << setw(2) << setfill('0') << month << "-" 
            << setw(2) << setfill('0') << day;        
        return stream.str();
    }
private:
    int year;
    int month;
    int day;    
};
//=================================================================================
// Перегруженный оператор "< меньше" для сравнения двух объектов класса Date
bool operator<(const Date& lhs, const Date& rhs) {
    int ldate = lhs.GetYear() * 10000 + lhs.GetMonth() * 100 + lhs.GetDay();
    int rdate = rhs.GetYear() * 10000 + rhs.GetMonth() * 100 + rhs.GetDay();
    return (ldate < rdate);
}
//=================================================================================
// Перегруженный оператор "== сравнение" для сравнения двух объектов класса Date
bool operator==(const Date& lhs, const Date& rhs) {
    return (lhs.GetYear() == rhs.GetYear()) and (lhs.GetMonth() == rhs.GetMonth())
        and (lhs.GetDay() == rhs.GetDay());
}
//=================================================================================
class Database {
public:        
    void AddEvent(const Date& date, const string& event) {
        // Добавление события в БД
        data[date].insert(event);
    }
    bool DeleteEvent(const Date& date, const string& event) {
        // Удаление события из БД
        // если заданная дата есть в БД и заданное событие есть в БД - удаляем
        if ((data.count(date) > 0) and (data[date].count(event) > 0)) {
            data[date].erase(event);                
            return true;
        }
        return false;        
    }
    int  DeleteDate(const Date& date) {
        // Удаление всех событий за указанную дату из БД
        if (data.count(date) == 0) {                // заданной даты нет в БД возвращаем 0
            return 0;
        }
        const int result = data[date].size();
        data.erase(date);
        return result;
    }

    set<string> Find(const Date& date) const {
        // Поиск множества событий на указанную дату        
        if (data.count(date) > 0) {       
            // метод метод at: в случае отсутствия ключа выбросит исключение 
            // и потому может бытьиспользован для константного объекта
            return data.at(date);                   
        }
        // возвращаем пустое множество
        return {};                                
    }

    void Print() const {
        for (auto& record : data) {
            for (auto& events : record.second) {
                cout << record.first.GetDate() << " " << events << endl;
            }
        }
    }
    
private:
    map<Date, set<string>> data;
};
//=================================================================================
void DelInfoInDB(Database& db, const string& param_1, const string& param_2) {
    Date fdate(param_1);
    if (param_2 != "") {
        if (db.DeleteEvent(fdate, param_2)) {
            cout << "Deleted successfully" << endl;
        }
        else {
            cout << "Event not found" << endl;
        }
    }
    else {        
        cout << "Deleted " << db.DeleteDate(fdate) << " events" << endl;
    }
}
//=================================================================================
void FindInfoInDB(const Database& db, const string& param_1) {
    Date fdate(param_1);
    set<string> events = db.Find(fdate);
    if (events.size() != 0) {        
        for (auto& event : events) {
            cout << event << endl;
        }
    }
}
//=================================================================================
int main() {
    Database db;
    string command;

    while (getline(cin, command)) {  
        try {
            string command_type, param_1, param_2;
            stringstream(command) >> command_type >> param_1 >> param_2;
            if (command_type == "Add") {
                db.AddEvent(param_1, param_2);
            }
            else if (command_type == "Del") {
                DelInfoInDB(db, param_1, param_2);
            }
            else if (command_type == "Find") {
                FindInfoInDB(db, param_1);
            }
            else if (command_type == "Print") {
                db.Print();
            }
            else if (command_type != "") {
                cout << "Unknown command: " << command_type << endl;
            }
        }
        catch (runtime_error & re) {
            cout << re.what() << endl;
        }
    }

    return 0;
}