#include "timer-dock.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPalette>

AppreciationDialog::AppreciationDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("赞赏支持");
    setFixedSize(400, 450);

    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // 创建标签显示二维码
    QLabel *qrLabel = new QLabel(this);
    QPixmap qrCode(":/images/appreciation.png");  // 二维码图片资源
    qrLabel->setPixmap(qrCode.scaled(380, 380, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    qrLabel->setAlignment(Qt::AlignCenter);
    
    // 创建说明文字标签
    QLabel *textLabel = new QLabel("感谢您的支持！\n如果这个工具对您有帮助，欢迎赞赏支持！", this);
    textLabel->setAlignment(Qt::AlignCenter);

    // 获取系统背景色
    QPalette pal = palette();
    QColor bgColor = pal.color(QPalette::Window);
    
    // 计算背景色的亮度 (使用相对亮度公式: 0.299R + 0.587G + 0.114B)
    double luminance = (0.299 * bgColor.red() + 0.587 * bgColor.green() + 0.114 * bgColor.blue()) / 255.0;
    
    // 如果亮度小于0.5，使用白色文字；否则使用黑色文字
    QString textColor = luminance < 0.5 ? "white" : "black";
    textLabel->setStyleSheet(QString("QLabel { color: %1; font-size: 14px; font-weight: bold; }").arg(textColor));
    
    layout->addWidget(qrLabel);
    layout->addWidget(textLabel);
    layout->setAlignment(Qt::AlignCenter);
} 