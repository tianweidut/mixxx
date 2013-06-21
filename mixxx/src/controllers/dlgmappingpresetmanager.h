#ifndef DLGMAPPINGPRESETMANAGER_H_
#define DLGMAPPINGPRESETMANAGER_H_
#include <QtGui>
#include <QHash>

#include "controllers/ui_dlgmappingpresetmanagerdlg.h"

class DlgMappingPresetManager : public QDialog {
    Q_OBJECT
  public:
    DlgMappingPresetManager(QWidget* parent);
    virtual ~DlgMappingPresetManager() {};
  protected:
    Ui::DlgPresetManagerDlg& getUi() {
        return m_ui;
    }
  private:
    Ui::DlgPresetManagerDlg m_ui;
};
#endif

