# 轨道转移 (Orbital Transfer)

一款网页/桌面 2D 轨道力学游戏。玩家控制飞行器从天体 A 变轨至天体 B，通过姿态旋转和油门控制实现轨道转移。

## 游戏截图

黑白像素风格，类似 Chrome 恐龙小游戏的极简美学。

## 操作方式

| 按键 | 功能 |
|------|------|
| ← → / A D | 旋转飞行器姿态 |
| ↑ ↓ / W S | 控制油门大小 |
| 空格 / ESC | 暂停游戏 |
| F1 | 切换调试信息 |

## 游戏目标

将飞行器从天体 A 变轨至天体 B，进入稳定轨道后完成一圈即胜利。

### 评分系统（满分 1000 分）
- **燃料效率**（500 分）：消耗越少分越高
- **轨道圆度**（500 分）：轨道越圆分越高

## 版本

### HTML/JS 版 (v1.5.11)
- 单文件 `Orbit_Transfer_v1.5.11.html`，浏览器直接打开
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
├── Orbit_Transfer_v1.5.11.html   # HTML/JS 版本
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
