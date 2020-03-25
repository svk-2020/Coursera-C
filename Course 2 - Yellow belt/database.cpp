#include "database.h"

//==================================================================================
void Database::Add(const Date& date, const string& event) {
    if (storage[date].second.find(event) == storage[date].second.end()) {
        storage[date].second.insert(event);
        storage[date].first.push_back(event);
    }
}
//==================================================================================
// map<Date, pair<vector<string>, set<string>>> storage
vector<string> Database::FindIf(function<bool(const Date & date, const string & even)> pred) const {
    vector<string> result;
    for (const auto& date_record : storage) {
        for (const auto& event_record : date_record.second.first) {
            if (pred(date_record.first, event_record)) {
                stringstream res;
                res << date_record.first << " " << event_record;
                result.push_back(res.str());
            }
        }
    }
    return result;
}
//==================================================================================
// map<Date, pair<vector<string>, set<string>>> storage
int Database::RemoveIf(function<bool(const Date & date, const string & even)> pred) {
    int count = 0;
    set<Date> marker;       // множество дат для удаляемых событий
    //маркирование элементов на удаление    
    for (auto& date_record : storage) {
        for (auto& event_record : date_record.second.first) {
            if (pred(date_record.first, event_record)) {
                date_record.second.second.erase(event_record);  // удаляем событие во множестве
                event_record = "del_mark";                      // маркируем событие в векторе
                marker.insert(date_record.first);
                count++;
            }
        }
    }
    // удаление
    for (const auto& mark : marker) {
        auto del_it = stable_partition(storage[mark].first.begin(), storage[mark].first.end(),
            [](string& ev)
            { return ev != "del_mark"; });
        storage[mark].first.erase(del_it, end(storage[mark].first));
        // удаление пустых дат
        if (storage[mark].first.size() == 0) {
            storage.erase(mark);
        }
    }
    return count;
};
//==================================================================================
// map<Date, pair<vector<string>, set<string>>> storage
string Database::Last(const Date& date) const {
    // проверка на пустую БД
    if (storage.empty()) {
        throw invalid_argument("");
    }
    stringstream result;
    // ищем первую дату БОЛЬШЕ либо РАВНУЮ заданной
    auto it = storage.lower_bound(date);
    // если указатель на первый элемент БД
    if (it == storage.begin()) {
        // заданная дата меньше самой маленькой в БД
        if (date < it->first) {
            throw invalid_argument("");
        }
        // заданная дата равна самой маленькой в БД
        else {
            result << it->first << " " << it->second.first.back();
            return result.str();
        }
    }
    // если указатель на конец БД, значит все даты в БД меньше заданной
    else if (it == storage.end()) {
        it--;
        result << it->first << " " << it->second.first.back();
        return result.str();
    }
    // если указатель где то в середине БД
    else {
        // заданная дата перед найденной в БД
        if (date < it->first) {
            it--;
            result << it->first << " " << it->second.first.back();
            return result.str();
        }
        // заданная дата равна найденной в БД
        else {
            result << it->first << " " << it->second.first.back();
            return result.str();
        }
    }
};
//==================================================================================
// map<Date, pair<vector<string>, set<string>>> storage
void Database::Print(ostream& stream) const {
    for (const auto& record : storage) {
        for (const string& event_vector : record.second.first) {
            stream << record.first << " " << event_vector << endl;
        }
    }
}