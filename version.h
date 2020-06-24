#ifndef VERSION_H
#define VERSION_H

#include <QMessageBox>
#include <QSysInfo>

#define TOOL_VERSION "1.0"

void display_version()
{
	QString os;
	#if defined(Q_OS_LINUX)
		os = "Operating system: Linux";
	#elif defined(Q_OS_WIN32)
		os = "Operating system: Windows";
	#elif defined(Q_OS_MAC)
		os = "Operating system: Mac";
	#else
		os = "Operating system: Unknown or unsupported";
	#endif
    QMessageBox version_info;
	version_info.setText(QStringLiteral("Current version: v" TOOL_VERSION).leftJustified(100));
    version_info.setDetailedText(os);
	//version_info.setWindowFlags(version_info.windowFlags());
    version_info.exec();
}

#endif
