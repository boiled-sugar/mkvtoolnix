/*
  mkvmerge GUI -- utility for splicing together matroska files
      from component media subtypes

  tab_input.cpp

  Written by Moritz Bunkus <moritz@bunkus.org>
  Parts of this code were written by Florian Wager <flo.wagner@gmx.de>

  Distributed under the GPL
  see the file COPYING for details
  or visit http://www.gnu.org/copyleft/gpl.html
*/

/*!
    \file
    \version $Id$
    \brief "settings" tab
    \author Moritz Bunkus <moritz@bunkus.org>
*/

#include "wx/wxprec.h"

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/listctrl.h"
#include "wx/statline.h"
#include "wx/config.h"

#include "common.h"
#include "matroskalogo_big.xpm"
#include "mmg.h"
#include "tab_settings.h"

tab_settings::tab_settings(wxWindow *parent):
  wxPanel(parent, -1, wxDefaultPosition, wxSize(100, 400),
          wxTAB_TRAVERSAL) {
  new wxStaticBox(this, -1, wxS("mkvmerge executable"), wxPoint(10, 5),
                  wxSize(475, 50));
  tc_mkvmerge =
    new wxTextCtrl(this, ID_TC_MKVMERGE, wxS(""), wxPoint(15, 25),
                   wxSize(370, -1), wxTE_READONLY);

  new wxButton(this, ID_B_BROWSEMKVMERGE, wxS("Browse"), wxPoint(395, 25),
               wxDefaultSize, 0);

  new wxStaticBox(this, -1, wxS("Miscellaneous options"), wxPoint(10, 65),
                  wxSize(475, 50));
  new wxStaticText(this, -1, wxS("Process priority:"), wxPoint(15, 85));
  cob_priority =
    new wxComboBox(this, ID_COB_PRIORITY, wxS(""), wxPoint(120, 85 + YOFF),
                   wxSize(90, -1), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  cob_priority->SetToolTip(wxS("Sets the priority that mkvmerge will run "
                               "with."));
#if defined(SYS_WINDOWS)
  cob_priority->Append(wxS("highest"));
  cob_priority->Append(wxS("higher"));
#endif
  cob_priority->Append(wxS("normal"));
  cob_priority->Append(wxS("lower"));
  cob_priority->Append(wxS("lowest"));


  new wxStaticBox(this, -1, wxS("About"), wxPoint(10, 350),
                  wxSize(475, 104));
  new wxStaticBitmap(this, -1, wxBitmap(matroskalogo_big_xpm),
                     wxPoint(20, 370), wxSize(64,64));
  new wxStaticText(this, wxID_STATIC,
                   wxS("mkvmerge GUI v" VERSION "\n"
                       "This GUI was written by Moritz Bunkus <moritz@"
                       "bunkus.org>\n"
                       "Based on mmg by Florian Wagner <flo.wagner@gmx.de>\n"
                       "mkvmerge GUI is licensed under the GPL.\n"
                       "http://www.bunkus.org/videotools/mkvtoolnix/"),
                   wxPoint(95, 360), wxDefaultSize, 0);

  load_preferences();
}

tab_settings::~tab_settings() {
  save_preferences();
}

void
tab_settings::on_browse(wxCommandEvent &evt) {
  wxFileDialog dlg(NULL, wxS("Choose the mkvmerge executable"),
                   tc_mkvmerge->GetValue().BeforeLast(PSEP), wxS(""),
#ifdef SYS_WINDOWS
                   wxS("Executable files (*.exe)|*.exe|" ALLFILES),
#else
                   wxS("All files (*)|*"),
#endif
                   wxOPEN);
  if(dlg.ShowModal() == wxID_OK) {
    tc_mkvmerge->SetValue(dlg.GetPath());
    mkvmerge_path = dlg.GetPath();
    save_preferences();
    query_mkvmerge_capabilities();
  }
}

void
tab_settings::on_priority_selected(wxCommandEvent &evt) {
  save_preferences();
}

void
tab_settings::load_preferences() {
  wxConfig *cfg = (wxConfig *)wxConfigBase::Get();
  wxString priority;
  int i;

  cfg->SetPath(wxS("/GUI"));
  if (!cfg->Read(wxS("mkvmerge_executable"), &mkvmerge_path))
    mkvmerge_path = wxS("mkvmerge");
  tc_mkvmerge->SetValue(mkvmerge_path);
  query_mkvmerge_capabilities();

  if (!cfg->Read(wxS("process_priority"), &priority))
    priority = wxS("normal");
  cob_priority->SetSelection(0);
  for (i = 0; i < cob_priority->GetCount(); i++)
    if (priority == cob_priority->GetString(i)) {
      cob_priority->SetSelection(i);
      break;
    }
}

void
tab_settings::save_preferences() {
  wxConfig *cfg = (wxConfig *)wxConfigBase::Get();
  cfg->SetPath(wxS("/GUI"));
  cfg->Write(wxS("mkvmerge_executable"), tc_mkvmerge->GetValue());
  cfg->Write(wxS("process_priority"), cob_priority->GetValue());
  cfg->Flush();
}

void
tab_settings::save(wxConfigBase *cfg) {
}

void
tab_settings::load(wxConfigBase *cfg) {
}

bool
tab_settings::validate_settings() {
  return true;
}

void
tab_settings::query_mkvmerge_capabilities() {
  wxString tmp;
  wxArrayString output, errors;
  vector<wxString> parts;
  int result, i;

  tmp = wxS("\"") + mkvmerge_path + wxS("\" --capabilities");
  result = wxExecute(tmp, output, errors);
  if (result == 0) {
    capabilities.clear();
    for (i = 0; i < output.Count(); i++) {
      tmp = output[i];
      strip(tmp);
      parts = split(tmp, "=", 2);
      if (parts.size() == 1)
        capabilities[parts[0]] = "true";
      else
        capabilities[parts[0]] = parts[1];
    }
  }
}

IMPLEMENT_CLASS(tab_settings, wxPanel);
BEGIN_EVENT_TABLE(tab_settings, wxPanel)
  EVT_BUTTON(ID_B_BROWSEMKVMERGE, tab_settings::on_browse)
  EVT_COMBOBOX(ID_COB_PRIORITY, tab_settings::on_priority_selected)
END_EVENT_TABLE();
