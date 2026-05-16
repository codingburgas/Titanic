#pragma once
#include "data.h"
#include <vector>
#include <string>

 // ---- Sorting ----
void sortByPriority(std::vector<Task>& tasks);
void sortByDeadline(std::vector<Task>& tasks);

// ---- Searching ----
std::vector<Task> searchByTitle(const std::string& query);
std::vector<Task> filterByPriority(Priority p);
std::vector<Task> filterByStatus(bool completed);

// ---- Recursive operations ----
int totalDurationRecursive(const std::vector<Task>& tasks, int index = 0);
int countHighPriorityRecursive(const std::vector<Task>& tasks, int index = 0);

// ---- Task management ----
std::string validateTask(const Task& task);
int  createTask(const std::string& title,
    const std::string& description,
    Priority           priority,
    const std::string& deadline,
    int                duration);

bool completeTask(int id);
bool deleteTask(int id);
std::vector<Task> getTasksSortedByPriority();
std::vector<Task> getTasksSortedByDeadline();