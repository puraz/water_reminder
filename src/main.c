/**
 * @file main.c
 * @brief 喝水提醒终端应用 - 主程序
 * @author zcg
 * @date 2024
 * @description 程序入口点，包含主循环和核心控制逻辑
 */

#include "water_reminder.h"

/* 全局应用状态 */
AppState g_app;

/**
 * @brief 信号处理函数 - 优雅退出
 */
void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\n%s感谢使用喝水提醒应用！保持健康！%s\n", 
               COLOR_GREEN, COLOR_RESET);
        g_app.is_running = 0;
        cleanup_app(&g_app);
        exit(0);
    }
}

/**
 * @brief 处理添加喝水记录的菜单选项
 */
void handle_add_water(AppState *app) {
    int amount;
    
    clear_screen();
    show_banner();
    
    printf("%s╭─────────────────────────────────────╮%s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s│           添加喝水记录              │%s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s╰─────────────────────────────────────╯%s\n", COLOR_CYAN, COLOR_RESET);
    printf("\n");
    
    printf("%s请选择喝水量：%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("  1. 小杯 (150ml)\n");
    printf("  2. 中杯 (250ml) %s[默认]%s\n", COLOR_DIM, COLOR_RESET);
    printf("  3. 大杯 (350ml)\n");
    printf("  4. 自定义\n");
    printf("  0. 返回主菜单\n");
    printf("\n%s请输入选择: %s", COLOR_BOLD, COLOR_RESET);
    
    int choice = get_user_choice();
    
    switch (choice) {
        case 1: amount = 150; break;
        case 2: amount = 250; break;
        case 3: amount = 350; break;
        case 4:
            printf("请输入喝水量(ml): ");
            scanf("%d", &amount);
            if (amount <= 0 || amount > 2000) {
                printf("%s❌ 无效的喝水量！%s\n", COLOR_RED, COLOR_RESET);
                sleep(2);
                return;
            }
            break;
        case 0: return;
        default:
            printf("%s❌ 无效选择！%s\n", COLOR_RED, COLOR_RESET);
            sleep(2);
            return;
    }
    
    add_water_record(app, amount);
    
    // 显示添加成功动画
    printf("\n%s✅ 成功记录喝水 %dml！%s\n", COLOR_GREEN, amount, COLOR_RESET);
    show_water_animation();
    
    // 检查是否达到目标
    int daily_goal_ml = app->config.daily_goal * app->config.cup_size;
    if (app->today_amount >= daily_goal_ml) {
        printf("\n%s%s 恭喜！您今天已经达到喝水目标！ %s%s\n", 
               COLOR_BOLD, TROPHY_CHAR, TROPHY_CHAR, COLOR_RESET);
    }
    
    printf("\n按任意键继续...");
    getchar();
    getchar();
}

/**
 * @brief 处理查看统计的菜单选项
 */
void handle_view_stats(const AppState *app) {
    int choice;
    
    while (1) {
        clear_screen();
        show_banner();
        
        printf("%s╭─────────────────────────────────────╮%s\n", COLOR_MAGENTA, COLOR_RESET);
        printf("%s│             统计信息                │%s\n", COLOR_MAGENTA, COLOR_RESET);
        printf("%s╰─────────────────────────────────────╯%s\n", COLOR_MAGENTA, COLOR_RESET);
        printf("\n");
        
        printf("  1. %s今日统计%s\n", COLOR_GREEN, COLOR_RESET);
        printf("  2. %s周统计%s\n", COLOR_YELLOW, COLOR_RESET);
        printf("  3. %s月统计%s\n", COLOR_BLUE, COLOR_RESET);
        printf("  0. %s返回主菜单%s\n", COLOR_WHITE, COLOR_RESET);
        printf("\n%s请输入选择: %s", COLOR_BOLD, COLOR_RESET);
        
        choice = get_user_choice();
        
        switch (choice) {
            case 1:
                clear_screen();
                show_stats_dashboard(app);
                printf("\n按任意键继续...");
                getchar(); getchar();
                break;
            case 2:
                clear_screen();
                show_weekly_stats(app);
                printf("\n按任意键继续...");
                getchar(); getchar();
                break;
            case 3:
                clear_screen();
                show_monthly_stats(app);
                printf("\n按任意键继续...");
                getchar(); getchar();
                break;
            case 0:
                return;
            default:
                printf("%s❌ 无效选择！%s\n", COLOR_RED, COLOR_RESET);
                sleep(2);
        }
    }
}

/**
 * @brief 处理设置的菜单选项
 */
void handle_settings(AppState *app) {
    int choice;
    
    while (1) {
        clear_screen();
        show_banner();
        
        printf("%s╭─────────────────────────────────────╮%s\n", COLOR_BLUE, COLOR_RESET);
        printf("%s│               设置                  │%s\n", COLOR_BLUE, COLOR_RESET);
        printf("%s╰─────────────────────────────────────╯%s\n", COLOR_BLUE, COLOR_RESET);
        printf("\n");
        
        printf("  1. 修改提醒间隔 %s(当前: %d分钟)%s\n", 
               COLOR_DIM, app->config.reminder_interval, COLOR_RESET);
        printf("  2. 修改每日目标 %s(当前: %d杯)%s\n", 
               COLOR_DIM, app->config.daily_goal, COLOR_RESET);
        printf("  3. 修改杯子容量 %s(当前: %dml)%s\n", 
               COLOR_DIM, app->config.cup_size, COLOR_RESET);
        printf("  4. 声音提醒 %s(当前: %s)%s\n", 
               COLOR_DIM, app->config.sound_enabled ? "开启" : "关闭", COLOR_RESET);
        printf("  5. 重新设置用户信息\n");
        printf("  0. 返回主菜单\n");
        printf("\n%s请输入选择: %s", COLOR_BOLD, COLOR_RESET);
        
        choice = get_user_choice();
        
        switch (choice) {
            case 1:
                printf("请输入新的提醒间隔(分钟): ");
                scanf("%d", &app->config.reminder_interval);
                if (app->config.reminder_interval < 5 || app->config.reminder_interval > 300) {
                    printf("%s❌ 间隔应在5-300分钟之间！%s\n", COLOR_RED, COLOR_RESET);
                    app->config.reminder_interval = DEFAULT_REMINDER_INTERVAL;
                } else {
                    printf("%s✅ 提醒间隔已更新！%s\n", COLOR_GREEN, COLOR_RESET);
                    save_config(&app->config);
                }
                sleep(2);
                break;
            case 2:
                printf("请输入每日目标杯数: ");
                scanf("%d", &app->config.daily_goal);
                if (app->config.daily_goal < 1 || app->config.daily_goal > 20) {
                    printf("%s❌ 目标应在1-20杯之间！%s\n", COLOR_RED, COLOR_RESET);
                    app->config.daily_goal = DEFAULT_DAILY_GOAL;
                } else {
                    printf("%s✅ 每日目标已更新！%s\n", COLOR_GREEN, COLOR_RESET);
                    save_config(&app->config);
                }
                sleep(2);
                break;
            case 3:
                printf("请输入杯子容量(ml): ");
                scanf("%d", &app->config.cup_size);
                if (app->config.cup_size < 50 || app->config.cup_size > 1000) {
                    printf("%s❌ 容量应在50-1000ml之间！%s\n", COLOR_RED, COLOR_RESET);
                    app->config.cup_size = DEFAULT_CUP_SIZE;
                } else {
                    printf("%s✅ 杯子容量已更新！%s\n", COLOR_GREEN, COLOR_RESET);
                    save_config(&app->config);
                }
                sleep(2);
                break;
            case 4:
                app->config.sound_enabled = !app->config.sound_enabled;
                printf("%s✅ 声音提醒已%s！%s\n", 
                       COLOR_GREEN, 
                       app->config.sound_enabled ? "开启" : "关闭", 
                       COLOR_RESET);
                save_config(&app->config);
                sleep(2);
                break;
            case 5:
                setup_user_config(&app->config);
                save_config(&app->config);
                break;
            case 0:
                return;
            default:
                printf("%s❌ 无效选择！%s\n", COLOR_RED, COLOR_RESET);
                sleep(2);
        }
    }
}

/**
 * @brief 主循环函数
 */
void main_loop(AppState *app) {
    int choice;
    
    while (app->is_running) {
        clear_screen();
        show_banner();
        show_stats_dashboard(app);
        show_main_menu();
        
        choice = get_user_choice();
        
        switch (choice) {
            case 1:
                handle_add_water(app);
                break;
            case 2:
                handle_view_stats(app);
                break;
            case 3:
                handle_settings(app);
                break;
            case 4:
                app->paused = !app->paused;
                printf("%s%s 提醒已%s！%s\n", 
                       COLOR_YELLOW, 
                       app->paused ? "⏸️" : "▶️",
                       app->paused ? "暂停" : "恢复", 
                       COLOR_RESET);
                sleep(2);
                break;
            case 0:
                printf("\n%s感谢使用喝水提醒应用！保持健康！%s\n", 
                       COLOR_GREEN, COLOR_RESET);
                app->is_running = 0;
                break;
            default:
                printf("%s❌ 无效选择！请重新输入。%s\n", COLOR_RED, COLOR_RESET);
                sleep(2);
        }
    }
}

/**
 * @brief 程序主入口
 */
int main(void) {
    // 设置信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGALRM, reminder_handler);
    
    // 初始化应用
    if (init_app(&g_app) != 0) {
        fprintf(stderr, "%s❌ 应用初始化失败！%s\n", COLOR_RED, COLOR_RESET);
        return 1;
    }
    
    // 显示欢迎信息
    clear_screen();
    show_banner();
    printf("\n%s%s 欢迎使用喝水提醒应用！ %s%s\n", 
           COLOR_BOLD, DROP_CHAR, DROP_CHAR, COLOR_RESET);
    printf("%s你好，%s！让我们一起养成健康的喝水习惯吧！%s\n", 
           COLOR_CYAN, g_app.config.name, COLOR_RESET);
    printf("\n按任意键开始...");
    getchar();
    
    // 设置提醒定时器
    setup_reminder_timer(&g_app);
    
    // 进入主循环
    main_loop(&g_app);
    
    // 清理并退出
    cleanup_app(&g_app);
    return 0;
} 