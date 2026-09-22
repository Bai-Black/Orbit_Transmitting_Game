# 轨道转移 (Orbital Transfer)

一款网页/桌面 2D 轨道力学游戏。玩家控制飞行器从天体 A 变轨至天体 B，通过姿态旋转和油门控制实现轨道转移。

**在线游玩：** https://bai-black.github.io/Orbit_Transmitting_Game/

## 游戏截图

黑白像素风格，类似 Chrome 恐龙小游戏的极简美学。
<img width="961" height="564" alt="image" src="https://github.com/user-attachments/assets/60b028a5-099c-4608-a782-0db97e176ec2" />

## 操作方式

### 桌面端

| 按键 | 功能 |
|------|------|
| ← → / A D | 旋转飞行器姿态 |
| ↑ ↓ / W S | 控制油门大小 |
| 空格 / ESC | 暂停游戏 |
| F1 | 切换调试信息 |

### 手机端（横屏）

| 触控区域 | 功能 |
|----------|------|
| 左下方向摇杆 | 在左下活动区根据手指落点自动定位；指定飞行器目标朝向并平滑转向 |
| 右下油门推杆 | 调节 0–100% 油门；松手后快速平滑归零 |
| 右上暂停按钮 | 暂停游戏 |
| 标题、结算界面 | 点击屏幕开始或重新开始 |

手机标题页采用两步启动：第一次点击仅向浏览器申请全屏并停留在标题页，第二次点击才开始游戏；若浏览器不支持或拒绝，全屏申请不会影响第二次点击开始。竖屏时游戏会暂停并提示旋转设备，恢复横屏后可继续游戏。触控操作支持双指同时控制方向和油门。

手机端游戏世界缩小约 30%（按 70% 比例显示），以扩大视野；HUD、操作提示和场景标签文字保持原尺寸，世界描边会进行宽度补偿以保持清晰。

## 游戏目标

将飞行器从天体 A 变轨至天体 B，进入稳定轨道后完成一圈即胜利。

手机版形成天体 B 的稳定闭合轨道后开始确认：稳定飞行满 5 秒，或在 5 秒内先完成一整圈，即可胜利。下方“计圈中”百分比显示距离胜利的综合进度。不限制最终轨道高度；高度 100–180 的较窄轨道带仅作为评分目标，不影响通关。

### 评分系统（满分 1000 分）

手机版：

- **燃料得分**（500 分）：`max(0, 500 - 燃料消耗 × 40)`。
- **轨道得分**（500 分）：分别计算近地点和远地点超出 100–180 范围的距离，取平均作为“目标范围差距”；得分为 `max(0, 500 - 差距 × 2)`。
- 近地点和远地点都在评分目标范围内时，轨道得分为满分；范围外的稳定轨道仍然可以通关。

桌面版继续使用原有规则：进入指定轨道范围并完成一圈后胜利，按燃料效率和轨道圆度评分。

## 版本

### HTML/JS 版 (v2.1.4)
- 单文件 `Orbit_Transfer_v2.1.4.html`，浏览器直接打开
- 零依赖，纯 JavaScript 实现

### C++ 版 (v2.0.8)
- 基于 SDL2 + SDL2_ttf
- 跨平台编译（Windows/macOS/Linux）

## 构建（C++ 版）

### 依赖
- SDL2
- SDL2_ttf
- C++17 编译器

### Windows (MSYS2)
```bash
# 安装依赖
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf

# 编译
cd cpp
g++ -std=c++17 -IC:/msys64/mingw64/include/SDL2 \
    -o orbit_transfer.exe src/*.cpp \
    -LC:/msys64/mingw64/lib -lSDL2 -lSDL2_ttf

# 运行
./orbit_transfer.exe
```

### macOS
```bash
brew install sdl2 sdl2_ttf
cd cpp
g++ -std=c++17 $(pkg-config --cflags sdl2 SDL2_ttf) \
    -o orbit_transfer src/*.cpp \
    $(pkg-config --libs sdl2 SDL2_ttf)
```

### Linux
```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev
cd cpp
g++ -std=c++17 $(pkg-config --cflags sdl2 SDL2_ttf) \
    -o orbit_transfer src/*.cpp \
    $(pkg-config --libs sdl2 SDL2_ttf)
```

## 项目结构

```
Orbit_Transmitting_Game/
├── index.html                    # GitHub Pages 入口
├── Orbit_Transfer_v2.1.4.html   # HTML/JS 2.1.4 版本
├── cpp/                           # C++ 版本
│   ├── src/
│   │   ├── main.cpp              # 入口、SDL 主循环
│   │   ├── game.h/cpp            # 游戏逻辑
│   │   ├── physics.h/cpp         # 物理引擎
│   │   ├── orbit.h/cpp           # 轨道预测分析
│   │   ├── renderer.h/cpp        # SDL2 渲染
│   │   ├── input.h/cpp           # 键盘输入
│   │   ├── camera.h/cpp          # 摄像机跟随
│   │   └── types.h               # 数据结构
│   └── CMakeLists.txt
└── README.md
```

## 物理模型

- **引力**：牛顿万有引力 F = GMm/r²
- **积分**：半隐式欧拉法（先更新速度，再更新位置）
- **SOI**：引力范围模型，飞行器只受最近天体引力
- **轨道分析**：基于开普勒公式计算半长轴和周期
- **稳定性判定**：连续 5 帧轨道参数一致 + 轨迹闭合

## 许可证

MIT License
