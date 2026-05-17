#include "data.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <algorithm>

 // ---- Internal state ----

static std::vector<Task> taskList; 
static int nextId = 1;     

// ---- Function implementations ----

std::vector<Task>& getAllTasks() {
    return taskList;
}

int addTask(const Task& task) {
    Task newTask = task;
    newTask.id = nextId++;
    taskList.push_back(newTask);
    return newTask.id;
}

bool removeTask(int id) {
    for (auto it = taskList.begin(); it != taskList.end(); ++it) {
        if (it->id == id) {
            taskList.erase(it);
            return true;
        }
    }
    return false;
}

bool updateTask(const Task& updatedTask) {
    for (auto& task : taskList) {
        if (task.id == updatedTask.id) {
            task = updatedTask;
            return true;
        }
    }
    return false;
}

int getNextId() {
    return nextId;
}

std::string priorityToString(Priority p) {
    switch (p) {
    case Priority::HIGH:   return "HIGH";
    case Priority::MEDIUM: return "MEDIUM";
    case Priority::LOW:    return "LOW";
    default:               return "LOW";
    }
}

Priority stringToPriority(const std::string& s) {
    if (s == "HIGH")   return Priority::HIGH;
    if (s == "MEDIUM") return Priority::MEDIUM;
    return Priority::LOW;
}

std::string recurTypeToString(RecurType r) {
    switch (r) {
    case RecurType::DAILY:   return "DAILY";
    case RecurType::WEEKLY:  return "WEEKLY";
    case RecurType::MONTHLY: return "MONTHLY";
    default:                 return "NONE";
    }
}

RecurType stringToRecurType(const std::string& s) {
    if (s == "DAILY")   return RecurType::DAILY;
    if (s == "WEEKLY")  return RecurType::WEEKLY;
    if (s == "MONTHLY") return RecurType::MONTHLY;
    return RecurType::NONE;
}

// ---- Date arithmetic helpers ----

static std::string todayStr() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    char buf[16];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
        now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
    return buf;
}

static bool parseDate(const std::string& s, int& y, int& m, int& d) {
    if (s.length() != 10) return false;
    y = std::stoi(s.substr(0, 4));
    m = std::stoi(s.substr(5, 2));
    d = std::stoi(s.substr(8, 2));
    return true;
}
static std::string formatDate(int y, int m, int d) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", y, m, d);
    return buf;
}

// Advance a date by one recurrence period
static std::string advanceDeadline(const std::string& dl, RecurType rt) {
    int y, m, d;
    if (!parseDate(dl, y, m, d)) return dl;

    tm t = {};
    t.tm_year = y - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = d;

    switch (rt) {
    case RecurType::DAILY:   t.tm_mday += 1;  break;
    case RecurType::WEEKLY:  t.tm_mday += 7;  break;
    case RecurType::MONTHLY: t.tm_mon += 1;  break;
    default: break;
    }
    mktime(&t); 
    return formatDate(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
}

void spawnDueRecurringTasks() {
    std::string today = todayStr();
    std::vector<Task*> templates;
    for (auto& task : taskList)
        if (task.recurType != RecurType::NONE && task.recurOriginId == 0)
            templates.push_back(&task);

    for (Task* tmpl : templates) {
        while (!tmpl->deadline.empty() && tmpl->deadline <= today) {
            Task copy = *tmpl;
            copy.id = nextId++;
            copy.completed = false;
            copy.recurType = RecurType::NONE;   
            copy.recurOriginId = tmpl->id;     
            taskList.push_back(copy);
            tmpl->deadline = advanceDeadline(tmpl->deadline, tmpl->recurType);
        }
    }
}

void saveTasksToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return;
    }

    // Write next ID on the first line
    file << nextId << "\n";

    for (const auto& task : taskList) {
        file << task.id << "|"
            << task.title << "|"
            << task.description << "|"
            << priorityToString(task.priority) << "|"
            << task.deadline << "|"
            << task.duration << "|"
            << (task.completed ? 1 : 0) << "|"
            << recurTypeToString(task.recurType) << "|"
            << task.recurOriginId << "\n";
    }
}

void loadTasksFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }

    taskList.clear();

    std::string line;
    // Read next ID
    if (std::getline(file, line)) {
        nextId = std::stoi(line);
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;
        Task task;

        // Parse pipe-separated fields
        std::getline(ss, token, '|'); task.id = std::stoi(token);
        std::getline(ss, token, '|'); task.title = token;
        std::getline(ss, token, '|'); task.description = token;
        std::getline(ss, token, '|'); task.priority = stringToPriority(token);
        std::getline(ss, token, '|'); task.deadline = token;
        std::getline(ss, token, '|'); task.duration = std::stoi(token);
        std::getline(ss, token, '|'); task.completed = (token == "1");
        if (std::getline(ss, token, '|'))
            task.recurType = stringToRecurType(token);
        if (std::getline(ss, token, '|'))
            task.recurOriginId = std::stoi(token);

        taskList.push_back(task);
    }
}