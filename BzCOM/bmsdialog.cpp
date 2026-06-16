#include "bmsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>

BmsDialog::BmsDialog(QWidget *parent)
    : QDialog(parent)
{
    for (int i = 0; i < 15; i++) m_cellLabels[i] = nullptr;
    buildUi();
}

BmsDialog::~BmsDialog() {}

void BmsDialog::buildUi(void)
{
    setWindowTitle(tr("BMS 电池监控"));
    setFixedSize(460, 340);

    auto *root = new QVBoxLayout(this);
    root->setSpacing(8);
    root->setContentsMargins(12, 12, 12, 12);

    // ── 电芯电压矩阵 ──
    auto *cellGroup = new QGroupBox(tr("电芯电压 (mV)"), this);
    auto *cellGrid = new QGridLayout;
    cellGrid->setSpacing(6);

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 5; col++) {
            int idx = row * 5 + col;
            if (idx >= 15) break;

            auto *card = new QWidget(this);
            card->setFixedSize(72, 56);
            card->setStyleSheet(
                "QWidget { background: #f5f7fa; border-radius: 6px; }");
            auto *cl = new QVBoxLayout(card);
            cl->setSpacing(2);
            cl->setContentsMargins(2, 4, 2, 4);

            auto *title = new QLabel(QString("CH%1").arg(idx + 1), card);
            title->setAlignment(Qt::AlignCenter);
            title->setStyleSheet("font-size: 10px; color: #888;");

            auto *value = new QLabel("--", card);
            value->setAlignment(Qt::AlignCenter);
            value->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a1a1a;");
            m_cellLabels[idx] = value;

            cl->addWidget(title);
            cl->addWidget(value);
            cellGrid->addWidget(card, row, col);
        }
    }
    cellGroup->setLayout(cellGrid);
    root->addWidget(cellGroup);

    // ── 汇总信息 ──
    auto *sumGroup = new QGroupBox(tr("汇总"), this);
    auto *sumGrid = new QGridLayout(sumGroup);
    sumGrid->setSpacing(8);

    auto makeLb = [&](const QString &t) {
        auto *lb = new QLabel(t, this);
        lb->setStyleSheet("font-size: 13px; color: #555;");
        return lb;
    };
    auto makeVl = [&]() {
        auto *lb = new QLabel("--", this);
        lb->setStyleSheet("font-size: 14px; font-weight: bold; color: #1a1a1a;");
        return lb;
    };

    int r = 0;
    sumGrid->addWidget(makeLb(tr("总压:")), r, 0);
    m_totalVLabel = makeVl(); sumGrid->addWidget(m_totalVLabel, r, 1);
    sumGrid->addWidget(makeLb(tr("SOC:")), r, 2);
    m_socLabel = makeVl(); sumGrid->addWidget(m_socLabel, r, 3);
    sumGrid->addWidget(makeLb(tr("电流:")), r, 4);
    m_currentLabel = makeVl(); sumGrid->addWidget(m_currentLabel, r, 5);

    r = 1;
    sumGrid->addWidget(makeLb(tr("温度:")), r, 0);
    m_tempLabel = makeVl(); sumGrid->addWidget(m_tempLabel, r, 1);
    sumGrid->addWidget(makeLb(tr("放电MOS:")), r, 2);
    m_dsgLabel = makeVl(); sumGrid->addWidget(m_dsgLabel, r, 3);
    sumGrid->addWidget(makeLb(tr("充电MOS:")), r, 4);
    m_chgLabel = makeVl(); sumGrid->addWidget(m_chgLabel, r, 5);

    sumGrid->setColumnStretch(1, 1);
    sumGrid->setColumnStretch(3, 1);
    sumGrid->setColumnStretch(5, 1);
    root->addWidget(sumGroup);

    root->addStretch();
}

void BmsDialog::UpdateBmsData(const quint16 *cellVoltage, int cellCount,
                               quint16 packVoltage, quint16 soc,
                               qint16 current, qint8 temperature,
                               quint8 dsgSta, quint8 chgSta)
{
    for (int i = 0; i < 15; i++) {
        if (!m_cellLabels[i]) continue;
        if (i < cellCount && cellVoltage[i] > 0) {
            m_cellLabels[i]->setText(QString::number(cellVoltage[i]));
            updateCellColor(i, cellVoltage[i]);
        } else {
            m_cellLabels[i]->setText("--");
            m_cellLabels[i]->setStyleSheet("font-size: 16px; font-weight: bold; color: #ccc;");
        }
    }


    m_totalVLabel->setText(packVoltage > 0 ? QString("%1 mV").arg(packVoltage) : "--");
    m_socLabel->setText(soc > 0 ? QString("%1%").arg(soc) : "--");
    m_currentLabel->setText(current ? QString("%1 mA").arg(current) : "--");
    m_tempLabel->setText(temperature ? QString("%1°C").arg(temperature) : "--");
    m_dsgLabel->setText(dsgSta ? tr("● 开启") : tr("○ 关闭"));
    m_dsgLabel->setStyleSheet(dsgSta ? "font-size: 14px; font-weight: bold; color: #27ae60;" : "font-size: 14px; font-weight: bold; color: #999;");
    m_chgLabel->setText(chgSta ? tr("● 开启") : tr("○ 关闭"));
    m_chgLabel->setStyleSheet(chgSta ? "font-size: 14px; font-weight: bold; color: #27ae60;" : "font-size: 14px; font-weight: bold; color: #999;");
}
void BmsDialog::updateCellColor(int idx, quint16 mv)
{
    if (!m_cellLabels[idx]) return;
    QString color = "#1a1a1a";
    if (mv > 4200 || mv < 2800)
        color = "#e74c3c";  // 红色: 异常
    else if (mv > 4100 || mv < 3000)
        color = "#e67e22";  // 橙色: 预警
    else
        color = "#27ae60";  // 绿色: 正常
    m_cellLabels[idx]->setStyleSheet(
        QString("font-size: 16px; font-weight: bold; color: %1;").arg(color));
}
