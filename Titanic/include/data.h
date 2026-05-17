#pragma once
#include <string>
#include <vector>

// Priority levels
enum class Priority {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3
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

    // This is a constructor: it initializes the variables correctly
    Task() : id(0), title(""), description(""), priority(Priority::MEDIUM),
        deadline("2026-01-01"), duration(0), completed(false) {
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