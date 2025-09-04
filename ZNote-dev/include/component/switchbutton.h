#pragma once
#include <QApplication>
#include <QWidget>
#include <QCheckBox>
#include <QPainter>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>

class SwitchButton : public QCheckBox {
	Q_OBJECT
		Q_PROPERTY(int offset READ offset WRITE setOffset)
public:
	explicit SwitchButton(QWidget* parent = nullptr)
		: QCheckBox(parent), m_offset(0)
	{
		setFixedSize(50, 28);
		setCursor(Qt::PointingHandCursor);
		connect(this, &QCheckBox::toggled, this, &SwitchButton::startAnimation);
		m_anim = new QPropertyAnimation(this, "offset", this);
		m_anim->setDuration(200);
	}

	int offset() const { return m_offset; }
	void setOffset(int o) { m_offset = o; update(); }

protected:
	void paintEvent(QPaintEvent*) override {
		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing);

		// 背景
		QColor bg = isChecked() ? QColor("#3B82F6") : QColor("#4B5563");
		p.setBrush(bg);
		p.setPen(Qt::NoPen);
		p.drawRoundedRect(rect(), height() / 2, height() / 2);

		// 圆点
		int diameter = height() - 4;
		QRectF circleRect(m_offset, 2, diameter, diameter);
		p.setBrush(QColor("#FFFFFF"));
		p.drawEllipse(circleRect);
	}

	void mouseReleaseEvent(QMouseEvent* event) override {
		if (event->button() == Qt::LeftButton) {
			setChecked(!isChecked());
		}
		QCheckBox::mouseReleaseEvent(event);
	}

private slots:
	void startAnimation() {
		m_anim->stop();
		m_anim->setStartValue(m_offset);
		m_anim->setEndValue(isChecked() ? width() - height() : 0);
		m_anim->start();
	}

private:
	int m_offset;
	QPropertyAnimation* m_anim;
};

// 带文字的滑块控件
class SwitchWidget : public QWidget {
	Q_OBJECT
public:
	explicit SwitchWidget(const QString& text, QWidget* parent = nullptr)
		: QWidget(parent)
	{
		QHBoxLayout* layout = new QHBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(8);

		QLabel* lbl = new QLabel(text);
		lbl->setStyleSheet("color: #F3F4F6; font-size: 13px;");

		m_switch = new SwitchButton();

		layout->addWidget(lbl);
		layout->addStretch();
		layout->addWidget(m_switch);
	}

	bool isChecked() const { return m_switch->isChecked(); }
	void setChecked(bool checked) { m_switch->setChecked(checked); }

private:
	SwitchButton* m_switch;
};