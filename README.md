# STM32 G4 Demo

该仓库包含 STM32 G4 系列微控制器的基础项目设置，使用 CLion 作为开发环境，并通过 CMake 和 STM32CubeMX 配置进行构建。

## 项目结构

项目结构如下：

```
stm32-G4-Demo/
├── 001_Led_Key/                # 主要应用代码
   ├── .idea/                       # CLion IDE 配置文件
   ├── cmake-build-debug/           # 构建目录
   ├── Core/                        # 核心功能代码
   ├── Drivers/                     # 外设驱动代码
   ├── myApp/                       # 应用程序代码
   ├── myDrivers/                   # 自定义驱动代码
   ├── .gitignore                   # Git 忽略文件
   ├── LICENSE                      # 开源许可证文件
   ├── README.md                    # 项目说明文件
   ├── CMakeLists.txt               # CMake 构建配置文件
   └── 001_Led_Key.ioc              # STM32CubeMX 配置文件
```

## 前提条件

- **IDE**: 本项目使用 CLion 作为开发 IDE。
- **工具链**: 项目使用 ARM GCC 工具链进行 STM32 开发。
- **CMake**: 项目的构建系统使用 CMake 配置。
- **STM32CubeMX**: 用于外设配置和代码生成。

## 设置与构建

1. **克隆仓库**：
   ```bash
   git clone https://github.com/D77go77/stm32-G4-Demo.git
   cd stm32-G4-Demo
   ```

2. **在 CLion 中打开项目**：
   - 打开 CLion，选择 "Open" 打开 `stm32-G4-Demo` 目录。

3. **配置 CMake**：
   - 确保 CLion 中已经正确配置了 CMake。
   - 仓库中已包含必要的 CMake 配置文件。
   - 确保 CLion 中已经正确配置了 Openocd
   - [配置内容可参考此链接](https://zhuanlan.zhihu.com/p/145801160)


4. **构建项目**：
   - 在 CLion 中点击 "Build" 按钮进行项目编译。

5. **烧录固件**：
   - 使用 CMSIS-DAP.cfg 或其他适用的工具将生成的固件烧录到 STM32 G4 开发板上。

## 许可证

本项目采用 GPL-3.0 许可证 - 详情请参见 [LICENSE](LICENSE) 文件。
