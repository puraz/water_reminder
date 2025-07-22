/**
 * @file core.c
 * @brief 喝水提醒终端应用 - 核心逻辑模块
 * @author zcg
 * @date 2024
 * @description 包含应用的核心功能：初始化、配置管理、数据管理、提醒系统
 */

#include "water_reminder.h"

/* 全局变量声明（在main.c中定义） */
extern AppState g_app;

/* ==================== 初始化和清理函数 ==================== */

/**
 * @brief 创建必要的目录结构
 */
int create_directories(void) {
    struct stat st = {0};
    
    // 创建config目录
    if (stat("config", &st) == -1) {
        if (mkdir("config", 0700) == -1) {
            perror("创建config目录失败");
            return -1;
        }
    }
    
    // 创建data目录
    if (stat("data", &st) == -1) {
        if (mkdir("data", 0700) == -1) {
            perror("创建data目录失败");
            return -1;
        }
    }
    
    // 创建logs目录
    if (stat("logs", &st) == -1) {
        if (mkdir("logs", 0700) == -1) {
            perror("创建logs目录失败");
            return -1;
        }
    }
    
    return 0;
}

/**
 * @brief 初始化应用状态
 */
int init_app(AppState *app) {
    if (!app) return -1;
    
    // 创建目录结构
    if (create_directories() != 0) {
        return -1;
    }
    
    // 初始化应用状态
    memset(app, 0, sizeof(AppState));
    app->is_running = 1;
    app->paused = 0;
    app->record_count = 0;
    app->today_count = 0;
    app->today_amount = 0;
    app->last_reminder = 0;
    
    // 加载或创建配置
    if (load_config(&app->config) != 0) {
        printf("%s⚠️  未找到配置文件，开始初始化设置...%s\n", 
               COLOR_YELLOW, COLOR_RESET);
        setup_user_config(&app->config);
        save_config(&app->config);
    }
    
    // 加载历史记录
    load_records(app);
    
    // 计算今日统计
    calculate_today_stats(app);
    
    log_message("应用初始化完成");
    return 0;
}

/**
 * @brief 清理应用资源
 */
void cleanup_app(AppState *app) {
    if (!app) return;
    
    // 保存配置和数据
    save_config(&app->config);
    save_records(app);
    
    log_message("应用正常退出");
}

/* ==================== 配置管理函数 ==================== */

/**
 * @brief 设置默认配置
 */
void set_default_config(UserConfig *config) {
    if (!config) return;
    
    strcpy(config->name, "用户");
    config->reminder_interval = DEFAULT_REMINDER_INTERVAL;
    config->daily_goal = DEFAULT_DAILY_GOAL;
    config->cup_size = DEFAULT_CUP_SIZE;
    config->sound_enabled = 1;
    config->notification_style = 0;
}

/**
 * @brief 设置用户配置
 */
void setup_user_config(UserConfig *config) {
    if (!config) return;
    
    clear_screen();
    show_banner();
    
    printf("%s╭─────────────────────────────────────╮%s\n", COLOR_GREEN, COLOR_RESET);
    printf("%s│             初始化设置              │%s\n", COLOR_GREEN, COLOR_RESET);
    printf("%s╰─────────────────────────────────────╯%s\n", COLOR_GREEN, COLOR_RESET);
    printf("\n");
    
    // 设置用户名
    printf("%s请输入您的姓名: %s", COLOR_CYAN, COLOR_RESET);
    fgets(config->name, MAX_NAME_LEN, stdin);
    config->name[strcspn(config->name, "\n")] = 0; // 移除换行符
    
    if (strlen(config->name) == 0) {
        strcpy(config->name, "用户");
    }
    
    // 设置提醒间隔
    printf("%s请输入提醒间隔(分钟，默认60): %s", COLOR_CYAN, COLOR_RESET);
    char input[10];
    fgets(input, sizeof(input), stdin);
    int interval = atoi(input);
    config->reminder_interval = (interval > 0 && interval <= 300) ? interval : DEFAULT_REMINDER_INTERVAL;
    
    // 设置每日目标
    printf("%s请输入每日喝水目标(杯，默认8): %s", COLOR_CYAN, COLOR_RESET);
    fgets(input, sizeof(input), stdin);
    int goal = atoi(input);
    config->daily_goal = (goal > 0 && goal <= 20) ? goal : DEFAULT_DAILY_GOAL;
    
    // 设置杯子容量
    printf("%s请输入杯子容量(ml，默认250): %s", COLOR_CYAN, COLOR_RESET);
    fgets(input, sizeof(input), stdin);
    int size = atoi(input);
    config->cup_size = (size > 0 && size <= 1000) ? size : DEFAULT_CUP_SIZE;
    
    // 设置声音提醒
    printf("%s是否启用声音提醒？(y/n，默认y): %s", COLOR_CYAN, COLOR_RESET);
    fgets(input, sizeof(input), stdin);
    config->sound_enabled = (input[0] == 'n' || input[0] == 'N') ? 0 : 1;
    
    config->notification_style = 0;
    
    printf("\n%s✅ 配置完成！%s\n", COLOR_GREEN, COLOR_RESET);
    sleep(2);
}

/**
 * @brief 加载配置文件
 */
int load_config(UserConfig *config) {
    if (!config) return -1;
    
    FILE *file = fopen(CONFIG_FILE, "rb");
    if (!file) {
        set_default_config(config);
        return -1;
    }
    
    size_t read_size = fread(config, sizeof(UserConfig), 1, file);
    fclose(file);
    
    if (read_size != 1) {
        set_default_config(config);
        return -1;
    }
    
    return 0;
}

/**
 * @brief 保存配置文件
 */
int save_config(const UserConfig *config) {
    if (!config) return -1;
    
    FILE *file = fopen(CONFIG_FILE, "wb");
    if (!file) {
        perror("保存配置文件失败");
        return -1;
    }
    
    size_t write_size = fwrite(config, sizeof(UserConfig), 1, file);
    fclose(file);
    
    if (write_size != 1) {
        return -1;
    }
    
    return 0;
}

/* ==================== 数据管理函数 ==================== */

/**
 * @brief 加载喝水记录
 */
int load_records(AppState *app) {
    if (!app) return -1;
    
    FILE *file = fopen(DATA_FILE, "rb");
    if (!file) {
        app->record_count = 0;
        return 0; // 文件不存在是正常的
    }
    
    app->record_count = fread(app->records, sizeof(WaterRecord), MAX_RECORDS, file);
    fclose(file);
    
    return 0;
}

/**
 * @brief 保存喝水记录
 */
int save_records(const AppState *app) {
    if (!app) return -1;
    
    FILE *file = fopen(DATA_FILE, "wb");
    if (!file) {
        perror("保存数据文件失败");
        return -1;
    }
    
    size_t write_size = fwrite(app->records, sizeof(WaterRecord), app->record_count, file);
    fclose(file);
    
    if (write_size != app->record_count) {
        return -1;
    }
    
    return 0;
}

/**
 * @brief 添加喝水记录
 */
void add_water_record(AppState *app, int amount) {
    if (!app || amount <= 0) return;
    
    // 检查记录数组是否已满
    if (app->record_count >= MAX_RECORDS) {
        // 移除最旧的记录为新记录腾出空间
        memmove(&app->records[0], &app->records[1], 
                (MAX_RECORDS - 1) * sizeof(WaterRecord));
        app->record_count = MAX_RECORDS - 1;
    }
    
    // 添加新记录
    WaterRecord *record = &app->records[app->record_count];
    record->timestamp = time(NULL);
    record->amount = amount;
    get_current_date_str(record->date_str);
    
    app->record_count++;
    
    // 更新今日统计
    calculate_today_stats(app);
    
    // 保存数据
    save_records(app);
    
    // 记录日志
    char log_msg[100];
    snprintf(log_msg, sizeof(log_msg), "添加喝水记录: %dml", amount);
    log_message(log_msg);
}

/**
 * @brief 计算今日统计数据
 */
void calculate_today_stats(AppState *app) {
    if (!app) return;
    
    char today[11];
    get_current_date_str(today);
    
    app->today_count = 0;
    app->today_amount = 0;
    
    for (int i = 0; i < app->record_count; i++) {
        if (is_same_date(app->records[i].date_str, today)) {
            app->today_count++;
            app->today_amount += app->records[i].amount;
        }
    }
}

/* ==================== 提醒系统函数 ==================== */

/**
 * @brief 设置提醒定时器
 */
void setup_reminder_timer(AppState *app) {
    if (!app) return;
    
    // 设置定时器，每分钟检查一次
    alarm(60);
}

/**
 * @brief 提醒信号处理函数
 */
void reminder_handler(int sig) {
    if (sig == SIGALRM) {
        // 检查是否需要提醒
        if (should_remind(&g_app)) {
            show_reminder_notification(&g_app);
            g_app.last_reminder = time(NULL);
        }
        
        // 重新设置定时器
        alarm(60);
    }
}

/**
 * @brief 判断是否应该提醒
 */
int should_remind(const AppState *app) {
    if (!app || app->paused) return 0;
    
    time_t now = time(NULL);
    time_t interval_seconds = app->config.reminder_interval * 60;
    
    // 如果从未提醒过，或者距离上次提醒已超过间隔时间
    return (app->last_reminder == 0) || 
           (now - app->last_reminder >= interval_seconds);
}

/* ==================== 工具函数 ==================== */

/**
 * @brief 获取当前日期字符串
 */
void get_current_date_str(char *date_str) {
    if (!date_str) return;
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(date_str, 11, "%Y-%m-%d", tm_info);
}

/**
 * @brief 比较两个日期字符串是否相同
 */
int is_same_date(const char *date1, const char *date2) {
    if (!date1 || !date2) return 0;
    return strcmp(date1, date2) == 0;
}

/**
 * @brief 记录日志消息
 */
void log_message(const char *message) {
    if (!message) return;
    
    FILE *log_file = fopen(LOG_FILE, "a");
    if (!log_file) return;
    
    time_t now = time(NULL);
    char time_str[64];
    struct tm *tm_info = localtime(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(log_file, "[%s] %s\n", time_str, message);
    fclose(log_file);
}

/**
 * @brief 播放音效
 */
void play_sound_effect(void) {
    // 在Linux系统上播放系统提示音
    system("pactl upload-sample /usr/share/sounds/alsa/Front_Left.wav bell");
    system("pactl play-sample bell");
} 