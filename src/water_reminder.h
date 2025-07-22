/**
 * @file water_reminder.h
 * @brief 喝水提醒终端应用 - 主头文件
 * @author zcg
 * @date 2024
 * @description 一个炫酷的喝水提醒终端应用，帮助用户养成良好的喝水习惯
 */

#ifndef WATER_REMINDER_H
#define WATER_REMINDER_H

/* 添加GNU扩展以支持usleep */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <termios.h>

/* ==================== 常量定义 ==================== */
#define MAX_NAME_LEN 50
#define MAX_RECORDS 1000
#define CONFIG_FILE "config/user_config.dat"
#define DATA_FILE "data/water_records.dat"
#define LOG_FILE "logs/app.log"

/* 默认设置 */
#define DEFAULT_REMINDER_INTERVAL 60  // 默认提醒间隔（分钟）
#define DEFAULT_DAILY_GOAL 8         // 默认每日目标（杯）
#define DEFAULT_CUP_SIZE 250         // 默认杯子容量（毫升）

/* 颜色定义 */
#define COLOR_RESET     "\033[0m"
#define COLOR_RED       "\033[31m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_BLUE      "\033[34m"
#define COLOR_MAGENTA   "\033[35m"
#define COLOR_CYAN      "\033[36m"
#define COLOR_WHITE     "\033[37m"
#define COLOR_BOLD      "\033[1m"
#define COLOR_DIM       "\033[2m"

/* 特效字符 */
#define WAVE_CHAR       "~"
#define DROP_CHAR       "💧"
#define CUP_CHAR        "🥤"
#define STAR_CHAR       "⭐"
#define TROPHY_CHAR     "🏆"

/* ==================== 数据结构定义 ==================== */

/**
 * @brief 用户配置结构体
 */
typedef struct {
    char name[MAX_NAME_LEN];        // 用户姓名
    int reminder_interval;          // 提醒间隔（分钟）
    int daily_goal;                // 每日目标杯数
    int cup_size;                  // 杯子容量（毫升）
    int sound_enabled;             // 是否启用声音提醒
    int notification_style;        // 通知样式（0-2）
} UserConfig;

/**
 * @brief 喝水记录结构体
 */
typedef struct {
    time_t timestamp;              // 记录时间戳
    int amount;                    // 喝水量（毫升）
    char date_str[11];            // 日期字符串 YYYY-MM-DD
} WaterRecord;

/**
 * @brief 应用状态结构体
 */
typedef struct {
    UserConfig config;             // 用户配置
    WaterRecord records[MAX_RECORDS]; // 喝水记录数组
    int record_count;             // 记录总数
    int today_count;              // 今日喝水次数
    int today_amount;             // 今日喝水总量
    time_t last_reminder;         // 上次提醒时间
    int is_running;               // 程序运行状态
    int paused;                   // 暂停状态
} AppState;

/* ==================== 函数声明 ==================== */

/* 初始化和清理函数 */
int  init_app(AppState *app);
void cleanup_app(AppState *app);
int  create_directories(void);

/* 配置管理函数 */
int  load_config(UserConfig *config);
int  save_config(const UserConfig *config);
void set_default_config(UserConfig *config);
void setup_user_config(UserConfig *config);

/* 数据管理函数 */
int  load_records(AppState *app);
int  save_records(const AppState *app);
void add_water_record(AppState *app, int amount);
void calculate_today_stats(AppState *app);

/* UI显示函数 */
void clear_screen(void);
void show_banner(void);
void show_main_menu(void);
void show_stats_dashboard(const AppState *app);
void show_progress_bar(int current, int goal, const char *label);
void show_water_animation(void);
void show_reminder_notification(const AppState *app);

/* 用户交互函数 */
int  get_user_choice(void);
char get_key_input(void);
void pause_program(void);
void resume_program(void);

/* 提醒系统函数 */
void setup_reminder_timer(AppState *app);
void reminder_handler(int sig);
int  should_remind(const AppState *app);

/* 统计分析函数 */
void show_weekly_stats(const AppState *app);
void show_monthly_stats(const AppState *app);
float calculate_daily_average(const AppState *app, int days);
int  get_streak_days(const AppState *app);

/* 工具函数 */
void get_current_date_str(char *date_str);
int  is_same_date(const char *date1, const char *date2);
void log_message(const char *message);
void play_sound_effect(void);

#endif /* WATER_REMINDER_H */ 