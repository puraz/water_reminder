/**
 * @file ui.c
 * @brief 喝水提醒终端应用 - UI显示模块
 * @author zcg
 * @date 2024
 * @description 包含所有用户界面显示功能：横幅、菜单、进度条、动画效果等
 */

#include "water_reminder.h"

/* ==================== 基础UI函数 ==================== */

/**
 * @brief 清屏函数
 */
void clear_screen(void) {
    system("clear");
}

/**
 * @brief 显示应用横幅
 */
void show_banner(void) {
    printf("%s%s", COLOR_BOLD, COLOR_CYAN);
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                              ║\n");
    printf("║    %s💧 喝水提醒应用 - Water Reminder v1.0 💧%s%s              ║\n", 
           COLOR_BLUE, COLOR_CYAN, COLOR_BOLD);
    printf("║                                                              ║\n");
    printf("║                   %s作者: zcg%s%s                             ║\n", 
           COLOR_YELLOW, COLOR_CYAN, COLOR_BOLD);
    printf("║                                                              ║\n");
    printf("║            %s🌊 健康生活，从每一滴水开始 🌊%s%s              ║\n", 
           COLOR_GREEN, COLOR_CYAN, COLOR_BOLD);
    printf("║                                                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("%s", COLOR_RESET);
}

/**
 * @brief 显示主菜单
 */
void show_main_menu(void) {
    printf("\n");
    printf("%s╭─────────────────────────────────────╮%s\n", COLOR_WHITE, COLOR_RESET);
    printf("%s│               主菜单                │%s\n", COLOR_WHITE, COLOR_RESET);
    printf("%s╰─────────────────────────────────────╯%s\n", COLOR_WHITE, COLOR_RESET);
    printf("\n");
    
    printf("  %s1.%s %s💧 记录喝水%s\n", COLOR_BOLD, COLOR_RESET, COLOR_BLUE, COLOR_RESET);
    printf("  %s2.%s %s📊 查看统计%s\n", COLOR_BOLD, COLOR_RESET, COLOR_MAGENTA, COLOR_RESET);
    printf("  %s3.%s %s⚙️  设置%s\n", COLOR_BOLD, COLOR_RESET, COLOR_YELLOW, COLOR_RESET);
    printf("  %s4.%s %s⏸️  暂停/恢复提醒%s\n", COLOR_BOLD, COLOR_RESET, COLOR_CYAN, COLOR_RESET);
    printf("  %s0.%s %s❌ 退出%s\n", COLOR_BOLD, COLOR_RESET, COLOR_RED, COLOR_RESET);
    printf("\n%s请选择操作: %s", COLOR_BOLD, COLOR_RESET);
}

/**
 * @brief 显示统计仪表板
 */
void show_stats_dashboard(const AppState *app) {
    if (!app) return;
    
    printf("\n");
    printf("%s╭─────────────────────────────────────╮%s\n", COLOR_GREEN, COLOR_RESET);
    printf("%s│            今日统计数据             │%s\n", COLOR_GREEN, COLOR_RESET);
    printf("%s╰─────────────────────────────────────╯%s\n", COLOR_GREEN, COLOR_RESET);
    printf("\n");
    
    // 计算目标完成度
    int daily_goal_ml = app->config.daily_goal * app->config.cup_size;
    float progress_percent = daily_goal_ml > 0 ? 
        ((float)app->today_amount / daily_goal_ml) * 100 : 0;
    
    if (progress_percent > 100) progress_percent = 100;
    
    // 显示用户信息
    printf("  %s👤 用户:%s %s%s\n", 
           COLOR_CYAN, COLOR_RESET, app->config.name, COLOR_RESET);
    
    // 显示今日喝水量
    printf("  %s🥤 今日喝水:%s %s%d次 / %dml%s\n", 
           COLOR_BLUE, COLOR_RESET, COLOR_BOLD, 
           app->today_count, app->today_amount, COLOR_RESET);
    
    // 显示每日目标
    printf("  %s🎯 每日目标:%s %s%d杯 (%dml)%s\n", 
           COLOR_YELLOW, COLOR_RESET, COLOR_BOLD,
           app->config.daily_goal, daily_goal_ml, COLOR_RESET);
    
    // 显示进度条
    printf("  %s📈 完成度:%s %.1f%%\n", COLOR_MAGENTA, COLOR_RESET, progress_percent);
    show_progress_bar(app->today_amount, daily_goal_ml, "喝水进度");
    
    // 显示提醒状态
    printf("  %s⏰ 提醒间隔:%s %s%d分钟%s", 
           COLOR_CYAN, COLOR_RESET, COLOR_BOLD,
           app->config.reminder_interval, COLOR_RESET);
    
    if (app->paused) {
        printf(" %s[已暂停]%s", COLOR_RED, COLOR_RESET);
    } else {
        printf(" %s[运行中]%s", COLOR_GREEN, COLOR_RESET);
    }
    printf("\n");
    
    // 显示连续天数
    int streak = get_streak_days(app);
    if (streak > 0) {
        printf("  %s🔥 连续喝水:%s %s%d天%s\n", 
               COLOR_RED, COLOR_RESET, COLOR_BOLD, streak, COLOR_RESET);
    }
    
    // 显示鼓励信息
    if (progress_percent >= 100) {
        printf("\n  %s%s 太棒了！今天的目标已完成！ %s%s\n", 
               COLOR_BOLD, TROPHY_CHAR, TROPHY_CHAR, COLOR_RESET);
    } else if (progress_percent >= 75) {
        printf("\n  %s%s 加油！距离目标只差一点点了！ %s%s\n", 
               COLOR_YELLOW, STAR_CHAR, STAR_CHAR, COLOR_RESET);
    } else if (progress_percent >= 50) {
        printf("\n  %s💪 不错！已经完成一半目标了！\n", COLOR_GREEN);
    } else if (app->today_count > 0) {
        printf("\n  %s☕ 好的开始！继续保持下去！\n", COLOR_BLUE);
    } else {
        printf("\n  %s💧 新的一天开始了，记得多喝水哦！\n", COLOR_CYAN);
    }
}

/**
 * @brief 显示进度条
 */
void show_progress_bar(int current, int goal, const char *label) {
    if (goal <= 0) return;
    
    const int bar_width = 30;
    float percentage = (float)current / goal;
    if (percentage > 1.0) percentage = 1.0;
    
    int filled = (int)(percentage * bar_width);
    
    printf("     %s[", COLOR_WHITE);
    
    // 绘制进度条
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) {
            if (percentage >= 1.0) {
                printf("%s█%s", COLOR_GREEN, COLOR_WHITE);
            } else if (percentage >= 0.75) {
                printf("%s█%s", COLOR_YELLOW, COLOR_WHITE);
            } else if (percentage >= 0.5) {
                printf("%s█%s", COLOR_BLUE, COLOR_WHITE);
            } else {
                printf("%s█%s", COLOR_CYAN, COLOR_WHITE);
            }
        } else {
            printf("░");
        }
    }
    
    printf("]%s %.1f%%\n", COLOR_RESET, percentage * 100);
}

/**
 * @brief 显示喝水动画
 */
void show_water_animation(void) {
    const char* frames[] = {
        "💧    ",
        " 💧   ",
        "  💧  ",
        "   💧 ",
        "    💧",
        "   🥤 "
    };
    
    printf("\n%s", COLOR_BLUE);
    for (int i = 0; i < 6; i++) {
        printf("\r  %s喝水中... %s", COLOR_CYAN, frames[i]);
        fflush(stdout);
        usleep(200000); // 200ms延迟
    }
    printf("%s ✨ 完成！\n", COLOR_GREEN);
    printf("%s", COLOR_RESET);
}

/**
 * @brief 显示提醒通知
 */
void show_reminder_notification(const AppState *app) {
    if (!app) return;
    
    // 保存当前屏幕内容并显示提醒
    printf("\n%s", COLOR_BOLD);
    printf("╔══════════════════════════════════════╗\n");
    printf("║                                      ║\n");
    printf("║     %s💧 喝水提醒 💧%s%s               ║\n", COLOR_BLUE, COLOR_BOLD, COLOR_WHITE);
    printf("║                                      ║\n");
    printf("║   %s是时候喝水啦！%s%s                  ║\n", COLOR_YELLOW, COLOR_BOLD, COLOR_WHITE);
    printf("║                                      ║\n");
    printf("║   %s保持健康，记得补充水分 🌊%s%s        ║\n", COLOR_GREEN, COLOR_BOLD, COLOR_WHITE);
    printf("║                                      ║\n");
    printf("╚══════════════════════════════════════╝%s\n", COLOR_RESET);
    
    // 播放音效（如果启用）
    if (app->config.sound_enabled) {
        play_sound_effect();
    }
    
    // 闪烁效果
    for (int i = 0; i < 3; i++) {
        printf("\a"); // 系统铃声
        fflush(stdout);
        usleep(300000);
    }
}

/* ==================== 统计显示函数 ==================== */

/**
 * @brief 显示周统计
 */
void show_weekly_stats(const AppState *app) {
    if (!app) return;
    
    printf("%s╭─────────────────────────────────────╮%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("%s│             近7天统计               │%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("%s╰─────────────────────────────────────╯%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("\n");
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    int weekly_total = 0;
    int weekly_days = 0;
    
    // 显示最近7天的数据
    for (int day = 6; day >= 0; day--) {
        time_t target_time = now - (day * 24 * 60 * 60);
        struct tm *target_tm = localtime(&target_time);
        
        char date_str[11];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d", target_tm);
        
        char weekday[10];
        strftime(weekday, sizeof(weekday), "%a", target_tm);
        
        // 计算这一天的喝水量
        int daily_amount = 0;
        int daily_count = 0;
        
        for (int i = 0; i < app->record_count; i++) {
            if (is_same_date(app->records[i].date_str, date_str)) {
                daily_amount += app->records[i].amount;
                daily_count++;
            }
        }
        
        if (daily_amount > 0) {
            weekly_total += daily_amount;
            weekly_days++;
        }
        
        // 显示这一天的数据
        printf("  %s %s:%s %s%4dml%s", 
               day == 0 ? COLOR_GREEN : COLOR_WHITE,
               weekday, COLOR_RESET,
               daily_amount > 0 ? COLOR_BOLD : COLOR_DIM,
               daily_amount, COLOR_RESET);
        
        // 显示进度条
        int goal_ml = app->config.daily_goal * app->config.cup_size;
        if (goal_ml > 0) {
            int progress = (daily_amount * 10) / goal_ml;
            if (progress > 10) progress = 10;
            
            printf(" [");
            for (int j = 0; j < 10; j++) {
                if (j < progress) {
                    printf("%s█%s", daily_amount >= goal_ml ? COLOR_GREEN : COLOR_BLUE, COLOR_RESET);
                } else {
                    printf("░");
                }
            }
            printf("]");
        }
        
        if (day == 0) {
            printf(" %s← 今天%s", COLOR_GREEN, COLOR_RESET);
        }
        printf("\n");
    }
    
    printf("\n");
    if (weekly_days > 0) {
        float daily_avg = (float)weekly_total / weekly_days;
        printf("  %s📊 周平均:%s %s%.0fml/天%s\n", 
               COLOR_MAGENTA, COLOR_RESET, COLOR_BOLD, daily_avg, COLOR_RESET);
        printf("  %s📈 周总量:%s %s%dml%s\n", 
               COLOR_BLUE, COLOR_RESET, COLOR_BOLD, weekly_total, COLOR_RESET);
        printf("  %s✅ 有记录天数:%s %s%d天%s\n", 
               COLOR_GREEN, COLOR_RESET, COLOR_BOLD, weekly_days, COLOR_RESET);
    } else {
        printf("  %s📝 本周还没有喝水记录，开始记录吧！%s\n", 
               COLOR_YELLOW, COLOR_RESET);
    }
}

/**
 * @brief 显示月统计
 */
void show_monthly_stats(const AppState *app) {
    if (!app) return;
    
    printf("%s╭─────────────────────────────────────╮%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s│             近30天统计              │%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s╰─────────────────────────────────────╯%s\n", COLOR_BLUE, COLOR_RESET);
    printf("\n");
    
    time_t now = time(NULL);
    int monthly_total = 0;
    int monthly_days = 0;
    int best_day = 0;
    int goal_achieved_days = 0;
    
    // 统计最近30天的数据
    for (int day = 29; day >= 0; day--) {
        time_t target_time = now - (day * 24 * 60 * 60);
        struct tm *target_tm = localtime(&target_time);
        
        char date_str[11];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d", target_tm);
        
        int daily_amount = 0;
        for (int i = 0; i < app->record_count; i++) {
            if (is_same_date(app->records[i].date_str, date_str)) {
                daily_amount += app->records[i].amount;
            }
        }
        
        if (daily_amount > 0) {
            monthly_total += daily_amount;
            monthly_days++;
            
            if (daily_amount > best_day) {
                best_day = daily_amount;
            }
            
            int goal_ml = app->config.daily_goal * app->config.cup_size;
            if (daily_amount >= goal_ml) {
                goal_achieved_days++;
            }
        }
    }
    
    if (monthly_days > 0) {
        float daily_avg = (float)monthly_total / monthly_days;
        float goal_rate = (float)goal_achieved_days / monthly_days * 100;
        
        printf("  %s📊 月平均:%s %s%.0fml/天%s\n", 
               COLOR_MAGENTA, COLOR_RESET, COLOR_BOLD, daily_avg, COLOR_RESET);
        printf("  %s📈 月总量:%s %s%.1fL%s\n", 
               COLOR_BLUE, COLOR_RESET, COLOR_BOLD, (float)monthly_total/1000, COLOR_RESET);
        printf("  %s🏆 最佳单日:%s %s%dml%s\n", 
               COLOR_YELLOW, COLOR_RESET, COLOR_BOLD, best_day, COLOR_RESET);
        printf("  %s✅ 有记录天数:%s %s%d天%s\n", 
               COLOR_GREEN, COLOR_RESET, COLOR_BOLD, monthly_days, COLOR_RESET);
        printf("  %s🎯 目标达成率:%s %s%.1f%% (%d/%d天)%s\n", 
               COLOR_CYAN, COLOR_RESET, COLOR_BOLD, goal_rate, 
               goal_achieved_days, monthly_days, COLOR_RESET);
        
        // 显示评价
        printf("\n");
        if (goal_rate >= 80) {
            printf("  %s%s 太棒了！你是喝水达人！ %s%s\n", 
                   COLOR_BOLD, TROPHY_CHAR, TROPHY_CHAR, COLOR_RESET);
        } else if (goal_rate >= 60) {
            printf("  %s%s 表现不错！继续保持！ %s%s\n", 
                   COLOR_GREEN, STAR_CHAR, STAR_CHAR, COLOR_RESET);
        } else if (goal_rate >= 40) {
            printf("  %s💪 还有提升空间，加油！%s\n", COLOR_YELLOW, COLOR_RESET);
        } else {
            printf("  %s💧 记得多喝水，健康最重要！%s\n", COLOR_BLUE, COLOR_RESET);
        }
    } else {
        printf("  %s📝 近30天还没有喝水记录，开始记录吧！%s\n", 
               COLOR_YELLOW, COLOR_RESET);
    }
}

/* ==================== 用户交互函数 ==================== */

/**
 * @brief 获取用户选择
 */
int get_user_choice(void) {
    int choice;
    if (scanf("%d", &choice) != 1) {
        // 清理输入缓冲区
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return -1;
    }
    return choice;
}

/**
 * @brief 获取按键输入
 */
char get_key_input(void) {
    struct termios old_tio, new_tio;
    char c;
    
    // 获取当前终端设置
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    
    // 设置为非缓冲模式
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    
    // 读取字符
    c = getchar();
    
    // 恢复终端设置
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    
    return c;
}

/* ==================== 统计计算函数 ==================== */

/**
 * @brief 计算每日平均值
 */
float calculate_daily_average(const AppState *app, int days) {
    if (!app || days <= 0) return 0.0;
    
    time_t now = time(NULL);
    int total_amount = 0;
    int valid_days = 0;
    
    for (int day = 0; day < days; day++) {
        time_t target_time = now - (day * 24 * 60 * 60);
        struct tm *target_tm = localtime(&target_time);
        
        char date_str[11];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d", target_tm);
        
        int daily_amount = 0;
        for (int i = 0; i < app->record_count; i++) {
            if (is_same_date(app->records[i].date_str, date_str)) {
                daily_amount += app->records[i].amount;
            }
        }
        
        if (daily_amount > 0) {
            total_amount += daily_amount;
            valid_days++;
        }
    }
    
    return valid_days > 0 ? (float)total_amount / valid_days : 0.0;
}

/**
 * @brief 获取连续喝水天数
 */
int get_streak_days(const AppState *app) {
    if (!app) return 0;
    
    time_t now = time(NULL);
    int streak = 0;
    int goal_ml = app->config.daily_goal * app->config.cup_size;
    
    // 从今天开始往前检查
    for (int day = 0; day < 365; day++) { // 最多检查一年
        time_t target_time = now - (day * 24 * 60 * 60);
        struct tm *target_tm = localtime(&target_time);
        
        char date_str[11];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d", target_tm);
        
        int daily_amount = 0;
        for (int i = 0; i < app->record_count; i++) {
            if (is_same_date(app->records[i].date_str, date_str)) {
                daily_amount += app->records[i].amount;
            }
        }
        
        if (daily_amount >= goal_ml) {
            streak++;
        } else {
            break; // 连续记录中断
        }
    }
    
    return streak;
} 