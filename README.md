# OBS Speech Timer

一个用于 OBS Studio 的演讲计时器插件，可以帮助记录和追踪演讲时间。

## 功能特点

- 支持多角色计时（讲者、讨论嘉宾）
- 可自定义最低时间要求
- 实时显示累计时间
- 支持多段计时
- 导出数据到 Excel 和文本文件
- 美观的用户界面

## 依赖文件

在构建项目之前，请先下载必要的依赖文件：
1. 下载 [windows-deps-2023-04-12-x64.zip](https://obsproject.com/downloads/dependencies2019.zip)
2. 将下载的文件解压到项目根目录的 `.deps` 文件夹中

## 安装说明

1. 下载最新版本的 `obs-speech-timer.dll`
2. 将 DLL 文件复制到 OBS Studio 的插件目录
3. 重启 OBS Studio
4. 在 OBS Studio 的"视图"菜单中找到"演讲计时器"

## 使用方法

1. 添加记录：点击"添加记录"按钮
2. 选择角色类型（讲者/讨论嘉宾）
3. 输入姓名
4. 点击"记录开始时间"开始计时
5. 点击"记录结束时间"结束计时
6. 可以添加多个时间段
7. 导出数据：点击"导出表格"或"导出文本"

## 开发环境

- Visual Studio 2019 或更高版本
- Qt 6.0 或更高版本
- CMake 3.15 或更高版本

## 构建说明

1. 克隆仓库
2. 创建构建目录：
   ```bash
   mkdir build
   cd build
   ```
3. 配置项目：
   ```bash
   cmake ..
   ```
4. 构建项目：
   ```bash
   cmake --build . --config Release
   ```

## 许可证

MIT License 