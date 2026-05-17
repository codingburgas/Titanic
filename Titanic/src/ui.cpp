#include "ui.h"
#include "logic.h"
#include "data.h"

#include "imgui.h"
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <algorithm>

 // ---- Palette helpers ----
static inline ImVec4 hex(float r, float g, float b, float a = 1.f)
{
    return { r, g, b, a };
}

namespace C {
    static const ImVec4 BG0 = hex(0.055f, 0.059f, 0.078f);
    static const ImVec4 BG1 = hex(0.078f, 0.082f, 0.114f);
    static const ImVec4 BG2 = hex(0.110f, 0.118f, 0.169f);
    static const ImVec4 BG3 = hex(0.141f, 0.149f, 0.220f);
    static const ImVec4 ACC = hex(0.486f, 0.416f, 0.969f);      
    static const ImVec4 ACC2 = hex(0.655f, 0.545f, 0.980f);     
    static const ImVec4 TEAL = hex(0.176f, 0.831f, 0.749f);
    static const ImVec4 AMB = hex(0.984f, 0.749f, 0.141f);
    static const ImVec4 RED = hex(0.973f, 0.443f, 0.443f);
    static const ImVec4 GRN = hex(0.290f, 0.871f, 0.502f);
    static const ImVec4 T1 = hex(0.910f, 0.902f, 1.000f);     
    static const ImVec4 T2 = hex(0.608f, 0.600f, 0.722f);     
    static const ImVec4 T3 = hex(0.361f, 0.353f, 0.471f);     
    static const ImVec4 ACC_DIM = hex(0.486f, 0.416f, 0.969f, 0.18f);
    static const ImVec4 RED_DIM = hex(0.973f, 0.443f, 0.443f, 0.15f);
    static const ImVec4 AMB_DIM = hex(0.984f, 0.749f, 0.141f, 0.15f);
    static const ImVec4 GRN_DIM = hex(0.290f, 0.871f, 0.502f, 0.12f);
    static const ImVec4 TRANS = hex(0.f, 0.f, 0.f, 0.f);
}

// ---- Font Awesome ----
#define ICON_FA_PLUS          u8"\uf067"
#define ICON_FA_TRASH         u8"\uf2ed"
#define ICON_FA_PENCIL        u8"\uf303"
#define ICON_FA_CHECK         u8"\uf00c"
#define ICON_FA_SEARCH        u8"\uf002"
#define ICON_FA_CHART_PIE     u8"\uf200"
#define ICON_FA_LIST          u8"\uf03a"
#define ICON_FA_FLAG          u8"\uf024"
#define ICON_FA_CALENDAR      u8"\uf073"
#define ICON_FA_CLOCK         u8"\uf017"
#define ICON_FA_FIRE          u8"\uf06d"
#define ICON_FA_CIRCLE_CHECK  u8"\uf058"
#define ICON_FA_HOURGLASS     u8"\uf252"
#define ICON_FA_FLOPPY        u8"\uf0c7"
#define ICON_FA_ROTATE        u8"\uf2f1"
#define ICON_FA_XMARK         u8"\uf00d"
#define ICON_FA_BOLT          u8"\uf0e7"
#define ICON_FA_LEAF          u8"\uf06c"
#define ICON_FA_ADJUST        u8"\uf042"
#define ICON_FA_DIAMOND       u8"\uf219"
#define ICON_FA_REPEAT        u8"\uf363"

// ---- UI state ----

static char  g_newTitle[128] = "";
static char  g_newDesc[256] = "";
static char  g_newDeadline[16] = "2026-01-01";
static int   g_newDuration = 30;
static int   g_newPriIdx = 1; 
static int   g_newRecurIdx = 0; 

static char  g_searchQuery[128] = "";
static int   g_filterMode = 0;  

static std::string g_feedback = "";
static bool        g_feedbackOk = true;

static bool  g_showStats = false;
static bool  g_showAddPanel = false;

static bool  g_editMode = false;
static Task  g_editTask = {};
static char  g_editTitle[128] = "";
static char  g_editDesc[256] = "";
static char  g_editDeadline[16] = "";
static int   g_editDuration = 0;
static int   g_editPriIdx = 0;
static int   g_editRecurIdx = 0;  

static ImFont* g_fontBody = nullptr;
static ImFont* g_fontMono = nullptr;
static ImFont* g_fontIcons = nullptr;  

// ---- Helpers ----
static const char* PRIORITY_LABELS[] = { "LOW", "MEDIUM", "HIGH" };
static const char* RECUR_LABELS[] = { "None", "Daily", "Weekly", "Monthly" };

static RecurType indexToRecurType(int i) {
    switch (i) {
    case 1:  return RecurType::DAILY;
    case 2:  return RecurType::WEEKLY;
    case 3:  return RecurType::MONTHLY;
    default: return RecurType::NONE;
    }
}
static int recurTypeToIndex(RecurType r) {
    switch (r) {
    case RecurType::DAILY:   return 1;
    case RecurType::WEEKLY:  return 2;
    case RecurType::MONTHLY: return 3;
    default:                 return 0;
    }
}

static Priority indexToPriority(int i) {
    switch (i) { case 0: return Priority::LOW; case 2: return Priority::HIGH; }
                       return Priority::MEDIUM;
}
static int priorityToIndex(Priority p) {
    if (p == Priority::LOW)  return 0;
    if (p == Priority::HIGH) return 2;
    return 1;
}
static ImVec4 priorityColor(Priority p) {
    if (p == Priority::HIGH)   return C::RED;
    if (p == Priority::MEDIUM) return C::AMB;
    return C::GRN;
}
static ImVec4 priorityDimColor(Priority p) {
    if (p == Priority::HIGH)   return C::RED_DIM;
    if (p == Priority::MEDIUM) return C::AMB_DIM;
    return C::GRN_DIM;
}
static const char* priorityIcon(Priority p) {
    if (p == Priority::HIGH)   return ICON_FA_FIRE   "  HIGH";
    if (p == Priority::MEDIUM) return ICON_FA_ADJUST "  MED";
    return                            ICON_FA_LEAF   "  LOW";
}

// Push a solid-colored small button style
static void pushActionButton(bool danger = false) {
    if (danger) {
        ImGui::PushStyleColor(ImGuiCol_Button, hex(0.973f, 0.443f, 0.443f, 0.14f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hex(0.973f, 0.443f, 0.443f, 0.30f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, hex(0.973f, 0.443f, 0.443f, 0.50f));
        ImGui::PushStyleColor(ImGuiCol_Text, C::RED);
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, C::BG3);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C::ACC_DIM);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, C::ACC);
        ImGui::PushStyleColor(ImGuiCol_Text, C::T2);
    }
}
static void popActionButton() { ImGui::PopStyleColor(4); }

// Coloured badge (draws a rounded rect behind the text)
static void badge(const char* label, ImVec4 textCol, ImVec4 bgCol) {
    ImVec2 size = ImGui::CalcTextSize(label);
    float padX = 7.f, padY = 3.f;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(
        ImVec2(pos.x - padX, pos.y - padY),
        ImVec2(pos.x + size.x + padX, pos.y + size.y + padY),
        ImGui::ColorConvertFloat4ToU32(bgCol), 5.f
    );
    ImGui::TextColored(textCol, "%s", label);
}

// Full-width horizontal separator with colour
static void colorSeparator(ImVec4 col = C::ACC_DIM) {
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddLine(
        p0, { p0.x + ImGui::GetContentRegionAvail().x, p0.y },
        ImGui::ColorConvertFloat4ToU32(col), 1.f
    );
    ImGui::Dummy({ 0, 4.f });
}

// ---- Apply global ImGui style ----
static void applyStyle() {
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding = 0.f;
    s.ChildRounding = 10.f;
    s.FrameRounding = 7.f;
    s.PopupRounding = 10.f;
    s.ScrollbarRounding = 6.f;
    s.GrabRounding = 4.f;
    s.TabRounding = 7.f;
    s.WindowBorderSize = 0.f;
    s.ChildBorderSize = 1.f;
    s.FrameBorderSize = 0.f;
    s.PopupBorderSize = 1.f;
    s.ItemSpacing = { 10.f, 8.f };
    s.ItemInnerSpacing = { 6.f, 4.f };
    s.FramePadding = { 10.f, 6.f };
    s.WindowPadding = { 16.f, 14.f };
    s.IndentSpacing = 18.f;
    s.ScrollbarSize = 10.f;
    s.CellPadding = { 8.f, 6.f };

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg] = C::BG0;
    c[ImGuiCol_ChildBg] = C::BG1;
    c[ImGuiCol_PopupBg] = C::BG1;
    c[ImGuiCol_Border] = hex(0.486f, 0.416f, 0.969f, 0.18f);
    c[ImGuiCol_FrameBg] = C::BG2;
    c[ImGuiCol_FrameBgHovered] = C::BG3;
    c[ImGuiCol_FrameBgActive] = C::BG3;
    c[ImGuiCol_TitleBg] = C::BG1;
    c[ImGuiCol_TitleBgActive] = C::BG1;
    c[ImGuiCol_TitleBgCollapsed] = C::BG0;
    c[ImGuiCol_MenuBarBg] = C::BG1;
    c[ImGuiCol_ScrollbarBg] = C::BG0;
    c[ImGuiCol_ScrollbarGrab] = C::BG3;
    c[ImGuiCol_ScrollbarGrabHovered] = hex(0.486f, 0.416f, 0.969f, 0.4f);
    c[ImGuiCol_ScrollbarGrabActive] = C::ACC;
    c[ImGuiCol_CheckMark] = C::ACC2;
    c[ImGuiCol_SliderGrab] = C::ACC;
    c[ImGuiCol_SliderGrabActive] = C::ACC2;
    c[ImGuiCol_Button] = C::BG3;
    c[ImGuiCol_ButtonHovered] = C::ACC_DIM;
    c[ImGuiCol_ButtonActive] = C::ACC;
    c[ImGuiCol_Header] = C::ACC_DIM;
    c[ImGuiCol_HeaderHovered] = hex(0.486f, 0.416f, 0.969f, 0.28f);
    c[ImGuiCol_HeaderActive] = C::ACC;
    c[ImGuiCol_Separator] = hex(0.486f, 0.416f, 0.969f, 0.18f);
    c[ImGuiCol_Tab] = C::BG2;
    c[ImGuiCol_TabHovered] = C::ACC_DIM;
    c[ImGuiCol_TabActive] = hex(0.486f, 0.416f, 0.969f, 0.35f);
    c[ImGuiCol_TableHeaderBg] = C::BG2;
    c[ImGuiCol_TableBorderLight] = hex(0.486f, 0.416f, 0.969f, 0.10f);
    c[ImGuiCol_TableBorderStrong] = hex(0.486f, 0.416f, 0.969f, 0.20f);
    c[ImGuiCol_TableRowBg] = C::TRANS;
    c[ImGuiCol_TableRowBgAlt] = hex(0.110f, 0.118f, 0.169f, 0.40f);
    c[ImGuiCol_TextSelectedBg] = hex(0.486f, 0.416f, 0.969f, 0.35f);
    c[ImGuiCol_Text] = C::T1;
    c[ImGuiCol_TextDisabled] = C::T3;
    c[ImGuiCol_ModalWindowDimBg] = hex(0.f, 0.f, 0.f, 0.65f);
}

// ---- Sidebar ----
static void renderSidebar() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C::BG1);
    ImGui::BeginChild("##sidebar", { 220.f, 0.f }, true);

    // Logo
    ImGui::Spacing();
    ImGui::TextColored(C::ACC2, ICON_FA_DIAMOND);
    ImGui::SameLine();
    ImGui::TextColored(C::T1, " Titanic");
    ImGui::TextColored(C::T3, "  Task Management System");
    ImGui::Spacing();
    colorSeparator();

    // Navigation
    ImGui::TextColored(C::T3, "VIEWS");
    ImGui::Spacing();

    const struct { const char* icon; const char* label; int mode; } navItems[] = {
        { ICON_FA_LIST,         "All Tasks",     0 },
        { ICON_FA_FIRE,         "High Priority", 1 },
        { ICON_FA_CALENDAR,     "By Deadline",   2 },
        { ICON_FA_CIRCLE_CHECK, "Completed",     3 },
        { ICON_FA_HOURGLASS,    "Pending",       4 },
        { ICON_FA_REPEAT,       "Recurring",     5 },
    };

    for (auto& item : navItems) {
        bool active = (g_filterMode == item.mode);
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button, C::ACC_DIM);
            ImGui::PushStyleColor(ImGuiCol_Text, C::ACC2);
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, C::TRANS);
            ImGui::PushStyleColor(ImGuiCol_Text, C::T2);
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C::ACC_DIM);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, C::ACC);

        std::string label = std::string(item.icon) + "  " + item.label;
        if (ImGui::Button(label.c_str(), { 188.f, 32.f })) {
            g_filterMode = item.mode;
            g_searchQuery[0] = '\0';
        }
        ImGui::PopStyleColor(4);
    }

    ImGui::Spacing();
    colorSeparator();
    ImGui::TextColored(C::T3, "TOOLS");
    ImGui::Spacing();

    // Stats toggle
    {
        ImGui::PushStyleColor(ImGuiCol_Button, g_showStats ? C::ACC_DIM : C::TRANS);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C::ACC_DIM);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, C::ACC);
        ImGui::PushStyleColor(ImGuiCol_Text, g_showStats ? C::ACC2 : C::T2);
        if (ImGui::Button(ICON_FA_CHART_PIE "  Statistics", { 188.f, 32.f }))
            g_showStats = !g_showStats;
        ImGui::PopStyleColor(4);
    }

    // Save / Reload
    {
        ImGui::PushStyleColor(ImGuiCol_Button, C::TRANS);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C::ACC_DIM);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, C::ACC);
        ImGui::PushStyleColor(ImGuiCol_Text, C::T2);
        if (ImGui::Button(ICON_FA_FLOPPY "  Save", { 188.f, 32.f })) {
            saveTasksToFile("tasks.dat");
            g_feedback = ICON_FA_CHECK "  Tasks saved to tasks.dat";
            g_feedbackOk = true;
        }
        if (ImGui::Button(ICON_FA_ROTATE "  Reload", { 188.f, 32.f })) {
            loadTasksFromFile("tasks.dat");
            g_feedback = ICON_FA_CHECK "  Tasks reloaded from tasks.dat";
            g_feedbackOk = true;
        }
        ImGui::PopStyleColor(4);
    }

    // Mini stats at bottom
    const std::vector<Task>& all = getAllTasks();
    int total = (int)all.size();
    int completed = (int)filterByStatus(true).size();
    int pending = total - completed;
    int highCnt = countHighPriorityRecursive(all);
    int totalMin = totalDurationRecursive(all);

    float remaining = ImGui::GetContentRegionAvail().y - 140.f;
    if (remaining > 0) ImGui::Dummy({ 0, remaining });

    colorSeparator();

    // 2x2 mini-stat grid
    auto miniStat = [&](const char* num, const char* lbl, ImVec4 col) {
        ImGui::BeginGroup();
        ImGui::TextColored(col, "%s", num);
        ImGui::TextColored(C::T3, "%s", lbl);
        ImGui::EndGroup();
        };

    char buf[32];
    snprintf(buf, sizeof(buf), "%d", total);
    miniStat(buf, "Total tasks", C::T1);
    ImGui::SameLine(0, 28);
    snprintf(buf, sizeof(buf), "%d", completed);
    miniStat(buf, "Done", C::TEAL);

    snprintf(buf, sizeof(buf), "%d", pending);
    miniStat(buf, "Pending", C::AMB);
    ImGui::SameLine(0, 28);
    snprintf(buf, sizeof(buf), "%d", highCnt);
    miniStat(buf, "High priority", C::RED);

    ImGui::Spacing();
    snprintf(buf, sizeof(buf), "%d h %d m", totalMin / 60, totalMin % 60);
    ImGui::TextColored(C::T3, "Total time: ");
    ImGui::SameLine();
    ImGui::TextColored(C::ACC2, "%s", buf);
    ImGui::Spacing();

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// ---- Top bar ----
static void renderTopBar() {
    // Search
    ImGui::SetNextItemWidth(300.f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, C::BG2);
    if (ImGui::InputTextWithHint("##search", ICON_FA_SEARCH "  Search tasks by title...",
        g_searchQuery, sizeof(g_searchQuery))) {
        if (g_searchQuery[0] != '\0') g_filterMode = 0;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine(0, 12);

    // Filter tabs
    const char* tabLabels[] = { "All", "Priority", "Deadline", "Done", "Pending" };
    for (int i = 0; i < 5; ++i) {
        bool active = (g_filterMode == i && g_searchQuery[0] == '\0');
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button, C::ACC);
            ImGui::PushStyleColor(ImGuiCol_Text, C::T1);
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, C::BG3);
            ImGui::PushStyleColor(ImGuiCol_Text, C::T2);
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C::ACC_DIM);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, C::ACC);
        if (ImGui::SmallButton(tabLabels[i])) {
            g_filterMode = i;
            g_searchQuery[0] = '\0';
        }
        ImGui::PopStyleColor(4);
        ImGui::SameLine(0, 4);
    }

    // Spacer then Add Task button
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 110.f + ImGui::GetCursorPosX() - 4.f);
    ImGui::PushStyleColor(ImGuiCol_Button, C::ACC);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C::ACC2);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, hex(0.38f, 0.30f, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_Text, C::T1);
    if (ImGui::Button(ICON_FA_PLUS "  Add Task", { 110.f, 0.f }))
        g_showAddPanel = !g_showAddPanel;
    ImGui::PopStyleColor(4);
}

// ---- Progress bar ----
static void renderProgressBar() {
    const std::vector<Task>& all = getAllTasks();
    int total = (int)all.size();
    int completed = (int)filterByStatus(true).size();
    float ratio = total > 0 ? (float)completed / (float)total : 0.f;

    ImGui::Spacing();

    // Label row
    char pctBuf[64];
    snprintf(pctBuf, sizeof(pctBuf), "%d / %d tasks  (%.0f%%)", completed, total, ratio * 100.f);
    ImGui::TextColored(C::T3, "Progress:");
    ImGui::SameLine();
    ImGui::TextColored(C::T2, "%s", pctBuf);

    // Bar
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, C::ACC);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, C::BG3);
    ImGui::ProgressBar(ratio, { -1.f, 6.f }, "");
    ImGui::PopStyleColor(2);
    ImGui::Spacing();
}

// ---- Add-task collapsible panel ----
static void renderAddTaskPanel() {
    if (!g_showAddPanel) return;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, C::BG2);
    ImGui::BeginChild("##addpanel", { 0.f, 160.f }, true);

    ImGui::TextColored(C::ACC2, ICON_FA_PLUS "  New Task");
    ImGui::Spacing();

    float w = (ImGui::GetContentRegionAvail().x - 12.f) * 0.5f;

    ImGui::SetNextItemWidth(w);
    ImGui::InputTextWithHint("##t", "Title*", g_newTitle, sizeof(g_newTitle));
    ImGui::SameLine(0, 12);
    ImGui::SetNextItemWidth(w);
    ImGui::InputTextWithHint("##d", "Description", g_newDesc, sizeof(g_newDesc));

    ImGui::SetNextItemWidth(140.f);
    ImGui::InputTextWithHint("##dl", "Deadline YYYY-MM-DD", g_newDeadline, sizeof(g_newDeadline));
    ImGui::SameLine(0, 12);
    ImGui::SetNextItemWidth(120.f);
    ImGui::InputInt("Duration (min)##add", &g_newDuration);
    if (g_newDuration < 1) g_newDuration = 1;
    ImGui::SameLine(0, 12);
    ImGui::SetNextItemWidth(120.f);
    ImGui::Combo("Priority##add", &g_newPriIdx, PRIORITY_LABELS, 3);
    ImGui::SameLine(0, 12);
    ImGui::SetNextItemWidth(120.f);
    ImGui::Combo("Recurs##add", &g_newRecurIdx, RECUR_LABELS, 4);
    if (g_newRecurIdx != 0) {
        ImGui::SameLine(0, 8);
        ImGui::TextColored(C::ACC2, ICON_FA_REPEAT "  Template task - copies created automatically");
    }

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button, C::ACC);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C::ACC2);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, hex(0.38f, 0.30f, 0.80f));
    if (ImGui::Button(ICON_FA_PLUS "  Create Task", { 140.f, 0.f })) {
        int id = createTask(g_newTitle, g_newDesc,
            indexToPriority(g_newPriIdx),
            g_newDeadline, g_newDuration);
        if (id > 0) {
            // Apply recurrence to the new task
            if (g_newRecurIdx != 0) {
                for (auto& t : getAllTasks()) {
                    if (t.id == id) {
                        t.recurType = indexToRecurType(g_newRecurIdx);
                        t.recurOriginId = 0;
                        break;
                    }
                }
            }
            g_feedback = std::string(ICON_FA_CHECK "  Task created - ID: ") + std::to_string(id);
            g_feedbackOk = true;
            saveTasksToFile("tasks.dat");
            g_newTitle[0] = '\0'; g_newDesc[0] = '\0';
            g_newDuration = 30;   g_newPriIdx = 1; g_newRecurIdx = 0;
            g_showAddPanel = false;
        }
        else {
            g_feedback = ICON_FA_XMARK "  Invalid data - check all required fields.";
            g_feedbackOk = false;
        }
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) g_showAddPanel = false;

    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::Spacing();
}

// ---- Task table ----
static void renderTaskTable() {
    // Build task list
    std::vector<Task> tasks;
    std::string query(g_searchQuery);
    if (!query.empty()) {
        tasks = searchByTitle(query);
    }
    else {
        switch (g_filterMode) {
        case 1:  tasks = getTasksSortedByPriority(); break;
        case 2:  tasks = getTasksSortedByDeadline(); break;
        case 3:  tasks = filterByStatus(true);        break;
        case 4:  tasks = filterByStatus(false);       break;
        case 5:
            // Show only recurring template tasks
            for (const auto& t : getAllTasks())
                if (t.recurType != RecurType::NONE && t.recurOriginId == 0)
                    tasks.push_back(t);
            break;
        default: tasks = getAllTasks();                break;
        }
    }

    if (tasks.empty()) {
        ImGui::Spacing();
        ImGui::TextColored(C::T3, "  No tasks to display.");
        return;
    }

    ImGuiTableFlags flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_SizingStretchProp;

    float tableH = ImGui::GetContentRegionAvail().y - 4.f;
    if (!ImGui::BeginTable("##tasks", 7, flags, { 0.f, tableH })) return;

    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 40.f);
    ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Priority", ImGuiTableColumnFlags_WidthFixed, 90.f);
    ImGui::TableSetupColumn("Deadline", ImGuiTableColumnFlags_WidthFixed, 105.f);
    ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 82.f);
    ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 148.f);

    // Styled header
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, C::BG2);
    ImGui::TableHeadersRow();
    ImGui::PopStyleColor();

    for (const auto& task : tasks) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextColored(C::T3, "#%03d", task.id);

        ImGui::TableSetColumnIndex(1);
        if (task.completed)
            ImGui::TextColored(C::T3, "%s", task.title.c_str());
        else
            ImGui::TextColored(C::T1, "%s", task.title.c_str());

        // Recurring badge shown inline after the title
        if (task.recurType != RecurType::NONE) {
            ImGui::SameLine(0, 8);
            const char* rl;
            if (task.recurOriginId == 0) {
                if (task.recurType == RecurType::DAILY)  rl = ICON_FA_REPEAT "  Daily";
                else if (task.recurType == RecurType::WEEKLY) rl = ICON_FA_REPEAT "  Weekly";
                else                                          rl = ICON_FA_REPEAT "  Monthly";
            }
            else {
                rl = ICON_FA_REPEAT "  copy";
            }
            badge(rl, C::ACC2, C::ACC_DIM);
        }

        ImGui::TableSetColumnIndex(2);
        badge(priorityIcon(task.priority),
            priorityColor(task.priority),
            priorityDimColor(task.priority));

        ImGui::TableSetColumnIndex(3);
        ImGui::TextColored(C::T2, "%s",
            task.deadline.empty() ? "-" : task.deadline.c_str());

        ImGui::TableSetColumnIndex(4);
        ImGui::TextColored(C::T2, "%d min", task.duration);

        ImGui::TableSetColumnIndex(5);
        if (task.completed)
            badge(ICON_FA_CIRCLE_CHECK "  Done", C::TEAL, hex(0.176f, 0.831f, 0.749f, 0.12f));
        else
            badge(ICON_FA_HOURGLASS   "  Pending", C::AMB, hex(0.984f, 0.749f, 0.141f, 0.12f));

        ImGui::TableSetColumnIndex(6);

        // Complete
        if (!task.completed) {
            ImGui::PushID(task.id * 10 + 1);
            pushActionButton(false);
            if (ImGui::SmallButton(ICON_FA_CHECK)) {
                completeTask(task.id);
                saveTasksToFile("tasks.dat");
                g_feedback = ICON_FA_CHECK "  Task marked as completed.";
                g_feedbackOk = true;
            }
            popActionButton();
            ImGui::PopID();
            ImGui::SameLine(0, 4);
        }

        // Edit
        ImGui::PushID(task.id * 10 + 2);
        pushActionButton(false);
        if (ImGui::SmallButton(ICON_FA_PENCIL)) {
            g_editTask = task;
            g_editMode = true;
            g_editPriIdx = priorityToIndex(task.priority);
            g_editRecurIdx = recurTypeToIndex(task.recurType);
            g_editDuration = task.duration;
            std::strncpy(g_editTitle, task.title.c_str(), sizeof(g_editTitle) - 1);
            std::strncpy(g_editDesc, task.description.c_str(), sizeof(g_editDesc) - 1);
            std::strncpy(g_editDeadline, task.deadline.c_str(), sizeof(g_editDeadline) - 1);
        }
        popActionButton();
        ImGui::PopID();
        ImGui::SameLine(0, 4);

        // Delete
        ImGui::PushID(task.id * 10 + 3);
        pushActionButton(true);
        if (ImGui::SmallButton(ICON_FA_TRASH)) {
            deleteTask(task.id);
            saveTasksToFile("tasks.dat");
            g_feedback = ICON_FA_TRASH "  Task deleted.";
            g_feedbackOk = true;
        }
        popActionButton();
        ImGui::PopID();
    }

    ImGui::EndTable();
}

// ---- Edit modal ----
static void renderEditModal() {
    if (!g_editMode) return;
    ImGui::OpenPopup("Edit Task##modal");

    ImGui::SetNextWindowSize({ 480.f, 0.f }, ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, C::BG1);
    if (ImGui::BeginPopupModal("Edit Task##modal", &g_editMode,
        ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(C::ACC2, ICON_FA_PENCIL "  Editing task #%d", g_editTask.id);
        ImGui::Spacing();
        colorSeparator();

        float fw = ImGui::GetContentRegionAvail().x;
        ImGui::SetNextItemWidth(fw);
        ImGui::InputTextWithHint("##et", "Title*", g_editTitle, sizeof(g_editTitle));
        ImGui::SetNextItemWidth(fw);
        ImGui::InputTextWithHint("##ed", "Description", g_editDesc, sizeof(g_editDesc));

        ImGui::SetNextItemWidth(160.f);
        ImGui::InputTextWithHint("##edl", "Deadline YYYY-MM-DD", g_editDeadline, sizeof(g_editDeadline));
        ImGui::SameLine(0, 12);
        ImGui::SetNextItemWidth(120.f);
        ImGui::InputInt("Duration (min)##edit", &g_editDuration);
        if (g_editDuration < 1) g_editDuration = 1;
        ImGui::SetNextItemWidth(140.f);
        ImGui::Combo("Priority##edit", &g_editPriIdx, PRIORITY_LABELS, 3);
        ImGui::SameLine(0, 12);
        ImGui::SetNextItemWidth(130.f);
        ImGui::Combo("Recurs##edit", &g_editRecurIdx, RECUR_LABELS, 4);
        if (g_editRecurIdx != 0)
            ImGui::TextColored(C::ACC2, ICON_FA_REPEAT "  Recurring template");

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Button, C::ACC);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C::ACC2);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, hex(0.38f, 0.30f, 0.80f));
        if (ImGui::Button(ICON_FA_CHECK "  Save Changes", { 160.f, 0.f })) {
            g_editTask.title = g_editTitle;
            g_editTask.description = g_editDesc;
            g_editTask.deadline = g_editDeadline;
            g_editTask.duration = g_editDuration;
            g_editTask.priority = indexToPriority(g_editPriIdx);
            g_editTask.recurType = indexToRecurType(g_editRecurIdx);

            std::string err = validateTask(g_editTask);
            if (err.empty()) {
                updateTask(g_editTask);
                saveTasksToFile("tasks.dat");
                g_feedback = ICON_FA_CHECK "  Task updated successfully.";
                g_feedbackOk = true;
                g_editMode = false;
                ImGui::CloseCurrentPopup();
            }
            else {
                g_feedback = ICON_FA_XMARK "  " + err;
                g_feedbackOk = false;
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine(0, 10);
        if (ImGui::Button("Cancel", { 80.f, 0.f })) {
            g_editMode = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleColor();
}

// ---- Statistics window ----
static void renderStatsWindow() {
    if (!g_showStats) return;

    ImGui::SetNextWindowSize({ 380.f, 280.f }, ImGuiCond_Once);
    ImGui::SetNextWindowPos({ 880.f,  80.f }, ImGuiCond_Once);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, C::BG1);
    if (ImGui::Begin(ICON_FA_CHART_PIE "  Statistics", &g_showStats)) {
        const std::vector<Task>& all = getAllTasks();
        int total = (int)all.size();
        int done = (int)filterByStatus(true).size();
        int pending = total - done;
        int highCnt = countHighPriorityRecursive(all);
        int totalMin = totalDurationRecursive(all);
        float ratio = total > 0 ? (float)done / (float)total : 0.f;

        colorSeparator();
        ImGui::TextColored(C::T3, "OVERVIEW");
        ImGui::Spacing();

        auto row = [&](const char* label, const char* val, ImVec4 col) {
            ImGui::TextColored(C::T2, "%-22s", label);
            ImGui::SameLine();
            ImGui::TextColored(col, "%s", val);
            };
        char buf[64];
        snprintf(buf, sizeof(buf), "%d", total);   row("Total tasks", buf, C::T1);
        snprintf(buf, sizeof(buf), "%d", done);    row("Completed", buf, C::TEAL);
        snprintf(buf, sizeof(buf), "%d", pending); row("Pending", buf, C::AMB);
        snprintf(buf, sizeof(buf), "%d", highCnt); row("HIGH priority (rec)", buf, C::RED);
        snprintf(buf, sizeof(buf), "%d h %d min", totalMin / 60, totalMin % 60);
        row("Total duration (rec)", buf, C::ACC2);

        ImGui::Spacing();
        colorSeparator();
        ImGui::TextColored(C::T3, "COMPLETION");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, C::ACC);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, C::BG3);
        ImGui::ProgressBar(ratio, { -1.f, 10.f }, "");
        ImGui::PopStyleColor(2);

        ImGui::Spacing();
        ImGui::TextColored(C::ACC2, "%.0f%%", ratio * 100.f);
        ImGui::SameLine();
        ImGui::TextColored(C::T2, " complete");
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

// ---- Feedback bar ----
static void renderFeedback() {
    if (g_feedback.empty()) return;
    ImGui::Spacing();
    ImVec4 col = g_feedbackOk ? C::TEAL : C::RED;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, g_feedbackOk
        ? hex(0.176f, 0.831f, 0.749f, 0.10f)
        : hex(0.973f, 0.443f, 0.443f, 0.10f));
    ImGui::BeginChild("##feedback", { 0.f, 32.f }, false);
    ImGui::SetCursorPosY(7.f);
    ImGui::TextColored(col, "  %s", g_feedback.c_str());
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20.f);
    ImGui::PushStyleColor(ImGuiCol_Button, C::TRANS);
    ImGui::PushStyleColor(ImGuiCol_Text, C::T3);
    if (ImGui::SmallButton(ICON_FA_XMARK)) g_feedback = "";
    ImGui::PopStyleColor(2);
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::Spacing();
}

// ---- Public API ----

void uiInit() {
    applyStyle();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    // 1. Body font
    ImFontConfig cfg;
    g_fontBody = io.Fonts->AddFontFromFileTTF(
        "assets/fonts/Syne-Medium.ttf", 15.f, &cfg);

    // 2. Merge Font Awesome icons into body font atlas
    static const ImWchar fa_ranges[] = { 0xf000, 0xf8ff, 0 };
    ImFontConfig cfgMerge;
    cfgMerge.MergeMode = true;
    cfgMerge.GlyphOffset = { 0, 2.f };
    io.Fonts->AddFontFromFileTTF(
        "assets/fonts/fa-solid-900.ttf", 14.f, &cfgMerge, fa_ranges);

    // 3. Monospace font for IDs / deadlines
    g_fontMono = io.Fonts->AddFontFromFileTTF(
        "assets/fonts/JetBrainsMono-Regular.ttf", 13.f);

    io.Fonts->Build();

    loadTasksFromFile("tasks.dat");
    spawnDueRecurringTasks(); 
}

void uiRender() {
    ImGuiIO& io = ImGui::GetIO();

    // Main full-screen window (no title bar, no padding)
    ImGui::SetNextWindowPos({ 0.f, 0.f });
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });
    ImGui::Begin("##root",
        nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings);
    ImGui::PopStyleVar();

    // Sidebar + content split
    renderSidebar();
    ImGui::SameLine(0, 0);

    // Main content panel
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C::BG0);
    ImGui::BeginChild("##content", { 0.f, 0.f }, false);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 16.f, 12.f });
    renderTopBar();
    ImGui::PopStyleVar();

    ImGui::SetCursorPosX(16.f);
    ImGui::BeginGroup();
    renderFeedback();
    renderProgressBar();
    renderAddTaskPanel();
    renderTaskTable();
    ImGui::EndGroup();

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::End();

    // Floating windows
    renderEditModal();
    renderStatsWindow();
}

void uiShutdown() {
    saveTasksToFile("tasks.dat");
}