#pragma once

#include "EngineCore.h"
#include <QWidget>
#include "DockManager.h"

class QLabel;

namespace ze::editor
{

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

	Q_DISABLE_COPY(MainWindow)

	/**
	 * Add a new window to be managed by the editor
	 * \remark The editor will take ownership of the widget!
	 */
	void add_window(OwnerPtr<QWidget> in_widget);

	ZE_FORCEINLINE QLabel* get_perf_text() const { return perf_text; }
protected:
	bool event(QEvent* event) override;
	bool nativeEvent(const QByteArray& event_type, void* message, long* result) override;
	void closeEvent(QCloseEvent* event) override;
private:
	std::vector<ads::CDockWidget*> windows;
	std::unique_ptr<ads::CDockManager> dock_mgr; 
	QLabel* perf_text;
};

}