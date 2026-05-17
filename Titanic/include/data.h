#pragma once
#include <string>
#include <vector>

// Priority levels
enum class Priority {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3
};

// Recurrence type
enum class RecurType {
    NONE = 0,
    DAILY = 1,
    WEEKLY = 2,
    MONTHLY = 3
};

// Task structure
struct Task {
    int id;
    std::string title;
    std::string description;
    Priority priority;
    std::string deadline;
    int duration;
    bool completed;

    // Recurrence
    RecurType   recurType;      
    int         recurOriginId; 
    Task() : id(0), title(""), description(""), priority(Priority::MEDIUM),
        deadline("2026-01-01"), duration(0), completed(false),
        recurType(RecurType::NONE), recurOriginId(0) {
    }
};

// Data layer functions
std::vector<Task>& getAllTasks();
int addTask(const Task& task);
bool removeTask(int id);
bool updateTask(const Task& updatedTask);
int getNextId();
void saveTasksToFile(const std::string& filename);
void loadTasksFromFile(const std::string& filename);
std::string priorityToString(Priority p);
Priority stringToPriority(const std::string& s);

std::string recurTypeToString(RecurType r);
RecurType   stringToRecurType(const std::string& s);

void spawnDueRecurringTasks();